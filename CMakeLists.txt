cmake_minimum_required(VERSION 3.10)

project(audio_systray VERSION 1.0 LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find GTK3 package using pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
pkg_check_modules(XAPP REQUIRED xapp)
# --- END ADDITION ---

file(GLOB SOURCES "*.c")
add_executable(audio_systray ${SOURCES})

# --- MODIFY THIS SECTION ---
# Now use the variables provided by pkg_check_modules for XApp
target_include_directories(audio_systray PRIVATE
    ${GTK3_INCLUDE_DIRS}
    ${XAPP_INCLUDE_DIRS} # Use pkg-config variable for XApp includes
)

# Link the GTK3, XApp, and PulseAudio libraries
target_link_libraries(audio_systray PRIVATE
    ${GTK3_LIBRARIES}
    ${XAPP_LIBRARIES} 
    pulse
    pulse-mainloop-glib
    m
    notify
    X11
)
# --- END MODIFICATION ---

# Set compile flags for GTK3 and C
target_compile_options(audio_systray PRIVATE ${GTK3_CFLAGS_OTHER})
