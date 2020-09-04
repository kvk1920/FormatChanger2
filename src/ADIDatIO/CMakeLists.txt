set(ADIDatIO_HEADERS
        "${PROJECT_SOURCE_DIR}/include/ADIDatIO/ADIDatIO/file_reader.hpp"
		"${PROJECT_SOURCE_DIR}/include/ADIDatIO/ADIDatIO/common.hpp"
		"${PROJECT_SOURCE_DIR}/src/ADIDatIO/errors.hpp"
		"${PROJECT_SOURCE_DIR}/include/ADIDatIO/ADIDatIO/channel_reader.hpp"
        "${PROJECT_SOURCE_DIR}/include/Son9/Son9/common.hpp")
set(ADIDatIO_SOURCES
		"${PROJECT_SOURCE_DIR}/src/ADIDatIO/errors.cpp"
		"${PROJECT_SOURCE_DIR}/src/ADIDatIO/file_reader.cpp"
		"${PROJECT_SOURCE_DIR}/src/ADIDatIO/channel_reader.cpp"
        "${PROJECT_SOURCE_DIR}/include/Son9/Son9/file_reader.hpp"
		)

add_library(ADIDatIO STATIC ${ADIDatIO_SOURCES} ${ADIDatIO_HEADERS})

target_include_directories(ADIDatIO PUBLIC "${PROJECT_SOURCE_DIR}/include/ADIDatIO/")

target_include_directories(ADIDatIO PRIVATE "${PROJECT_SOURCE_DIR}/src/ADIDatIO/")

target_link_libraries(ADIDatIO PRIVATE externLibADIDatIO)
target_link_libraries(ADIDatIO PUBLIC utils)
