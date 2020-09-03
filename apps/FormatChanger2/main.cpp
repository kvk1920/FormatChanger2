#include <ADIDatIO/channel_reader.hpp>
#include <Son9/file_writer.hpp>
#include <tinyfiledialogs.h>
#include <utils/ui/console_progress_bar.hpp>

#include <utils/streams/log_stream.hpp>
#include <utils/streams/tee_stream.hpp>

#include <filesystem>
#include <iostream>
#include <fstream>
#include <numeric>
#include <string>
#include <vector>

namespace fs = std::filesystem;

using namespace utils::streams;
using namespace utils::ui;

namespace
{

enum class ChooseMode
{
    Files,
    Directories,
};

ChooseMode getChooseMode()
{
    int code{tinyfd_messageBoxW(L"FormatChanger2",
                                L"\"Да\" - режим выбора директорий\n"
                                "\"Нет\" - режим выбора файлов",
                                L"yesno", L"question", 0)};
    return code ? ChooseMode::Directories : ChooseMode::Files;
}

std::vector<std::wstring> splitWString(const std::wstring& str, const wchar_t delimiter)
{
    std::vector<std::wstring> result{1};
    for (auto ch : str)
    {
        if (delimiter == ch)
            result.emplace_back();
        else
            result.back().push_back(ch);
    }
    return result;
}

std::vector<fs::path> getPathsFromDialog(ChooseMode mode, const fs::path& default_path)
{
    std::wstring paths;
    const wchar_t* filters[]{L"*.adicht"};
    switch (mode)
    {
        case ChooseMode::Files:
        {
            const wchar_t* ptr{tinyfd_openFileDialogW(
                    L"Файлы для обработки (в формате .adicht)",
                    default_path.wstring().c_str(),
                    1,
                    filters,
                    nullptr,
                    1
            )};
            if (nullptr == ptr)
                return {};
            paths = ptr;
        }
            break;
        case ChooseMode::Directories:
        {
            const wchar_t* ptr{tinyfd_selectFolderDialogW(
                    L"Директория для обработки",
                    default_path.wstring().c_str()
            )};
            if (nullptr == ptr)
                return {};
            for (const auto& file : fs::directory_iterator(ptr))
            {
                if (L".adicht" != file.path().extension())
                    continue;
                if (!paths.empty())
                    paths.push_back(L'|');
                paths.append(file.path());
            }
        }
            break;
    }
    std::vector<fs::path> result;
    for (const auto& str : splitWString(paths, L'|'))
        result.emplace_back(str);
    return result;
}

fs::path getResultDirectory(const fs::path& default_path)
{
    std::wstring message{
            L"Сохранить в директорию " + default_path.wstring() +
            L"\nЕсли ничего не выбрать, то будет выбрана директория"
            "с исходными файлами"
    };
    const wchar_t* path_to_save{tinyfd_selectFolderDialogW(
            message.c_str(),
            default_path.wstring().c_str()
    )};
    if (nullptr == path_to_save)
        return default_path;
    return path_to_save;
}

std::vector<std::size_t> determineChunkSize(const std::vector<ADIDatIO::ChannelReader>& channels)
{
    if (channels.empty())
        return {};
    std::vector<std::size_t> us_sample_period(channels.size());
    for (std::size_t channel_id{0}; channel_id < channels.size(); ++channel_id)
    {
        const auto& channel{channels[channel_id]};
        // TODO: check if there are different sample periods in different records
        us_sample_period[channel_id] = std::round(channel.channelInfo().records[0].sample_period * 1'000'000);
    }
    std::size_t lcm{us_sample_period[0]};
    for (std::size_t i{1}; i < channels.size(); ++i)
        lcm = std::lcm(lcm, us_sample_period[i]);
    std::vector<std::size_t> chunk_sizes(channels.size());
    for (std::size_t i{0}; i < channels.size(); ++i)
        chunk_sizes[i] = (lcm / us_sample_period[i]) * 512;
    return chunk_sizes;
}

inline
Son9::Time convert(const ADIDatIO::Time& t)
{
    return {t.seconds, t.frac_seconds};
}

void transferChannels(const fs::path& input_file, const fs::path& output_directory, std::wostream& log)
{
    log << "transfer from:\n" << input_file.wstring() << std::endl;
    log << "transfer to:\n" << output_directory.wstring() << std::endl;
    {
        std::error_code error;
        if (fs::exists(output_directory))
            if (!fs::remove_all(output_directory, error))
                throw std::system_error(error);
        if (!fs::create_directory(output_directory, error))
            throw std::system_error(error);
    }

    auto reader{ADIDatIO::FileReader::load(input_file)};
    const auto number_of_channels{reader->channelsInfo().size()};

    std::vector<ADIDatIO::ChannelReader> channels;
    channels.reserve(number_of_channels);
    for (std::size_t i{0}; i < number_of_channels; ++i)
        channels.emplace_back(reader, i);

    std::vector<std::vector<float>> buffers{channels.size()};

    const auto chunk_sizes{determineChunkSize(channels)};

    Son9::Config file_config;
    file_config.start = convert(reader->fileStart());
    file_config.channels.resize(number_of_channels);
    for (std::size_t channel_id{0}; channel_id < number_of_channels; ++channel_id)
    {
        Son9::ChannelConfig& channel_config{file_config.channels[channel_id]};
        const auto& info{channels[channel_id].channelInfo()};
        channel_config.units = info.units;
        channel_config.name = info.name;
        channel_config.sample_period = info.records[0].sample_period;
        log << "calculating offset and scale for channel " << info.name << std::endl;
        std::unique_ptr<IProgressBar> progress_bar{new ConsoleProgressBar(std::wcout)};
        channels[channel_id].setProgressBar(progress_bar.get());
        channel_config.calculateScaleInfo([&](std::vector<float>& buff) -> bool {
            return channels[channel_id].load(buff, 1024);
        });
        channels[channel_id].setProgressBar();
        channels[channel_id].reset();
    }

    std::size_t current_file_number{0};

    const auto get_file_number = [&current_file_number] {
        std::string number{std::to_string(current_file_number)};
        std::string result{std::string(3 - number.length(), '0') + number};
        return std::wstring{result.begin(), result.end()};
    };

    file_config.path = fs::current_path() /
                       (input_file.filename().stem().wstring() + L"___" +
                        get_file_number() + input_file.filename().extension().wstring());

    auto writer{Son9::FileWriter::create(file_config)};
    bool done{false};

    const std::size_t size_limit{1024 * 1024 * 512};

    const auto check_file = [&] {
        if (!writer)
        {
            ++current_file_number;
            file_config.start = convert(channels[0].currentPosition().toTime(channels[0]));
            file_config.path = fs::current_path() /
                               (input_file.filename().stem().wstring() + L"___" +
                                get_file_number() + input_file.filename().extension().wstring());
            writer = Son9::FileWriter::create(file_config);
        }
    };

    const auto flush_file = [&](bool force = false) {
        if (!writer)
            return;
        const auto expected_size{writer->expectedSize()};
        if (force || expected_size >= size_limit)
        {
            const auto source{writer->path()};
            auto destination{output_directory / file_config.path.filename()};
            destination.replace_extension(L".SMR");
            writer.reset();
            if (source != destination)
            {
                if (fs::exists(destination))
                {
                    std::wstring text{
                            L"Файл " + destination.wstring() + L" уже существует. Заменить его?"
                    };
                    if (tinyfd_messageBoxW(L"FormatChanger2",
                                           text.c_str(),
                                           L"yesno", L"question", 0))
                    {
                        fs::remove(destination);
                        fs::rename(source, destination);
                    }
                }
                else
                {
                    fs::rename(source, destination);
                }
            }
        }
    };

    log << "transfer all channels from " << input_file.filename() << " to " << output_directory << std::endl;

    std::unique_ptr<IProgressBar> transfer_pb{new ConsoleProgressBar(std::wcout)};

    {
        int64_t max_progress{0};
        for (const auto& channel : reader->channelsInfo())
            for (const auto& record : channel.records)
                max_progress += record.number_of_samples;
        transfer_pb->setMaxProgress(max_progress, true);
    }

    reader->setProgressBar(transfer_pb.get());

    while (!done)
    {
        done = true;
        check_file();
        auto current_time{convert(channels[0].currentPosition().toTime(channels[0]))};

        writer->skipTo(current_time);

        for (std::size_t channel_id{0}; channel_id < number_of_channels; ++channel_id)
        {
            if (channels[channel_id].load(buffers[channel_id], chunk_sizes[channel_id]))
            {
                done = false;
                writer->write(channel_id, buffers[channel_id]);
            }
        }

        flush_file();
    }
    flush_file(true);
    reader->setProgressBar();
    log << "\nall channel successfully transferred" << std::endl;
}

int exitMessage()
{
    tinyfd_messageBoxW(L"FormatChanger2", L"Процесс завершён", L"ok", L"info", 0);
    return 0;
}

fs::path createOutputDirectory(fs::path result_dir, const fs::path& input)
{
    result_dir /= input.filename().stem();
    return result_dir;
}

struct ReplaceInfo
{
    fs::path input;
    fs::path output;
    bool need_replace;
};

std::vector<ReplaceInfo> checkOutputDirectory(const std::vector<fs::path>& input, const std::vector<fs::path>& output)
{
    std::size_t n{output.size()};
    std::vector<ReplaceInfo> result;
    for (std::size_t i{0}; i < input.size(); ++i)
        result.push_back({input[i], output[i], fs::exists(output.at(i))});
    return result;
}

std::vector<std::pair<fs::path, fs::path>> createConversionList(const std::vector<ReplaceInfo>& replace_info, std::wostream& logger)
{
    enum class ReplacementPolicy
    {
        ReplaceAll,
        NoReplace,
        Undefined,
    };
    ReplacementPolicy replacement_policy{ReplacementPolicy::Undefined};
    if (std::any_of(replace_info.begin(), replace_info.end(), [](const ReplaceInfo& info) { return info.need_replace; }))
    {
        int code{tinyfd_messageBoxW(
                L"FormatChanger2",
                L"Один или несколько файлов уже существуют в директории назначения.\n"
                L"Хотите ли вы заменить их все?",
                L"yesno",
                L"question",
                0
        )};
        if (code)
            replacement_policy = ReplacementPolicy::ReplaceAll;
        else
        {
            code = tinyfd_messageBoxW(
                    L"FormatChanger",
                    L"Хотите ли вы ничего не заменять?\nЕсли нет, то каждый случай будет рассмотрен отдельно.",
                    L"yesno",
                    L"question",
                    0
            );
            if (code)
                replacement_policy = ReplacementPolicy::NoReplace;
        }
    }
    std::vector<std::pair<fs::path, fs::path>> result;
    result.reserve(replace_info.size());
    for (const auto& info : replace_info)
    {
        if (!info.need_replace)
        {
            result.emplace_back(info.input, info.output);
            continue;
        }
        if (replacement_policy != ReplacementPolicy::Undefined)
        {
            if (ReplacementPolicy::ReplaceAll == replacement_policy)
            {
                logger << info.output << L" будет удалена вместе со всем содержимым" << std::endl;
                result.emplace_back(info.input, info.output);
            }
            continue;
        }
        bool is_directory{fs::is_directory(info.output)};
        std::wstring msg{is_directory ? L"Директория\n" : L"Файл\n"};
        msg.append(info.output);
        msg.append(L"\nуже существует. Хотите заменить ");
        msg.append(is_directory ? L"её " : L"его ");
        msg.append(L"со всем содержимым?");
        int code{tinyfd_messageBoxW(
                L"FormatChanger2",
                msg.c_str(),
                L"yesno",
                L"question",
                0
        )};
        if (code)
        {
            logger << info.output << L" будет удалена вместе со всем содержимым" << std::endl;
            result.emplace_back(info.input, info.output);
        }
    }
    return result;
}

}

