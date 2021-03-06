cmake_minimum_required(VERSION 2.6)
project(all)

set( CMAKE_POSITION_INDEPENDENT_CODE ON )

# cmake_policy(SET CMP0011 NEW)
# set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake/modules")
# include(cotire)

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

find_package(Doxygen)
if(DOXYGEN_FOUND)
	file(GLOB_RECURSE DOXYGEN_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/core/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/core/*.h)
	file(GLOB_RECURSE DOXYGEN_INPUT ${DOXYGEN_INPUT} ${CMAKE_CURRENT_SOURCE_DIR}/tools/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/tools/*.h)
	# file(GLOB_RECURSE DOXYGEN_INPUT ${DOXYGEN_INPUT} ${CMAKE_CURRENT_SOURCE_DIR}/game/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/game/*.h)
	string(REPLACE ".cpp" ".cpp " DOXYGEN_INPUT ${DOXYGEN_INPUT})
	string(REPLACE ".h" ".h " DOXYGEN_INPUT ${DOXYGEN_INPUT})
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
	add_custom_target(docs
		${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Generating API documentation with Doxygen" VERBATIM
		)
endif(DOXYGEN_FOUND)

# Make sure there is no in source build
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(SEND_ERROR "In-source builds are not allowed.")
endif ()

set_property(GLOBAL PROPERTY CXX_STANDARD 14)
set(CXX_STANDARD_REQUIRED ON)

set(EXTRA_FLAGS "-Wall -Wextra -Wno-unknown-pragmas -fpermissive -fstack-protector")

# If compiling for windows statically link everything
if (${CMAKE_SYSTEM_NAME} MATCHES Window)
	set(EXTRA_FLAGS "${EXTRA_FLAGS} -static-libgcc -static-libstdc++ -static -DGLEW_STATIC")
endif()
message("System name: ${CMAKE_SYSTEM_NAME}")

# NOTE: Build flags by default are set to the debug flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG} ${EXTRA_FLAGS}")
# message("Default flags: ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${EXTRA_FLAGS}")
# message("Debug flags: ${CMAKE_CXX_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${EXTRA_FLAGS}")
# message("Release flags: ${CMAKE_CXX_FLAGS_RELEASE}")

message("Build type: ${CMAKE_BUILD_TYPE}")

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build/${CMAKE_SYSTEM_NAME}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/${CMAKE_SYSTEM_NAME}/lib)
 
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
 
include_directories(${PROJECT_INCLUDE_DIR})
include_directories(${PROJECT_SOURCE_DIR})

if (WIN32)
  set(CMAKE_SHARED_LIBRARY_PREFIX "")
endif ()

# Libraries
include_directories(${PROJECT_SOURCE_DIR}/libs/cmdline)
include_directories(${PROJECT_SOURCE_DIR}/libs/glm)
add_subdirectory(libs/imgui)
add_subdirectory(libs/zlib-ng EXCLUDE_FROM_ALL)
add_subdirectory(libs/tinyobjloader)
include_directories(${PROJECT_SOURCE_DIR}/libs/tinyobjloader)
add_subdirectory(libs/xxHash/cmake_unofficial)
include_directories(${PROJECT_SOURCE_DIR}/libs/xxHash)

# Core engine components
add_subdirectory(core/extra)
add_subdirectory(core/flux)
add_subdirectory(core/fuse)
add_subdirectory(core/flare)

# Demo game
add_subdirectory(game)

# Tools
add_subdirectory(tools/fluxuate)

# Plugins for fluxuate
add_subdirectory(tools/plugins/png)
add_subdirectory(tools/plugins/obj)
add_subdirectory(tools/plugins/glsl)

if(UNIX)
	add_custom_target(assets
		COMMAND ./fluxuate -d ${CMAKE_SOURCE_DIR}/game/assets -p demo
		COMMAND ./fluxuate -d ${CMAKE_SOURCE_DIR}/core/flare/assets -p core
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

	add_custom_target(run
		COMMAND ./game
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

	add_custom_target(debug
		COMMAND gdb ./game -ex run -ex bt -ex quit --silent
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

	add_custom_target(dist
		COMMAND mkdir -p ${CMAKE_SOURCE_DIR}/dist
		COMMAND tar -cvf ${CMAKE_SOURCE_DIR}/dist/release-${CMAKE_SYSTEM_NAME}.tar game core.flx demo.flx
		COMMAND gzip ${CMAKE_SOURCE_DIR}/dist/release-${CMAKE_SYSTEM_NAME}.tar
		DEPENDS assets
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endif(UNIX)
if(WIN32)
	add_custom_target(assets
		COMMAND wine fluxuate.exe -d ${CMAKE_SOURCE_DIR}/game/assets -p demo
		COMMAND wine fluxuate.exe -d ${CMAKE_SOURCE_DIR}/core/flare/assets -p core
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

	add_custom_target(run
		COMMAND wine game.exe
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

	add_custom_target(debug
		COMMAND wine game.exe
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

	add_custom_target(dist
		COMMAND mkdir -p ${CMAKE_SOURCE_DIR}/dist
		COMMAND zip ${CMAKE_SOURCE_DIR}/dist/release-${CMAKE_SYSTEM_NAME} game.exe core.flx demo.flx
		DEPENDS assets
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endif(WIN32)

add_custom_target(distclean @echo cleaning for source distribution)
	set(DISTCLEANED
		${CMAKE_SOURCE_DIR}/build
		*
		)

add_custom_command(
    DEPENDS clean
    COMMENT "distribution clean"
    COMMAND rm
    ARGS	-rf ${DISTCLEANED}
    TARGET  distclean
    )
