add_executable(DirectoryWalker main.cpp)
target_link_libraries(DirectoryWalker PRIVATE TinyFileDialogs utils -static-libgcc -static-libstdc++ -static -lpthread)
set_target_properties(DirectoryWalker PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/"
        )