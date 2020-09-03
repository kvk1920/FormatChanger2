add_executable(FormatChanger main.cpp)
target_link_libraries(FormatChanger PRIVATE TinyFileDialogs Son9 externLibADIDatIO -static-libgcc -static-libstdc++)