# Platform options.
# Debug/Release.
# Enable address sanitizer in debug mode if possible.
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    if (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang"
            OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "AppleClang"
            OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
        # Enable address sanitizer.
        add_compile_options (-fno-omit-frame-pointer -fsanitize=address)
        add_link_options (-fno-omit-frame-pointer -fsanitize=address)
        message (STATUS "Address sanitizer is enabled.")

    endif()

endif ()

# Compiler.
if (MSVC)
    # Set compile options.
    if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "AMD64")
        set (CMAKE_C_FLAGS_DEBUG "/DAMD64 /DDEBUG /RTC1 /Od ${C_CXX_FLAGS}")
        set (CMAKE_C_FLAGS_RELEASE "/DAMD64 /DNDEBUG /O2 ${C_CXX_FLAGS}")

    elseif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86")
        set (CMAKE_C_FLAGS_DEBUG "/DDEBUG /RTC1 /Od ${C_CXX_FLAGS} /arch:IA32")
        set (CMAKE_C_FLAGS_RELEASE "/DNDEBUG /O2 ${C_CXX_FLAGS} /arch:IA32")

    else ()
        message (FATAL_ERROR "Unsupported Platform: ${CMAKE_SYSTEM_PROCESSOR}")

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

    if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi")
        set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DEBUG /OPT:REF /OPT:ICF")

    endif ()
else ()
    if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
        set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -g")

    endif ()

endif ()

# Config.
if (WIN32)
    include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/windows")
    set (OS_WINDOWS     ON)

elseif (UNIX)
    set (OS_LINUX       ON)

endif ()
 
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set (DEBUG_MACRO    "DEBUG")

else ()
    set (DEBUG_MACRO    "NDEBUG")

endif ()

