cmake_minimum_required (VERSION 3.14)

#setup our app
project (VKLearning)
file(GLOB SOURCES "src/*.cpp")
add_executable(VKLearning ${SOURCES})

#setup vulkan
#https://cmake.org/cmake/help/v3.7/module/FindVulkan.html
find_package(Vulkan REQUIRED)
include_directories(${Vulkan_INCLUDE_DIRS})
target_link_libraries(VKLearning ${Vulkan_LIBRARIES})

#setup glfw
#https://www.glfw.org/docs/latest/build_guide.html#build_link_cmake_source
#note, this repo has been submoduled in
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(lib/glfw)
target_link_libraries(VKLearning glfw)