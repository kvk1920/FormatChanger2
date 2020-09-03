set(ADIDatIO_HEADERS
	"${PROJECT_SOURCE_DIR}/src/Son32/errors.hpp"
	"${PROJECT_SOURCE_DIR}/include/Son32/file_writer.hpp"
		"${PROJECT_SOURCE_DIR}/src/Son32/str.hpp"
		)
set(ADIDatIO_SOURCES
	"${PROJECT_SOURCE_DIR}/src/Son32/errors.cpp"
	"${PROJECT_SOURCE_DIR}/src/Son32/file_writer.cpp"

		str.cpp)
add_library(Son32_facade STATIC ${ADIDatIO_SOURCES} ${ADIDatIO_HEADERS})
target_include_directories(Son32_facade PUBLIC "${PROJECT_SOURCE_DIR}/include/")
target_include_directories(Son32_facade PRIVATE "${PROJECT_SOURCE_DIR}/src/")
target_link_libraries(Son32_facade PRIVATE Son9)
