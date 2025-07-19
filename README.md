# Horoscope CLI

A C++ command-line tool for calculating horoscopes using Swiss Ephemeris.

## Features

- Calculate planetary positions for any given date, time, and location
- Generate birth charts with house calculations
- Calculate aspects between planets
- Support for various house systems (Placidus, Koch, Equal House, etc.)
- **Eastern/Vedic chart drawing** - Visual ASCII charts in North Indian and South Indian styles
- Multiple output formats (text, JSON)
- Accurate astronomical calculations using Swiss Ephemeris

## Dependencies

- Swiss Ephemeris library (libswe-dev)
- CMake 3.10 or higher
- C++17 compatible compiler

## Installation

### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install libswe-dev cmake build-essential
```

### Building
```bash
mkdir build
cd build
cmake ..
make
```

## Usage

```bash
./horoscope_cli --date "1990-01-15" --time "14:30:00" --lat 40.7128 --lon -74.0060 --timezone -5
```

### Parameters

- `--date`: Birth date in YYYY-MM-DD format
- `--time`: Birth time in HH:MM:SS format (24-hour)
- `--lat`: Latitude in decimal degrees
- `--lon`: Longitude in decimal degrees
- `--timezone`: Timezone offset from UTC in hours
- `--house-system`: House system (P=Placidus, K=Koch, E=Equal, W=Whole Sign)
- `--chart-style`: Chart style (western, north-indian, south-indian)
- `--output`: Output format (text, json)

### Example Output

**Western Style (Default):**
```
Birth Chart for 1990-01-15 14:30:00 UTC-5
Location: 40.7128°N, 74.0060°W

=== PLANETARY POSITIONS ===
Sun:        25°42' Capricorn (House 5)
Moon:       12°18' Scorpio (House 3)
Mercury:    05°33' Aquarius (House 6)
...
```

**North Indian Vedic Chart:**
```
=== NORTH INDIAN VEDIC CHART ===

┌─────────────┬─────────────┬─────────────┬─────────────┐
│             │             │             │             │
│     12      │      1      │      2      │      3      │
│             │             │  Su,Me      │             │
├─────────────┼─────────────┼─────────────┼─────────────┤
│             │                           │             │
│     11      │                           │      4      │
│             │           RASI            │  Ve         │
├─────────────┤                           ├─────────────┤
│             │                           │             │
│     10      │                           │      5      │
│   Ju        │                           │  Ma,Sa      │
├─────────────┼─────────────┼─────────────┼─────────────┤
│             │             │             │             │
│      9      │      8      │      7      │      6      │
│             │             │             │             │
└─────────────┴─────────────┴─────────────┴─────────────┘
```

### Eastern Chart Examples

```bash
# North Indian style
./horoscope_cli --date "1990-01-15" --time "14:30:00" --lat 40.7128 --lon -74.0060 --timezone -5 --chart-style north-indian

# South Indian style  
./horoscope_cli --date "1990-01-15" --time "14:30:00" --lat 40.7128 --lon -74.0060 --timezone -5 --chart-style south-indian

# Compare all styles
./examples/chart_comparison.sh 1990-01-15 14:30:00 40.7128 -74.0060 -5
```

## License

MIT License
