# Solar System ASCII Art Feature

## Overview

The Horoscope CLI now includes a beautiful ASCII art solar system visualization feature that can display:

1. **Static Solar System**: Orbital paths of all planets (no birth data required)
2. **Dynamic Solar System**: Planetary positions based on actual astronomical data for any given date/time
3. **Multiple Perspectives**: View the solar system from different planetary viewpoints

## Perspectives Available

### Heliocentric (Default)
- **Sun-centered view**: Traditional solar system with Sun at center
- All planets orbit around the Sun
- Shows true orbital relationships

### Geocentric 
- **Earth-centered view**: Ancient astronomical perspective
- Earth is at the center with all other bodies orbiting around it
- Includes Moon and Sun as "planets" from Earth's perspective
- Historical and astrological significance

### Planet-Centric Views
- **Mars-Centric**: View from Mars perspective
- **Jupiter-Centric**: View from Jupiter perspective  
- **Saturn-Centric**: View from Saturn perspective
- Shows relative positions as seen from the selected planet
- Unique astronomical perspectives rarely visualized

## Usage

### Show Orbital Paths Only
```bash
./bin/horoscope_cli --solar-system
```

This displays the basic solar system structure with orbital paths and no birth data required.

### Show Planets at Specific Date/Time (Heliocentric)
```bash
./bin/horoscope_cli --date YYYY-MM-DD --time HH:MM:SS --lat LATITUDE --lon LONGITUDE --timezone HOURS --chart-style solar-system
```

### Show Different Perspectives
```bash
./bin/horoscope_cli --date YYYY-MM-DD --time HH:MM:SS --lat LATITUDE --lon LONGITUDE --timezone HOURS --chart-style solar-system --perspective [PERSPECTIVE]
```

Where `[PERSPECTIVE]` can be:
- `heliocentric` (default)
- `geocentric` 
- `mars-centric`
- `jupiter-centric`
- `saturn-centric`

## Examples

### Basic Solar System
```bash
./bin/horoscope_cli --solar-system
```

### Geocentric View - Historical Perspective
```bash
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 --lat 40.7128 --lon -74.0060 --timezone -5 --chart-style solar-system --perspective geocentric
```

### Mars-Centric View - What Would Mars See?
```bash
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 --lat 40.7128 --lon -74.0060 --timezone -5 --chart-style solar-system --perspective mars-centric
```

### Jupiter-Centric View - Giant Planet Perspective
```bash
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 --lat 40.7128 --lon -74.0060 --timezone -5 --chart-style solar-system --perspective jupiter-centric
```

### Historical Event - July 4, 1776 (Independence Day) - Geocentric
```bash
./bin/horoscope_cli --date 1776-07-04 --time 12:00:00 --lat 39.9526 --lon -75.1652 --timezone -5 --chart-style solar-system --perspective geocentric
```

## Features

### Visual Elements
- **Center Bodies**: Different symbols based on perspective (O=Sun, E=Earth, M=Mars, J=Jupiter, S=Saturn)
- **Planets**: Each planet has its own symbol (H=Mercury, V=Venus, M=Mars, J=Jupiter, S=Saturn, U=Uranus, N=Neptune, P=Pluto)
- **Orbital Paths**: Shown as dotted circles using '.' characters
- **Legends**: Clear symbol explanation and perspective information
- **Moon**: Special handling in geocentric view (D=Moon)

### Perspective-Specific Features
- **Heliocentric**: Traditional solar system layout with accurate orbital radii
- **Geocentric**: Earth at center with Moon prominently displayed
- **Planet-Centric**: Selected planet at center with relative positions of all other bodies

### Display Options
- Shows orbital paths with accurate relative spacing
- Displays planet positions based on Swiss Ephemeris calculations
- Includes detailed planet position information (degree, sign, house)
- Perspective-aware legends and titles
- Supports all the same astronomical accuracy as other chart styles

## Educational Value

### Astronomical Concepts
- **Historical Perspectives**: See how ancient astronomers viewed the cosmos
- **Relative Motion**: Understand how planetary positions appear from different vantage points
- **Orbital Mechanics**: Visualize true vs apparent planetary motions

### Astrological Applications
- **Geocentric Astrology**: Traditional Earth-centered astrological calculations
- **Heliocentric Astrology**: Modern Sun-centered perspective
- **Planetary Astrology**: Unique perspectives from other planets

## Technical Details

### Coordinate System
- Uses ecliptic longitude to position planets
- Orbital radii are scaled for ASCII display (not to true scale for visual clarity)
- Positions are calculated using Swiss Ephemeris data
- Relative positioning calculations for planet-centric views

### Planet Coverage
- All traditional planets: Sun, Mercury, Venus, Mars, Jupiter, Saturn
- Outer planets: Uranus, Neptune, Pluto
- Moon: Featured prominently in geocentric view
- Earth: Shown as center reference in geocentric view

### ASCII Canvas
- 80x60 character canvas
- Dynamic center positioning based on perspective
- Circular orbital representation with dots
- Perspective-appropriate legends

## Integration

This feature integrates seamlessly with the existing Horoscope CLI:
- Uses the same Swiss Ephemeris backend
- Follows the same command-line interface patterns
- Compatible with all existing date/time/location options
- Can be combined with other output formats
- New `--perspective` option for selecting viewpoint

## Testing

Run the comprehensive demonstration script to see all features:
```bash
./test_solar_system.sh
```

This will demonstrate:
1. Basic orbital paths
2. Heliocentric view
3. Geocentric view
4. Mars-centric view
5. Jupiter-centric view
6. Saturn-centric view
7. Historical examples with multiple perspectives

## Implementation

The enhanced solar system feature is implemented in:
- `include/solar_system_drawer.h` - Header with perspective enum and method definitions
- `src/solar_system_drawer.cpp` - Implementation with multiple perspective algorithms
- `src/main.cpp` - Command-line integration with perspective option
- Perspective calculation algorithms for relative positioning
- Enhanced legends and display formatting

The implementation uses modern C++17 features and follows the same architectural patterns as other chart drawing classes, with added mathematical calculations for perspective transformations.
