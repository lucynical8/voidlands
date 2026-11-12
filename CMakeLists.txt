cmake_minimum_required(VERSION 3.10)
project(voidlands)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -w")

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

add_executable(voidlands
    include/glad/glad.c
    source/main.cpp
)

target_include_directories(voidlands PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/source
)

target_link_libraries(voidlands PRIVATE
    OpenGL::GL
    glm::glm
    glfw
)
