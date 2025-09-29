# 🌟 Horoscope CLI ✨

A powerful C++ command-line astrology tool featuring comprehensive horoscope calculations, traditional Vedic chart systems, authentic Hindu Panchanga calendar, and complete Chinese calendar system with Swiss Ephemeris precision.

## 🎯 Best Features

### 🪐 **Astronomical Precision**
- ⭐ **Swiss Ephemeris Integration** - Industry-leading astronomical accuracy for planetary calculations
- 🔮 **Planetary Positions** - Calculate exact positions for any date, time, and location worldwide
- 🌙 **Lunar Calculations** - Precise moon phases, nodes, and lunar mansion positions
- ☀️ **Solar Analysis** - Solar returns, transits, and seasonal transitions
- 📿 **Historical Coverage** - Complete astronomical calculations from ancient BC era through 2400 AD
- 🏛️ **Ancient Astronomy** - Calculate planetary positions for historical events like Julius Caesar's assassination (44 BC)
- 🌍 **Eclipse History** - Find eclipses visible from any location across millennia of recorded history

### 🏠 **Advanced House Systems**
- 🏛️ **Multiple House Systems** - Placidus, Koch, Equal House, Whole Sign, and more
- 📐 **Accurate Cusps** - Precise house cusp calculations for professional astrology
- 🎯 **House Placement** - Automatic planet-to-house assignments with degrees

### 🎨 **Visual Chart Mastery**
- 🇮🇳 **Eastern/Vedic Charts** - Authentic North Indian, South Indian, and East Indian chart styles
- 🎭 **Multiple Chart Formats** - Traditional ASCII art charts with planetary symbols
- 📊 **Western Charts** - Classic Western astrological chart representations
- 🖼️ **Visual Comparisons** - Side-by-side chart style comparisons

### 🕉️ **Complete Hindu Panchanga System**
- 📅 **Traditional Calendar** - Full Hindu lunar calendar with Vikram Samvat years
- 🌟 **Five Limbs (Panchangam)**:
  - 🌛 **Tithi** - Lunar day calculations with precise timing
  - 📅 **Vara** - Weekday and planetary rulership
  - ⭐ **Nakshatra** - 27 lunar mansions with characteristics
  - 🤝 **Yoga** - 27 planetary combinations and their meanings
  - ⏰ **Karana** - Half-day periods for activity planning
- 🎉 **Festival Calendar** - Automatic detection of Hindu festivals and observances
- ⏰ **Muhurta Analysis** - Auspicious timing calculations for important events
- 📊 **Multiple Formats** - Detailed view, table, CSV, and JSON outputs

### 🐉 **Chinese Calendar System (NEW!)**
- 🎭 **60-Year Sexagenary Cycle** - Traditional Gan-Zhi (干支) stem-branch combinations with astronomical precision
- 🐲 **12-Year Zodiac Animals** - Complete zodiac system with 5-element associations (生肖)
- 🌿 **Wu Xing Five Elements** - Traditional Wood-Fire-Earth-Metal-Water theory with generating/overcoming cycles
- 🌙 **24 Solar Terms** - Precise seasonal markers based on sun longitude (節氣)
- 🌚 **Lunar Calendar** - Traditional Chinese lunar months with leap month detection
- 🎊 **Festival Calendar** - Spring Festival, Mid-Autumn, Dragon Boat, and traditional observances
- 🔮 **Four Pillars of Destiny** - Complete BaZi (八字) astrological analysis system
- 🌏 **Multi-language Support** - Traditional Chinese characters, English names, and Pinyin romanization
- ⚡ **Swiss Ephemeris Integration** - Astronomical precision for all calculations

### 🌐 **Professional Astrology Tools**
- 📐 **Aspect Calculator** - Major and minor planetary aspects with orbs
- 🌍 **Location Manager** - Global coordinate support with timezone handling
- 🎯 **KP System Support** - Krishnamurti Paddhati calculations
- ⚡ **Advanced Conjunction Analysis** - Revolutionary birthfinder algorithms with Newton-Raphson refinement
  - 🎯 **Sub-Arcsecond Precision** - Up to ±0.000001° accuracy with derivative calculations
  - 🧮 **Progressive Tolerance Search** - Multi-level refinement from 5.0° to 0.01° precision
  - 🌌 **Multi-Planet Detection** - Automatic triple and grand conjunction identification
  - ⚔️ **Graha Yuddha Analysis** - Planetary wars with winner determination and effects

