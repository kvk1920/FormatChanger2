#include <tinyfiledialogs.h>
#include <Son9/file_reader.hpp>

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

int main()
{
    const wchar_t* filters[] = {L"*.SMR"};
    fs::path src{tinyfd_openFileDialogW(
            nullptr,
            nullptr,
            std::size(filters),
            filters,
            nullptr, false)};
    using namespace Son9;
    auto reader{FileReader::load(src)};
    std::wofstream out{L"result.txt"};
    int id{0};
    for (const auto& channel : reader->fileInfo().channels)
    {
        if (channel.type == ChannelType::ADC)
        {
            out << channel.name.c_str() << std::endl;
            std::vector<float> buff;
            reader->loadRealWaveData(id, buff);
            for (auto x : buff)
                out << x << L'\n';
            break;
        }
        ++id;
    }
}