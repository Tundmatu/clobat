cmake_minimum_required(VERSION 2.8.4)
project(clobat)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# local sources
aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/src" SOURCE_FILES)

# external dependencies to compile
aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/vendor" SOURCE_FILES)

add_executable(
    clobat
    ${SOURCE_FILES}
)

include(FindPkgConfig)

# look for required libraries
pkg_search_module(SDL2 REQUIRED sdl2)
pkg_search_module(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)

find_package(GLM REQUIRED)

find_package(OpenGL REQUIRED)

# include
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${GLM_INCLUDE_DIRS}
    ${SDL2_INCLUDE_DIRS}
    ${SDL2IMAGE_INCLUDE_DIRS}
)

# link libraries
target_link_libraries(
    clobat
    ${OPENGL_LIBRARIES}
    ${SDL2_LIBRARIES}
    ${SDL2IMAGE_LIBRARIES}
)
