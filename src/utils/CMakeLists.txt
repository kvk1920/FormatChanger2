set(UTILS_HEADERS
		"${PROJECT_SOURCE_DIR}/include/utils/utils/progress_bar.hpp"
		"${PROJECT_SOURCE_DIR}/include/utils/utils/console_progress_bar.hpp"
		"${PROJECT_SOURCE_DIR}/include/utils/utils/streams/tee_buf.hpp"
		"${PROJECT_SOURCE_DIR}/include/utils/utils/streams/tee_stream.hpp"
		"${PROJECT_SOURCE_DIR}/include/utils/utils/streams/log_buf.hpp"
		"${PROJECT_SOURCE_DIR}/include/utils/utils/streams/log_stream.hpp"
		)
set(UTILS_SOURCES
		"${PROJECT_SOURCE_DIR}/src/utils/console_progress_bar.cpp"
		"${PROJECT_SOURCE_DIR}/src/utils/streams/tee_buf.cpp"
		"${PROJECT_SOURCE_DIR}/src/utils/streams/log_buf.cpp"
		"${PROJECT_SOURCE_DIR}/src/utils/streams/log_stream.cpp"
		"${PROJECT_SOURCE_DIR}/src/utils/streams/tee_stream.cpp"
		)

add_library(utils STATIC ${UTILS_SOURCES} ${UTILS_HEADERS})
target_include_directories(utils PUBLIC "${PROJECT_SOURCE_DIR}/include/utils/")
target_include_directories(utils PRIVATE "${PROJECT_SOURCE_DIR}/src/")