### 💾 **Developer-Friendly Output**
- 📋 **Multiple Formats** - Text, JSON, CSV for easy integration
- 🔧 **API Ready** - JSON output perfect for web applications
- 📊 **Spreadsheet Compatible** - CSV export for data analysis
- 🎨 **Rich Text Output** - Colorful terminal display with emojis

### 🚀 **Performance & Reliability**
- ⚡ **Fast Calculations** - Optimized C++ performance for quick results
- 📦 **Self-Contained** - No external dependencies, Swiss Ephemeris embedded
- 🔒 **Cross-Platform** - Works on Linux, macOS, and Windows
- 🛠️ **Easy Installation** - Simple Makefile or CMake build process

## Dependencies

- C++17 compatible compiler
- CMake 3.10 or higher (optional - Makefile also provided)
- Standard C math library

**Note:** Swiss Ephemeris is embedded in the project - no external installation required!

### 📿 **Historical Calculation Coverage**
The project includes comprehensive ephemeris data files covering:
- **Ancient Era**: 6000 BC through 0 AD (BC/AD boundary calculations)
- **Classical Period**: 0 AD through 600 AD (early civilizations)
- **Medieval Era**: 600 AD through 1200 AD (medieval period)
- **Renaissance**: 1200 AD through 1800 AD (renaissance to modern)
- **Contemporary**: 1800 AD through 2400 AD (modern era and future)

**Historical Examples:**
```bash
# Julius Caesar's assassination (44 BC)
./bin/horoscope_cli --date -0044-03-15 --time 12:00:00 --lat 41.9028 --lon 12.4964 --timezone 1

# Ancient eclipse calculations
./bin/horoscope_cli --eclipse-range -0100-01-01 -0001-12-31 --lat 41.9028 --lon 12.4964

# Myanmar calendar for historical dates
./bin/horoscope_cli --date 1200-01-01 --myanmar-calendar --lat 16.8661 --lon 96.1951 --timezone 6.5
```

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

### Chinese Calendar System (NEW!) 🐉

Complete traditional Chinese calendar with astronomical precision:

```bash
# Basic Chinese calendar for current date
./horoscope_cli --chinese-calendar

# Chinese calendar for specific date
./horoscope_cli --chinese-calendar --date 2024-02-12

# Your Chinese zodiac animal
./horoscope_cli --chinese-zodiac --date 1990-08-15

# Complete Four Pillars analysis (BaZi)
./horoscope_cli --chinese-bazi --date 1985-03-15 --time 14:30 --lat 39.9042 --lon 116.4074 --timezone 8

# Chinese monthly calendar
./horoscope_cli --chinese-monthly --month 2024-02

# Multi-language output (Chinese/English/Pinyin)
./horoscope_cli --chinese-calendar --date 2024-12-25 --chinese-lang zh
```

**Features:**
- 🎭 **60-Year Sexagenary Cycle** (Gan-Zhi 干支) - Traditional stem-branch combinations
- 🐲 **12-Year Zodiac Animals** (生肖) - Complete zodiac with element associations  
- 🌿 **5-Element Theory** (Wu Xing 五行) - Wood, Fire, Earth, Metal, Water cycles
- 🌙 **24 Solar Terms** (Jie Qi 節氣) - Precise seasonal transitions
- 🌚 **Lunar Calendar** - New moon calculations with leap month detection
- 🎊 **Traditional Festivals** - Spring Festival, Mid-Autumn, Dragon Boat, etc.
- 🔮 **Four Pillars of Destiny** (BaZi 八字) - Complete astrological analysis
- 🌏 **Multi-language Support** - Chinese characters, English names, Pinyin romanization

**Sample Chinese Calendar Output:**
```
🐉 Chinese Calendar Data
=======================
Chinese Date: 4721/11/23
Gan-Zhi Year: 甲辰 (Jia-Chen) - Wood Dragon
Zodiac Animal: 龍 Dragon (Wood Element)
Solar Term: 冬至 Winter Solstice
Current Festival: Winter Solstice Festival

60-Year Cycle Position: 41
├── Year Stem: 甲 Jia (Wood, Yang)
├── Year Branch: 辰 Chen (Earth, Yang)  
├── Month Stem: 丙 Bing (Fire, Yang)
└── Day Stem: 戊 Wu (Earth, Yang)

Auspicious: ✅ Yes
Lucky Elements: Wood, Water
```

## Compare Traditional Systems
./examples/chart_comparison.sh 1990-01-15 14:30:00 40.7128 -74.0060 -5
```

## License

MIT License
