# Build type
if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE "Debug")

endif ()

set (CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}"  CACHE STRING "${CMAKE_BUILD_TYPE}" FORCE)

if (GENERATOR_IS_MULTI_CONFIG OR CMAKE_CONFIGURATION_TYPES)
    set (CMAKE_CONFIGURATION_TYPES "${CMAKE_BUILD_TYPE}" CACHE STRING "${CMAKE_BUILD_TYPE}" FORCE)

endif()

# Output path.
if (UNIX)
    execute_process (
        COMMAND             lsb_release -ir
        RESULT_VARIABLE     lsb_result
        OUTPUT_VARIABLE     lsb_output)

    if (lsb_result EQUAL 0)
        string (REGEX REPLACE   ".*Distributor[ \t]+ID:[ \t]+([^\n]+).*"
            "\\1"
            DISTRO_NAME
            "${lsb_output}"
        )
        string (REGEX REPLACE   ".*Release:[ \t]+([^\n]+).*"
            "\\1"
            DISTRO_RELEASE
            "${lsb_output}"
        )
        if (${DISTRO_RELEASE} STREQUAL "rolling")
            set (HOST_DISTRO        "${DISTRO_NAME}" CACHE STRING "Host distro." FORCE)

        else ()
            set (HOST_DISTRO        "${DISTRO_NAME} ${DISTRO_RELEASE}" CACHE STRING "Host distro." FORCE)

        endif ()

    else ()
        set (HOST_DISTRO        "Unknow" CACHE STRING "Host distro." FORCE)

    endif ()

    unset (lsb_result)
    unset (lsb_output)

    if (NOT TARGET_DISTRO)
        set (TARGET_DISTRO      "${HOST_DISTRO}" CACHE STRING "Target distro." FORCE)

    endif ()

    set (OUTPUT_SUB_DIR "${CMAKE_BUILD_TYPE}/${CMAKE_SYSTEM_NAME}/${TARGET_DISTRO}/${CMAKE_SYSTEM_PROCESSOR}")

    message (STATUS "Host Distro - ${HOST_DISTRO}.")
    message (STATUS "Target Distro - ${TARGET_DISTRO}.")

else ()
    set (OUTPUT_SUB_DIR "${CMAKE_BUILD_TYPE}/${CMAKE_SYSTEM_NAME}/${CMAKE_SYSTEM_PROCESSOR}")

endif ()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY  "${CMAKE_CURRENT_SOURCE_DIR}/lib/${OUTPUT_SUB_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY  "${CMAKE_CURRENT_SOURCE_DIR}/lib/${OUTPUT_SUB_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  "${CMAKE_CURRENT_SOURCE_DIR}/bin/${OUTPUT_SUB_DIR}")
