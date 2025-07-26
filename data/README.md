# Ephemeris Data Files

This directory contains the ephemeris data files required by Swiss Ephemeris for astronomical calculations.

## Files

### Fixed Stars
- `sefstars.txt` - Fixed stars catalog with positions and magnitudes (134K)

### Planetary Ephemeris Files (473K each)
- `sepl_00.se1` - Planetary ephemeris data for 0 AD (BC/AD boundary era)
- `sepl_06.se1` - Planetary ephemeris data for 600 AD (early medieval period)
- `sepl_12.se1` - Planetary ephemeris data for 1200 AD (medieval period)
- `sepl_18.se1` - Planetary ephemeris data for 1800 AD (modern period)
- `sepl_24.se1` - Planetary ephemeris data for 2400 AD (future period)

### Moon Ephemeris Files (1.3M each)
- `semo_00.se1` - Moon ephemeris data for 0 AD (BC/AD boundary era)
- `semo_06.se1` - Moon ephemeris data for 600 AD (early medieval period)
- `semo_12.se1` - Moon ephemeris data for 1200 AD (medieval period)
- `semo_18.se1` - Moon ephemeris data for 1800 AD (modern period)
- `semo_24.se1` - Moon ephemeris data for 2400 AD (future period)

### Asteroid Ephemeris Files (218-220K each)
- `seas_00.se1` - Asteroid ephemeris data for 0 AD (BC/AD boundary era)
- `seas_06.se1` - Asteroid ephemeris data for 600 AD (early medieval period)
- `seas_12.se1` - Asteroid ephemeris data for 1200 AD (medieval period)
- `seas_18.se1` - Asteroid ephemeris data for 1800 AD (modern period)
- `seas_24.se1` - Asteroid ephemeris data for 2400 AD (future period)

### Special Data Files
- `seasnam.txt` - Asteroid names database (15M)
- `seorbel.txt` - Orbital elements for asteroids and comets (6K)
- `seleapsec.txt` - Leap seconds data for time calculations (282 bytes)

## Time Coverage

**Historical Range**: BC era through 2400 AD
- **Ancient Era**: 6000 BC - 0 AD (requires sepl_00.se1, semo_00.se1, seas_00.se1)
- **Classical Era**: 0 AD - 600 AD (sepl_00.se1 and sepl_06.se1 overlap)
- **Medieval Era**: 600 AD - 1200 AD (sepl_06.se1 and sepl_12.se1)
- **Renaissance-Modern**: 1200 AD - 1800 AD (sepl_12.se1 and sepl_18.se1)
- **Contemporary**: 1800 AD - 2400 AD (sepl_18.se1 and sepl_24.se1)
- **Future**: 2400 AD+ (sepl_24.se1 coverage)

## Historical Calculation Examples

```bash
# Julius Caesar's assassination (44 BC)
./bin/horoscope_cli --date -0044-03-15 --time 12:00:00 \
                    --lat 41.9028 --lon 12.4964 --timezone 1

# Ancient Athens (500 BC)
./bin/horoscope_cli --date -0500-03-15 --time 12:00:00 \
                    --lat 37.9755 --lon 23.7348 --timezone 2

# Medieval Burma/Myanmar (1200 AD)
./bin/horoscope_cli --date 1200-01-01 --time 12:00:00 \
                    --lat 16.8661 --lon 96.1951 --timezone 6.5 \
                    --myanmar-calendar

# Eclipse in ancient Rome (1st century BC)
./bin/horoscope_cli --eclipse-range -0100-01-01 -0001-12-31 \
                    --lat 41.9028 --lon 12.4964
```

## File Naming Convention

- `sepl_XX.se1` - Planets (Mercury, Venus, Mars, Jupiter, Saturn, Uranus, Neptune, Pluto)
- `semo_XX.se1` - Moon
- `seas_XX.se1` - Asteroids and other small bodies
- XX represents centuries from J2000.0:
  - 00 = 0 AD (±600 years, covers BC era)
  - 06 = 600 AD (±600 years)
  - 12 = 1200 AD (±600 years)
  - 18 = 1800 AD (±600 years)
  - 24 = 2400 AD (±600 years)

## Notes

- **Complete Coverage**: These files provide comprehensive astronomical data from ancient times through the far future
- **High Precision**: Swiss Ephemeris calculations maintain arc-second accuracy
- **BC/AD Support**: Seamless calculations across the BC/AD boundary
- **Eclipse Calculations**: Full eclipse visibility calculations for any historical period
- **Source**: Ephemeris files from Swiss Ephemeris master collection
- **Download**: Additional files available from https://www.astro.com/swisseph/

## Usage

The ephemeris data path is automatically configured during build and the files are accessed by the Swiss Ephemeris library functions when calculating planetary positions.
