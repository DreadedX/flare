cmake_minimum_required(VERSION 2.6)
project(flux)

set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include
    ${extra_INCLUDE_DIRS}
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

include_directories(${${PROJECT_NAME}_INCLUDE_DIRS})

file(GLOB_RECURSE CPP_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_library(${PROJECT_NAME} STATIC ${CPP_FILES})
