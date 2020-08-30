set(ADIDatIO_HEADERS
        "${PROJECT_SOURCE_DIR}/include/ADIDatIO/file_reader.hpp"
		"${PROJECT_SOURCE_DIR}/include/ADIDatIO/common.hpp"

		"${PROJECT_SOURCE_DIR}/src/ADIDatIO/errors.hpp"
		"${PROJECT_SOURCE_DIR}/include/ADIDatIO/channel_reader.hpp")
set(ADIDatIO_SOURCES
		"${PROJECT_SOURCE_DIR}/src/ADIDatIO/errors.cpp"
		"${PROJECT_SOURCE_DIR}/src/ADIDatIO/file_reader.cpp"
		"${PROJECT_SOURCE_DIR}/src/ADIDatIO/channel_reader.cpp")
add_library(ADIDatIO_facade STATIC ${ADIDatIO_SOURCES} ${ADIDatIO_HEADERS})
target_include_directories(ADIDatIO_facade PUBLIC "${PROJECT_SOURCE_DIR}/include/")
target_include_directories(ADIDatIO_facade PRIVATE "${PROJECT_SOURCE_DIR}/src/")
target_link_libraries(ADIDatIO_facade PRIVATE ADIDatIO)
target_link_libraries(ADIDatIO_facade PUBLIC utils)
