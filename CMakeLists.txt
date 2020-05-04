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
find_package (PythonInterp REQUIRED 3)
find_package (Qt5Core REQUIRED)
find_package (Qt5Widgets REQUIRED)
find_package (Qt5Network REQUIRED)
find_package (Doxygen)

if (NOT DOXYGEN_EXECUTABLE)
    message (STATUS "Doxygen is not found. Code document will not be generated.")
    
endif()

# Sources
file (GLOB_RECURSE HEADERS
	"${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
	)

file (GLOB_RECURSE SRC
	"${CMAKE_CURRENT_SOURCE_DIR}/source/*.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/*.cc"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/*.C"
	)

if (WIN32)
	file (GLOB_RECURSE WINDOWS_SRC
		"${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/windows/*.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/windows/*.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/windows/*.cc"
		"${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/windows/*.C"
		)

	list (APPEND SRC
		${WINDOWS_SRC}
		)

endif ()

# Recources
set (RESOURCE_LIST_FILE		"${CMAKE_CURRENT_SOURCE_DIR}/resource/resources.qrc")
set (GENERATE_RESOURCE_CMD	"${PYTHON_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/generate_resource.py")

file (GLOB_RECURSE RESOURCES
	"${CMAKE_CURRENT_SOURCE_DIR}/resource/*"
	)
list (REMOVE_ITEM RESOURCES		${RESOURCE_LIST_FILE})

add_custom_command (
	OUTPUT		${RESOURCE_LIST_FILE}
	COMMAND		${GENERATE_RESOURCE_CMD} ${RESOURCES} -r "${CMAKE_CURRENT_SOURCE_DIR}/resource" -o "${RESOURCE_LIST_FILE}"
	DEPENDS		${RESOURCES})

# Qt wrappers
qt5_wrap_cpp (WRAPPED_HEADERS ${HEADERS})
qt5_add_resources (WRAPPED_RESOURCE "${RESOURCE_LIST_FILE}")

add_executable(${PROJECT_NAME}
	${SRC}
	${WRAPPED_HEADERS}
	${WRAPPED_RESOURCE})

target_link_libraries(${PROJECT_NAME}
	Qt5::Core
	Qt5::Widgets
	Qt5::Network
	)

#Doc
if (DOXYGEN_EXECUTABLE)
    add_custom_target("doc" ALL
        COMMAND				${DOXYGEN_EXECUTABLE}
        WORKING_DIRECTORY	${CMAKE_CURRENT_SOURCE_DIR})
   
endif ()

