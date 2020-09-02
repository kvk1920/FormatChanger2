add_executable(FormatChanger2_Legacy main_legacy.cpp)
target_link_libraries(FormatChanger2_Legacy PRIVATE TinyFileDialogs Son32 ADIDatIO -static-libgcc -static-libstdc++)

add_executable(FormatChanger2 main.cpp ../include/utils/streams/tee.hpp ../include/utils/streams/log_buf.hpp ../include/utils/streams/log_stream.hpp)
target_link_libraries(FormatChanger2 PRIVATE ADIDatIO_facade Son32_facade TinyFileDialogs utils -static-libgcc -static-libstdc++)

add_executable(ProgressBarTest progress_bar.cpp)
target_link_libraries(ProgressBarTest utils -static-libgcc -static-libstdc++)