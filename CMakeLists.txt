cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

set (CMAKE_CXX_STANDARD 17) 

# Project
project (x4-station-calc)

# Build type
if (NOT CMAKE_BUILD_TYPE)
	set (CMAKE_BUILD_TYPE "Debug")

endif ()

set (CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}"  CACHE STRING "${CMAKE_BUILD_TYPE}" FORCE)

if (GENERATOR_IS_MULTI_CONFIG OR CMAKE_CONFIGURATION_TYPES)
    set (CMAKE_CONFIGURATION_TYPES "${CMAKE_BUILD_TYPE}" CACHE STRING "${CMAKE_BUILD_TYPE}" FORCE)

endif()

message (STATUS "Build Type - ${CMAKE_BUILD_TYPE}.")
message (STATUS "Host System - ${CMAKE_HOST_SYSTEM_NAME}.")
message (STATUS "Host Processor - ${CMAKE_HOST_SYSTEM_PROCESSOR}.")

# Environment.
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/include")
add_definitions ("-DQIODEVICE_BUFFERSIZE=Q_INT64_C(64 * 1024 * 1024)")

if (WIN32)
	include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/windows")

endif ()

# Output path.
set (OUTPUT_SUB_DIR "${CMAKE_BUILD_TYPE}/${CMAKE_HOST_SYSTEM_NAME}/${CMAKE_HOST_SYSTEM_PROCESSOR}")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY	"${CMAKE_CURRENT_SOURCE_DIR}/lib/${OUTPUT_SUB_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY	"${CMAKE_CURRENT_SOURCE_DIR}/lib/${OUTPUT_SUB_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY	"${CMAKE_CURRENT_SOURCE_DIR}/bin/${OUTPUT_SUB_DIR}")

# Platform options.
# Debug/Release.
# Enable address sanitizer in debug mode if possible.
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"
			OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang"
			OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		# Enable address sanitizer.
		add_compile_options (-fno-omit-frame-pointer -fsanitize=address)
		add_link_options (-fno-omit-frame-pointer -fsanitize=address)
		message (STATUS "Address sanitizer is enabled.")

	endif()

endif ()

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    add_compile_options ("-DQT_NO_DEBUG")

endif ()

# Compiler.
if (MSVC)
    # Set compile options.
    if (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "AMD64")
        set (CMAKE_C_FLAGS_DEBUG "/DAMD64 /DDEBUG /RTC1 /Od ${C_CXX_FLAGS}")
        set (CMAKE_C_FLAGS_RELEASE "/DAMD64 /DNDEBUG /O2 ${C_CXX_FLAGS}")

    elseif (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "x86")
        set (CMAKE_C_FLAGS_DEBUG "/DDEBUG /RTC1 /Od ${C_CXX_FLAGS} /arch:IA32")
        set (CMAKE_C_FLAGS_RELEASE "/DNDEBUG /O2 ${C_CXX_FLAGS} /arch:IA32")

    else ()
        message (FATAL_ERROR "Unsupported Platform: ${CMAKE_HOST_SYSTEM_PROCESSOR}")

    endif ()

    # Output directory.
    set (MSVC_CONFIG_TYPE ${CMAKE_BUILD_TYPE})
    string (TOUPPER ${CMAKE_BUILD_TYPE} OUTPUTCONFIG)
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${CMAKE_CURRENT_SOURCE_DIR}/bin/${OUTPUT_SUB_DIR}")
    set (CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${CMAKE_CURRENT_SOURCE_DIR}/lib/${OUTPUT_SUB_DIR}")
    set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${CMAKE_CURRENT_SOURCE_DIR}/lib/${OUTPUT_SUB_DIR}")

    # Ignore warnings of functions like strcpy(), strcmp()...
    add_compile_options ("-D_CRT_SECURE_NO_WARNINGS")
    if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
		add_link_options ("/subsystem:console" "/entry:mainCRTStartup")

    elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
		add_link_options ("/subsystem:windows" "/entry:mainCRTStartup")

	endif ()
endif ()

# Dependencies
find_package (PythonInterp REQUIRED 3)
find_package (Qt5Core REQUIRED)
find_package (Qt5Widgets REQUIRED)
find_package (Qt5Network REQUIRED)

if ("${Qt5Core_VERSION}" VERSION_LESS "5.14.1")
    message (FATAL_ERROR "The version of Qt5 must be greater than 5.14.1.")

endif ()

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
        "${CMAKE_CURRENT_SOURCE_DIR}/resource/*.rc"
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
    DEPENDS		${RESOURCES} "${CMAKE_CURRENT_SOURCE_DIR}/CHANGELOG" "${CMAKE_CURRENT_SOURCE_DIR}/generate_resource.py")

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

