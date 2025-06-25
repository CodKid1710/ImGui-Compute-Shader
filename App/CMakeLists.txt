
# App

set(SOURCE_DIR "Source")

set(SOURCES
    Source/Main.cpp
    Source/Shader.h
    Source/Shader.cpp
    Source/Renderer.h
    Source/Renderer.cpp
    vendor/stb/stb_image.h
    vendor/stb/stb_image.cpp
    vendor/imgui/imgui.h
    vendor/imgui/imgui.cpp
    vendor/imgui/imconfig.h 
    vendor/imgui/imgui_demo.cpp
    vendor/imgui/imgui_draw.cpp 
    vendor/imgui/imgui_internal.h
    vendor/imgui/imgui_tables.cpp 
    vendor/imgui/imgui_widgets.cpp 
    vendor/imgui/imstb_rectpack.h 
    vendor/imgui/imstb_textedit.h 
    vendor/imgui/imstb_truetype.h
    vendor/imgui/backends/imgui_impl_opengl3.cpp 
    vendor/imgui/backends/imgui_impl_opengl3_loader.h 
    vendor/imgui/backends/imgui_impl_opengl3.h 
    vendor/imgui/backends/imgui_impl_glfw.cpp 
    vendor/imgui/backends/imgui_impl_glfw.h 
)

# Add the executable target
add_executable(App ${SOURCES})

target_link_libraries(App glfw)
target_link_libraries(App glad)
target_link_libraries(App glm)

target_include_directories(App PRIVATE vendor/stb)
target_include_directories(App PRIVATE vendor/imgui)
target_include_directories(App PRIVATE vendor/imgui/backends)