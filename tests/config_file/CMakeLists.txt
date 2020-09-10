add_executable(config_file_test_create create.cpp)
target_link_libraries(config_file_test_create PRIVATE utils)
set_target_properties(config_file_test_create PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/"
        )
add_executable(config_file_test_load load.cpp)
target_link_libraries(config_file_test_load PRIVATE utils)
set_target_properties(config_file_test_load PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/"
        )
enable_testing()