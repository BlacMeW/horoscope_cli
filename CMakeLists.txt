cmake_minimum_required(VERSION 3.10)
project(HoroscopeCLI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(include)
include_directories(third_party/swisseph)

# Swiss Ephemeris source files
set(SWISSEPH_SOURCES
    third_party/swisseph/sweph.c
    third_party/swisseph/swephlib.c
    third_party/swisseph/swehouse.c
    third_party/swisseph/swedate.c
    third_party/swisseph/swejpl.c
    third_party/swisseph/swemplan.c
    third_party/swisseph/swemmoon.c
)

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
    src/solar_system_drawer.cpp
    src/kp_system.cpp
    ${SWISSEPH_SOURCES}
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
    include/solar_system_drawer.h
    include/kp_system.h
)

# Create executable
add_executable(horoscope_cli ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(horoscope_cli m)

# Set ephemeris data path
target_compile_definitions(horoscope_cli PRIVATE SE_EPHE_PATH="${CMAKE_CURRENT_SOURCE_DIR}/data")

# Installation
install(TARGETS horoscope_cli DESTINATION bin)
install(DIRECTORY data/ DESTINATION share/horoscope_cli/data)
