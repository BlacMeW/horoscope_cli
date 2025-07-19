# Horoscope CLI

A C++ command-line tool for calculating horoscopes using Swiss Ephemeris.

## Features

- Calculate planetary positions for any given date, time, and location
- Generate birth charts with house calculations
- Calculate aspects between planets
- Support for various house systems (Placidus, Koch, Equal House, etc.)
- **Eastern/Vedic chart drawing** - Visual ASCII charts in North Indian, South Indian, and East Indian styles
- **Hindu Calendar System (Panchanga)** 🕉️ - Complete traditional Hindu calendar calculations
  - Tithi (lunar day), Vara (weekday), Nakshatra (lunar mansion)
  - Yoga (planetary union), Karana (half-day period)
  - Hindu months, Vikram Samvat years, Paksha (lunar fortnight)
  - Festival identification and special observances
  - Muhurta analysis for auspicious timing
  - Multiple output formats (detailed view, table, CSV, JSON)
- Multiple output formats (text, JSON)
- Accurate astronomical calculations using Swiss Ephemeris

## Dependencies

- C++17 compatible compiler
- CMake 3.10 or higher (optional - Makefile also provided)
- Standard C math library

**Note:** Swiss Ephemeris is embedded in the project - no external installation required!

## Installation

### Building with Makefile (Recommended)
```bash
make clean
make
```

### Building with CMake
```bash
mkdir build
cd build
cmake ..
make
```

### No External Dependencies Required
The Swiss Ephemeris library is embedded in the project under `third_party/swisseph/`.
No external package installation is needed.

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
- `--chart-style`: Chart style (western, north-indian, south-indian, east-indian)
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

**East Indian Vedic Chart (Bengali/Oriya Style):**
```
=== EAST INDIAN VEDIC CHART ===

┌─────────┬─────────┬─────────┐
│   Ve,Ke │  Su,Me  │    2    │
├─────────┼─────────┼─────────┤
│   Ju    │         │   Sa    │
├─────────┼─────────┼─────────┤
│   10    │         │   Mo    │
├─────────┼─────────┼─────────┤
│   Ma    │   Ra    │    7    │
└─────────┴─────────┴─────────┘
```

### Eastern Chart Examples

```bash
# North Indian style
./horoscope_cli --date "1990-01-15" --time "14:30:00" --lat 40.7128 --lon -74.0060 --timezone -5 --chart-style north-indian

# South Indian style
./horoscope_cli --date "1990-01-15" --time "14:30:00" --lat 40.7128 --lon -74.0060 --timezone -5 --chart-style south-indian

# East Indian style (Bengali/Oriya tradition)
./horoscope_cli --date "1990-01-15" --time "14:30:00" --lat 40.7128 --lon -74.0060 --timezone -5 --chart-style east-indian

# Compare all styles
```

### Hindu Calendar (Panchanga) Examples 🕉️

The Hindu calendar system provides complete traditional Panchanga calculations:

```bash
# Basic Panchanga for a specific date
./horoscope_cli --date "2024-01-15" --time "12:00:00" --lat 28.6139 --lon 77.2090 --timezone 5.5 --panchanga

# Panchanga for date range (3 days)
./horoscope_cli --date "2024-01-14" --time "12:00:00" --lat 28.6139 --lon 77.2090 --timezone 5.5 --panchanga-range 2024-01-14 2024-01-16

# JSON output for API integration
./horoscope_cli --date "2024-01-15" --time "12:00:00" --lat 28.6139 --lon 77.2090 --timezone 5.5 --panchanga --panchanga-format json

# CSV output for spreadsheet analysis
./horoscope_cli --date "2024-01-14" --time "12:00:00" --lat 28.6139 --lon 77.2090 --timezone 5.5 --panchanga-range 2024-01-14 2024-01-16 --panchanga-format csv

# Show only festivals and special observances
./horoscope_cli --date "2024-03-14" --time "12:00:00" --lat 28.6139 --lon 77.2090 --timezone 5.5 --panchanga --festivals-only
```

**Sample Panchanga Output:**
```
═══════════════════════════════════════════════════════════════════
                          🕉️  HINDU PANCHANGA  🕉️
═══════════════════════════════════════════════════════════════════

📅 DATE INFORMATION:
   Hindu Year (Vikram Samvat): 2373
   Hindu Month: Pausha
   Paksha: Shukla (Bright)

🌟 PANCHANGAM (FIVE LIMBS):
   1. Tithi:     Panchami (ends in 21.3 hours)
   2. Vara:      Monday
   3. Nakshatra: Purva Phalguni (ends in 12.7 hours)
   4. Yoga:      Saubhagya (ends in 5.4 hours)
   5. Karana:    Taitila (ends in 8.8 hours)

🌞 CELESTIAL POSITIONS:
   Sun in:   Makara (Capricorn) (270.24°)
   Moon in:  Simha (Leo) (140.02°)
   Lunar Phase: 229.8°

🌙 SPECIAL OBSERVANCES:
   • Sankranti - Solar transition

🕐 MUHURTA ASSESSMENT:
   Status: ✅ Shubha (Auspicious)
   Note: Generally auspicious time for most activities
```

## Compare Traditional Systems
./examples/chart_comparison.sh 1990-01-15 14:30:00 40.7128 -74.0060 -5
```

## License

MIT License
