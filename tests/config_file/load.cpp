#include <utils/config/config_file.hpp>
#include <utils/config/text.hpp>

int main()
{
    setlocale(LC_ALL, "");
    using utils::config::ConfigFile;
    using utils::config::Text;
    using namespace std;
    auto& config{ConfigFile::instance()};
    config.createVar(std::make_unique<Text>("user.name"));
    const auto name{config.loadFile().loadVar("user.name")->get<std::wstring>()};
    std::wcout << name << std::endl;
    std::wcout << L"Петр Петров" << std::endl;
}