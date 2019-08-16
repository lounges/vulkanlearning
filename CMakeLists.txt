cmake_minimum_required (VERSION 2.8)

find_package(glfw3 3.3 REQUIRED)

project (VKLearning)
file(GLOB SOURCES "src/*.cpp")
add_executable(VKLearning ${SOURCES})