#include <utils/config/config_file.hpp>
#include <utils/config/text.hpp>

int main()
{
    setlocale(LC_ALL, "");
    using utils::config::ConfigFile;
    using utils::config::Text;
    using namespace std;
    auto user_name = make_unique<Text>("user.name");
    user_name->setString(L"Петя Петров");
    ConfigFile::instance().createVar(move(user_name));
    //ConfigFile::instance().loadVar("user.name")->set(L"Петя Петров");
    ConfigFile::instance().saveFile();
}