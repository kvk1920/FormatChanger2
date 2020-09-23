#include <Son9/file_reader.hpp>

#include <tinyfiledialogs.h>

#include <iostream>
#include <fstream>

namespace fs = std::filesystem;

namespace
{

std::vector<int> chooseChannels()
{
    using namespace std;
    wstring s;
    getline(wcin, s);
    wistringstream parser(s);
    vector<int> result;
    int x;
    while (parser >> x)
        result.push_back(x);
    return result;
}

fs::path chooseFile()
{
    const wchar_t* filters[] = {L"*.SMR"};
    return tinyfd_openFileDialogW(L"ChannelExtractor",
                                  L"",
                                  1,
                                  filters,
                                  L"",
                                  0
                                  );
}

fs::path createResultFilename(const fs::path& directory, const std::string& channel_name)
{
    return (directory / std::wstring(channel_name.begin(), channel_name.end())).replace_extension(L".txt");
}

fs::path createOutputDirectory(fs::path input)
{
    auto result{input.replace_extension("")};
    try
    {
        fs::create_directory(result);
    } catch (...) {}
    return result;
}

void exportChannel(const fs::path& path, std::shared_ptr<Son9::FileReader> reader, int channel_id)
{
    std::wcerr << path.wstring() << std::endl;
    std::wofstream out(path.wstring().c_str());
    const auto& info{reader->fileInfo().channels[channel_id]};
    out << L"\"CHANNEL\"\t\"" << info.name.c_str() << '"' << std::endl;
    out << '"' << Son9::channelTypeToString(info.type) << '"' << std::endl;
    out << "\"No comment\"" << std::endl;
    out << '"' << info.name.c_str() << '"' << std::endl;
    out << std::endl;
    std::vector<long> buff;
    reader->loadEventTimes(channel_id, buff);
    out << std::fixed << std::setprecision(5);
    for (int64_t event : buff)
        out << event * reader->fileInfo().us_per_time / 1000000.0 << std::endl;
}

void run()
{
    const auto path{chooseFile()};
    auto reader = Son9::FileReader::load(path);
    using std::wcout, std::endl;
    std::size_t number_of_channels{reader->fileInfo().channels.size()};
    wcout << "number of channels in file: " << number_of_channels << endl;
    std::vector<std::size_t> event_channels;
    std::size_t channel_number{0};
    for (const auto& info : reader->fileInfo().channels)
    {
        if (Son9::ChannelType::EventRise == info.type)
            event_channels.push_back(channel_number);
        ++channel_number;
    }
    wcout << "Event channels:" << endl;
    channel_number = 0;
    for (auto channel_id : event_channels)
    {
        ++channel_number;
        wcout << channel_number << ": " << reader->fileInfo().channels.at(channel_id).name.c_str() << endl;
    }
    std::vector<int> channels_to_export{chooseChannels()};
    for (int& channel_id : channels_to_export)
        try
        {
            channel_id = event_channels.at(channel_id - 1);
        }
        catch (...) {}
    auto directory{createOutputDirectory(path)};
    for (int channel_id : channels_to_export)
    {
        const auto output{createResultFilename(directory,
                                               reader->fileInfo().channels.at(channel_id).name)};
        exportChannel(output,
                      reader,
                      channel_id);
    }
}

}

int main()
{
    setlocale(LC_ALL, "");
    run();
}