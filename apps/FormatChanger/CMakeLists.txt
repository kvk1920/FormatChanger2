add_executable(FormatChanger main.cpp)
target_link_libraries(FormatChanger PRIVATE TinyFileDialogs Son32 ADIDatIO -static-libgcc -static-libstdc++)