cmake_minimum_required (VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#setup our app
project (VKLearning)
file(GLOB SOURCES "src/*.cpp")
add_executable(VKLearning ${SOURCES})

#turn this back on for builds without a console window
#set_target_properties(VKLearning PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup /SUBSYSTEM:WINDOWS")

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

#glm
add_subdirectory(lib/glm EXCLUDE_FROM_ALL) #exclude from all as we dont intend to build any of their targets
target_link_libraries(VKLearning glm)