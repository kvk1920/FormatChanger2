#include <utils/streams/tee_stream.hpp>

#include <tinyfiledialogs.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

void traverseDirectory(const fs::path& root, const fs::path& current, std::wostream& result, int depth = 0)
{
    for (int i{0}; i < depth; ++i)
        result << L'\t';
    result << fs::relative(current, root).filename().wstring() << std::endl;
    if (fs::is_directory(current))
    {
        for (const auto& entry : fs::directory_iterator(current))
            traverseDirectory(root, entry.path(), result, depth + 1);
    }
}

int main()
{
    setlocale(LC_ALL, "");
    fs::path root{tinyfd_selectFolderDialogW(L"Выберите корневую папку", L"")};
    const wchar_t* filters[] = {L".doc"};
    fs::path result_file{tinyfd_saveFileDialogW(
            L"Выберите, куда сохранять",
            L"",
            std::size(filters),
            filters,
            NULL
            )};
    std::wofstream file_output{result_file};
    utils::streams::TeeStream result;
    result.addStream(file_output).addStream(std::wcout);
    traverseDirectory(root.parent_path(), root, result);
}