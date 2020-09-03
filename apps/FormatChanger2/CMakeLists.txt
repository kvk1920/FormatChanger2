add_executable(FormatChanger2 main.cpp)
target_link_libraries(FormatChanger2 PRIVATE ADIDatIO Son9 TinyFileDialogs utils -static-libgcc -static-libstdc++)