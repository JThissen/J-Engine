# PROJECT
cmake_minimum_required(VERSION 3.1...3.15)

if(${CMAKE_VERSION} VERSION_LESS 3.15)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
    cmake_policy(VERSION 3.15)
endif()

project(JEngine VERSION 1.0 DESCRIPTION "J-Engine by Joost Thissen -- Github.com/JThissen/JEngine")

# LINKER DIRECTORIES
link_directories("${CMAKE_SOURCE_DIR}/lib" "${CMAKE_SOURCE_DIR}/lib/assimp")

# FIND ALL CPP FILES
file(GLOB_RECURSE SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/src/*.c
        ${CMAKE_SOURCE_DIR}/src/*.cpp)

# FIND ALL HEADER FILES
file(GLOB_RECURSE HEADER_FILES
        ${CMAKE_SOURCE_DIR}/src/*.h
        ${CMAKE_SOURCE_DIR}/src/*.hpp)

# COPY ASSIMP32.DLL TO BUILD
if(WIN32)
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".dll")
    find_library(
        ASSIMP_DLL
        NAMES "Assimp32.dll"
        PATHS
        "${CMAKE_SOURCE_DIR}/lib/assimp"
    )
    set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
    file(COPY ${ASSIMP_DLL} DESTINATION ${EXECUTABLE_OUTPUT_PATH})
endif(WIN32)

# COPY ASSETS - SHADERS & IMAGES
set(SHADERFILES "${CMAKE_SOURCE_DIR}/shaderfiles")
file(COPY ${SHADERFILES} DESTINATION ${EXECUTABLE_OUTPUT_PATH})
set(IMAGES "${CMAKE_SOURCE_DIR}/images")
file(COPY ${IMAGES} DESTINATION ${EXECUTABLE_OUTPUT_PATH})

# DEFINE EXECUTABLE
add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

# DIRECTORY TO FIND EXTERNAL DEPENDENCIES
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# ADD 3RD PARTY LIBRARIES
find_package(OpenGL REQUIRED)

find_package(glm REQUIRED)
message(STATUS "GLM included at ${GLM_INCLUDE_DIR}")

find_package(GLFW3 REQUIRED)
message(STATUS "Found GLFW3 in ${GLFW3_INCLUDE_DIR}")

find_package(ASSIMP REQUIRED)
message(STATUS "Found ASSIMP in ${ASSIMP_INCLUDE_DIR}")

# HEADER ONLY 3RD PARTY LIBRARIES
add_library(STB_IMAGE "${CMAKE_SOURCE_DIR}/thirdparty/stb_image.cpp")
add_library(GLAD "${CMAKE_SOURCE_DIR}/thirdparty/glad.c")

set(IMGUI_LIB_NAME "imgui")
set(IMGUI_SOURCES
    "${CMAKE_SOURCE_DIR}/thirdparty/imgui.cpp"
    "${CMAKE_SOURCE_DIR}/thirdparty/imgui_demo.cpp"
    "${CMAKE_SOURCE_DIR}/thirdparty/imgui_draw.cpp"
    "${CMAKE_SOURCE_DIR}/thirdparty/imgui_widgets.cpp"
    "${CMAKE_SOURCE_DIR}/thirdparty/imgui_impl_glfw.cpp"
    "${CMAKE_SOURCE_DIR}/thirdparty/imgui_impl_opengl3.cpp"
)
set(IMGUI_HEADERS
    "${CMAKE_SOURCE_DIR}/include/${IMGUI_LIB_NAME}/imconfig.h"
    "${CMAKE_SOURCE_DIR}/include/${IMGUI_LIB_NAME}/imgui.h"
    "${CMAKE_SOURCE_DIR}/include/${IMGUI_LIB_NAME}/imgui_internal.h"
    "${CMAKE_SOURCE_DIR}/include/${IMGUI_LIB_NAME}/imstb_rectpack.h"
    "${CMAKE_SOURCE_DIR}/include/${IMGUI_LIB_NAME}/imstb_textedit.h"
    "${CMAKE_SOURCE_DIR}/include/${IMGUI_LIB_NAME}/imstb_truetype.h"
    "${CMAKE_SOURCE_DIR}/include/${IMGUI_LIB_NAME}/imgui_impl_glfw.h"
    "${CMAKE_SOURCE_DIR}/include/${IMGUI_LIB_NAME}/imgui_impl_opengl3.h"
)
add_library(${IMGUI_LIB_NAME}
    STATIC
        ${IMGUI_SOURCES}
        ${IMGUI_HEADERS}
)

# PUT ALL LIBRARIES IN A VARIABLE
set(LIBS glfw3 opengl32 assimp STB_IMAGE GLAD ${IMGUI_LIB_NAME})

# DEFINE INCLUDE DIRECTORIES
include_directories(
	"${CMAKE_SOURCE_DIR}/src"
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/include/imgui"
    "${CMAKE_SOURCE_DIR}/thirdparty"
)

# DEFINE LINKER LIBRARIES
target_link_libraries(${PROJECT_NAME} ${LIBS})