cmake_minimum_required(VERSION 3.12)

project(FormatChanger2)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(extern/tinyfiledialogs)
add_subdirectory(extern/son32)

add_executable(FormatChanger2 main.cpp)

target_link_libraries(FormatChanger2 PRIVATE TinyFileDialogs Son32)
target_link_libraries(FormatChanger2 PRIVATE
    "${PROJECT_SOURCE_DIR}/ADIDatIO/ADIDatIOWin.lib"
    )

file(COPY "${PROJECT_SOURCE_DIR}/ADIDatIO/ADIDatIOWin.dll" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
