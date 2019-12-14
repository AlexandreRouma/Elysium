cmake_minimum_required(VERSION 3.9)
project(Elysium)

include_directories(Elysium "include")
include_directories(Elysium "src")

file(GLOB SRCS "src/*.cpp")
file(GLOB LIBS "include/*.cpp")
file(GLOB RT "include/raytracing/*.cpp")
file(GLOB IMGUI "include/imgui/*.cpp")

add_executable(Elysium ${SRCS} ${LIBS} ${RT} ${IMGUI})

find_package(OpenMP)

if(OpenMP_CXX_FOUND)
    target_link_libraries(Elysium OpenMP::OpenMP_CXX)
endif()

target_link_libraries(Elysium "../lib/glew32")
target_link_libraries(Elysium "../lib/glfw3")
target_link_libraries(Elysium "../lib/OpenImageDenoise")
target_link_libraries(Elysium "../lib/tbb")
target_link_libraries(Elysium "../lib/tbbmalloc")
target_link_libraries(Elysium "opengl32")