add_executable(FormatChanger2_Legacy main_legacy.cpp)
target_link_libraries(FormatChanger2_Legacy PRIVATE TinyFileDialogs Son32 ADIDatIO -static-libgcc -static-libstdc++)

add_executable(FormatChanger2 main.cpp)
target_link_libraries(FormatChanger2 PRIVATE ADIDatIO_facade Son32_facade TinyFileDialogs utils -static-libgcc -static-libstdc++)