cmake_minimum_required(VERSION 3.1...3.15)

if(${CMAKE_VERSION} VERSION_LESS 3.15)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
    cmake_policy(VERSION 3.15)
endif()

project(JEngine VERSION 1.0 DESCRIPTION "J-Engine by Joost Thissen" LANGUAGES CXX)

#source files location
file(GLOB_RECURSE SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/src/*.c)
        ${CMAKE_SOURCE_DIR}/src/*.cpp)

#header files location
file(GLOB_RECURSE HEADER_FILES
        ${CMAKE_SOURCE_DIR}/src/*.h)
        ${CMAKE_SOURCE_DIR}/src/*.hpp)

#Include .lib files
link_directories(${CMAKE_SOURCE_DIR}/lib)

#Configure assets header file -- 3d models, textures etc.
configure_file(src/helpers/RootDir.h.in src/helpers/RootDir.h)
include_directories(${CMAKE_BINARY_DIR}/src)

#define executable
add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

#CMAKE_DIR to find external dependencies
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

#opengl
find_package(OpenGL REQUIRED)

#glm
find_package(GLM REQUIRED)
message(STATUS "GLM included at ${GLM_INCLUDE_DIR}")

# GLFW
find_package(GLFW3 REQUIRED)
message(STATUS "Found GLFW3 in ${GLFW3_INCLUDE_DIR}")

# ASSIMP
find_package(ASSIMP REQUIRED)
message(STATUS "Found ASSIMP in ${ASSIMP_INCLUDE_DIR}")

#HEADER ONLY LIBRARIES
# STB_IMAGE
add_library(STB_IMAGE "thirdparty/stb_image.cpp")

# GLAD
add_library(GLAD "thirdparty/glad.c")

#put all libs into a variable
set(LIBS glfw3 opengl32 assimp STB_IMAGE, GLAD)

# Define the include DIRs
include_directories(
	"${CMAKE_SOURCE_DIR}/src"
	"${CMAKE_SOURCE_DIR}/include"
)

# Define the link libraries
target_link_libraries(${PROJECT_NAME} ${LIBS})