cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

# Build type
if (NOT CMAKE_BUILD_TYPE)
	set (CMAKE_BUILD_TYPE "Debug")

endif ()

# Project
project (x4-station-calc)


message (STATUS "Build Type - ${CMAKE_BUILD_TYPE}.")
message (STATUS "Host System - ${CMAKE_HOST_SYSTEM_NAME}.")
message (STATUS "Host Processor - ${CMAKE_HOST_SYSTEM_PROCESSOR}.")

# Environment
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/include")
add_definitions ("-DQIODEVICE_BUFFERSIZE=Q_INT64_C(64 * 1024 * 1024)")

if (WIN32)
	include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/windows")

endif ()

set (OUTPUT_SUB_DIR "${CMAKE_BUILD_TYPE}/${CMAKE_HOST_SYSTEM_NAME}/${CMAKE_HOST_SYSTEM_PROCESSOR}")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY	"${CMAKE_CURRENT_SOURCE_DIR}/lib/${OUTPUT_SUB_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY	"${CMAKE_CURRENT_SOURCE_DIR}/lib/${OUTPUT_SUB_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY	"${CMAKE_CURRENT_SOURCE_DIR}/bin/${OUTPUT_SUB_DIR}")

if (WIN32)
    add_compile_options ("-D_CRT_SECURE_NO_WARNINGS")
	if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
		add_link_options ("/subsystem:windows" "/entry:mainCRTStartup")

	endif ()
endif ()

# Dependencies
find_package (Qt5Core REQUIRED)
find_package (Qt5Widgets REQUIRED)
find_package (Doxygen)

if (NOT DOXYGEN_EXECUTABLE)
    message (STATUS "Doxygen is not found. Code document will not be generated.")
    
endif()

# Sources
file (GLOB_RECURSE HEADERS
	"include/*.h"
	"include/*.hpp"
	)

file (GLOB_RECURSE SRC
	"source/*.c"
	"source/*.cpp"
	"source/*.cc"
	"source/*.C"
	)

if (WIN32)
	file (GLOB_RECURSE WINDOWS_SRC
		"3rd-party/windows/*.c"
		"3rd-party/windows/*.cpp"
		"3rd-party/windows/*.cc"
		"3rd-party/windows/*.C"
		)

	list (APPEND SRC
		${WINDOWS_SRC}
		)

endif ()

# Qt wrappers
qt5_wrap_cpp (WRAPPED_HEADERS ${HEADERS})
qt5_add_resources (WRAPPED_RESOURCE "${CMAKE_CURRENT_SOURCE_DIR}/resource/resources.qrc")

add_executable(${PROJECT_NAME}
	${SRC}
	${WRAPPED_HEADERS}
	${WRAPPED_RESOURCE})

target_link_libraries(${PROJECT_NAME}
	Qt5::Core
	Qt5::Widgets)

#Doc
if (DOXYGEN_EXECUTABLE)
    add_custom_target("doc" ALL
        COMMAND				${DOXYGEN_EXECUTABLE}
        WORKING_DIRECTORY	${CMAKE_CURRENT_SOURCE_DIR})
   
endif ()

