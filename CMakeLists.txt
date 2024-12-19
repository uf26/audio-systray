cmake_minimum_required(VERSION 3.10)

# Project name and version
project(audio_systray VERSION 1.0 LANGUAGES C)  # Change C++ (CXX) to C

# Enable generation of compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set the build type to Debug (this enables debug symbols by default)
set(CMAKE_BUILD_TYPE Debug)

# Find GTK3 package using pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

# Get all .c files in the current directory (and subdirectories if needed)
file(GLOB SOURCES "*.c")  # Change *.cpp to *.c for C source files

# Add the executable with all .c files
add_executable(audio_systray ${SOURCES})

# Manually add the XApp include directory (adjust the path to where xapp-status-icon.h is located)
target_include_directories(audio_systray PRIVATE 
    ${GTK3_INCLUDE_DIRS}
    /usr/include/xapp/libxapp   # Manually add the XApp header directory
)

# Link the GTK3, XApp, and PulseAudio libraries
target_link_libraries(audio_systray PRIVATE 
    ${GTK3_LIBRARIES}
    /usr/lib/libxapp.so      # Manually specify the XApp library
    pulse                     # Link with libpulse (PulseAudio)
    m
    notify
    X11
)

# Set compile flags for GTK3 and C
target_compile_options(audio_systray PRIVATE ${GTK3_CFLAGS_OTHER})

# Add debug-specific compile flags (optional, CMake will usually do this when build type is Debug)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(audio_systray PRIVATE -g)  # Add the -g flag for debug symbols
endif()
