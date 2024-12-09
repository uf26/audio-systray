cmake_minimum_required(VERSION 3.10)

# Project name and version
project(gtk_hello_world VERSION 1.0 LANGUAGES C)

# Enable generation of compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find GTK3 package
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

# Add the executable
add_executable(gtk_hello_world main.c menu.c status_icon.c handle_events.c volume.c)

# Manually add the XApp include directory (adjust the path to where xapp-status-icon.h is located)
target_include_directories(gtk_hello_world PRIVATE 
    ${GTK3_INCLUDE_DIRS}
    /usr/include/xapp/libxapp  # Manually add the XApp header directory
)

# Link the GTK3 and XApp libraries
target_link_libraries(gtk_hello_world PRIVATE 
    ${GTK3_LIBRARIES}
    /usr/lib/libxapp.so  # Manually specify the XApp library
)

# Set compile flags for GTK3
target_compile_options(gtk_hello_world PRIVATE ${GTK3_CFLAGS_OTHER})
