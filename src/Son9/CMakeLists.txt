set(Son9_HEADERS
	"${PROJECT_SOURCE_DIR}/src/Son9/errors.hpp"
	"${PROJECT_SOURCE_DIR}/include/Son9/Son9/file_writer.hpp"
		"${PROJECT_SOURCE_DIR}/src/Son9/str.hpp"
		)
set(Son9_SOURCES
	"${PROJECT_SOURCE_DIR}/src/Son9/errors.cpp"
	"${PROJECT_SOURCE_DIR}/src/Son9/file_writer.cpp"

		str.cpp)
add_library(Son9 STATIC ${Son9_HEADERS} ${Son9_SOURCES})
target_include_directories(Son9 PUBLIC "${PROJECT_SOURCE_DIR}/include/Son9/")
target_include_directories(Son9 PRIVATE "${PROJECT_SOURCE_DIR}/src/Son9/")
target_link_libraries(Son9 PRIVATE externLibSon9)
