cmake_minimum_required(VERSION 2.8.4)
project(clobat)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
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

# include
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${SDL2_INCLUDE_DIRS}
    ${SDL2IMAGE_INCLUDE_DIRS}
)

# link libraries
target_link_libraries(
    clobat
    ${SDL2_LIBRARIES}
    ${SDL2IMAGE_LIBRARIES}
)
