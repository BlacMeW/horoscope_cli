# Ephemeris Data Files

This directory contains the ephemeris data files required by Swiss Ephemeris for astronomical calculations.

## Files

### Fixed Stars
- `sefstars.txt` - Fixed stars catalog with positions and magnitudes

### Planetary Ephemeris
- `sepl_18.se1` - Planetary ephemeris data for the time period around 1800 CE
- `semo_18.se1` - Moon ephemeris data for the time period around 1800 CE
- `seas_18.se1` - Asteroid ephemeris data for the time period around 1800 CE

## Notes

- These are sample files covering a limited time range
- For production use, you may want to download additional ephemeris files from:
  https://www.astro.com/swisseph/
- File naming convention:
  - `sepl_XX.se1` - Planets (Mercury, Venus, Mars, Jupiter, Saturn, Uranus, Neptune, Pluto)
  - `semo_XX.se1` - Moon
  - `seas_XX.se1` - Asteroids and other small bodies
  - XX represents the time period (centuries from J2000.0)

## Usage

The ephemeris data path is automatically configured during build and the files are accessed by the Swiss Ephemeris library functions when calculating planetary positions.
