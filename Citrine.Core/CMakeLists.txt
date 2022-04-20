cmake_minimum_required(VERSION 3.21)
project(Citrine)

set(CMAKE_CXX_STANDARD 23)

link_libraries(-lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi)

add_executable(Citrine main.cpp src/renderer/glfw/Window.cpp src/renderer/glfw/Window.h src/renderer/vk/VkWindow.cpp src/renderer/vk/VkWindow.h src/renderer/vk/VkHelper.h src/renderer/vk/GraphicsPipeline.cpp src/renderer/vk/GraphicsPipeline.h src/renderer/vk/RenderPass.cpp src/renderer/vk/RenderPass.h)

add_custom_command(
        TARGET Citrine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders
        ${CMAKE_CURRENT_BINARY_DIR}/shaders
)