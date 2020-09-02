set(UTILS_HEADERS
	"${PROJECT_SOURCE_DIR}/include/utils/progress_bar.hpp"
		"${PROJECT_SOURCE_DIR}/include/utils/console_progress_bar.hpp"
		"${PROJECT_SOURCE_DIR}/include/utils/logger.hpp"
		../../include/utils/output_stream.hpp ../../include/utils/ostream_tee.hpp
		"${PROJECT_SOURCE_DIR}/include/utils/streams/tee_buf.hpp"
		"${PROJECT_SOURCE_DIR}/include/utils/streams/tee_stream.hpp"
		"${PROJECT_SOURCE_DIR}/include/utils/streams/log_buf.hpp"
		"${PROJECT_SOURCE_DIR}/include/utils/streams/log_stream.hpp")
set(UTILS_SOURCES
	"${PROJECT_SOURCE_DIR}/src/utils/console_progress_bar.cpp"
		"${PROJECT_SOURCE_DIR}/src/utils/logger.cpp"
		streams/tee_buf.cpp streams/log_buf.cpp streams/log_stream.cpp streams/tee_stream.cpp)
add_library(utils STATIC ${UTILS_SOURCES} ${UTILS_HEADERS})
target_include_directories(utils PUBLIC "${PROJECT_SOURCE_DIR}/include/")
target_include_directories(utils PRIVATE "${PROJECT_SOURCE_DIR}/src/")
