# HOROSCOPE CLI
## Professional Astronomical Calculations & Astrological Analysis

**A high-performance command-line tool for precise astronomical computations using Swiss Ephemeris**

---

## Core Features

### Astronomical Calculations
- **Swiss Ephemeris Integration**: Arc-second precision planetary positions
- **Date Range**: 6000 BC to 7000 AD (13,000 years of accurate data)
- **Coordinate Systems**: Geocentric, heliocentric, topocentric calculations
- **Real-time Precision**: Accurate to minutes for timing

### Chart Generation
- **Multiple House Systems**: Placidus, Koch, Equal, Whole Sign, Campanus, Regiomontanus
- **Chart Styles**: Western wheel, North/South/East Indian, Solar system view
- **Output Formats**: ASCII art, JSON data structure, CSV tables
- **Professional Layout**: Clean, readable chart presentations

### Eclipse Analysis
- **Solar Eclipses**: Total, partial, annular, hybrid types
- **Lunar Eclipses**: Total, partial, penumbral classifications
- **Geographic Precision**: Magnitude calculations for specific locations
- **Historical Tracking**: Search eclipses across any time period

### Conjunction Detection
- **Planetary Alignments**: All major celestial bodies
- **Configurable Orbs**: Set precision thresholds (degrees)
- **Temporal Analysis**: Applying/separating aspect detection
- **Event Timing**: Precise conjunction timestamps

### KP System (Krishnamurti Paddhati)
- **5-Level Sub-Lords**: Complete hierarchical analysis
- **27 Nakshatras**: Traditional lunar mansion system
- **Transition Tracking**: Monitor planetary lord changes
- **Professional Tables**: Standard KP notation format

### Ephemeris Generation
- **Customizable Intervals**: Daily, weekly, monthly tables
- **Complete Planetary Data**: Sun through Pluto, lunar nodes, Chiron
- **Motion Indicators**: Retrograde status, speed calculations
- **Export Ready**: Multiple format support for analysis

---

## Technical Specifications

**Language**: C++17
**Dependencies**: Swiss Ephemeris library
**Platform**: Linux/Unix systems
**Performance**: Optimized for fast calculations
**Precision**: Arc-second accuracy
**Memory**: Efficient ephemeris data caching

---

## Sample Usage

```bash
# Basic birth chart
horoscope_cli --date 1990-01-15 --time 14:30:00 \
              --lat 40.7128 --lon -74.0060 --timezone -5

# Eclipse search
horoscope_cli --eclipse-range 2024-01-01 2024-12-31 \
              --lat 40.7128 --lon -74.0060

# KP analysis
horoscope_cli --date 1990-01-15 --time 14:30:00 \
              --lat 40.7128 --lon -74.0060 --timezone -5 \
              --kp-table

# JSON output
horoscope_cli --date 1990-01-15 --time 14:30:00 \
              --lat 40.7128 --lon -74.0060 --timezone -5 \
              --output json
```

---

## Architecture

```
┌─────────────────┐    ┌──────────────────┐    ┌────────────────┐
│   CLI Parser    │───▶│  Calculation     │───▶│   Output       │
│                 │    │  Engine          │    │   Formatter    │
│ • Arguments     │    │                  │    │                │
│ • Validation    │    │ • Swiss Ephemeris│    │ • Text Charts  │
│ • Date Parsing  │    │ • House Systems  │    │ • JSON Data    │
└─────────────────┘    │ • KP Calculations│    │ • CSV Tables   │
                       │ • Eclipse Engine │    └────────────────┘
                       │ • Aspects        │
                       └──────────────────┘
```

---

## Key Algorithms

### Planetary Position Calculation
- Swiss Ephemeris `swe_calc_ut()` for precise planetary coordinates
- Coordinate transformation for different reference frames
- Speed and acceleration calculations for motion analysis

### House System Implementation
- Multiple algorithms: Placidus, Koch, Equal, Whole Sign
- Accurate cusp calculations using `swe_houses_ex()`
- Geographic and temporal corrections

### Eclipse Detection
- `swe_sol_eclipse_when_glob()` for solar eclipse timing
- `swe_lun_eclipse_when()` for lunar eclipse detection
- Local visibility calculations with `swe_sol_eclipse_how()`

### KP Sub-Lord System
- Vimshottari Dasha proportional subdivisions
- 5-level hierarchical lord calculations
- Precise transition timing algorithms

---

## Output Examples

### Western Chart
```
      *
  ******* *******
 **             **
**    ☽       ♃  **
*       ·         *
*    ♂     ☉      *
**               **
 **             **
  ******* *******
      *
```

### JSON Structure
```json
{
  "birthData": {...},
  "planets": [
    {
      "name": "Sun",
      "longitude": 295.396,
      "sign": "Capricorn",
      "house": 8
    }
  ],
  "aspects": [...]
}
```

---

## Professional Applications

**Astrology Software**: Backend engine for chart calculations
**Research**: Historical astronomical event analysis
**Education**: Teaching tool for astronomical concepts
**Development**: API integration for astrological applications
**Analysis**: Bulk data processing for statistical studies

---

**Built with Swiss Ephemeris • Open Source • Professional Grade**

#AstronomicalCalculations #SwissEphemeris #CLI #OpenSource #Astrology #Programming #DataAnalysis