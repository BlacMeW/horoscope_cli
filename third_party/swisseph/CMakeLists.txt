# Swiss Ephemeris Library Configuration
# This file contains the configuration for the embedded Swiss Ephemeris

# Swiss Ephemeris Source Files
set(SWISSEPH_SOURCES
    sweph.c
    swephlib.c
    swehouse.c
    swedate.c
    swejpl.c
    swemplan.c
)

# Swiss Ephemeris Header Files
set(SWISSEPH_HEADERS
    swephexp.h
    sweph.h
    swephlib.h
    swehouse.h
    swedate.h
    swejpl.h
    sweodef.h
    swemptab.h
    swenut2000a.h
)

# Compiler definitions for Swiss Ephemeris
add_definitions(-DSWISSEPH)
add_definitions(-DNO_JPL)
add_definitions(-DNO_MOSHIER)
