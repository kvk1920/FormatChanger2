set(ADIDatIO_HEADERS
	"${PROJECT_SOURCE_DIR}/include/Son32/detail/errors.h"
	"${PROJECT_SOURCE_DIR}/include/Son32/writer.h")
set(ADIDatIO_SOURCES
	"${PROJECT_SOURCE_DIR}/src/Son32/errors.cpp"
	"${PROJECT_SOURCE_DIR}/src/Son32/writer.cpp"
)
add_library(Son32_facade STATIC ${ADIDatIO_SOURCES} ${ADIDatIO_HEADERS})
target_include_directories(Son32_facade PUBLIC "${PROJECT_SOURCE_DIR}/include/")
target_link_libraries(Son32_facade PRIVATE Son32)
