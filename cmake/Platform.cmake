set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PLATFORM_FOREIGN_ENV FALSE)

if(WIN32)
    set(PLATFORM_WINDOWS TRUE)
else()
    set(PLATFORM_WINDOWS FALSE)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(PLATFORM_WINDOWS TRUE)
    set(PLATFORM_FOREIGN_ENV TRUE)
else()
    set(CMAKE_CXX_FLAGS "-pipe ${CMAKE_CXX_FLAGS}")
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -Wall")
    set(CMAKE_CXX_FLAGS_RELEASE "-O2")
    set(CMAKE_CXX_FLAGS_RELWITHDBGINFO "${CMAKE_CXX_FLAGS_RELEASE} -g")

    if(MINGW OR MSYS)
        set(PLATFORM_WINDOWS TRUE)
        string(APPEND CMAKE_CXX_FLAGS " -mwin32 -Wl,-subsystem,windows")
    endif()

    if(CYGWIN)
        set(PLATFORM_FOREIGN_ENV TRUE)
    endif()
endif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "${BoldGreen}Assuming this is a release build. ${BoldYellow}-DCMAKE_BUILD_TYPE=Debug${Green} otherwise.${ColourReset}")
    set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)

if(PLATFORM_WINDOWS)
    message(STATUS "Compiling for Windows - good luck, commander.
    Remember to track down and bundle any DLLs you may need.")
    set(PLATFORM_TYPE win32)
else()
    set(PLATFORM_TYPE linux)
endif()

if(PLATFORM_FOREIGN_ENV)
    message(WARNING "${Yellow}This is an unfamiliar build environment.${ColourReset} Tread carefully and expect failure.
    If you manage to get it working I'd love to hear about it.")
endif()
