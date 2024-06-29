cmake_minimum_required(VERSION 3.25.0 FATAL_ERROR)

set (CMAKE_CXX_STANDARD 20) 

# Project
project (x4-station-calc C CXX)

message (STATUS "Build Type - ${CMAKE_BUILD_TYPE}.")
message (STATUS "Host System - ${CMAKE_HOST_SYSTEM_NAME}.")
message (STATUS "Host Processor - ${CMAKE_HOST_SYSTEM_PROCESSOR}.")
message (STATUS "Target System - ${CMAKE_SYSTEM_NAME}.")
message (STATUS "Target Processor - ${CMAKE_SYSTEM_PROCESSOR}.")

include ("${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.cmake")
include ("${CMAKE_CURRENT_SOURCE_DIR}/cmake/build_config.cmake")
include ("${CMAKE_CURRENT_SOURCE_DIR}/cmake/configure.cmake")


include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories ("${CMAKE_CURRENT_BINARY_DIR}/include")


# Core.
set (CORE_TARGET_NAME    "x4-station-calc-core")

# Configure file.
configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/include/x4-station-calc/core/configure.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/include/x4-station-calc/core/configure.h"
    NEWLINE_STYLE       UNIX
)

file (GLOB_RECURSE CORE_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/source/core/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/source/core/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/source/core/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/source/core/*.C"
)

add_library (${CORE_TARGET_NAME} OBJECT
    ${CORE_SRC}
)

set_target_properties (${CORE_TARGET_NAME} 
    PROPERTIES 
    VERSION     "${VERSION}"
)

# getopt
if (WIN32)
    set (GETOPT_TARGET_NAME    "getopt")
    file (GLOB_RECURSE GETOPT_SRC
        "${CMAKE_CURRENT_SOURCE_DIR}/source/3rd-party/windows/getopt/*.c"
    )
    add_library (${GETOPT_TARGET_NAME}  OBJECT
        ${GETOPT_SRC}
    )
    target_include_libraries (${GETOPT_TARGET_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include/3rd-party/windows/getopt"
    )
endif ()

# Qt GUI
set (GUI_TARGET_NAME    "x4-station-calc")
file (GLOB_RECURSE QT_GUI_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/source/qt_gui/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/source/qt_gui/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/source/qt_gui/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/source/qt_gui/*.C"
)

file (GLOB_RECURSE QT_GUI_HDR
    "${CMAKE_CURRENT_SOURCE_DIR}/include/x4-station-calc/qt_gui/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/x4-station-calc/qt_gui/*.hpp"
)

if (WIN32)
    file (GLOB_RECURSE WINDOWS_SRC
        "${CMAKE_CURRENT_SOURCE_DIR}/resource/*.rc"
    )

    list (APPEND QT_GUI_SRC
        ${WINDOWS_SRC}
    )
endif ()

add_executable (${GUI_TARGET_NAME}
    ${QT_GUI_SRC}
)

target_link_libraries(${GUI_TARGET_NAME}
    ${CORE_TARGET_NAME}
)


if (WIN32)
    target_include_libraries (${GUI_TARGET_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include/3rd-party/windows/getopt"
    )
    
    target_link_libraries(${GUI_TARGET_NAME}
        ${GETOPT_TARGET_NAME}
        Dbghelp
        shell32
    )

endif ()


#Doc
#if (DOXYGEN_EXECUTABLE)
#    add_custom_target("doc" ALL
#        COMMAND             ${DOXYGEN_EXECUTABLE}
#        WORKING_DIRECTORY   ${CMAKE_CURRENT_SOURCE_DIR})
#   
#endif ()

