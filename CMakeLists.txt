cmake_minimum_required(VERSION 3.5)

project(FormatChanger2)

set(CMAKE_CXX_STANDARD 17)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(extern/tinyfiledialogs)

add_executable(FormatChanger2
    main.cpp
    )
# set_target_properties(FormatChanger2 PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")

target_link_libraries(FormatChanger2 TinyFileDialogs)
target_link_libraries(FormatChanger2
    "${PROJECT_SOURCE_DIR}/ADIDatIO/ADIDatIOWin.lib"
    "${PROJECT_SOURCE_DIR}/son32/son32.lib"
    )

file(COPY "${PROJECT_SOURCE_DIR}/ADIDatIO/ADIDatIOWin.dll" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
file(COPY "${PROJECT_SOURCE_DIR}/son32/son32.dll" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
