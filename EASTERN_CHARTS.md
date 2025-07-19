# Eastern Chart Drawing Guide

## Overview

The Horoscope CLI now supports Eastern/Vedic astrology chart drawing in addition to traditional Western charts. This feature provides visual ASCII representations of birth charts in the traditional Indian astrology formats.

## Supported Chart Styles

### 1. Western Style (Default)
- Traditional Western astrology format
- Detailed planetary positions with degrees and minutes
- House cusps and angles
- Major aspects between planets

### 2. North Indian Style
- Diamond-shaped chart format
- Houses arranged in a 4x4 grid pattern
- Fixed house positions relative to the Ascendant
- Planets shown with abbreviated symbols

### 3. South Indian Style
- Square format with triangular house divisions
- Fixed zodiac signs in each position
- Houses move relative to the Ascendant
- Traditional Tamil/Kerala style presentation

## Usage

### Command Line Options

```bash
# Basic Eastern chart
./horoscope_cli --date YYYY-MM-DD --time HH:MM:SS --lat LATITUDE --lon LONGITUDE --timezone OFFSET --chart-style STYLE

# Where STYLE can be:
# - western (default)
# - north-indian
# - south-indian
```

### Examples

```bash
# Mahatma Gandhi's chart in North Indian style
./horoscope_cli --date "1869-10-02" --time "07:45:00" --lat 21.6416 --lon 69.6293 --timezone 5.5 --chart-style north-indian

# Same chart in South Indian style
./horoscope_cli --date "1869-10-02" --time "07:45:00" --lat 21.6416 --lon 69.6293 --timezone 5.5 --chart-style south-indian

# Compare all three styles
./examples/chart_comparison.sh 1869-10-02 07:45:00 21.6416 69.6293 5.5
```

## Planet Symbols

The Eastern charts use abbreviated planet symbols to fit in the compact format:

| Planet | Symbol | Sanskrit Name |
|--------|--------|---------------|
| Sun | Su | Surya |
| Moon | Mo | Chandra |
| Mercury | Me | Budha |
| Venus | Ve | Shukra |
| Mars | Ma | Mangal |
| Jupiter | Ju | Guru/Brihaspati |
| Saturn | Sa | Shani |
| Rahu (North Node) | Ra | Rahu |
| Ketu (South Node) | Ke | Ketu |
| Uranus | Ur | - |
| Neptune | Ne | - |
| Pluto | Pl | - |

## Chart Interpretation

### North Indian Chart
```
┌─────────────┬─────────────┬─────────────┬─────────────┐
│             │             │             │             │
│     12      │      1      │      2      │      3      │
│             │             │             │             │
├─────────────┼─────────────┼─────────────┼─────────────┤
│             │                           │             │
│     11      │                           │      4      │
│             │           RASI            │             │
├─────────────┤                           ├─────────────┤
│             │                           │             │
│     10      │                           │      5      │
│             │                           │             │
├─────────────┼─────────────┼─────────────┼─────────────┤
│             │             │             │             │
│      9      │      8      │      7      │      6      │
│             │             │             │             │
└─────────────┴─────────────┴─────────────┴─────────────┘
```

- House 1 (Ascendant) is always in the top-center position
- Houses proceed clockwise from there
- The center shows "RASI" (zodiac/chart)

### South Indian Chart
```
        ┌─────────────┬─────────────┐        
        │      2      │      3      │        
        │             │             │        
┌───────┼─────────────┼─────────────┼───────┐
│   1   │                           │   4   │
│       │                           │       │
├───────┤           RASI            ├───────┤
│  12   │                           │   5   │
│       │                           │       │
└───────┼─────────────┼─────────────┼───────┘
        │     11      │      6      │        
        │             │             │        
        └─────────────┴─────────────┘        
```

- Fixed zodiac positions (Aries always top-left)
- Houses move based on Ascendant position
- More traditional Kerala/Tamil format

## Example Scripts

### 1. Eastern Chart Examples
```bash
./examples/eastern_chart_examples.sh
```
Shows various famous personalities' charts in different styles.

### 2. Chart Comparison
```bash
./examples/chart_comparison.sh DATE TIME LAT LON TZ
```
Displays the same birth data in all three chart styles.

### 3. Test Famous Charts
```bash
# Use the provided configuration files
cat examples/indian_personalities.conf
cat examples/birth_data.conf
```

## Integration with Western Features

Eastern charts can be combined with Western chart features:

```bash
# Show both Western details and Eastern visual chart
./horoscope_cli --date "1990-01-15" --time "14:30:00" --lat 40.7128 --lon -74.0060 --timezone -5 --chart-style north-indian

# This will display:
# 1. Traditional Western planetary positions and aspects
# 2. Eastern visual chart in the specified style
```

## Technical Notes

- All charts use the same astronomical calculations (Swiss Ephemeris)
- House systems apply to both Western and Eastern formats
- Eastern charts are purely visual representations
- Aspects are calculated and shown in Western format regardless of chart style
- Planetary positions are identical across all chart styles

## Cultural Context

- **North Indian Style**: Common in Hindi-speaking regions
- **South Indian Style**: Traditional in Tamil Nadu, Kerala, Karnataka
- Both styles show the same astronomical information
- Choice of style is often based on regional tradition or personal preference

## Troubleshooting

### Common Issues

1. **Planets not showing in chart**
   - Check if planets are in the same house (they might overlap)
   - Some planets might be very close together

2. **Chart looks garbled**
   - Ensure your terminal supports Unicode characters
   - Use a monospace font for proper alignment

3. **Missing planets in display**
   - Long planet lists in one house might be truncated
   - Check the Western format for complete planetary information

### Terminal Requirements

- Unicode support for box-drawing characters
- Monospace font (Courier, Consolas, etc.)
- Minimum 80-character width for proper display
