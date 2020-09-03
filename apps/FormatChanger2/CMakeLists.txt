add_executable(FormatChanger2 main.cpp)
target_link_libraries(FormatChanger2 PRIVATE ADIDatIO Son9 TinyFileDialogs utils -static-libgcc -static-libstdc++)
set_target_properties(FormatChanger2 PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/"
        )