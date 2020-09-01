set(UTILS_HEADERS
	"${PROJECT_SOURCE_DIR}/include/utils/progress_bar.hpp"
		"${PROJECT_SOURCE_DIR}/include/utils/console_progress_bar.hpp"
		"${PROJECT_SOURCE_DIR}/include/utils/logger.hpp"
		../../include/utils/output_stream.hpp ../../include/utils/ostream_tee.hpp)
set(UTILS_SOURCES
	"${PROJECT_SOURCE_DIR}/src/utils/console_progress_bar.cpp"
		"${PROJECT_SOURCE_DIR}/src/utils/logger.cpp"
		streams/tee.cpp)
add_library(utils STATIC ${UTILS_SOURCES} ${UTILS_HEADERS})
target_include_directories(utils PUBLIC "${PROJECT_SOURCE_DIR}/include/")
target_include_directories(utils PRIVATE "${PROJECT_SOURCE_DIR}/src/")
