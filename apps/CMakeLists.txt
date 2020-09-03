add_subdirectory(FormatChanger)

add_executable(FormatChanger2 format_changer2.cpp)
target_link_libraries(FormatChanger2 PRIVATE ADIDatIO_facade Son32_facade TinyFileDialogs utils -static-libgcc -static-libstdc++)