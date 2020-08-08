add_library(tiny_file_dialogs src/tinyfiledialogs.c)
target_include_directories(tiny_file_dialogs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_library(TinyFileDialogs ALIAS tiny_file_dialogs)
