# Solar System ASCII Art Feature

## Overview

The Horoscope CLI now includes a beautiful ASCII art solar system visualization feature that can display:

1. **Static Solar System**: Orbital paths of all planets (no birth data required)
2. **Dynamic Solar System**: Planetary positions based on actual astronomical data for any given date/time

## Usage

### Show Orbital Paths Only
```bash
./bin/horoscope_cli --solar-system
```

This displays the basic solar system structure with orbital paths and no birth data required.

### Show Planets at Specific Date/Time
```bash
./bin/horoscope_cli --date YYYY-MM-DD --time HH:MM:SS --lat LATITUDE --lon LONGITUDE --timezone HOURS --chart-style solar-system
```

This displays the solar system with planets positioned according to their actual astronomical positions at the specified date and time.

## Examples

### Basic Solar System
```bash
./bin/horoscope_cli --solar-system
```

### Historical Event - July 4, 1776 (Independence Day)
```bash
./bin/horoscope_cli --date 1776-07-04 --time 12:00:00 --lat 39.9526 --lon -75.1652 --timezone -5 --chart-style solar-system
```

### Modern Birth Chart in Solar System View
```bash
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 --lat 40.7128 --lon -74.0060 --timezone -5 --chart-style solar-system
```

## Features

### Visual Elements
- **Sun**: Displayed as 'O' at the center
- **Planets**: Each planet has its own symbol (H=Mercury, V=Venus, M=Mars, J=Jupiter, S=Saturn, U=Uranus, N=Neptune, P=Pluto)
- **Orbital Paths**: Shown as dotted circles using '.' characters
- **Legend**: Clear symbol explanation and planet information

### Display Options
- Shows orbital paths with accurate relative spacing
- Displays planet positions based on Swiss Ephemeris calculations
- Includes detailed planet position information (degree, sign, house)
- Supports all the same astronomical accuracy as other chart styles

## Technical Details

### Coordinate System
- Uses ecliptic longitude to position planets
- Orbital radii are scaled for ASCII display (not to true scale for visual clarity)
- Positions are calculated using Swiss Ephemeris data

### Planet Coverage
- All traditional planets: Sun, Mercury, Venus, Mars, Jupiter, Saturn
- Outer planets: Uranus, Neptune, Pluto
- Moon positioning is omitted as it's Earth-centric (not heliocentric)

### ASCII Canvas
- 80x60 character canvas
- Centered sun position
- Circular orbital representation

## Integration

This feature integrates seamlessly with the existing Horoscope CLI:
- Uses the same Swiss Ephemeris backend
- Follows the same command-line interface patterns
- Compatible with all existing date/time/location options
- Can be combined with other output formats

## Testing

Run the demonstration script to see all features:
```bash
./test_solar_system.sh
```

This will show:
1. Basic orbital paths
2. Planet positions for a sample birth date
3. Historical astronomical positions

## Implementation

The solar system feature is implemented in:
- `include/solar_system_drawer.h` - Header file with class definition
- `src/solar_system_drawer.cpp` - Implementation with ASCII art generation
- Integrated into main.cpp for command-line handling

The implementation uses modern C++17 features and follows the same architectural patterns as other chart drawing classes in the application.