#include <io.h>

int main()
{
    // setmode(U16TEXT)
    setlocale(LC_ALL, "");
    std::wofstream log_file;
    log_file.open(L"log.txt", std::ios::out);
    LogStream log_stream(log_file);
    TeeStream log;
    log.addStream(log_stream).addStream(std::wcout);

    const auto input_files{getPathsFromDialog(getChooseMode(), fs::current_path())};
    if (input_files.empty())
    {
        log << "No files chosen" << std::endl;
        return exitMessage();
    }
    log << "input directory: " << input_files.at(0).parent_path() << std::endl;
    for (const auto& input_file : input_files)
        log << input_file.filename() << std::endl;
    const auto result_dir{getResultDirectory(input_files.at(0).parent_path())};

    std::vector<fs::path> output_directories(input_files.size());
    for (std::size_t i{0}; i < input_files.size(); ++i)
        output_directories[i] = createOutputDirectory(result_dir, input_files[i]);
    auto conversion_info{createConversionList(checkOutputDirectory(input_files, output_directories), log)};

    for (const auto& files : conversion_info)
    {
        log << "processing file: " << files.first.filename() << std::endl;
        try
        {
            transferChannels(files.first, files.second, log);
        }
        catch (std::exception& ex)
        {
            log << "error:\n" << ex.what() << std::endl;
        }
        catch (...)
        {
            log << "unknown error" << std::endl;
        }
    }
    tinyfd_messageBoxW(L"FormatChanger2", L"Процесс завершён", L"ok", L"info", 0);
}