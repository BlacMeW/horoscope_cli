cmake_minimum_required(VERSION 3.10)
project(HoroscopeCLI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)

# Swiss Ephemeris library
find_library(SWISSEPH_LIB swe REQUIRED)
if(NOT SWISSEPH_LIB)
    message(FATAL_ERROR "Swiss Ephemeris library not found. Please install libswe-dev")
endif()

# Include directories
include_directories(include)
include_directories(/usr/include/swisseph)

# Source files
set(SOURCES
    src/main.cpp
    src/horoscope_calculator.cpp
    src/planet_calculator.cpp
    src/house_calculator.cpp
    src/aspect_calculator.cpp
    src/birth_chart.cpp
    src/ephemeris_manager.cpp
    src/astro_types.cpp
    src/eastern_chart_drawer.cpp
)

# Headers
set(HEADERS
    include/horoscope_calculator.h
    include/planet_calculator.h
    include/house_calculator.h
    include/aspect_calculator.h
    include/birth_chart.h
    include/ephemeris_manager.h
    include/astro_types.h
    include/eastern_chart_drawer.h
)

# Create executable
add_executable(horoscope_cli ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(horoscope_cli ${SWISSEPH_LIB} m)

# Installation
install(TARGETS horoscope_cli DESTINATION bin)
