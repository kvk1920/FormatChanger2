#include <Son32/str.hpp>

#include <unordered_map>

namespace Son32
{
namespace
{

const std::unordered_map<wchar_t, std::string> g_translate_table = {
        {L'А', "A"},
        {L'Б', "B"},
        {L'В', "V"},
        {L'Г', "G"},
        {L'Д', "D"},
        {L'Е', "E"},
        {L'Ё', "Yo"},
        {L'Ж', "Zh"},
        {L'З', "Z"},
        {L'И', "I"},
        {L'Й', "I"},
        {L'К', "K"},
        {L'Л', "L"},
        {L'М', "M"},
        {L'Н', "N"},
        {L'О', "O"},
        {L'П', "P"},
        {L'Р', "R"},
        {L'С', "S"},
        {L'Т', "T"},
        {L'У', "U"},
        {L'Ф', "F"},
        {L'Х', "H"},
        {L'Ц', "C"},
        {L'Ч', "Ch"},
        {L'Ш', "Sh"},
        {L'Щ', "Sh'"},
        {L'Ъ', "`"},
        {L'Ы', "Y"},
        {L'Ь', "'"},
        {L'Э', "E"},
        {L'Ю', "Yu"},
        {L'Я', "Ya"},
        {L'а', "a"},
        {L'б', "b"},
        {L'в', "v"},
        {L'г', "g"},
        {L'д', "d"},
        {L'е', "e"},
        {L'ё', "yo"},
        {L'ж', "zh"},
        {L'з', "z"},
        {L'и', "i"},
        {L'й', "i"},
        {L'к', "k"},
        {L'л', "l"},
        {L'м', "m"},
        {L'н', "n"},
        {L'о', "o"},
        {L'п', "p"},
        {L'р', "r"},
        {L'с', "s"},
        {L'т', "t"},
        {L'у', "u"},
        {L'ф', "f"},
        {L'х', "h"},
        {L'ц', "c"},
        {L'ч', "ch"},
        {L'ш', "sh"},
        {L'щ', "sh'"},
        {L'ъ', "`"},
        {L'ы', "y"},
        {L'ь', "'"},
        {L'э', "e"},
        {L'ю', "yu"},
        {L'я', "ya"},
};

}

std::string
wstringToString(const std::wstring& wstr)
{
    std::string str;
    for (auto c : wstr)
    {
        if (static_cast<int>(c) > 127)
        {
            if (auto it{g_translate_table.find(c)}; it != g_translate_table.end())
                str.append(it->second);
            else
                str.push_back('_');
        }
        else
            str.push_back(c);
        if (isblank(str.back()))
            str.back() = '_';
    }
    return str;
}
}