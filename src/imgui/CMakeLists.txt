add_library(imgui STATIC
        imgui.cpp
        imgui_demo.cpp
        imgui_draw.cpp
        imgui_impl_glfw.cpp
        imgui_impl_opengl3.cpp
        imgui_widgets.cpp
        )
target_include_directories(imgui INTERFACE include/)

find_package(OpenGL REQUIRED)
target_include_directories(imgui PUBLIC ${OPENGL_INCLUDE_DIRS})
target_link_libraries(imgui PUBLIC ${OPENGL_LIBRARIES})

find_package(glfw3 REQUIRED)
target_include_directories(imgui PUBLIC ${GLFW_INCLUDE_DIRS})
target_link_libraries(imgui PUBLIC glfw)

find_package(glbinding REQUIRED)
target_link_libraries(imgui PUBLIC glbinding)