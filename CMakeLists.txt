cmake_minimum_required(VERSION 3.10)

# Project name and version
project(audio_systray VERSION 1.0 LANGUAGES CXX)

# Enable generation of compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find GTK3 package using pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

# Get all .cpp files in the current directory (and subdirectories if needed)
file(GLOB SOURCES "*.cpp")

# Add the executable with all .cpp files
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
)

# Set compile flags for GTK3 and C++
target_compile_options(audio_systray PRIVATE ${GTK3_CFLAGS_OTHER})

# Enable C++11 or later, if desired
target_compile_features(audio_systray PRIVATE cxx_std_11)  # Enable C++11 features (or change to cxx_std_14 for C++14, etc.)
