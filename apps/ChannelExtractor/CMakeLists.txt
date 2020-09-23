add_executable(ChannelExtractor main.cpp)
target_link_libraries(ChannelExtractor PRIVATE Son9 TinyFileDialogs utils -static-libgcc -static-libstdc++)
set_target_properties(ChannelExtractor PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/"
        )