add_executable(test_load_real_wave main.cpp)
target_link_libraries(test_load_real_wave PRIVATE Son9 TinyFileDialogs -static-libgcc -static-libstdc++ -static)
set_target_properties(test_load_real_wave PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/"
        )