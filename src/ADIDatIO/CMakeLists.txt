set(ADIDatIO_HEADERS
        "${PROJECT_SOURCE_DIR}/include/ADIDatIO/detail/errors.h"
        "${PROJECT_SOURCE_DIR}/include/ADIDatIO/detail/str_convert.h"

        "${PROJECT_SOURCE_DIR}/include/ADIDatIO/reader.h")
set(ADIDatIO_SOURCES
	"${PROJECT_SOURCE_DIR}/src/ADIDatIO/errors.cpp"
        "${PROJECT_SOURCE_DIR}/src/ADIDatIO/str_convert.cpp"
        "${PROJECT_SOURCE_DIR}/src/ADIDatIO/reader.cpp"
)
add_library(ADIDatIO_facade STATIC ${ADIDatIO_SOURCES} ${ADIDatIO_HEADERS})
target_include_directories(ADIDatIO_facade PUBLIC "${PROJECT_SOURCE_DIR}/include/")
target_link_libraries(ADIDatIO_facade PRIVATE ADIDatIO)
