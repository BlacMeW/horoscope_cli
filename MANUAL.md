# 🌟 Horoscope CLI Manual ✨

**Version:** 2.0.0
**Built with:** Swiss Ephemeris
**Copyright:** © 2025

## 📑 Table of Contents

1. [🌟 Overview](#overview)
2. [🔧 Installation](#installation)
3. [🚀 Quick Start](#quick-start)
4. [📋 Command Line Arguments](#command-line-arguments)
5. [🎨 Chart Types & Styles](#chart-types--styles)
6. [🕉️ Hindu Panchanga System](#hindu-panchanga-system)
7. [⚡ Advanced Features](#advanced-features)
8. [📊 Output Formats](#output-formats)
9. [💡 Examples & Use Cases](#examples--use-cases)
10. [⚠️ Error Handling](#error-handling)

---
11. [🔬 Technical Details](#technical-details)
12. [🛠️ Troubleshooting](#troubleshooting)

## 🌟 Overview

**Horoscope CLI** is a professional-grade astronomical calculation tool that combines ancient wisdom with modern precision. Using the renowned Swiss Ephemeris library, it delivers accurate birth charts, planetary calculations, eclipse predictions, and comprehensive astrological analysis with support for traditional Vedic systems and the complete Hindu Panchanga calendar.

### 🎯 Key Capabilities

- 🪐 **Swiss Ephemeris Precision** - Industry-leading astronomical accuracy
- 🎨 **Multiple Chart Styles** - Western, North Indian, East Indian, South Indian, Solar System
- 🏠 **Advanced House Systems** - Placidus, Koch, Equal, Whole Sign, Campanus, Regiomontanus
- 🌙 **Eclipse & Conjunction Analysis** - Real-time astronomical event calculations with latitude filtering
- ⚔️ **Graha Yuddha (Planetary Wars)** - Traditional Vedic analysis of close planetary conjunctions
- 🕉️ **Hindu Panchanga System** - Complete traditional Hindu calendar with festivals
- ⭐ **KP System Support** - Complete Krishnamurti Paddhati with 5-level sub-lord analysis
- 📅 **Ancient Date Support** - Historical calculations including BC era
- 📊 **Multiple Output Formats** - Text, JSON, CSV for integration and analysis
- 📈 **Ephemeris Tables** - Generate comprehensive astronomical data tables

## 🔧 Installation

### 📋 Prerequisites

- 🐧 **Linux/Unix System** (Ubuntu, CentOS, macOS)
- 🛠️ **C++ Compiler** with C++17 support (GCC 7+ or Clang 5+)
- 📦 **Swiss Ephemeris Data** (included in project)

### ⚡ Quick Build

```bash
# Clone and build in one go
git clone <repository-url>
cd horoscope_cli
make clean && make
```

### 🏗️ Alternative Build Methods

#### Using CMake
```bash
mkdir build && cd build
cmake .. && make
```

#### Custom Build Path
```bash
make PREFIX=/usr/local install
```

### 📁 Swiss Ephemeris Data

The application includes Swiss Ephemeris data files in `./data/`. For custom installations:
- 📂 Default path: `./data/`
- 🔧 Custom path: Use `--ephe-path /custom/path`
- 📥 Additional data: Download from https://www.astro.com/ftp/swisseph/ephe/

## 🚀 Quick Start

### 🎯 Minimum Required Command

Generate a basic birth chart with just these essentials:

```bash
./bin/horoscope_cli --date YYYY-MM-DD --time HH:MM:SS --lat LATITUDE --lon LONGITUDE --timezone HOURS
```

### 🌟 Your First Chart

```bash
# Birth chart for New York, January 15, 1990, 2:30 PM EST
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 --lat 40.7128 --lon -74.0060 --timezone -5
```

### 🎨 Try Different Styles

```bash
# Traditional North Indian Vedic chart
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 --lat 40.7128 --lon -74.0060 --timezone -5 --chart-style north-indian

# Complete Hindu Panchanga
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 --lat 28.6139 --lon 77.2090 --timezone 5.5 --panchanga
```

## 📋 Command Line Arguments

### 🔑 Required Arguments

| Argument | Description | Format | Range | Example |
|----------|-------------|--------|-------|---------|
| `--date` | Birth date | YYYY-MM-DD | Any valid date | `1990-01-15` |
| `--time` | Birth time | HH:MM:SS | 24-hour format | `14:30:00` |
| `--lat` | Latitude | Decimal degrees | -90 to 90 | `40.7128` |
| `--lon` | Longitude | Decimal degrees | -180 to 180 | `-74.0060` |
| `--timezone` | UTC offset | Hours | -12 to +14 | `-5` |

### 🎨 Chart & Display Options

| Argument | Description | Values | Default |
|----------|-------------|--------|---------|
| `--chart-style` | Chart visualization | western, north-indian, south-indian, east-indian, solar-system | western |
| `--house-system` | House calculation | P, K, E, W, C, R | P (Placidus) |
| `--output` | Output format | text, json, csv | text |
| `--perspective` | Solar system view | heliocentric, geocentric, mars-centric, jupiter-centric | heliocentric |

### 🏠 House Systems Legend

- **P** = 🏛️ **Placidus** (default) - Most popular modern system
- **K** = 🔍 **Koch** - German precision method
- **E** = ⚖️ **Equal House** - 30° equal divisions
- **W** = 🌟 **Whole Sign** - Traditional ancient method
- **C** = 📐 **Campanus** - Medieval geometric system
- **R** = 👑 **Regiomontanus** - Renaissance royal method

### 🕉️ Hindu Panchanga Options

| Argument | Description | Format | Example |
|----------|-------------|--------|---------|
| `--panchanga` | Show complete Panchanga | Flag | `--panchanga` |
| `--panchanga-range FROM TO` | Panchanga date range | YYYY-MM-DD YYYY-MM-DD | `--panchanga-range 2024-01-01 2024-01-07` |
| `--panchanga-format FORMAT` | Output format | detailed, table, csv, json | `--panchanga-format json` |
| `--festivals-only` | Show only festivals | Flag | `--festivals-only` |
| `--muhurta-analysis` | Include auspicious timing | Flag | `--muhurta-analysis` |

### 🌙 Eclipse Options

| Argument | Description | Format | Example |
|----------|-------------|--------|---------|
| `--eclipses` | Show eclipses near birth | Flag | `--eclipses` |
| `--eclipse-range FROM TO` | Eclipse date range | YYYY-MM-DD YYYY-MM-DD | `--eclipse-range 2024-01-01 2024-12-31` |
| `--eclipse-years-before N` | Years before birth | Integer | `--eclipse-years-before 2` |
| `--eclipse-years-after N` | Years after birth | Integer | `--eclipse-years-after 2` |
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

### 🤝 **Conjunction Analysis Options**

| Argument | Description | Format | Example |
|----------|-------------|--------|---------|
| `--conjunctions` | Show conjunctions near birth | Flag | `--conjunctions` |
| `--conjunction-range FROM TO` | Conjunction date range | YYYY-MM-DD YYYY-MM-DD | `--conjunction-range 2024-01-01 2024-03-31` |
| `--conjunction-orb DEGREES` | Maximum orb | Decimal (default: 3.0) | `--conjunction-orb 2.0` |
| `--conjunction-latitude-range MIN MAX` | Filter by planetary latitude range | Decimal Decimal (-90.0 to +90.0) | `--conjunction-latitude-range -2.0 2.0` |
| `--graha-yuddha` | Find Graha Yuddha (Planetary Wars) | Flag | `--graha-yuddha` |
| `--graha-yuddha-range FROM TO` | Planetary wars in date range | YYYY-MM-DD YYYY-MM-DD | `--graha-yuddha-range 2024-01-01 2024-12-31` |
| `--graha-yuddha-orb DEGREES` | Maximum orb for planetary wars | Decimal (default: 1.0) | `--graha-yuddha-orb 0.5` |

### 📊 **Ephemeris Table Options**

| Argument | Description | Format | Example |
|----------|-------------|--------|---------|
| `--ephemeris` | Generate ephemeris table | Flag | `--ephemeris` |
| `--ephemeris-range FROM TO` | Date range | YYYY-MM-DD YYYY-MM-DD | `--ephemeris-range 2025-01-01 2025-01-31` |
| `--ephemeris-interval DAYS` | Interval between entries | Integer (default: 1) | `--ephemeris-interval 7` |
| `--ephemeris-format FORMAT` | Output format | table, csv, json | `--ephemeris-format csv` |

### ⭐ **KP System Options**

| Argument | Description | Format | Example |
|----------|-------------|--------|---------|
| `--kp-table` | Show KP Sub Lord 5 Levels | Flag | `--kp-table` |
| `--kp-transitions` | Show KP transitions | Flag | `--kp-transitions` |
| `--kp-transition-range FROM TO` | KP transition date range | YYYY-MM-DD YYYY-MM-DD | `--kp-transition-range 2025-01-01 2025-01-07` |
| `--kp-transition-planet PLANET` | Specific planet | SUN, MOON, MERCURY, etc. | `--kp-transition-planet SUN` |
| `--kp-transition-level LEVEL` | Transition level | sign, star, sub, sub-sub, sub³ | `--kp-transition-level sub` |
| `--kp-format FORMAT` | KP output format | table, csv, json | `--kp-format json` |

### 🌍 **Location & Utility Options**

| Argument | Description | Format | Example |
|----------|-------------|--------|---------|
| `--location NAME` | Search location by name | String | `--location "New York"` |
| `--search-location NAME` | Search and list matching locations | String | `--search-location "Paris"` |
| `--list-locations` | Show all available predefined locations | Flag | `--list-locations` |
| `--solar-system` | Show solar system orbital paths only | Flag | `--solar-system` |
| `--no-drawing` | Disable all chart and ASCII art | Flag | `--no-drawing` |
| `--ephe-path PATH` | Custom Swiss Ephemeris data path | String | `--ephe-path /custom/path` |
| `--help, -h` | Show comprehensive help message | Flag | `--help` |
| `--features, -f` | Show colorful feature showcase | Flag | `--features` |
| `--version, -v` | Show version and build information | Flag | `--version` |

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

## 🌌 Solar System & Perspective Views

The horoscope_cli tool offers revolutionary **perspective-based** astronomical calculations, allowing you to view planetary relationships from any celestial body in our solar system. This unique feature provides insights impossible with traditional Earth-centered astrology.

### 🎯 **Available Perspectives**

| Perspective | Center Point | Symbol | Astrological Use |
|-------------|--------------|--------|------------------|
| **Geocentric** | 🌍 Earth | Traditional | Classical astrology, personal charts |
| **Heliocentric** | ☀️ Sun | Modern | Natural planetary relationships, orbital harmony |
| **Mars-centric** | ♂ Mars | Specialized | Mars-related themes, warrior energy |
| **Jupiter-centric** | ♃ Jupiter | Expansive | Wisdom, expansion, philosophical insights |
| **Saturn-centric** | ♄ Saturn | Structural | Discipline, structure, karmic patterns |

### ☀️ **Heliocentric System**

**Heliocentric** astrology places the Sun at the center, showing true planetary relationships and orbital mechanics.

#### 🌟 **Heliocentric Advantages**
- **🔬 Scientific Accuracy**: Reflects actual planetary motions and distances
- **🎯 Pure Relationships**: Shows direct planet-to-planet aspects without Earth's bias
- **⚡ Cosmic Perspective**: Understanding of true solar system dynamics
- **🌊 Natural Rhythms**: Alignment with actual orbital periods and cycles

#### 💫 **Heliocentric Applications**
- **Evolutionary Astrology**: Soul-level planetary influences
- **Astronomical Research**: True planetary aspect patterns
- **Cosmic Timing**: Natural celestial rhythms and cycles
- **Transpersonal Analysis**: Beyond personal, Earth-centric viewpoints

### 🪐 **Planetary-Centric Systems**

Each planetary perspective offers unique insights based on that celestial body's characteristics and astronomical position.

#### ♂ **Mars-Centric Perspective**
- **Focus**: Action, energy, drive, warrior consciousness
- **Insights**: How other planets appear from Mars' viewpoint
- **Applications**: Military strategy, sports astrology, assertiveness training
- **Unique Features**: Mars retrograde loops, Earth-Mars opposition cycles

#### ♃ **Jupiter-Centric Perspective**
- **Focus**: Expansion, wisdom, teaching, philosophical growth
- **Insights**: Jupiter's role as solar system "protector" and guide
- **Applications**: Educational planning, spiritual development, success timing
- **Unique Features**: Jupiter's massive gravitational influence on other planets

#### ♄ **Saturn-Centric Perspective**
- **Focus**: Structure, discipline, karma, life lessons
- **Insights**: Saturn's role as "timekeeper" and teacher
- **Applications**: Career planning, discipline development, karmic astrology
- **Unique Features**: Saturn's rings, slow orbital period, generational influence

### 🎨 **Visual Solar System Display**

The solar system chart style provides stunning ASCII art visualization of planetary orbits and positions:

```
        🌞 SOLAR SYSTEM VIEW 🌞

    Mercury ☿ ·
            Venus ♀  · ·
                  Earth 🌍 · · ·
                      Mars ♂ · · · ·
                            Jupiter ♃ · · · · ·
                                  Saturn ♄ · · · · · ·
                                        Uranus ⛢ · · · · · · ·
                                              Neptune ♆ · · · · · · · ·
```

### 🔬 **Coordinate System Differences**

#### 🌍 **Geocentric Coordinates**
- **Reference**: Earth's center
- **Longitude**: Based on Earth's equinox and ecliptic
- **Latitude**: Angular distance from Earth's ecliptic plane
- **Use**: Traditional astrology, personal interpretation

#### ☀️ **Heliocentric Coordinates**
- **Reference**: Sun's center
- **Longitude**: True orbital positions around Sun
- **Latitude**: Angular distance from solar system's invariable plane
- **Use**: Astronomical research, evolutionary astrology

#### 🪐 **Planetary-Centric Coordinates**
- **Reference**: Selected planet's center
- **Longitude**: Angular positions as seen from that planet
- **Latitude**: Relative to that planet's orbital plane
- **Use**: Specialized astrological research, perspective studies

## 🕉️ Hindu Panchanga System

The **Hindu Panchanga** (पञ्चाङ्ग) is a traditional lunar calendar system providing complete temporal and spiritual context for any given moment. The word "Panchanga" means "five limbs" representing the five essential elements of time calculation in Hindu astronomy.

### 🌟 The Five Limbs (Panchangam)

#### 1. 🌛 **Tithi (Lunar Day)**
- 30 lunar phases in a lunar month
- Each Tithi represents moon's 12° movement from the sun
- Names: Pratipada, Dwitiya, Tritiya... Chaturdashi, Purnima/Amavasya
- **Spiritual Significance**: Determines ritual timing and festivals

#### 2. 📅 **Vara (Weekday)**
- Seven days ruled by planets
- Sunday (Sun), Monday (Moon), Tuesday (Mars)... Saturday (Saturn)
- **Astrological Influence**: Planetary energy dominates the day

#### 3. ⭐ **Nakshatra (Lunar Mansion)**
- 27 (sometimes 28) star groups along moon's path
- Each covers 13°20' of zodiac
- Names: Ashwini, Bharani, Krittika... Revati
- **Characteristics**: Personality traits, compatibility, timing

#### 4. 🤝 **Yoga (Planetary Combination)**
- 27 combinations of sun and moon positions
- Mathematical: (Sun longitude + Moon longitude) / 13°20'
- Names: Vishkumbha, Priti, Ayushman... Vaidhriti, Vishti
- **Effects**: Auspicious or inauspicious influences

#### 5. ⏰ **Karana (Half-day Period)**
- 11 Karanas, each half of a Tithi
- Four fixed: Shakuni, Chatushpada, Naga, Kimstughna
- Seven movable: Bava, Balava, Kaulava, Taitila, Gara, Vanija, Vishti
- **Activity Planning**: Suitable actions for each period

### 🎉 Festival & Observance Detection

The system automatically identifies:
- **Major Festivals**: Diwali, Holi, Navaratri, Ekadashi
- **Sankranti**: Solar transitions into zodiac signs
- **Amavasya/Purnima**: New and full moon observances
- **Regional Festivals**: Based on geographic location
- **Vrat Dates**: Fasting and observance days

### 📊 Hindu Calendar Elements

#### 🗓️ **Calendar Systems**
- **Vikram Samvat**: Traditional Hindu year (57 BC origin)
- **Shaka Samvat**: National calendar of India (78 AD origin)
- **Kali Yuga**: Cosmic age calculation (3102 BC start)

#### 🌙 **Lunar Months**
12 lunar months with Sanskrit names:
- Chaitra, Vaisakha, Jyeshtha, Ashadha
- Shravana, Bhadrapada, Ashvin, Kartik
- Margashirsha, Pausha, Magha, Phalguna

#### 🌗 **Paksha (Lunar Fortnight)**
- **Shukla Paksha**: Bright half (new moon to full moon)
- **Krishna Paksha**: Dark half (full moon to new moon)

### 🕐 Muhurta Analysis

**Muhurta** (मुहूर्त) represents auspicious timing for important activities:

#### ✅ **Auspicious Activities**
- **Shubha Muhurta**: Good for ceremonies, travel, business
- **Amrita Kala**: Nectar time - highly favorable
- **Brahma Muhurta**: Pre-dawn spiritual practice time

#### ⚠️ **Inauspicious Periods**
- **Rahu Kala**: Mars influence - avoid important starts
- **Yamaganda**: Death-related influence
- **Gulika Kala**: Saturn influence - delays possible

### 🎯 Panchanga Usage Examples

#### Basic Panchanga Display
```bash
# Today's Panchanga for Delhi
./bin/horoscope_cli --date 2024-01-15 --time 12:00:00 --lat 28.6139 --lon 77.2090 --timezone 5.5 --panchanga
```

#### Festival Calendar
```bash
# January 2024 festivals
./bin/horoscope_cli --panchanga-range 2024-01-01 2024-01-31 --lat 28.6139 --lon 77.2090 --timezone 5.5 --festivals-only
```

#### Export for Calendar Apps
```bash
# JSON format for app integration
./bin/horoscope_cli --panchanga-range 2024-01-01 2024-12-31 --lat 28.6139 --lon 77.2090 --timezone 5.5 --panchanga-format json
```

## 🇲🇲 Myanmar Calendar System

The **Myanmar Calendar** (မြန်မာ့ပြက္ခဒိန်) is a traditional lunar-solar calendar system used in Myanmar (Burma), integrating Buddhist astronomical principles with practical agricultural and religious observances.

### 📅 **Myanmar Calendar Structure**

#### 🏛️ **Era Systems**
- **Myanmar Era (ME)**: Traditional Myanmar calendar starting from 638 AD
- **Sasana Era (SE)**: Buddhist Era starting from Buddha's Enlightenment (544 BC)
- **Calculation**: ME Year + 638 = Gregorian AD Year

#### 🌙 **Lunar-Solar Integration**
- **12 Regular Months**: Based on lunar cycles
- **Watat (Leap) Years**: Extra month added every 2-3 years
- **Month Names**: Tagu, Kason, Nayon, Waso, Wagaung, Tawthalin, Thadingyut, Tazaungmone, Nadaw, Pyatho, Tabodwe, Tabaung

#### 🌗 **Moon Phases & Fortnights**
- **Waxing Fortnight**: လဆန်း (La Hsan) - 1st to 15th
- **Waning Fortnight**: လဆုတ် (La Hsote) - 1st to 14th/15th
- **Full Moon Day**: လပြည့် (La Pyei) - Most auspicious
- **New Moon Day**: လကွယ် (La Gwe) - Spiritual significance

### 🔮 **Astrological Days & Observances**

#### ✅ **Auspicious Days (Thamanyo)**
- **Definition**: Days favorable for important activities
- **Activities**: Marriages, business openings, travel
- **Calculation**: Based on weekday, moon phase, and planetary positions

#### ⚠️ **Inauspicious Days**

##### 🕳️ **Yatyaza (ရက်တစ်ရာ)**
- **Meaning**: "One in a hundred" - unlucky days
- **Restriction**: Avoid starting new ventures
- **Frequency**: Occurs on specific weekdays in certain months

##### 🌊 **Pyathada (ပြားထားတာ)**
- **Meaning**: "Widespread misfortune"
- **Nature**: Days of potential obstacles and delays
- **Avoidance**: Important ceremonies and contracts

#### 🙏 **Buddhist Sabbath Days**

##### 📿 **Uposatha (ဥပုသ်)**
- **Full Moon Sabbath**: Most significant religious observance
- **New Moon Sabbath**: Secondary observance day
- **Activities**: Meditation, temple visits, precept observance
- **Frequency**: Twice monthly on full and new moons

##### 🌅 **Sabbath Eve**
- **Day Before**: Preparation for sabbath observance
- **Significance**: Begin spiritual practices
- **Community**: Temple activities and gatherings

### 🎯 Myanmar Calendar Usage Examples

#### Basic Myanmar Calendar Display
```bash
# Today's Myanmar calendar for Yangon
./bin/horoscope_cli --date 2024-01-15 --time 12:00:00 --lat 16.8661 --lon 96.1951 --timezone 6.5 --myanmar-calendar
```

#### Astrological Days Only
```bash
# Show only astrological days and events
./bin/horoscope_cli --myanmar-calendar-range 2024-01-01 2024-01-31 --lat 16.8661 --lon 96.1951 --timezone 6.5 --astrological-days-only
```

#### Export Options
```bash
# JSON format for app integration
./bin/horoscope_cli --myanmar-calendar-range 2024-01-01 2024-12-31 --lat 16.8661 --lon 96.1951 --timezone 6.5 --myanmar-calendar-format json

# CSV format for analysis
./bin/horoscope_cli --myanmar-calendar-range 2024-01-01 2024-03-31 --lat 16.8661 --lon 96.1951 --timezone 6.5 --myanmar-calendar-format csv
```

## 🔍 Advanced Calendar Search Features

### 🕉️ **Hindu Calendar Search**

The Hindu calendar search feature allows you to find specific dates matching traditional Hindu calendar criteria across any date range.

#### 🎯 **Search Capabilities**
- **Tithi Search**: Find specific lunar days (Purnima, Amavasya, etc.)
- **Nakshatra Search**: Search by lunar mansions
- **Yoga Search**: Find specific planetary combinations
- **Festival Search**: Locate religious observances
- **Multiple Criteria**: Combine different search parameters

#### 🔍 **Search Examples**

##### Find All Full Moons (Purnima) in 2025
```bash
./bin/horoscope_cli --hindu-search 2025-01-01 2025-12-31 \
  --search-tithi 15 \
  --lat 28.6139 --lon 77.2090 \
  --hindu-search-format table
```

##### Find Ekadashi Days (11th Tithi) in March 2025
```bash
./bin/horoscope_cli --hindu-search 2025-03-01 2025-03-31 \
  --search-tithi 11 \
  --lat 19.0760 --lon 72.8777 \
  --hindu-search-format list
```

##### Search for Specific Month and Tithi Range
```bash
./bin/horoscope_cli --hindu-search 2025-01-01 2025-12-31 \
  --search-month 5 --search-tithi-range 14 16 \
  --lat 28.6139 --lon 77.2090 \
  --hindu-search-format csv
```

### 🇲🇲 **Myanmar Calendar Search**

Advanced search functionality for Myanmar calendar system with Buddhist astronomical principles.

#### 🔍 **Search Capabilities**
- **Moon Phase Search**: Find specific lunar phases
- **Sabbath Search**: Locate Buddhist observance days
- **Astrological Day Search**: Find auspicious/inauspicious days
- **Month Search**: Search by Myanmar months
- **Fortnight Day Search**: Search by specific days in lunar fortnight

#### 🎯 **Search Examples**

##### Find All Full Moon Sabbath Days in 2025
```bash
./bin/horoscope_cli --myanmar-search 2025-01-01 2025-12-31 \
  --myanmar-search-moon-phase 1 \
  --myanmar-search-sabbath \
  --lat 16.8661 --lon 96.1951 \
  --myanmar-search-format table
```

##### Find Auspicious Days (Thamanyo) in Waso Month
```bash
./bin/horoscope_cli --myanmar-search 2025-06-01 2025-08-31 \
  --myanmar-search-month 4 \
  --myanmar-search-thamanyo \
  --lat 16.8661 --lon 96.1951 \
  --myanmar-search-format json
```

##### Search for Yatyaza (Inauspicious) Days
```bash
./bin/horoscope_cli --myanmar-search 2025-01-01 2025-03-31 \
  --myanmar-search-yatyaza \
  --lat 16.8661 --lon 96.1951 \
  --myanmar-search-format list
```

## 🌟 Astro Calendar System (Combined Calendars)

The **Astro Calendar** combines Gregorian, Hindu Panchanga, and Myanmar calendar systems into a unified, comprehensive temporal framework showing festivals, planetary movements, and astrological significance.

### 🎯 **Unified Calendar Features**

#### 📅 **Three Calendar Integration**
- **Gregorian Calendar**: International standard dating
- **Hindu Panchanga**: Traditional Vedic calendar with festivals
- **Myanmar Calendar**: Buddhist lunar-solar system
- **Planetary Positions**: Real-time astronomical data
- **Festival Correlation**: Shows festivals from all traditions

#### 🎨 **Display Formats**
- **Calendar Layout**: Beautiful monthly calendar grid
- **Table Format**: Detailed tabular information
- **Professional Format**: Saint John Astronomy Club style with KP
- **JSON/CSV**: Data export for integration

### 🌟 **Professional Astro Calendar Examples**

#### Monthly Astro Calendar
```bash
# January 2024 comprehensive calendar
./bin/horoscope_cli --astro-calendar-monthly 2024-01 \
  --lat 28.6139 --lon 77.2090 \
  --astro-calendar-format calendar \
  --all-festivals
```

#### Professional Format with Planetary Transitions
```bash
# Professional astrology format with KP
./bin/horoscope_cli --astro-calendar-monthly 2024-03 \
  --lat 40.7128 --lon -74.0060 \
  --astro-calendar-format professional \
  --planetary-transitions
```

## 🌍 Location Management System

The **Location Management System** provides convenient city-name based coordinate lookup, eliminating the need to manually specify latitude, longitude, and timezone for major world cities.

### 🏙️ **Predefined Locations**

The system includes major cities worldwide with accurate coordinates and timezone information:

#### 🌎 **Americas**
- **New York**: `40.7128°N, 74.0060°W, UTC-5`
- **Los Angeles**: `34.0522°N, 118.2437°W, UTC-8`
- **Chicago**: `41.8781°N, 87.6298°W, UTC-6`
- **Toronto**: `43.6532°N, 79.3832°W, UTC-5`

#### 🌍 **Europe**
- **London**: `51.5074°N, 0.1278°W, UTC+0`
- **Paris**: `48.8566°N, 2.3522°E, UTC+1`
- **Berlin**: `52.5200°N, 13.4050°E, UTC+1`
- **Rome**: `41.9028°N, 12.4964°E, UTC+1`

#### 🌏 **Asia**
- **Tokyo**: `35.6762°N, 139.6503°E, UTC+9`
- **Mumbai**: `19.0760°N, 72.8777°E, UTC+5.5`
- **Delhi**: `28.6139°N, 77.2090°E, UTC+5.5`
- **Shanghai**: `31.2304°N, 121.4737°E, UTC+8`

#### 🌏 **Other Regions**
- **Sydney**: `-33.8688°S, 151.2093°E, UTC+10`
- **Cairo**: `30.0444°N, 31.2357°E, UTC+2`
- **Johannesburg**: `-26.2041°S, 28.0473°E, UTC+2`

### 🔍 **Location Search Features**

#### City Name Usage
```bash
# Use city name instead of coordinates
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 --location "New York"

# Birth chart using location name
./bin/horoscope_cli --date 1985-06-20 --time 09:15:30 --location "London" --chart-style north-indian
```

#### Search for Locations
```bash
# Find all locations matching search term
./bin/horoscope_cli --search-location "Paris"

# Search with partial names
./bin/horoscope_cli --search-location "New"
```

#### List All Available Locations
```bash
# Show all predefined cities
./bin/horoscope_cli --list-locations
```

### 🎯 **Location Usage Examples**

#### Quick Birth Charts with City Names
```bash
# New York birth - no coordinates needed
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 --location "New York"

# London birth with Vedic chart
./bin/horoscope_cli --date 1985-06-20 --time 09:15:30 --location "London" --chart-style north-indian

# Tokyo birth with JSON output
./bin/horoscope_cli --date 1992-03-20 --time 18:45:00 --location "Tokyo" --output json
```

#### Location Discovery
```bash
# Find cities containing "York"
./bin/horoscope_cli --search-location "York"

# Find Asian cities
./bin/horoscope_cli --search-location "Tokyo"
./bin/horoscope_cli --search-location "Mumbai"
./bin/horoscope_cli --search-location "Shanghai"
```

## ⚡ Advanced Features

### 🌙 Eclipse Calculations

The application uses authentic Swiss Ephemeris eclipse algorithms to calculate:

#### 🌞 **Solar Eclipses**
- **Total Solar Eclipse** ⚫ - Moon completely covers the Sun
- **Partial Solar Eclipse** 🌓 - Moon partially covers the Sun
- **Annular Solar Eclipse** 💍 - Moon appears smaller, creating "ring of fire"
- **Hybrid Solar Eclipse** 🔄 - Changes between total and annular

#### 🌚 **Lunar Eclipses**
- **Total Lunar Eclipse** 🔴 - Moon passes through Earth's umbra
- **Partial Lunar Eclipse** 🌗 - Moon partially passes through umbra
- **Penumbral Lunar Eclipse** 🌫️ - Moon passes through Earth's penumbra

#### 📍 **Location-Specific Features**
- **Eclipse Magnitude**: Percentage of eclipse coverage
- **Visibility Analysis**: Whether eclipse is observable from coordinates
- **Contact Times**: Beginning, maximum, and end times
- **Path Calculations**: Eclipse visibility maps

### 🪐 Planetary Conjunction Analysis

#### 🎯 **Conjunction Detection**
- **Configurable Orbs**: Set maximum degrees (default: 3°)
- **Multiple Planet Types**: Major planets, lunar nodes, asteroids
- **Temporal Precision**: Exact timing to the minute
- **Applying/Separating**: Movement direction analysis

#### 📊 **Conjunction Types**
- **Exact Conjunctions** (0° orb)
- **Close Conjunctions** (1-3° orb)
- **Wide Conjunctions** (3-8° orb)
- **Stelliums**: Multiple planet groupings

#### 🌐 **Advanced Filtering Options**

##### 📏 **Latitude Range Filtering**
Filter conjunctions based on planetary ecliptic latitudes:
- **Purpose**: Find planets aligned in the same orbital plane
- **Range**: -90° (South) to +90° (North)
- **Use Cases**:
  - `--conjunction-latitude-range -2.0 2.0`: Planets close to ecliptic
  - `--conjunction-latitude-range -0.5 0.5`: Very tight alignment
  - `--conjunction-latitude-range 5.0 15.0`: High-latitude interactions

##### ⚔️ **Graha Yuddha (Planetary Wars)**
Traditional Vedic analysis of very close planetary conjunctions:

**🏛️ War Conditions:**
- **Maximum Orb**: < 1° separation (configurable)
- **Participants**: Only visible planets (Mercury, Venus, Mars, Jupiter, Saturn)
- **Exclusions**: Sun, Moon, nodes, and outer planets don't participate

**🏆 Winner Determination Rules:**
1. **Primary**: Planet with higher ecliptic latitude (more southward) wins
2. **Secondary**: If latitudes equal, brighter (closer) planet wins
3. **Tertiary**: Traditional hierarchy → Jupiter > Saturn > Mars > Venus > Mercury

**✨ Astrological Effects:**
- **Winner**: Planet's significations are enhanced and empowered
- **Loser**: Planet's significations are weakened or modified
- **Duration**: Effects last during the close approach period

**🔮 Traditional Interpretations:**
- **Jupiter Wins**: Enhances wisdom, righteousness, and fortune
- **Saturn Wins**: Brings discipline, delays, and karmic lessons
- **Mars Wins**: Increases energy, courage, and potential conflicts
- **Venus Wins**: Enhances beauty, arts, and relationships
- **Mercury Wins**: Strengthens intellect, communication, and commerce

### ⭐ KP System (Krishnamurti Paddhati)

#### 🏗️ **5-Level Sub-Lord Hierarchy**
1. **Sign Lord** 👑 - Traditional zodiac sign ruler
2. **Star Lord** ⭐ - Nakshatra ruler (27 lunar mansions)
3. **Sub Lord** 🔍 - First-level subdivision (2°13'20")
4. **Sub-Sub Lord** 🔬 - Second-level subdivision
5. **Sub³ Lord** ⚛️ - Third-level subdivision (highest precision)

#### 📈 **KP Transitions & Timing**
- **Dasha Periods**: Major planetary periods
- **Bhukti Periods**: Sub-periods within dashas
- **Antardasha**: Sub-sub periods for precise timing
- **Event Prediction**: Based on sub-lord changes

#### 🎯 **KP Applications**
- **Horary Astrology**: Answering specific questions
- **Event Timing**: When will something happen?
- **Result Prediction**: Will the event be favorable?
- **Remedy Suggestions**: Planetary strengthening methods

### 📅 Ancient Date Support

Calculate charts for any historical period:

#### 🏛️ **Historical Calculations**
```bash
# Julius Caesar's assassination (44 BC)
./bin/horoscope_cli --date -0044-03-15 --time 12:00:00 --lat 41.9028 --lon 12.4964 --timezone 1

# Buddha's birth (563 BC)
./bin/horoscope_cli --date -0563-04-15 --time 06:00:00 --lat 27.4728 --lon 83.4142 --timezone 5.75

# Ancient Babylon (2000 BC)
./bin/horoscope_cli --date -2000-01-01 --time 12:00:00 --lat 32.5355 --lon 44.4275 --timezone 3
```

#### ⏰ **Date Format for BC**
- Use negative years: `-0044` for 44 BC
- Standard format: `-YYYY-MM-DD`
- Valid range: Approximately 6000 BC to 7000 AD

### 🌌 **Heliocentric & Planetary-Centric Systems**

The horoscope_cli tool revolutionizes astrological analysis by offering multiple **cosmic perspectives** beyond traditional Earth-centered calculations.

#### 🎯 **Perspective System Applications**

##### ☀️ **Heliocentric Applications**
- **🔬 Scientific Astrology**: True astronomical relationships without Earth's observational bias
- **🌊 Evolutionary Astrology**: Soul-level development and cosmic consciousness
- **📊 Research Applications**: Studying planetary cycles and natural rhythms
- **🎯 Pure Aspect Analysis**: Direct planet-to-planet relationships

##### 🪐 **Planetary-Centric Applications**

###### ♂ **Mars-Centric Uses**
- **⚔️ Warrior Training**: Military strategy and assertiveness development
- **🏃‍♂️ Athletic Performance**: Sports timing and competitive analysis
- **💪 Energy Management**: Understanding drive and motivation cycles
- **🎯 Action Planning**: Optimal timing for initiating projects

###### ♃ **Jupiter-Centric Uses**
- **📚 Educational Timing**: Best periods for learning and teaching
- **🙏 Spiritual Development**: Religious and philosophical growth periods
- **💰 Business Expansion**: Optimal timing for growth and investment
- **🌍 Travel Planning**: Auspicious periods for journeys and exploration

###### ♄ **Saturn-Centric Uses**
- **💼 Career Planning**: Professional development and responsibility phases
- **🏗️ Structure Building**: Creating lasting foundations and systems
- **📜 Karmic Analysis**: Understanding life lessons and challenges
- **⏰ Long-term Planning**: Multi-year goal setting and achievement

#### 🔬 **Technical Advantages of Multi-Perspective Analysis**

##### 📐 **Coordinate Precision**
- **Heliocentric**: True orbital mechanics and planetary relationships
- **Geocentric**: Traditional astrological interpretation with Earth reference
- **Planetary-Centric**: Specialized viewpoints for targeted analysis

##### ⚡ **Computational Benefits**
- **No Retrograde Confusion**: Heliocentric eliminates apparent retrograde motion
- **True Distances**: Accurate inter-planetary spacing and relationships
- **Orbital Harmonics**: Understanding of natural planetary cycles
- **Gravitational Influences**: Perspective-based gravitational effect analysis

#### 🎭 **Interpretive Differences**

##### 🌍 **Geocentric vs Heliocentric**
```bash
# Compare same birth chart from Earth vs Sun perspective
DATE="1990-01-15"
TIME="14:30:00"
LOCATION="New York"

echo "Earth-Centered (Traditional):"
./bin/horoscope_cli --date $DATE --time $TIME --location "$LOCATION" --perspective geocentric

echo "Sun-Centered (Natural):"
./bin/horoscope_cli --date $DATE --time $TIME --location "$LOCATION" --perspective heliocentric
```

##### 🪐 **Planetary Perspective Insights**
- **Mars Perspective**: Reveals how other planets support or challenge personal drive
- **Jupiter Perspective**: Shows expansion opportunities and wisdom development
- **Saturn Perspective**: Illuminates structural challenges and karmic lessons

#### 🌟 **Advanced Research Applications**

##### 📊 **Comparative Analysis**
```bash
# Generate multi-perspective dataset for research
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 --location "New York" --perspective geocentric --output json > geo_data.json
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 --location "New York" --perspective heliocentric --output json > helio_data.json
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 --location "New York" --perspective mars-centric --output json > mars_data.json
```

##### 🔍 **Pattern Recognition**
- **Heliocentric Cycles**: Natural planetary return periods
- **Perspective Correlations**: How different viewpoints reveal hidden patterns
- **Multi-Dimensional Analysis**: Combining perspectives for comprehensive insights
## 📊 Output Formats

### 📄 Text Format (Default)
**Human-readable charts with beautiful ASCII art and formatted tables**

#### Features:
- 🎨 **Visual Charts**: ASCII art representations of birth charts
- 📋 **Formatted Tables**: Organized planetary positions and aspects
- 🌈 **Color Support**: Terminal colors for better readability
- 📐 **Precise Calculations**: Degrees, minutes, and house positions

```
Birth Chart for 1990-01-15 14:30:00 UTC-5
Location: 40.7128°N, 74.0060°W

=== 🪐 PLANETARY POSITIONS ===
Sun:     ☉  25°42' ♑ Capricorn    (House 5)
Moon:    ☽  12°18' ♏ Scorpio      (House 3)
Mercury: ☿  05°33' ♒ Aquarius     (House 6)
Venus:   ♀  14°27' ♓ Pisces       (House 7)
Mars:    ♂  08°15' ♐ Sagittarius  (House 4)
```

### 📊 JSON Format
**Complete structured data for API integration and web applications**

#### Advantages:
- 🔧 **API Ready**: Perfect for web services and mobile apps
- 📱 **Integration Friendly**: Easy to parse in any programming language
- 🗄️ **Database Storage**: Structured for efficient data storage
- 🔄 **Data Exchange**: Standard format for astrological data sharing

```json
{
  "birthData": {
    "date": "1990-01-15",
    "time": "14:30:00",
    "latitude": 40.712800,
    "longitude": -74.006000,
    "timezone": -5.000000,
    "location": "New York, NY"
  },
  "planets": [
    {
      "name": "Sun",
      "symbol": "☉",
      "longitude": 295.70,
      "sign": "Capricorn",
      "degree": 25,
      "minute": 42,
      "house": 5,
      "retrograde": false
    }
  ],
  "houses": {
    "system": "Placidus",
    "cusps": [120.45, 150.23, 180.67, ...]
  },
  "aspects": [
    {
      "planet1": "Sun",
      "planet2": "Moon",
      "aspect": "Sextile",
      "orb": 2.34,
      "applying": true
    }
  ],
  "panchanga": {
    "tithi": "Panchami",
    "vara": "Monday",
    "nakshatra": "Purva Phalguni",
    "yoga": "Saubhagya",
    "karana": "Taitila"
  }
}
```

### 📈 CSV Format
**Tabular data perfect for spreadsheet analysis and data science**

#### Use Cases:
- 📊 **Spreadsheet Analysis**: Excel, Google Sheets, LibreOffice
- 📈 **Data Science**: Python pandas, R, statistical analysis
- 🔍 **Pattern Recognition**: Large dataset analysis
- 📋 **Report Generation**: Automated report systems

```csv
Date,Time,Planet,Sign,Degree,Minute,House,Longitude
1990-01-15,14:30:00,Sun,Capricorn,25,42,5,295.70
1990-01-15,14:30:00,Moon,Scorpio,12,18,3,222.30
1990-01-15,14:30:00,Mercury,Aquarius,5,33,6,305.55
```

### 🎯 Format Selection Guide

| **Use Case** | **Recommended Format** | **Why** |
|--------------|----------------------|---------|
| 👁️ **Visual Analysis** | Text | Beautiful charts and readable layout |
| 🌐 **Web Development** | JSON | Easy API integration and parsing |
| 📊 **Data Analysis** | CSV | Spreadsheet compatibility and statistics |
| 🖨️ **Documentation** | Text | Professional reports and consultations |
| 📱 **Mobile Apps** | JSON | Structured data for app development |
| 🔬 **Research** | CSV | Large dataset analysis and patterns |

## 💡 Examples & Use Cases

### 🎯 Basic Birth Chart Examples

#### 🏙️ **Using Location Names**
```bash
# New York birth - automatic coordinates
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 --location "New York"

# London birth with Koch houses
./bin/horoscope_cli --date 1985-06-20 --time 09:15:30 --location "London" --house-system K

# Tokyo birth with JSON output
./bin/horoscope_cli --date 1992-03-20 --time 18:45:00 --location "Tokyo" --output json

# Mumbai birth with North Indian chart
./bin/horoscope_cli --date 1995-08-15 --time 11:30:00 --location "Mumbai" --chart-style north-indian
```

#### 🌍 **Major World Cities (Manual Coordinates)**

```bash
# New York City (EST)
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 --lat 40.7128 --lon -74.0060 --timezone -5

# London (GMT)
./bin/horoscope_cli --date 1985-06-20 --time 09:15:30 --lat 51.5074 --lon -0.1278 --timezone 0

# Mumbai (IST)
./bin/horoscope_cli --date 1995-03-10 --time 18:45:00 --lat 19.0760 --lon 72.8777 --timezone 5.5

# Tokyo (JST)
./bin/horoscope_cli --date 1988-12-25 --time 23:30:00 --lat 35.6762 --lon 139.6503 --timezone 9

# Sydney (AEDT)
./bin/horoscope_cli --date 1992-07-04 --time 11:20:00 --lat -33.8688 --lon 151.2093 --timezone 10
```

#### 🏠 **Different House Systems**

```bash
# Placidus (Default - Most Popular)
./bin/horoscope_cli --date 1985-06-20 --time 09:15:30 --location "London" --house-system P

# Koch (German Precision)
./bin/horoscope_cli --date 1985-06-20 --time 09:15:30 --location "London" --house-system K

# Equal House (Simple Division)
./bin/horoscope_cli --date 1985-06-20 --time 09:15:30 --location "London" --house-system E

# Whole Sign (Traditional Vedic)
./bin/horoscope_cli --date 1985-06-20 --time 09:15:30 --location "London" --house-system W
```

### 🎨 Chart Style Comparisons

#### 🇮🇳 **Vedic Chart Styles**

```bash
# North Indian (Diamond Shape)
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 --location "Delhi" --chart-style north-indian

# South Indian (Fixed Square)
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 --location "Delhi" --chart-style south-indian

# East Indian (Bengali/Oriya Style)
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 --location "Delhi" --chart-style east-indian

# Compare All Styles
./examples/chart_comparison.sh 1990-01-15 14:30:00 28.6139 77.2090 5.5
```

#### 🌍 **Western vs Vedic Comparison**
```bash
# Western Tropical
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 --location "New York" --chart-style western

# North Indian Sidereal
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 --location "New York" --chart-style north-indian
```

### 🏛️ **Historical Chart Examples (BC Era)**

#### Ancient Civilizations
```bash
# Julius Caesar's assassination (44 BC) - Rome
./bin/horoscope_cli --date 44BC-03-15 --time 12:00:00 --lat 41.9028 --lon 12.4964 --timezone 1

# Cleopatra VII birth (69 BC) - Alexandria
./bin/horoscope_cli --date 69BC-01-15 --time 18:00:00 --lat 31.2001 --lon 29.9187 --timezone 2

# Buddha's birth (563 BC) - Lumbini
./bin/horoscope_cli --date 563BC-04-15 --time 06:00:00 --lat 27.4728 --lon 83.4142 --timezone 5.75

# Ancient Babylon (2000 BC)
./bin/horoscope_cli --date 2000BC-01-01 --time 12:00:00 --lat 32.5355 --lon 44.4275 --timezone 3
```

### 🏛️ **Buddha Birth Date Finder Script**

One of the most fascinating applications of the horoscope_cli tool is historical astronomical research. The included Buddha birth date finder script (`examples/buddha_birth_date_finder.sh`) demonstrates advanced calendar search techniques to find the most likely astronomical date for Buddha's birth.

#### 🔍 **Historical Research Methodology**
The script combines traditional Buddhist accounts with modern astronomical calculations:

- **📅 Traditional Account**: Vaishakha month, Purnima (Full Moon), Friday
- **📍 Location**: Lumbini, Nepal (27.4833°N, 83.2767°E)
- **⏰ Time Period**: Approximately 600-500 BCE
- **🔬 Method**: Hindu calendar search with multiple output formats

#### 🌟 **Key Search Examples from the Script**

##### Find Buddha Purnima Candidates (563 BCE)
```bash
# Search for all Purnima (Full Moon) dates in 563 BCE
./bin/horoscope_cli --hindu-search 563BC-01-01 563BC-12-31 \
  --search-tithi 15 \
  --lat 27.4833 --lon 83.2767 --timezone 5.75 \
  --hindu-search-format table

# CSV format for data analysis
./bin/horoscope_cli --hindu-search 563BC-01-01 563BC-12-31 \
  --search-tithi 15 \
  --lat 27.4833 --lon 83.2767 --timezone 5.75 \
  --hindu-search-format csv
```

##### Multi-Year Comparison (566-560 BCE)
```bash
# Compare Buddha Purnima dates across traditional year candidates
for year in 566 565 564 563 562 561 560; do
  echo "Year ${year} BCE:"
  ./bin/horoscope_cli --hindu-search ${year}BC-04-01 ${year}BC-06-01 \
    --search-tithi 15 \
    --lat 27.4833 --lon 83.2767 --timezone 5.75 \
    --hindu-search-format list
done
```

##### Friday Purnima Search (Specific Day + Lunar Phase)
```bash
# Find Friday + Purnima combinations in 563 BCE
./bin/horoscope_cli --hindu-search 563BC-01-01 563BC-12-31 \
  --search-tithi 15 --search-weekday 5 \
  --lat 27.4833 --lon 83.2767 --timezone 5.75 \
  --hindu-search-format json
```

#### 🎯 **Running the Complete Script**
```bash
# Execute the comprehensive Buddha birth finder analysis
cd examples
chmod +x buddha_birth_date_finder.sh
./buddha_birth_date_finder.sh

# The script includes:
# - Traditional accounts analysis
# - Multi-year date comparison
# - CSV data export for research
# - List format for scripting
# - Day-of-week correlations
# - Astronomical event analysis
```

#### 📊 **Research Output Examples**

The script generates multiple output formats for different research needs:

**Table Format**: Detailed Hindu calendar information with festival context
**CSV Format**: Structured data for spreadsheet analysis and statistical research
**JSON Format**: Machine-readable data for integration with other tools
**List Format**: Clean date lists perfect for shell scripting and automation

#### 🔬 **Advanced Historical Research Applications**
```bash
# Find eclipse correlations during Buddha's era
./bin/horoscope_cli --eclipse-range 563BC-01-01 563BC-12-31 \
  --lat 27.4833 --lon 83.2767 --timezone 5.75

# Search for planetary conjunctions during traditional birth period
./bin/horoscope_cli --conjunction-range 563BC-04-01 563BC-06-01 \
  --lat 27.4833 --lon 83.2767 --timezone 5.75 --conjunction-orb 5.0

# Combine calendar search with astronomical events
./bin/horoscope_cli --hindu-search 563BC-01-01 563BC-12-31 \
  --search-month 2 --search-tithi 15 \
  --lat 27.4833 --lon 83.2767 --timezone 5.75 \
  --hindu-search-format json > buddha_candidates_563bc.json
```

### 📜 **Additional Example Scripts**

The `examples/` directory contains several powerful scripts demonstrating advanced features:

#### 📖 **Calendar Search Quick Reference**
```bash
# Run the comprehensive calendar search demo
cd examples
./calendar_search_quick_reference.sh

# Features demonstrated:
# - Hindu calendar search examples
# - Myanmar calendar search examples
# - Multiple output formats
# - Search parameter combinations
```

#### 🌏 **Combined Calendar Search Examples**
```bash
# Compare Hindu and Myanmar calendar systems
cd examples
./combined_calendar_search_examples.sh

# Shows:
# - Side-by-side calendar comparisons
# - Different location calculations
# - Cross-cultural festival correlations
# - Export formats for research
```

#### 🎨 **Chart Comparison Script**
```bash
# Compare different chart styles for same birth data
cd examples
./chart_comparison.sh 1990-01-15 14:30:00 28.6139 77.2090 5.5

# Generates:
# - Western chart style
# - North Indian Vedic style
# - South Indian Vedic style
# - East Indian/Bengali style
# - Side-by-side comparisons
```

#### 🇮🇳 **Specialized Regional Examples**
```bash
# Hindu calendar search examples (comprehensive)
cd examples
./hindu_calendar_search_examples.sh

# Myanmar calendar search examples
cd examples
./myanmar_calendar_search_examples.sh

# East Indian chart examples (Bengali/Oriya style)
cd examples
./east_indian_examples.sh

# Eastern chart examples (all Vedic styles)
cd examples
./eastern_chart_examples.sh
```

### 🕉️ Hindu Panchanga Examples

#### 📅 **Daily Panchanga**
```bash
# Today's Panchanga for Delhi
./bin/horoscope_cli --date 2024-01-15 --time 12:00:00 --location "Delhi" --panchanga

# Panchanga with Muhurta analysis
./bin/horoscope_cli --date 2024-01-15 --time 12:00:00 --location "Delhi" --panchanga --muhurta-analysis
```

#### 🎉 **Festival Calendar**
```bash
# This month's festivals only
./bin/horoscope_cli --panchanga-range 2024-01-01 2024-01-31 --location "Delhi" --festivals-only

# Full year Hindu calendar
./bin/horoscope_cli --panchanga-range 2024-01-01 2024-12-31 --location "Delhi" --panchanga-format table
```

#### 📊 **Export Options**
```bash
# JSON for app integration
./bin/horoscope_cli --date 2024-01-15 --time 12:00:00 --location "Delhi" --panchanga --panchanga-format json > panchanga.json

# CSV for spreadsheet
./bin/horoscope_cli --panchanga-range 2024-01-01 2024-01-31 --location "Delhi" --panchanga-format csv > panchanga_january.csv
```

### 🇲🇲 **Myanmar Calendar Examples**

#### 📅 **Daily Myanmar Calendar**
```bash
# Today's Myanmar calendar for Yangon
./bin/horoscope_cli --date 2024-01-15 --time 12:00:00 --lat 16.8661 --lon 96.1951 --timezone 6.5 --myanmar-calendar

# Range with astrological days only
./bin/horoscope_cli --myanmar-calendar-range 2024-01-01 2024-01-31 --lat 16.8661 --lon 96.1951 --timezone 6.5 --astrological-days-only
```

#### 📊 **Export Formats**
```bash
# JSON format
./bin/horoscope_cli --myanmar-calendar-range 2024-01-01 2024-03-31 --lat 16.8661 --lon 96.1951 --timezone 6.5 --myanmar-calendar-format json

# CSV format for analysis
./bin/horoscope_cli --myanmar-calendar-range 2024-01-01 2024-12-31 --lat 16.8661 --lon 96.1951 --timezone 6.5 --myanmar-calendar-format csv
```

### 🔍 **Calendar Search Examples**

#### 🕉️ **Hindu Calendar Search**
```bash
# Find all Purnima (Full Moon) days in 2025
./bin/horoscope_cli --hindu-search 2025-01-01 2025-12-31 --search-tithi 15 --location "Delhi" --hindu-search-format table

# Find Ekadashi days (11th Tithi) in March 2025
./bin/horoscope_cli --hindu-search 2025-03-01 2025-03-31 --search-tithi 11 --location "Mumbai" --hindu-search-format list

# Search for specific month and tithi range
./bin/horoscope_cli --hindu-search 2025-01-01 2025-12-31 --search-month 5 --search-tithi-range 14 16 --location "Delhi" --hindu-search-format csv

# Find all Sundays with Purnima in 2025
./bin/horoscope_cli --hindu-search 2025-01-01 2025-12-31 --search-weekday 0 --search-tithi 15 --location "Delhi" --hindu-search-format json
```

#### 🇲🇲 **Myanmar Calendar Search**
```bash
# Find all Full Moon Sabbath days in 2025
./bin/horoscope_cli --myanmar-search 2025-01-01 2025-12-31 --myanmar-search-moon-phase 1 --myanmar-search-sabbath --lat 16.8661 --lon 96.1951 --myanmar-search-format table

# Find auspicious days (Thamanyo) in Waso month
./bin/horoscope_cli --myanmar-search 2025-06-01 2025-08-31 --myanmar-search-month 4 --myanmar-search-thamanyo --lat 16.8661 --lon 96.1951 --myanmar-search-format json

# Search for inauspicious days (Yatyaza)
./bin/horoscope_cli --myanmar-search 2025-01-01 2025-03-31 --myanmar-search-yatyaza --lat 16.8661 --lon 96.1951 --myanmar-search-format list

# Find all Sabbath eve days
./bin/horoscope_cli --myanmar-search 2025-01-01 2025-12-31 --myanmar-search-sabbath-eve --lat 16.8661 --lon 96.1951 --myanmar-search-format csv
```

### 🌟 **Astro Calendar Examples**

#### 📅 **Monthly Astro Calendar**
```bash
# January 2024 comprehensive calendar
./bin/horoscope_cli --astro-calendar-monthly 2024-01 --location "Delhi" --astro-calendar-format calendar --all-festivals

# Professional format with planetary transitions
./bin/horoscope_cli --astro-calendar-monthly 2024-03 --location "New York" --astro-calendar-format professional --planetary-transitions

# Export for astrology software integration
./bin/horoscope_cli --astro-calendar-monthly 2024-06 --location "London" --astro-calendar-format json --all-festivals --planetary-transitions > june_2024_astro.json
```

#### 🌟 **Daily Astro Calendar**
```bash
# Today's comprehensive astro-calendar
./bin/horoscope_cli --date 2024-01-15 --time 12:00:00 --location "Delhi" --astro-calendar

# Astro-calendar with all systems combined
./bin/horoscope_cli --date 2024-01-15 --time 12:00:00 --location "Mumbai" --astro-calendar --astro-calendar-format table
```

### 🌙 Eclipse Analysis Examples

#### 🌞 **Solar Eclipse Tracking**
```bash
# Eclipses around birth using location name
./bin/horoscope_cli --date 1985-06-20 --time 09:15:30 --location "London" --eclipses

# 2024 Eclipse Season
./bin/horoscope_cli --eclipse-range 2024-01-01 2024-12-31 --location "New York"

# Historical Great American Eclipse 2017
./bin/horoscope_cli --eclipse-range 2017-08-20 2017-08-22 --lat 36.9741 --lon -109.0452 --timezone -7
```

#### 🏛️ **Historical Eclipses**
```bash
# Ancient Roman eclipses
./bin/horoscope_cli --eclipse-range 100BC-01-01 1BC-12-31 --lat 41.9028 --lon 12.4964 --timezone 1

# Medieval European eclipses
./bin/horoscope_cli --eclipse-range 1000-01-01 1099-12-31 --location "Paris"
```

### 🔍 **Eclipse Search Features**

The eclipse search functionality allows you to find specific types of eclipses across any date range, with detailed visibility information for specific locations.

#### 🌞 **Solar Eclipse Search Examples**
```bash
# Find all solar eclipses visible from New York in 2024
./bin/horoscope_cli --eclipse-range 2024-01-01 2024-12-31 --location "New York"

# Search for total solar eclipses in the 21st century (USA visibility)
./bin/horoscope_cli --eclipse-range 2001-01-01 2100-12-31 --lat 39.8283 --lon -98.5795 --timezone -6

# Find eclipses during Buddha's era (563 BCE)
./bin/horoscope_cli --eclipse-range 563BC-01-01 563BC-12-31 --lat 27.4833 --lon 83.2767 --timezone 5.75

# Great American Eclipse path (2017)
./bin/horoscope_cli --eclipse-range 2017-08-15 2017-08-25 --lat 36.9741 --lon -109.0452 --timezone -7
```

#### 🌚 **Lunar Eclipse Search Examples**
```bash
# Find all lunar eclipses visible from London in 2024-2025
./bin/horoscope_cli --eclipse-range 2024-01-01 2025-12-31 --location "London"

# Historical lunar eclipses during Roman Empire peak
./bin/horoscope_cli --eclipse-range 100-01-01 200-12-31 --location "Rome"

# Ancient Greek astronomical observations
./bin/horoscope_cli --eclipse-range 500BC-01-01 300BC-12-31 --lat 37.9755 --lon 23.7348 --timezone 2
```

#### 🔬 **Eclipse Research Applications**
```bash
# Export eclipse data for research (CSV format)
./bin/horoscope_cli --eclipse-range 1900-01-01 2000-12-31 --location "Greenwich" > 20th_century_eclipses.txt

# Find eclipses near significant historical dates
# Example: Eclipses around World War II
./bin/horoscope_cli --eclipse-range 1939-01-01 1945-12-31 --location "London"

# Ancient eclipse verification for historical dating
./bin/horoscope_cli --eclipse-range 585BC-05-01 585BC-06-01 --lat 39.9208 --lon 32.8541 --timezone 3
```

### 🪐 Conjunction Examples

#### 🎯 **Planetary Meetings**
```bash
# Great Conjunction 2020 (Jupiter-Saturn)
./bin/horoscope_cli --conjunction-range 2020-12-15 2020-12-25 --location "New York" --conjunction-orb 1.0

# Venus-Jupiter conjunctions (2024)
./bin/horoscope_cli --conjunction-range 2024-01-01 2024-12-31 --location "Delhi" --conjunction-orb 3.0

# Tight conjunctions only
./bin/horoscope_cli --conjunction-range 2024-01-01 2024-12-31 --location "New York" --conjunction-orb 0.5

# Conjunctions with latitude filtering (planets near ecliptic)
./bin/horoscope_cli --conjunction-range 2024-01-01 2024-12-31 --location "London" --conjunction-latitude-range -2.0 2.0

# Very precise orbital alignment (tight latitude range)
./bin/horoscope_cli --conjunction-range 2024-01-01 2024-12-31 --location "Tokyo" --conjunction-latitude-range -0.5 0.5
```

#### ⚔️ **Graha Yuddha (Planetary Wars)**
```bash
# Find all planetary wars in 2024
./bin/horoscope_cli --graha-yuddha-range 2024-01-01 2024-12-31 --location "Delhi"

# Planetary wars around birth date
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 --location "Mumbai" --graha-yuddha

# Very tight planetary wars (0.25° orb)
./bin/horoscope_cli --graha-yuddha-range 2024-01-01 2024-12-31 --location "New York" --graha-yuddha-orb 0.25

# Historical planetary wars during Renaissance
./bin/horoscope_cli --graha-yuddha-range 1400-01-01 1600-12-31 --location "Florence" --graha-yuddha-orb 0.5
```

### 🔍 **Conjunction Search Features**

The conjunction search functionality allows you to find planetary meetings across any time period, with configurable orbs and specific location calculations.

#### 🪐 **Advanced Conjunction Search Examples**
```bash
# Find all Jupiter-Saturn conjunctions in the 20th century
./bin/horoscope_cli --conjunction-range 1901-01-01 2000-12-31 --location "Greenwich" --conjunction-orb 5.0

# Search for Venus-Mars conjunctions (relationship aspects)
./bin/horoscope_cli --conjunction-range 2024-01-01 2024-12-31 --location "Paris" --conjunction-orb 3.0

# Very tight conjunctions (less than 1 degree) - rare events
./bin/horoscope_cli --conjunction-range 2020-01-01 2030-12-31 --location "Tokyo" --conjunction-orb 0.5

# Historical conjunction during Buddha's era
./bin/horoscope_cli --conjunction-range 563BC-01-01 563BC-12-31 --lat 27.4833 --lon 83.2767 --timezone 5.75 --conjunction-orb 4.0
```

#### 🌟 **Multiple Planet Conjunctions (Stelliums)**
```bash
# Find periods with multiple conjunctions (stelliums)
./bin/horoscope_cli --conjunction-range 2024-01-01 2024-12-31 --location "Mumbai" --conjunction-orb 8.0

# Rare triple conjunctions and stelliums
./bin/horoscope_cli --conjunction-range 2000-01-01 2025-12-31 --location "London" --conjunction-orb 6.0

# Ancient stellium during historical periods
./bin/horoscope_cli --conjunction-range 44BC-01-01 44BC-12-31 --location "Rome" --conjunction-orb 10.0
```

#### 🔬 **Conjunction Research Applications**
```bash
# Export conjunction data for astrological research
./bin/horoscope_cli --conjunction-range 1900-01-01 2000-12-31 --location "New York" --conjunction-orb 3.0 > 20th_century_conjunctions.txt

# Find conjunctions during major historical events
# Example: Conjunctions during Renaissance period
./bin/horoscope_cli --conjunction-range 1400-01-01 1600-12-31 --location "Florence" --conjunction-orb 5.0

# Personal conjunction analysis around birth
./bin/horoscope_cli --conjunction-range 1990-01-01 1990-12-31 --location "Delhi" --conjunction-orb 4.0
```

### ⭐ KP System Examples

#### 🔍 **Complete KP Analysis**
```bash
# Full KP table with 5 levels
./bin/horoscope_cli --date 1985-06-20 --time 09:15:30 --location "London" --kp-table

# KP with JSON export
./bin/horoscope_cli --date 1985-06-20 --time 09:15:30 --location "London" --kp-table --kp-format json
```

#### 📈 **Transition Tracking**
```bash
# Sun transitions for one week
./bin/horoscope_cli --kp-transitions --kp-transition-range 2025-01-01 2025-01-07 --kp-transition-planet SUN --kp-transition-level sub

# All planet transitions (one day)
./bin/horoscope_cli --kp-transitions --kp-transition-range 2025-01-15 2025-01-15 --kp-format csv
```

### 📊 Ephemeris Table Examples

#### 📅 **Monthly Ephemeris**
```bash
# January 2025 daily positions
./bin/horoscope_cli --ephemeris --ephemeris-range 2025-01-01 2025-01-31 --ephemeris-format table

# Weekly ephemeris in CSV
./bin/horoscope_cli --ephemeris --ephemeris-range 2025-01-01 2025-01-31 --ephemeris-interval 7 --ephemeris-format csv

# Annual overview (monthly intervals)
./bin/horoscope_cli --ephemeris --ephemeris-range 2025-01-01 2025-12-31 --ephemeris-interval 30 --ephemeris-format json
```

### 🌟 **Solar System Views & Perspective Examples**

#### ☀️ **Heliocentric Solar System**
```bash
# Current solar system from Sun's perspective (default)
./bin/horoscope_cli --solar-system

# Birth chart with heliocentric perspective
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 --location "New York" --chart-style solar-system --perspective heliocentric

# Compare birth with current heliocentric positions
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 --location "New York" --chart-style solar-system --perspective heliocentric --output json
```

#### 🌍 **Geocentric Solar System**
```bash
# Traditional Earth-centered perspective
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 --location "New York" --chart-style solar-system --perspective geocentric

# Geocentric with JSON output for comparison
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 --location "New York" --chart-style solar-system --perspective geocentric --output json
```

#### ♂ **Mars-Centric Perspective**
```bash
# View from Mars - warrior planet perspective
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 --location "New York" --chart-style solar-system --perspective mars-centric

# Mars perspective for action-oriented analysis
./bin/horoscope_cli --date 2024-07-20 --time 12:00:00 --location "Delhi" --chart-style solar-system --perspective mars-centric

# Historical Mars perspective (Mars in ancient times)
./bin/horoscope_cli --date 44BC-03-15 --time 12:00:00 --location "Rome" --chart-style solar-system --perspective mars-centric
```

#### ♃ **Jupiter-Centric Perspective**
```bash
# View from Jupiter - expansion and wisdom perspective
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 --location "New York" --chart-style solar-system --perspective jupiter-centric

# Jupiter perspective for educational/spiritual timing
./bin/horoscope_cli --date 2024-09-01 --time 09:00:00 --location "Tokyo" --chart-style solar-system --perspective jupiter-centric

# Jupiter perspective with birth data analysis
./bin/horoscope_cli --date 1985-06-20 --time 09:15:30 --location "London" --chart-style solar-system --perspective jupiter-centric --output json
```

#### ♄ **Saturn-Centric Perspective**
```bash
# View from Saturn - structure and discipline perspective
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 --location "New York" --chart-style solar-system --perspective saturn-centric

# Saturn perspective for career and life structure analysis
./bin/horoscope_cli --date 2024-01-01 --time 00:00:00 --location "London" --chart-style solar-system --perspective saturn-centric

# Historical Saturn perspective (ancient timing)
./bin/horoscope_cli --date 563BC-04-15 --time 06:00:00 --lat 27.4833 --lon 83.2767 --timezone 5.75 --chart-style solar-system --perspective saturn-centric
```

### 🔬 **Comparative Perspective Analysis**

#### 🌟 **All Perspectives Comparison Script**
```bash
#!/bin/bash
# Compare all perspectives for same birth data
DATE="1990-01-15"
TIME="14:30:00"
LOCATION="New York"

echo "🌍 GEOCENTRIC PERSPECTIVE:"
./bin/horoscope_cli --date $DATE --time $TIME --location "$LOCATION" --chart-style solar-system --perspective geocentric

echo -e "\n☀️ HELIOCENTRIC PERSPECTIVE:"
./bin/horoscope_cli --date $DATE --time $TIME --location "$LOCATION" --chart-style solar-system --perspective heliocentric

echo -e "\n♂ MARS-CENTRIC PERSPECTIVE:"
./bin/horoscope_cli --date $DATE --time $TIME --location "$LOCATION" --chart-style solar-system --perspective mars-centric

echo -e "\n♃ JUPITER-CENTRIC PERSPECTIVE:"
./bin/horoscope_cli --date $DATE --time $TIME --location "$LOCATION" --chart-style solar-system --perspective jupiter-centric

echo -e "\n♄ SATURN-CENTRIC PERSPECTIVE:"
./bin/horoscope_cli --date $DATE --time $TIME --location "$LOCATION" --chart-style solar-system --perspective saturn-centric
```

#### 📊 **JSON Export for Research**
```bash
# Export all perspectives in JSON format for analysis
DATE="1990-01-15"
TIME="14:30:00"
LOCATION="New York"

# Create perspective comparison dataset
./bin/horoscope_cli --date $DATE --time $TIME --location "$LOCATION" --chart-style solar-system --perspective geocentric --output json > geocentric_$DATE.json

./bin/horoscope_cli --date $DATE --time $TIME --location "$LOCATION" --chart-style solar-system --perspective heliocentric --output json > heliocentric_$DATE.json

./bin/horoscope_cli --date $DATE --time $TIME --location "$LOCATION" --chart-style solar-system --perspective mars-centric --output json > mars_centric_$DATE.json

./bin/horoscope_cli --date $DATE --time $TIME --location "$LOCATION" --chart-style solar-system --perspective jupiter-centric --output json > jupiter_centric_$DATE.json

./bin/horoscope_cli --date $DATE --time $TIME --location "$LOCATION" --chart-style solar-system --perspective saturn-centric --output json > saturn_centric_$DATE.json
```

### 🎯 **Specialized Applications**

#### 🔬 **Astronomical Research**
```bash
# Heliocentric positions for scientific research
./bin/horoscope_cli --ephemeris --ephemeris-range 2024-01-01 2024-12-31 --ephemeris-format csv --chart-style solar-system --perspective heliocentric

# Compare planetary positions from different viewpoints
./bin/horoscope_cli --date 2024-07-20 --time 12:00:00 --location "Greenwich" --chart-style solar-system --perspective heliocentric --output json
```

#### 🎭 **Astrological Analysis**
```bash
# Mars-centric for analyzing assertiveness and drive
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 --location "New York" --chart-style solar-system --perspective mars-centric

# Jupiter-centric for spiritual and educational timing
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 --location "New York" --chart-style solar-system --perspective jupiter-centric

# Saturn-centric for career and life structure planning
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 --location "New York" --chart-style solar-system --perspective saturn-centric
```

#### 🌌 **Cosmic Perspective Studies**
```bash
# Historical heliocentric analysis (ancient civilizations)
./bin/horoscope_cli --date 44BC-03-15 --time 12:00:00 --location "Rome" --chart-style solar-system --perspective heliocentric

# Futuristic projections with different perspectives
./bin/horoscope_cli --date 2100-01-01 --time 12:00:00 --location "London" --chart-style solar-system --perspective saturn-centric
```

### 🔧 **Data-Only Output Examples**
```bash
# Disable all charts and ASCII art
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 --location "New York" --no-drawing

# Pure data output with JSON
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 --location "New York" --no-drawing --output json
```

### 🔧 Professional Use Cases

#### 👩‍💼 **For Astrologers**
```bash
# Complete client consultation package
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 --location "New York" --output json > client_chart.json
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 --location "New York" --eclipses > client_eclipses.txt
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 --location "New York" --kp-table --kp-format json > client_kp.json
```

#### 🔬 **For Researchers**
```bash
# Historical data collection
./bin/horoscope_cli --ephemeris --ephemeris-range 1900-01-01 2000-01-01 --ephemeris-interval 365 --ephemeris-format csv > 20th_century_data.csv

# Pattern analysis
./bin/horoscope_cli --conjunction-range 1900-01-01 2000-01-01 --location "London" --conjunction-orb 2.0 --output json > conjunctions_20th.json

# Festival correlation analysis
./bin/horoscope_cli --hindu-search 2020-01-01 2024-12-31 --search-tithi 15 --location "Delhi" --hindu-search-format csv > purnima_dates.csv
```

#### 💻 **For Developers**
```bash
# API endpoint testing
curl -X GET "localhost:3000/api/chart" \
  -H "Content-Type: application/json" \
  -d "$(./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 --location "New York" --output json)"

# Batch processing for calendar applications
for date in 2024-01-{01..31}; do
  ./bin/horoscope_cli --date $date --time 12:00:00 --location "Delhi" --panchanga --panchanga-format json > "panchanga_$date.json"
done

# Generate monthly festival calendars
for month in {01..12}; do
  ./bin/horoscope_cli --astro-calendar-monthly 2024-$month --location "Mumbai" --astro-calendar-format json --all-festivals > "calendar_2024_$month.json"
done
```

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

### New Feature Notes

**Latitude Range Filtering**:
- Uses ecliptic latitude (not geographic latitude)
- Filters based on planets' distance from ecliptic plane
- Useful for finding orbital alignments

**Graha Yuddha (Planetary Wars)**:
- Based on traditional Vedic astrology principles
- Uses Swiss Ephemeris for precise latitude calculations
- Only considers classical visible planets (Mercury through Saturn)
- Winner determination follows authentic ancient rules

## 🔗 Support and Resources

### 📚 **Additional Learning**

- 📖 **Swiss Ephemeris**: https://www.astro.com/swisseph/
- 🕉️ **Hindu Calendar System**: Traditional Panchanga calculations and meanings
- ⭐ **KP System**: Krishnamurti Paddhati principles and applications
- 🎯 **Astrological Methods**: Traditional and modern astrological techniques

### 🤝 **Community & Support**

- 🐛 **Bug Reports**: Use GitHub issues for technical problems
- 💡 **Feature Requests**: Suggest new features and improvements
- 📧 **Technical Support**: For professional astrology consultations
- 🔧 **Contributing**: Help improve the codebase and documentation

### 📈 **Version History**

- **v2.0.0** - Complete Hindu Panchanga system, enhanced manual
- **v1.5.0** - Added KP system support and ancient date calculations
- **v1.1.0** - Sidereal zodiac support with 21 ayanamsa types and advanced calculation flags
- **v1.0.0** - Initial release with Swiss Ephemeris integration

---

## 🌟 **About This Manual**

**Last Updated**: July 2025
**Manual Version**: 2.0
**Application Version**: 2.0.0
**Documentation**: Comprehensive user guide with examples

**📜 Copyright**: © 2025 - Licensed under MIT License
**🌟 Built with Love**: For the global astrology community
**🚀 Powered by**: Swiss Ephemeris - The gold standard in astronomical calculations

---

*May the stars guide your journey* ⭐✨
