cmake_minimum_required(VERSION 3.20)

project(livedelay VERSION 1.0.0)

# OBS Studio Plugin Template
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Try to find OBS as a package first (installed OBS)
find_package(libobs QUIET)
find_package(obs-frontend-api QUIET)

# If not found, try source directory
if(NOT libobs_FOUND)
    if(NOT DEFINED OBS_SOURCE_DIR)
        set(OBS_SOURCE_DIR "" CACHE PATH "Path to OBS Studio source code")
    endif()

    if(NOT EXISTS "${OBS_SOURCE_DIR}/libobs")
        message(FATAL_ERROR "OBS_SOURCE_DIR is not set correctly. Please set it to the OBS Studio source directory. Current value: ${OBS_SOURCE_DIR}")
    endif()

    # Add OBS build directory to CMAKE_PREFIX_PATH
    list(APPEND CMAKE_PREFIX_PATH "${OBS_SOURCE_DIR}/build")

    # Try finding again with build directory
    find_package(libobs REQUIRED)
    find_package(obs-frontend-api REQUIRED)
endif()

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Plugin source files
set(livedelay_SOURCES
    src/plugin-main.cpp
    src/delay-manager.cpp
    src/ui-components.cpp
)

set(livedelay_HEADERS
    src/delay-manager.h
    src/ui-components.h
)

# Create the plugin
add_library(livedelay MODULE
    ${livedelay_SOURCES}
    ${livedelay_HEADERS}
)

# Link libraries
target_link_libraries(livedelay
    OBS::libobs
    OBS::frontend-api
    Qt6::Core
    Qt6::Widgets
)

# Include directories
target_include_directories(livedelay PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${OBS_SOURCE_DIR}/UI/obs-frontend-api
)

# Set plugin properties
set_target_properties(livedelay PROPERTIES
    FOLDER "plugins"
    VERSION ${PROJECT_VERSION}
)

# Install plugin
install_obs_plugin(livedelay)

# Copy locale files
install(DIRECTORY data/locale/
    DESTINATION "${CMAKE_INSTALL_PREFIX}/data/obs-plugins/livedelay/locale"
)
