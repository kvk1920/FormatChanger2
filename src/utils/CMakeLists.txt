set(UTILS_HEADERS
		"${PROJECT_SOURCE_DIR}/include/utils/utils/ui/progress_bar.hpp"
		"${PROJECT_SOURCE_DIR}/include/utils/utils/ui/console_progress_bar.hpp"
		"${PROJECT_SOURCE_DIR}/include/utils/utils/streams/tee_buf.hpp"
		"${PROJECT_SOURCE_DIR}/include/utils/utils/streams/tee_stream.hpp"
		"${PROJECT_SOURCE_DIR}/include/utils/utils/streams/log_buf.hpp"
		"${PROJECT_SOURCE_DIR}/include/utils/utils/streams/log_stream.hpp"
		"${PROJECT_SOURCE_DIR}/include/utils/utils/config/var.hpp"
		"${PROJECT_SOURCE_DIR}/include/utils/utils/config/integer.hpp"
		"${PROJECT_SOURCE_DIR}/include/utils/utils/config/real.hpp"
		"${PROJECT_SOURCE_DIR}/include/utils/utils/config/text.hpp"
		"${PROJECT_SOURCE_DIR}/include/utils/utils/config/boolean.hpp"
		"${PROJECT_SOURCE_DIR}/include/utils/utils/config/config_file.hpp"
		)
set(UTILS_SOURCES
		"${PROJECT_SOURCE_DIR}/src/utils/ui/console_progress_bar.cpp"
		"${PROJECT_SOURCE_DIR}/src/utils/streams/tee_buf.cpp"
		"${PROJECT_SOURCE_DIR}/src/utils/streams/log_buf.cpp"
		"${PROJECT_SOURCE_DIR}/src/utils/streams/log_stream.cpp"
		"${PROJECT_SOURCE_DIR}/src/utils/streams/tee_stream.cpp"
		"${PROJECT_SOURCE_DIR}/src/utils/config/var.cpp"
		"${PROJECT_SOURCE_DIR}/src/utils/config/integer.cpp"
		"${PROJECT_SOURCE_DIR}/src/utils/config/real.cpp"
		"${PROJECT_SOURCE_DIR}/src/utils/config/text.cpp"
		"${PROJECT_SOURCE_DIR}/src/utils/config/boolean.cpp"
		"${PROJECT_SOURCE_DIR}/src/utils/config/config_file.cpp"
		)

add_library(utils STATIC ${UTILS_SOURCES} ${UTILS_HEADERS})
target_include_directories(utils PUBLIC "${PROJECT_SOURCE_DIR}/include/utils/")
target_include_directories(utils PRIVATE "${PROJECT_SOURCE_DIR}/src/")
