# Horoscope CLI Manual

**Version:** 1.0.0
**Built with:** Swiss Ephemeris
**Copyright:** © 2025

## Table of Contents

1. [Overview](#overview)
2. [Installation](#installation)
3. [Basic Usage](#basic-usage)
4. [Command Line Arguments](#command-line-arguments)
5. [Chart Types](#chart-types)
6. [Advanced Features](#advanced-features)
7. [Examples](#examples)
8. [Error Handling](#error-handling)
9. [Technical Details](#technical-details)
10. [Troubleshooting](#troubleshooting)

## Overview

Horoscope CLI is a professional-grade astronomical calculation tool that generates birth charts, calculates planetary positions, finds eclipses and conjunctions, and provides comprehensive astrological analysis using the Swiss Ephemeris library. It supports multiple house systems, chart styles, and output formats.

### Key Features

- **Accurate Calculations**: Uses Swiss Ephemeris for precise astronomical data
- **Multiple House Systems**: Placidus, Koch, Equal, Whole Sign, Campanus, Regiomontanus
- **Various Chart Styles**: Western, North Indian, East Indian, South Indian, Solar System
- **Eclipse & Conjunction Analysis**: Real-time astronomical event calculations
- **KP System**: Complete Krishnamurti Paddhati with 5-level sub-lord analysis
- **Ancient Date Support**: Calculate charts for historical dates (including BC era)
- **Multiple Output Formats**: Text, JSON, CSV
- **Ephemeris Tables**: Generate astronomical data tables

## Installation

### Prerequisites

- Linux/Unix system
- C++ compiler with C++17 support
- Swiss Ephemeris data files

### Build from Source

```bash
git clone <repository-url>
cd horoscope_cli
make
```

The compiled binary will be available at `bin/horoscope_cli`.

### Swiss Ephemeris Data

The application requires Swiss Ephemeris data files. Default path is `./data/`. You can specify a custom path using `--ephe-path`.

## Basic Usage

### Minimum Required Arguments

To generate a basic birth chart, you need:

```bash
./bin/horoscope_cli --date YYYY-MM-DD --time HH:MM:SS --lat LATITUDE --lon LONGITUDE --timezone HOURS
```

### Simple Example

```bash
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 --lat 40.7128 --lon -74.0060 --timezone -5
```

This generates a Western-style birth chart for someone born in New York on January 15, 1990, at 2:30 PM EST.

## Command Line Arguments

### Required Arguments

| Argument | Description | Format | Range |
|----------|-------------|--------|-------|
| `--date` | Birth date | YYYY-MM-DD | Any valid date (use -YYYY for BC) |
| `--time` | Birth time | HH:MM:SS | 24-hour format |
| `--lat` | Latitude | Decimal degrees | -90 to 90 |
| `--lon` | Longitude | Decimal degrees | -180 to 180 |
| `--timezone` | Timezone offset | Hours from UTC | -12 to +14 |

### Chart Options

| Argument | Description | Values | Default |
|----------|-------------|--------|---------|
| `--house-system` | House system | P, K, E, W, C, R | P (Placidus) |
| `--output` | Output format | text, json | text |
| `--chart-style` | Chart style | western, north-indian, south-indian, east-indian, solar-system | western |
| `--perspective` | Solar system perspective | heliocentric, geocentric, mars-centric, jupiter-centric, saturn-centric | heliocentric |

### House Systems

- **P** = Placidus (default)
- **K** = Koch
- **E** = Equal House
- **W** = Whole Sign
- **C** = Campanus
- **R** = Regiomontanus

### Eclipse Options

| Argument | Description | Format |
|----------|-------------|--------|
| `--eclipses` | Show eclipses near birth date | Flag |
| `--eclipse-range FROM TO` | Eclipse date range | YYYY-MM-DD YYYY-MM-DD |
| `--eclipse-years-before N` | Years before birth | Integer (default: 1) |
| `--eclipse-years-after N` | Years after birth | Integer (default: 1) |

### Conjunction Options

| Argument | Description | Format |
|----------|-------------|--------|
| `--conjunctions` | Show conjunctions near birth | Flag |
| `--conjunction-range FROM TO` | Conjunction date range | YYYY-MM-DD YYYY-MM-DD |
| `--conjunction-orb DEGREES` | Maximum orb | Decimal (default: 3.0) |

### Ephemeris Options

| Argument | Description | Format |
|----------|-------------|--------|
| `--ephemeris` | Generate ephemeris table | Flag |
| `--ephemeris-range FROM TO` | Date range | YYYY-MM-DD YYYY-MM-DD |
| `--ephemeris-interval DAYS` | Interval between entries | Integer (default: 1) |
| `--ephemeris-format FORMAT` | Output format | table, csv, json (default: table) |

### KP System Options

| Argument | Description | Format |
|----------|-------------|--------|
| `--kp-table` | Show KP Sub Lord 5 Levels | Flag |
| `--kp-transitions` | Show KP transitions | Flag |
| `--kp-transition-range FROM TO` | KP transition date range | YYYY-MM-DD YYYY-MM-DD |
| `--kp-transition-planet PLANET` | Specific planet | SUN, MOON, MERCURY, etc. |
| `--kp-transition-level LEVEL` | Transition level | sign, star, sub, sub-sub, sub³ |
| `--kp-format FORMAT` | KP output format | table, csv, json (default: table) |

### Utility Options

| Argument | Description |
|----------|-------------|
| `--solar-system` | Show solar system orbital paths |
| `--ephe-path PATH` | Custom Swiss Ephemeris data path |
| `--help, -h` | Show help message |
| `--version, -v` | Show version information |

## Chart Types

### Western Chart

Default chart style showing:
- Circular chart wheel with houses and planets
- Rectangular house layout
- Aspect grid with orbs
- Planetary positions in signs and houses

### Indian Charts

#### North Indian Style
```
+-------------+-------------+-------------+-------------+
|     12      |      1      |      2      |      3      |
+-------------+-------------+-------------+-------------+
|     11      |                           |      4      |
+-------------+     RASI CHART            +-------------+
|     10      |                           |      5      |
+-------------+-------------+-------------+-------------+
|      9      |      8      |      7      |      6      |
+-------------+-------------+-------------+-------------+
```

#### South Indian Style
Traditional square format with fixed house positions.

#### East Indian Style
Bengali-style chart with specific house arrangements.

### Solar System View
Displays orbital paths of planets around the Sun, useful for understanding planetary relationships from a heliocentric perspective.

## Advanced Features

### Eclipse Calculations

The application uses real Swiss Ephemeris eclipse functions to calculate:

- **Solar Eclipses**: Total, Partial, Annular, Hybrid
- **Lunar Eclipses**: Total, Partial, Penumbral
- **Eclipse Magnitude**: Calculated for specific geographic locations
- **Visibility**: Whether eclipse is visible from given coordinates

#### Eclipse Types Explained

- **Total Solar Eclipse**: Moon completely covers the Sun
- **Partial Solar Eclipse**: Moon partially covers the Sun
- **Annular Solar Eclipse**: Moon appears smaller than Sun, creating a "ring of fire"
- **Total Lunar Eclipse**: Moon passes through Earth's umbra
- **Partial Lunar Eclipse**: Moon partially passes through Earth's umbra
- **Penumbral Lunar Eclipse**: Moon passes through Earth's penumbra

### Conjunction Analysis

Precise planetary conjunction detection with:

- **Configurable Orbs**: Set maximum degrees for conjunction detection
- **Multiple Planet Types**: Major planets, lunar nodes, asteroids
- **Temporal Accuracy**: Exact timing of conjunctions
- **Applying/Separating**: Shows whether planets are moving toward or away from exact conjunction

### KP System (Krishnamurti Paddhati)

Complete implementation of the KP system with:

#### 5-Level Sub-Lord System
1. **Sign Lord**: Traditional zodiac sign ruler
2. **Star Lord (Nakshatra)**: Ruler of the 27 lunar mansions
3. **Sub Lord**: First-level subdivision
4. **Sub-Sub Lord**: Second-level subdivision
5. **Sub³ Lord**: Third-level subdivision

#### KP Transitions
Track planetary movements through different sub-lord periods, useful for:
- Timing events
- Predictive analysis
- Understanding planetary strength changes

### Ancient Date Support

Calculate charts for historical dates including BC era:

```bash
# Julius Caesar's assassination (44 BC)
./bin/horoscope_cli --date -0044-03-15 --time 12:00:00 --lat 41.9028 --lon 12.4964 --timezone 1

# Ancient Athens (500 BC)
./bin/horoscope_cli --date -0500-01-01 --time 12:00:00 --lat 37.9755 --lon 23.7348 --timezone 2
```

### Output Formats

#### Text Format (Default)
Human-readable charts with ASCII art and formatted tables.

#### JSON Format
Complete data structure suitable for:
- Web applications
- API integration
- Data analysis
- Custom chart rendering

```json
{
  "birthData": {
    "date": "1990-01-15",
    "time": "14:30:00",
    "latitude": 40.712800,
    "longitude": -74.006000,
    "timezone": -5.000000
  },
  "planets": [...],
  "houses": {...},
  "aspects": [...]
}
```

#### CSV Format
Tabular data for spreadsheet analysis.

## Examples

### Basic Birth Chart

```bash
./bin/horoscope_cli --date 1985-06-20 --time 09:15:30 --lat 51.5074 --lon -0.1278 --timezone 1
```

### Birth Chart with Koch Houses

```bash
./bin/horoscope_cli --date 1985-06-20 --time 09:15:30 --lat 51.5074 --lon -0.1278 --timezone 1 --house-system K
```

### North Indian Chart Style

```bash
./bin/horoscope_cli --date 1985-06-20 --time 09:15:30 --lat 51.5074 --lon -0.1278 --timezone 1 --chart-style north-indian
```

### JSON Output

```bash
./bin/horoscope_cli --date 1985-06-20 --time 09:15:30 --lat 51.5074 --lon -0.1278 --timezone 1 --output json
```

### Eclipse Analysis

```bash
# Eclipses around birth
./bin/horoscope_cli --date 1985-06-20 --time 09:15:30 --lat 51.5074 --lon -0.1278 --timezone 1 --eclipses

# Eclipses in 2024
./bin/horoscope_cli --eclipse-range 2024-01-01 2024-12-31 --lat 40.7128 --lon -74.0060

# Historical eclipses
./bin/horoscope_cli --eclipse-range -0500-01-01 -0499-12-31 --lat 37.9755 --lon 23.7348
```

### Conjunction Analysis

```bash
# Conjunctions with default 3° orb
./bin/horoscope_cli --date 1985-06-20 --time 09:15:30 --lat 51.5074 --lon -0.1278 --timezone 1 --conjunctions

# Conjunctions with 2° orb in date range
./bin/horoscope_cli --conjunction-range 2024-01-01 2024-03-31 --lat 40.7128 --lon -74.0060 --conjunction-orb 2.0
```

### KP System Analysis

```bash
# Complete KP table
./bin/horoscope_cli --date 1985-06-20 --time 09:15:30 --lat 51.5074 --lon -0.1278 --timezone 1 --kp-table

# Sun transitions for one week
./bin/horoscope_cli --kp-transitions --kp-transition-range 2025-01-01 2025-01-07 --kp-transition-planet SUN

# Sub-level transitions only
./bin/horoscope_cli --kp-transitions --kp-transition-range 2025-01-01 2025-01-07 --kp-transition-level sub
```

### Ephemeris Tables

```bash
# Daily ephemeris for January 2025
./bin/horoscope_cli --ephemeris --ephemeris-range 2025-01-01 2025-01-31

# Weekly ephemeris in CSV format
./bin/horoscope_cli --ephemeris --ephemeris-range 2025-01-01 2025-01-31 --ephemeris-interval 7 --ephemeris-format csv
```

### Solar System View

```bash
# Standalone solar system display
./bin/horoscope_cli --solar-system

# Solar system with birth data
./bin/horoscope_cli --date 1985-06-20 --time 09:15:30 --lat 51.5074 --lon -0.1278 --timezone 1 --chart-style solar-system
```

## Error Handling

### Common Errors and Solutions

#### "Required argument missing"
**Cause**: One or more required arguments (--date, --time, --lat, --lon, --timezone) not provided.
**Solution**: Ensure all required arguments are specified.

#### "Invalid date format"
**Cause**: Date not in YYYY-MM-DD format.
**Solution**: Use correct format, e.g., `--date 1990-01-15` or `--date -0044-03-15` for BC dates.

#### "Invalid time format"
**Cause**: Time not in HH:MM:SS format or values out of range.
**Solution**: Use 24-hour format with valid values, e.g., `--time 14:30:00`.

#### "Latitude must be between -90 and 90"
**Cause**: Latitude value outside valid range.
**Solution**: Use decimal degrees between -90 (South Pole) and +90 (North Pole).

#### "Longitude must be between -180 and 180"
**Cause**: Longitude value outside valid range.
**Solution**: Use decimal degrees between -180 (West) and +180 (East).

#### "Timezone must be between -12 and +14"
**Cause**: Timezone offset outside valid range.
**Solution**: Use valid timezone offsets, e.g., `-5` for EST, `1` for CET.

#### "Swiss Ephemeris error"
**Cause**: Missing or corrupted ephemeris data files.
**Solution**: Ensure Swiss Ephemeris data files are available in the data directory or specify correct path with `--ephe-path`.

## Technical Details

### Swiss Ephemeris Integration

The application uses the Swiss Ephemeris library for astronomical calculations:

- **Planetary Positions**: Accurate to arc-seconds
- **Date Range**: Approximately 13,000 years (6000 BC to 7000 AD)
- **Coordinate Systems**: Tropical, sidereal, heliocentric, geocentric
- **Eclipse Calculations**: Uses `swe_sol_eclipse_when_glob`, `swe_lun_eclipse_when`
- **House Calculations**: Multiple house systems via `swe_houses_ex`

### Coordinate Systems

- **Geocentric**: Earth-centered (default for most calculations)
- **Heliocentric**: Sun-centered (for solar system view)
- **Tropical Zodiac**: Based on seasons (default)
- **Sidereal Zodiac**: Based on fixed stars (available on request)

### Precision

- **Time**: Calculations accurate to the minute
- **Positions**: Arc-second precision
- **Aspects**: Configurable orbs with applying/separating analysis
- **Houses**: Precise cusps calculation for all supported systems

### Performance

- **Speed**: Fast calculations using optimized Swiss Ephemeris routines
- **Memory**: Efficient memory usage with ephemeris caching
- **Scalability**: Handles large date ranges for eclipse/conjunction searches

## Troubleshooting

### Installation Issues

**Problem**: Compilation errors
**Solution**: Ensure C++17 compiler and required development libraries are installed.

**Problem**: Missing Swiss Ephemeris data
**Solution**: Download Swiss Ephemeris data files and place in `./data/` directory.

### Runtime Issues

**Problem**: Slow eclipse calculations
**Solution**: Swiss Ephemeris calculations are intensive; large date ranges may take time.

**Problem**: No eclipses found
**Solution**: Try larger date ranges; eclipses are relatively rare events.

**Problem**: Inaccurate ancient dates
**Solution**: Swiss Ephemeris accuracy decreases for dates before 2000 BC and after 6000 AD.

### Output Issues

**Problem**: Garbled chart display
**Solution**: Ensure terminal supports UTF-8 encoding for astrological symbols.

**Problem**: JSON parsing errors
**Solution**: Use proper JSON parsing libraries; validate output with JSON validators.

## Support and Resources

### Additional Information

- Swiss Ephemeris: https://www.astro.com/swisseph/
- Astrological calculations: Based on traditional and modern astrological methods
- KP System: Follows Krishnamurti Paddhati principles

### Contributing

This manual covers the current feature set. For feature requests or bug reports, please refer to the project repository.

---

**Last Updated**: July 2025
**Version**: 1.0.0
**Manual Version**: 1.0
