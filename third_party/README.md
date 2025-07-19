# Third Party Libraries

This directory contains third-party libraries used by the Horoscope CLI project.

## Swiss Ephemeris

The Swiss Ephemeris is a high-precision ephemeris developed by Astrodienst. It is used for calculating planetary positions, house cusps, and other astronomical data required for astrological calculations.

### Version
- Source: Swiss Ephemeris Master from swisseph-master
- License: AGPL-3.0 (see third_party/swisseph/LICENSE)

### Files Included
- Core calculation engine (sweph.c, swephlib.c)
- House calculation system (swehouse.c)
- Date/time utilities (swedate.c)
- JPL ephemeris support (swejpl.c)
- Moshier planetary theory (swemplan.c)
- All necessary headers

### Data Files
Ephemeris data files are stored in the `data/` directory:
- Fixed stars catalog (sefstars.txt)
- Sample planetary ephemeris files (.se1 format)

### Integration
The Swiss Ephemeris is compiled directly into the horoscope_cli binary, eliminating the need for external library dependencies. The ephemeris data path is configured at compile time.

### Usage
The Swiss Ephemeris functions are accessed through the EphemerisManager class, which provides a C++ wrapper around the C library functions.

### License Compliance
This project complies with the AGPL-3.0 license under which Swiss Ephemeris is distributed. See the main LICENSE file for details.
