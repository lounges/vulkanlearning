#setup
cmake_minimum_required(VERSION 3.12)
project(clion_vulkan)
set(CMAKE_CXX_STANDARD 17)

add_executable(clion_vulkan main.cpp)

#vulkan
find_package(Vulkan REQUIRED)
target_link_libraries(clion_vulkan Vulkan::Vulkan)

#glfw
find_package(glfw3 REQUIRED)
target_link_libraries(clion_vulkan glfw3)

#glm
find_package(glm REQUIRED)

#copy compiled shaders
add_custom_command(TARGET clion_vulkan PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:clion_vulkan>/shaders)