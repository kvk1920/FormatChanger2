add_executable(FormatChanger main.cpp)
target_link_libraries(FormatChanger
        PRIVATE TinyFileDialogs externLibSon9 externLibADIDatIO
        -static-libgcc -static-libstdc++)
set_target_properties(FormatChanger PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/"
        )