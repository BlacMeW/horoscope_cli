# 🌟 HOROSCOPE CLI - COMPLETE USER MANUAL ⭐

<div align="center">

[![Version](https://img.shields.io/badge/version-2.0.0-blue.svg)](https://github.com/BlacMeW/horoscope_cli)
[![Swiss Ephemeris](https://img.shields.io/badge/powered%20by-Swiss%20Ephemeris-orange.svg)](https://www.astro.com/swisseph/)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20macOS%20%7C%20Windows-lightgrey.svg)](#installation)

**🔮 The Most Comprehensive Command-Line Astrology Toolkit 🔮**

*Professional astronomical calculations • Multi-cultural calendar systems • Historical BC era support*

</div>

---

## 📑 TABLE OF CONTENTS

<div align="center">

| 🎯 **ESSENTIALS** | 🔮 **ASTROLOGY** | 📅 **CALENDARS** | 🛠️ **ADVANCED** |
|:--:|:--:|:--:|:--:|
| [Quick Start](#-quick-start) | [Birth Charts](#-birth-chart-generation) | [Hindu Calendar](#%EF%B8%8F-hindu-calendar-panchanga) | [Eclipse Analysis](#-eclipse-analysis) |
| [Installation](#-installation) | [Chart Styles](#-chart-styles) | [Myanmar Calendar](#-myanmar-calendar) | [Conjunctions](#-planetary-conjunctions) |
| [Basic Usage](#-basic-usage) | [House Systems](#-house-systems) | [Monthly Calendars](#-monthly-calendars) | [KP System](#%EF%B8%8F-kp-system) |
| [Parameters](#-required-parameters) | [Planetary Data](#-planetary-positions) | [Calendar Search](#-calendar-search) | [Ephemeris Tables](#-ephemeris-tables) |

</div>

---

## 🚀 QUICK START

### ⚡ Installation (2 Minutes)

```bash
# Clone the repository
git clone https://github.com/BlacMeW/horoscope_cli.git
cd horoscope_cli

# Build (choose one method)
make                    # Simple Makefile build
# OR
mkdir build && cd build && cmake .. && make    # CMake build
```

### 🎯 Your First Chart (30 Seconds)

```bash
# Basic birth chart
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 \
                    --lat 40.7128 --lon -74.0060 --timezone -5

# With visual chart
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 \
                    --lat 40.7128 --lon -74.0060 --timezone -5 \
                    --chart-style western

# Today's Hindu calendar
./bin/horoscope_cli --panchanga \
                    --lat 28.6139 --lon 77.2090 --timezone 5.5 \
                    --date $(date +%Y-%m-%d) --time 12:00:00
```

---

## 📋 REQUIRED PARAMETERS

### 🌍 Location & Time Essentials

| Parameter | Format | Description | Examples |
|:----------|:-------|:------------|:---------|
| `--date` | `YYYY-MM-DD` | Birth date (supports BC) | `1990-01-15`, `44BC-03-15`, `-0043-03-15` |
| `--time` | `HH:MM:SS` | Birth time (24-hour) | `14:30:00`, `09:15:30`, `23:45:12` |
| `--lat` | `±DD.DDDD` | Latitude (±90°) | `40.7128` (NYC), `-33.8688` (Sydney) |
| `--lon` | `±DDD.DDDD` | Longitude (±180°) | `-74.0060` (NYC), `151.2093` (Sydney) |
| `--timezone` | `±H.H` | UTC offset | `-5` (EST), `5.5` (India), `9` (JST) |

### 📍 Quick Location Setup

```bash
# Use predefined locations
./bin/horoscope_cli --location "New York" --date 1990-01-15 --time 14:30:00

# Search available locations
./bin/horoscope_cli --search-location "London"
./bin/horoscope_cli --list-locations
```

---

## 🔮 BIRTH CHART GENERATION

### 🎨 Chart Styles

<div align="center">

| Style | Command | Best For |
|:------|:--------|:---------|
| 🌍 **Western** | `--chart-style western` | Western astrology, psychological analysis |
| 🕉️ **North Indian** | `--chart-style north-indian` | Vedic astrology, traditional Indian |
| 🏛️ **South Indian** | `--chart-style south-indian` | Tamil Nadu, Karnataka, Andhra Pradesh |
| 🌸 **East Indian** | `--chart-style east-indian` | Bengali, Oriya traditions |
| 🌌 **Solar System** | `--chart-style solar-system` | Orbital mechanics, astronomy |

</div>

#### 📊 Western Chart Example

```bash
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 \
                    --lat 40.7128 --lon -74.0060 --timezone -5 \
                    --chart-style western --house-system placidus
```

**Sample Output:**
```
═══════════════════════════════════════════════════════════════
                    🌟 WESTERN ASTROLOGICAL CHART 🌟
═══════════════════════════════════════════════════════════════

                           MC (29° ♐)
                              |
                    ♑     ♒  |  ♓     ♈
                 ♃        ☽    |    ☿    ♀
            ♒ ────────────────┼────────────── ♉
                           |  |  |
                        ♇  |  |  |  ☉
            ♓ ────────────────┼────────────── ♊
                              |
                    ♏     ♎  |  ♍     ♌
                              |
                          IC (29° ♊)

🌟 PLANETARY POSITIONS:
☉ Sun:        25°42' ♑ Capricorn    (House 5)
☽ Moon:       12°18' ♏ Scorpio      (House 3)
☿ Mercury:    05°33' ♒ Aquarius     (House 6)
♀ Venus:      18°25' ♓ Pisces       (House 7)
♂ Mars:       22°17' ♐ Sagittarius  (House 4)
♃ Jupiter:    08°44' ♋ Cancer       (House 11)
♄ Saturn:     15°32' ♑ Capricorn    (House 5)
♅ Uranus:     07°28' ♑ Capricorn    (House 5)
♆ Neptune:    13°15' ♑ Capricorn    (House 5)
♇ Pluto:      16°38' ♏ Scorpio      (House 3)
```

#### 🕉️ North Indian Vedic Chart Example

```bash
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 \
                    --lat 28.6139 --lon 77.2090 --timezone 5.5 \
                    --chart-style north-indian --zodiac-mode sidereal
```

**Sample Output:**
```
═══════════════════════════════════════════════════════════════
                 🕉️ NORTH INDIAN VEDIC CHART 🕉️
═══════════════════════════════════════════════════════════════

┌─────────────┬─────────────┬─────────────┬─────────────┐
│             │             │             │             │
│     12      │      1      │      2      │      3      │
│     ♐       │     ♑       │     ♒       │     ♓       │
│             │   ☉,♄,♅,♆   │    ☽        │             │
├─────────────┼─────────────┼─────────────┼─────────────┤
│             │                           │             │
│     11      │                           │      4      │
│     ♏       │           RASI            │     ♈       │
│    ♇        │                           │             │
├─────────────┤                           ├─────────────┤
│             │                           │             │
│     10      │                           │      5      │
│     ♎       │                           │     ♉       │
│             │                           │    ♀        │
├─────────────┼─────────────┼─────────────┼─────────────┤
│             │             │             │             │
│      9      │      8      │      7      │      6      │
│     ♍       │     ♌       │     ♋       │     ♊       │
│    ♂        │             │    ♃        │    ☿        │
└─────────────┴─────────────┴─────────────┴─────────────┘

🌟 PLANETARY POSITIONS (Sidereal):
☉ Sun:        01°18' ♑ Capricorn    (House 1)
☽ Moon:       17°54' ♒ Aquarius     (House 2)
☿ Mercury:    11°09' ♊ Gemini       (House 6)
♀ Venus:      24°01' ♉ Taurus       (House 5)
♂ Mars:       27°53' ♍ Virgo        (House 9)
♃ Jupiter:    14°20' ♋ Cancer       (House 7)
♄ Saturn:     21°08' ♐ Sagittarius  (House 12)
```

### 🏠 House Systems

<div align="center">

| System | Code | Description | Best For |
|:-------|:-----|:------------|:---------|
| **Placidus** | `P` | Time-based, unequal houses | Modern Western astrology |
| **Koch** | `K` | Birthplace-based | European traditions |
| **Equal House** | `E` | 30° equal divisions | Simple, traditional |
| **Whole Sign** | `W` | Sign = House | Ancient Hellenistic |
| **Campanus** | `C` | Prime vertical | Medieval traditions |
| **Regiomontanus** | `R` | Celestial equator | Renaissance era |

</div>

```bash
# Compare house systems
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 \
                    --lat 40.7128 --lon -74.0060 --timezone -5 \
                    --house-system placidus

./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 \
                    --lat 40.7128 --lon -74.0060 --timezone -5 \
                    --house-system whole-sign
```

### 🌍 Zodiac Modes & Ayanamsa

#### Tropical vs Sidereal

```bash
# Tropical (Western) - Season-based
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 \
                    --lat 40.7128 --lon -74.0060 --timezone -5 \
                    --zodiac-mode tropical

# Sidereal (Vedic) - Star-based
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 \
                    --lat 40.7128 --lon -74.0060 --timezone -5 \
                    --zodiac-mode sidereal --ayanamsa lahiri
```

#### Ayanamsa Options

<div align="center">

| Ayanamsa | Code | Traditional Use |
|:---------|:-----|:----------------|
| **Lahiri** | `lahiri` | Most common in India 🇮🇳 |
| **Krishnamurti** | `krishnamurti` | KP System 🔢 |
| **Raman** | `raman` | B.V. Raman school 📚 |
| **Fagan-Bradley** | `fagan-bradley` | Western sidereal 🌍 |
| **Yukteshwar** | `yukteshwar` | Kriya Yoga tradition 🧘 |

</div>

---

## 📅 MONTHLY CALENDARS

### 🕉️ Hindu Monthly Calendar

<div align="center">

**✨ FEATURES:**
- ✅ BC Era Support (6000 BC - 7000 AD)
- ✅ Complete Panchanga (5 elements)
- ✅ Festival Detection
- ✅ Varna Day Classification
- ✅ Julian Day References

</div>

#### Basic Usage

```bash
# Current month
./bin/horoscope_cli --hindu-monthly $(date +%Y-%m)

# Specific month
./bin/horoscope_cli --hindu-monthly 2025-07

# BC Era examples
./bin/horoscope_cli --hindu-monthly 500BC-03    # Buddha's time
./bin/horoscope_cli --hindu-monthly -0499-03    # Astronomical notation
```

#### Output Formats

```bash
# Traditional Panchanga layout
./bin/horoscope_cli --hindu-monthly 2025-07 --hindu-monthly-format traditional

# Detailed with muhurta timings
./bin/horoscope_cli --hindu-monthly 2025-07 --hindu-monthly-format detailed --hindu-monthly-muhurta

# Focus on festivals
./bin/horoscope_cli --hindu-monthly 2025-12 --hindu-monthly-format festivals

# Data export
./bin/horoscope_cli --hindu-monthly 2025-07 --hindu-monthly-format json
./bin/horoscope_cli --hindu-monthly 2025-07 --hindu-monthly-format csv
```

#### Sample Output

```
╔══════════════════════════════════════════════════════════════════════════════════╗
║                        🕉️ ENHANCED HINDU CALENDAR 🕉️                               ║
╠══════════════════════════════════════════════════════════════════════════════════╣
║  July 2025 CE | Shravana 2082 VS | Grishma (Uttarayana)                        ║
╚══════════════════════════════════════════════════════════════════════════════════╝

┌────┬───────────────┬───────────────┬───────────────┬───────────────┬───────────────┬───────────────┬───────────────┐
│Week│      SUN      │      MON      │      TUE      │      WED      │      THU      │      FRI      │      SAT      │
├────┼───────────────┼───────────────┼───────────────┼───────────────┼───────────────┼───────────────┼───────────────┤
│Fmt │[GG][T##][P]   │Hindu Day      │ Tithi/        │ Nakshatra/    │ Yoga/         │ Karana/       │ Festivals/    │
│    │Gregorian      │Tithi+Paksha   │ Special       │ Star Info     │ Combination   │ Half-Tithi    │ Events        │
├────┼───────────────┼───────────────┼───────────────┼───────────────┼───────────────┼───────────────┼───────────────┤
│ 1  │               │               │ 01T01SB       │ 02T02SB       │ 03T03SB       │ 04T04SB       │ 05T05SB       │
│    │               │               │ . Pu Ash F   │ * Ek Bha W    │ . Am Kri w    │ ! Sa Roh N    │ . Ga Ush W    │
├────┼───────────────┼───────────────┼───────────────┼───────────────┼───────────────┼───────────────┼───────────────┤
│ 2  │ 06T06SB       │ 07T07SB       │ 08T08SB       │ 09T09SB       │ 10T10SB       │ 11T11SB       │ 12T12SB       │
│    │ . Na Mag W    │ . Pu Mag w    │ ! Ch Ati F    │ . Sw Vis W    │ * Vy Sha w    │ # Sa Sha N    │ . Si Bra F    │
└────┴───────────────┴───────────────┴───────────────┴───────────────┴───────────────┴───────────────┴───────────────┘

🕉️ ENHANCED HINDU CALENDAR LEGEND:
═══════════════════════════════════════════════════════════════════════════════
Day Format: [GG][T##][P][V] = Gregorian / Hindu Tithi+Paksha / Varna
           GG = Gregorian day (1-31)
           T## = Tithi number (1-15 in each Paksha)
           P = Paksha (S=Shukla/Waxing, K=Krishna/Waning)
           V = Varna Day (B=Brahmin, K=Kshatriya, V=Vaishya, S=Shudra)

🌟 PANCHANGA INDICATORS:
   Special Days: Ek=Ekadashi, Pu=Purnima, Am=Amavasya, Sa=Sankranti
   Nakshatra: First 3 letters of current Nakshatra
   Quality: ✨ = Highly Auspicious, * = Auspicious, ! = Caution, # = Festival, . = Normal
   Yoga: Combination of Sun and Moon positions
   Karana: Half-Tithi periods

🌙 LUNAR PHASES: 🌕=Full, 🌗=Waxing, 🌑=New, 🌘=Waning
🎯 SPECIAL EVENTS: Festival days, Vratams, and significant observances
```

### 🇲🇲 Myanmar Monthly Calendar

<div align="center">

**✨ FEATURES:**
- ✅ Traditional Myanmar Era (ME)
- ✅ Buddhist Sasana Era (SE)
- ✅ Astrological Days (Yatyaza, Pyathada)
- ✅ Buddhist Sabbath Detection
- ✅ Moon Phase Tracking

</div>

#### Basic Usage

```bash
# Current month
./bin/horoscope_cli --myanmar-monthly $(date +%Y-%m)
# Short form
./bin/horoscope_cli -m 2025-07

# Different formats
./bin/horoscope_cli --myanmar-monthly 2025-07 --myanmar-monthly-format traditional
./bin/horoscope_cli --myanmar-monthly 2025-07 --myanmar-monthly-format blog-style
./bin/horoscope_cli --myanmar-monthly 2025-07 --myanmar-monthly-format multi-calendar
```

---

## 🕉️ HINDU CALENDAR (PANCHANGA)

### 🌟 Five Elements of Panchanga

<div align="center">

| Element | Sanskrit | Meaning | Significance |
|:--------|:---------|:--------|:-------------|
| **1. Tithi** | तिथि | Lunar day | Moon's angular distance from Sun |
| **2. Vara** | वार | Weekday | Planetary day rulership |
| **3. Nakshatra** | नक्षत्र | Star mansion | Moon's zodiacal position |
| **4. Yoga** | योग | Union | Combined movement of Sun & Moon |
| **5. Karana** | करण | Half-tithi | Half of lunar day period |

</div>

### 📅 Daily Panchanga

```bash
# Today's Panchanga
./bin/horoscope_cli --panchanga \
                    --lat 28.6139 --lon 77.2090 --timezone 5.5 \
                    --date $(date +%Y-%m-%d) --time 12:00:00

# Date range (3 days)
./bin/horoscope_cli --panchanga-range 2025-07-25 2025-07-27 \
                    --lat 28.6139 --lon 77.2090 --timezone 5.5

# Festivals only
./bin/horoscope_cli --panchanga --festivals-only \
                    --lat 28.6139 --lon 77.2090 --timezone 5.5 \
                    --date 2025-03-14 --time 12:00:00
```

### 📊 Panchanga Output Formats

```bash
# Detailed table format
./bin/horoscope_cli --panchanga --panchanga-format table

# Traditional compact format (like Pancanga3.14.pl)
./bin/horoscope_cli --panchanga --panchanga-format compact

# JSON for API integration
./bin/horoscope_cli --panchanga --panchanga-format json

# CSV for spreadsheet analysis
./bin/horoscope_cli --panchanga-range 2025-07-01 2025-07-31 \
                    --panchanga-format csv
```

### Sample Panchanga Output

```
═══════════════════════════════════════════════════════════════════
                          🕉️  HINDU PANCHANGA  🕉️
═══════════════════════════════════════════════════════════════════

📅 DATE INFORMATION:
   Gregorian Date: Friday, July 25, 2025
   Hindu Year (Vikram Samvat): 2082
   Hindu Month: Shravana (श्रावण)
   Paksha: Krishna (Waning)

🌟 PANCHANGAM (FIVE LIMBS):
   1. 🌙 Tithi:     Chaturthi (4th day) Krishna Paksha
                    ⏰ Ends: July 26, 2025 at 03:24 AM

   2. 📅 Vara:      Shukravar (Friday) - Venus Day
                    🪐 Day Lord: Venus ♀

   3. ⭐ Nakshatra: Uttara Ashadha (उत्तराषाढ़ा)
                    🌟 Lord: Sun ☉ | Symbol: Elephant tusk
                    ⏰ Ends: July 25, 2025 at 11:47 PM

   4. 🤝 Yoga:      Shukla (शुक्ल)
                    ⏰ Ends: July 26, 2025 at 07:33 AM

   5. ⏰ Karana:    Taitila (तैतिल)
                    ⏰ Ends: July 26, 2025 at 03:24 AM

🌞 CELESTIAL POSITIONS:
   Sun in:   Karkata (Cancer) ♋ (3°42')
   Moon in:  Makara (Capricorn) ♑ (18°15')
   Lunar Phase: Waning (Krishna Paksha)

🌟 SPECIAL OBSERVANCES:
   • Normal day - No major festivals
   • Krishna Paksha - Good for introspection
   • Uttara Ashadha - Favorable for completion of projects

🕐 MUHURTA ASSESSMENT:
   Status: ✅ Shubha (Auspicious)
   Note: Good day for routine activities and spiritual practices
```

---

## 🔍 CALENDAR SEARCH

### 🕉️ Hindu Calendar Search

<div align="center">

**🎯 SEARCH BY:**
- 📅 Year, Month, Date Range
- 🌙 Tithi, Paksha
- ⭐ Nakshatra, Yoga, Karana
- 📅 Weekday, Special Days
- 🔢 Julian Day Numbers
- 🕉️ Varna Days

</div>

#### Basic Search Examples

```bash
# Find all Ekadashi days in 2025
./bin/horoscope_cli --hindu-search 2025-01-01 2025-12-31 \
                    --hindu-search-ekadashi

# Find all full moons (Purnima) in July 2025
./bin/horoscope_cli --hindu-search 2025-07-01 2025-07-31 \
                    --hindu-search-purnima

# Find specific Tithi (e.g., 11th lunar day)
./bin/horoscope_cli --hindu-search 2025-01-01 2025-12-31 \
                    --search-tithi 11

# Find specific Nakshatra days
./bin/horoscope_cli --hindu-search 2025-07-01 2025-07-31 \
                    --hindu-search-nakshatra 21  # Uttara Ashadha
```

#### Advanced Multi-Criteria Search

```bash
# Ekadashi on Sunday (spiritual combination)
./bin/horoscope_cli --hindu-search 2025-01-01 2025-12-31 \
                    --hindu-search-ekadashi --search-weekday 0 \
                    --hindu-search-logic and

# Full moon OR Ekadashi (auspicious days)
./bin/horoscope_cli --hindu-search 2025-07-01 2025-07-31 \
                    --hindu-search-purnima --hindu-search-ekadashi \
                    --hindu-search-logic or

# Varna day search (spiritual classification)
./bin/horoscope_cli --hindu-search 2025-07-01 2025-07-31 \
                    --search-varna-day brahmin
```

#### Historical BC Era Search

```bash
# Buddha's lifetime (563-483 BC)
./bin/horoscope_cli --hindu-search 563BC-05-01 563BC-05-31 \
                    --hindu-search-purnima

# Julius Caesar era (50-40 BC)
./bin/horoscope_cli --hindu-search -0049-01-01 -0040-12-31 \
                    --search-tithi-range 14 16  # Around full moon
```

#### Julian Day Search

```bash
# Simple JD search (exact day)
./bin/horoscope_cli --search-jd 1555550

# JD range search within date range
./bin/horoscope_cli --hindu-search 563BC-05-01 563BC-05-31 \
                    --search-julian-day-range 1555550 1555580
```

### 🇲🇲 Myanmar Calendar Search

```bash
# Buddhist sabbath days
./bin/horoscope_cli --myanmar-search 2025-07-01 2025-07-31 \
                    --myanmar-search-sabbath

# Full moon in Myanmar calendar
./bin/horoscope_cli --myanmar-search 2025-01-01 2025-12-31 \
                    --myanmar-search-moon-phase 1

# Yatyaza (inauspicious) days
./bin/horoscope_cli --myanmar-search 2025-07-01 2025-07-31 \
                    --myanmar-search-yatyaza

# Multi-criteria: Full moon AND sabbath
./bin/horoscope_cli --myanmar-search 2025-01-01 2025-12-31 \
                    --myanmar-search-moon-phase 1 --myanmar-search-sabbath \
                    --myanmar-search-logic and
```

### 📊 Search Output Formats

```bash
# Table format (default)
./bin/horoscope_cli --hindu-search 2025-07-01 2025-07-31 \
                    --hindu-search-ekadashi --hindu-search-format table

# List format (for scripting)
./bin/horoscope_cli --hindu-search 2025-07-01 2025-07-31 \
                    --hindu-search-purnima --hindu-search-format list

# CSV for data analysis
./bin/horoscope_cli --hindu-search 2025-01-01 2025-12-31 \
                    --hindu-search-ekadashi --hindu-search-format csv

# JSON for API integration
./bin/horoscope_cli --myanmar-search 2025-07-01 2025-07-31 \
                    --myanmar-search-sabbath --myanmar-search-format json
```

---

## 🌚 ECLIPSE ANALYSIS

<div align="center">

**✨ FEATURES:**
- 🌞 Solar & Lunar Eclipses
- 🌍 Global Visibility Calculations
- 📅 Historical Coverage (6000 BC - 7000 AD)
- 🔍 Custom Date Range Search
- 📊 Multiple Output Formats

</div>

### Basic Eclipse Search

```bash
# Eclipses near birth date (±1 year)
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 \
                    --lat 40.7128 --lon -74.0060 --timezone -5 \
                    --eclipses

# Custom time range
./bin/horoscope_cli --eclipse-range 2025-01-01 2025-12-31 \
                    --lat 28.6139 --lon 77.2090

# Historical eclipses (Julius Caesar era)
./bin/horoscope_cli --eclipse-range -0050-01-01 -0040-12-31 \
                    --lat 41.9028 --lon 12.4964  # Rome coordinates
```

### Eclipse Output Formats

```bash
# Professional table format
./bin/horoscope_cli --eclipse-range 2025-01-01 2026-12-31 \
                    --lat 40.7128 --lon -74.0060 \
                    --eclipse-format table

# CSV for data analysis
./bin/horoscope_cli --eclipse-range 2025-01-01 2025-12-31 \
                    --lat 28.6139 --lon 77.2090 \
                    --eclipse-format csv

# JSON for API integration
./bin/horoscope_cli --eclipse-range 2025-01-01 2025-12-31 \
                    --lat 51.5074 --lon -0.1278 \
                    --eclipse-format json
```

### Sample Eclipse Output

```
═══════════════════════════════════════════════════════════════════════════════
                         🌚 ECLIPSE ANALYSIS 🌞
                    Swiss Ephemeris Professional Calculations
═══════════════════════════════════════════════════════════════════════════════

📍 LOCATION: 40.7128°N, 74.0060°W (New York City)
📅 SEARCH PERIOD: January 1, 2025 - December 31, 2025

┌─────────────┬──────────┬────────────┬──────────────┬──────────────┬────────────┐
│    DATE     │   TYPE   │ MAX. TIME  │  MAGNITUDE   │  VISIBILITY  │  DURATION  │
├─────────────┼──────────┼────────────┼──────────────┼──────────────┼────────────┤
│ 2025-03-14  │ Total    │ 02:58 UTC  │    1.0285    │   Visible    │ 4m 09s     │
│             │ Solar    │ 21:58 EST  │              │   88% max    │            │
├─────────────┼──────────┼────────────┼──────────────┼──────────────┼────────────┤
│ 2025-03-29  │ Partial  │ 10:58 UTC  │    0.0681    │ Not Visible  │    -       │
│             │ Lunar    │ 05:58 EST  │              │ Below Horiz. │            │
├─────────────┼──────────┼────────────┼──────────────┼──────────────┼────────────┤
│ 2025-09-07  │ Total    │ 18:11 UTC  │    1.2816    │   Visible    │ 1h 23m     │
│             │ Lunar    │ 13:11 EST  │              │   Perfect    │            │
└─────────────┴──────────┴────────────┴──────────────┴──────────────┴────────────┘

🌟 VISIBILITY SUMMARY:
   • Total Solar Eclipse (March 14): Maximum 88% coverage in NYC
   • Total Lunar Eclipse (September 7): Fully visible, perfect viewing
   • Peak viewing times adjusted for Eastern Time Zone

🔮 ASTROLOGICAL SIGNIFICANCE:
   • Solar eclipses: New beginnings, major life changes
   • Lunar eclipses: Emotional revelations, completion of cycles
   • Effects typically manifest ±6 months from eclipse date
```

---

## 🪐 PLANETARY CONJUNCTIONS

<div align="center">

**✨ FEATURES:**
- 🎯 Multi-Planet Alignment Detection
- 📐 Customizable Orb Settings (0.1° - 15°)
- 📈 Historical Analysis
- ⚔️ Graha Yuddha (Planetary Wars)
- 🔗 Aspect Relationship Mapping

</div>

### Basic Conjunction Analysis

```bash
# Conjunctions near birth
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 \
                    --lat 40.7128 --lon -74.0060 --timezone -5 \
                    --conjunctions

# Custom date range with tight orb
./bin/horoscope_cli --conjunction-range 2025-01-01 2025-12-31 \
                    --lat 28.6139 --lon 77.2090 \
                    --conjunction-orb 2.0

# Graha Yuddha (Planetary Wars) - very close conjunctions
./bin/horoscope_cli --graha-yuddha-range 2025-01-01 2025-12-31 \
                    --lat 28.6139 --lon 77.2090 \
                    --graha-yuddha-orb 1.0
```

### Advanced Conjunction Search

```bash
# Filter by planetary latitude range
./bin/horoscope_cli --conjunction-range 2025-01-01 2025-12-31 \
                    --lat 40.7128 --lon -74.0060 \
                    --conjunction-latitude-range -5.0 5.0

# Historical conjunctions (Ancient Rome)
./bin/horoscope_cli --conjunction-range -0050-01-01 -0040-12-31 \
                    --lat 41.9028 --lon 12.4964 \
                    --conjunction-orb 5.0
```

### Sample Conjunction Output

```
═══════════════════════════════════════════════════════════════════════════════
                      🪐 PLANETARY CONJUNCTIONS 🪐
                         High-Precision Analysis
═══════════════════════════════════════════════════════════════════════════════

📍 LOCATION: 28.6139°N, 77.2090°E (New Delhi)
📅 SEARCH PERIOD: January 1, 2025 - December 31, 2025
🎯 MAXIMUM ORB: 3.0°

┌─────────────┬─────────────────┬──────────────┬──────────┬─────────────────────┐
│    DATE     │    PLANETS      │   ORB (°)   │   TIME   │     SIGNIFICANCE    │
├─────────────┼─────────────────┼──────────────┼──────────┼─────────────────────┤
│ 2025-02-28  │ ♀ Venus         │    1.2°      │ 15:34    │ Love & Communication│
│             │ ☿ Mercury       │              │   IST    │ Artistic Expression │
├─────────────┼─────────────────┼──────────────┼──────────┼─────────────────────┤
│ 2025-05-17  │ ♃ Jupiter       │    0.8°      │ 08:22    │ 🔥 GRAHA YUDDHA    │
│             │ ☿ Mercury       │              │   IST    │ Jupiter Wins        │
├─────────────┼─────────────────┼──────────────┼──────────┼─────────────────────┤
│ 2025-10-03  │ ♂ Mars          │    2.7°      │ 22:15    │ Energy & Action     │
│             │ ♃ Jupiter       │              │   IST    │ Powerful Motivation │
└─────────────┴─────────────────┴──────────────┴──────────┴─────────────────────┘

🌟 GRAHA YUDDHA DETAILS (May 17, 2025):
   • ♃ Jupiter vs ☿ Mercury in Gemini
   • Winner: Jupiter (larger apparent diameter)
   • Effect: Jupiter's wisdom overcomes Mercury's logic
   • Duration: ±2 days from exact conjunction
   • Recommendation: Focus on higher learning, spiritual pursuits

🔮 ASTROLOGICAL INTERPRETATION:
   • Venus-Mercury: Excellent for creative projects, negotiations
   • Mars-Jupiter: Dynamic energy for new ventures, leadership
   • Close conjunctions (<1°): Intense blending of planetary energies
```

---

## 🔢 KP SYSTEM

<div align="center">

**🇮🇳 Krishnamurti Paddhati System**

**✨ FEATURES:**
- 🧮 5-Level Sub-Lord Calculations
- 🌟 27 Nakshatra Integration
- 📊 Planetary Transition Tracking
- 📋 Complete KP Tables

</div>

### KP Analysis Commands

```bash
# Complete KP table for birth chart
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 \
                    --lat 28.6139 --lon 77.2090 --timezone 5.5 \
                    --kp-table

# KP transitions in date range
./bin/horoscope_cli --kp-transition-range 2025-07-01 2025-07-31 \
                    --lat 28.6139 --lon 77.2090

# Specific planet transitions
./bin/horoscope_cli --kp-transition-range 2025-01-01 2025-12-31 \
                    --kp-transition-planet JUPITER \
                    --kp-transition-level star

# Different KP output formats
./bin/horoscope_cli --kp-table --kp-format json
```

### Sample KP Output

```
═══════════════════════════════════════════════════════════════════════════════
                       🔢 KP SYSTEM ANALYSIS 🔢
                    Krishnamurti Paddhati Method
═══════════════════════════════════════════════════════════════════════════════

📅 BIRTH DATA: January 15, 1990, 14:30:00 IST
📍 LOCATION: 28.6139°N, 77.2090°E (New Delhi)
🌟 AYANAMSA: Krishnamurti (23°54'21")

┌─────────┬────────────┬─────────┬──────────┬─────────┬─────────┬─────────────┐
│ PLANET  │ LONGITUDE  │  SIGN   │   STAR   │  SUB    │ SUB-SUB │  KP NOTATION│
├─────────┼────────────┼─────────┼──────────┼─────────┼─────────┼─────────────┤
│ ☉ Sun   │ 01°18'42"  │    1    │    21    │    3    │    7    │ 1-21-3-7   │
│         │ Capricorn  │   ♑     │ U.Ash    │   Mars  │  Venus  │             │
├─────────┼────────────┼─────────┼──────────┼─────────┼─────────┼─────────────┤
│ ☽ Moon  │ 17°54'18"  │    11   │    23    │    1    │    4    │ 11-23-1-4  │
│         │ Aquarius   │   ♒     │ Dhan     │   Sun   │  Rahu   │             │
├─────────┼────────────┼─────────┼──────────┼─────────┼─────────┼─────────────┤
│ ☿ Merc  │ 11°09'33"  │    3    │    7     │    4    │    2    │ 3-7-4-2    │
│         │ Gemini     │   ♊     │ Puna     │  Rahu   │  Moon   │             │
├─────────┼────────────┼─────────┼──────────┼─────────┼─────────┼─────────────┤
│ ♀ Venus │ 24°01'27"  │    2    │    6     │    2    │    6    │ 2-6-2-6    │
│         │ Taurus     │   ♉     │ Mrig     │  Moon   │  Jupiter│             │
└─────────┴────────────┴─────────┴──────────┴─────────┴─────────┴─────────────┘

🌟 SUB-LORD SIGNIFICANCE:
   • Sun's Sub-Lord (Mars): Leadership through action
   • Moon's Sub-Lord (Sun): Strong willpower, royal nature
   • Mercury's Sub-Lord (Rahu): Innovative thinking, technology
   • Venus's Sub-Lord (Moon): Emotional creativity, nurturing love

🔮 KP PREDICTIONS:
   • Primary Significators: 1st, 5th, 9th houses
   • Secondary Significators: 2nd, 11th houses
   • Key Periods: Jupiter Sub-periods favorable
   • Career: Technology, Communication, Creative Arts
```

---

## 📊 EPHEMERIS TABLES

<div align="center">

**✨ FEATURES:**
- 🗓️ Daily/Weekly/Monthly Positions
- 📈 Arc-Second Precision
- 💾 Multiple Export Formats
- 🌈 Color-Coded Motion
- 📅 Calendar Integration

</div>

### Basic Ephemeris

```bash
# Standard ephemeris table
./bin/horoscope_cli --ephemeris-range 2025-07-01 2025-07-31

# Compact Astrodienst-style format
./bin/horoscope_cli --ephemeris-range 2025-07-01 2025-07-31 \
                    --ephemeris-compact

# Weekly intervals
./bin/horoscope_cli --ephemeris-range 2025-01-01 2025-12-31 \
                    --ephemeris-interval 7

# Include sidereal time
./bin/horoscope_cli --ephemeris-range 2025-07-01 2025-07-07 \
                    --ephemeris-sidereal-time
```

### Advanced Ephemeris Options

```bash
# Declination instead of longitude
./bin/horoscope_cli --ephemeris-range 2025-07-01 2025-07-31 \
                    --ephemeris-declination

# Both longitude and declination
./bin/horoscope_cli --ephemeris-range 2025-07-01 2025-07-07 \
                    --ephemeris-coordinates both

# Sidereal zodiac with Lahiri ayanamsa
./bin/horoscope_cli --ephemeris-range 2025-07-01 2025-07-31 \
                    --zodiac-mode sidereal --ayanamsa lahiri \
                    --ephemeris-compact

# Historical BC era ephemeris
./bin/horoscope_cli --ephemeris-range -0043-03-01 -0043-03-31 \
                    --ephemeris-format table
```

### Calendar Display Options

```bash
# Julian calendar only (for historical dates)
./bin/horoscope_cli --ephemeris-range 1500-07-01 1500-07-31 \
                    --ephemeris-julian-calendar

# Both Julian and Gregorian calendars
./bin/horoscope_cli --ephemeris-range 1582-10-01 1582-10-31 \
                    --ephemeris-calendar both

# No color coding (for printing)
./bin/horoscope_cli --ephemeris-range 2025-07-01 2025-07-31 \
                    --ephemeris-no-colors
```

### Sample Ephemeris Output

```
═══════════════════════════════════════════════════════════════════════════════
                          📊 EPHEMERIS TABLE 📊
                     Swiss Ephemeris Professional Data
═══════════════════════════════════════════════════════════════════════════════

🌍 ZODIAC MODE: Tropical | 📅 PERIOD: July 1-31, 2025 | ⏰ INTERVAL: Daily

┌──────┬─────┬────────┬────────┬────────┬────────┬────────┬────────┬────────┬────────┐
│ DATE │ DAY │   ☉    │   ☽    │   ☿    │   ♀    │   ♂    │   ♃    │   ♄    │  GST   │
│      │     │  SUN   │  MOON  │ MERCURY│  VENUS │  MARS  │JUPITER │ SATURN │        │
├──────┼─────┼────────┼────────┼────────┼────────┼────────┼────────┼────────┼────────┤
│ 07/01│ Tue │09♋38'  │18♍42'  │15♋28'  │02♌18'  │22♌35'  │07♊12'  │19♓14'  │18h40m  │
│ 07/02│ Wed │10♋39'  │01♎15'  │16♋41'  │03♌29'  │23♌18'  │07♊13'  │19♓15'  │18h44m  │
│ 07/03│ Thu │11♋40'  │13♎52'  │17♋55'  │04♌40'  │24♌01'  │07♊14'  │19♓16'  │18h48m  │
│ 07/04│ Fri │12♋41'  │26♎33'  │19♋10'  │05♌52'  │24♌44'  │07♊15'  │19♓17'  │18h52m  │
│ 07/05│ Sat │13♋42'  │09♏18'  │20♋26'  │07♌04'  │25♌27'  │07♊16'  │19♓18'  │18h56m  │
│ 07/06│ Sun │14♋43'  │22♏07'  │21♋43'  │08♌16'  │26♌10'  │07♊17'  │19♓19'  │19h00m  │
│ 07/07│ Mon │15♋44'  │05♐00'  │23♋01'  │09♌29'  │26♌53'  │07♊18'  │19♓20'  │19h04m  │
└──────┴─────┴────────┴────────┴────────┴────────┴────────┴────────┴────────┴────────┘

🌈 COLOR LEGEND:
   • 🔴 Red = Retrograde motion
   • 🟢 Green = Fast direct motion
   • 🟡 Yellow = Slow direct motion
   • 🔵 Blue = Nearly stationary
   • ⚪ White = Normal motion

📝 NOTES:
   • All positions at 00:00 UTC
   • GST = Greenwich Sidereal Time
   • Precision: ±0.1 arc-second
   • Calendar: Gregorian (post-1582), Julian (pre-1582)
```

---

## 🌌 ADVANCED FEATURES

### 🌍 Solar System Views

```bash
# Solar system orbital view (no birth data needed)
./bin/horoscope_cli --solar-system

# Different perspectives
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 \
                    --lat 40.7128 --lon -74.0060 --timezone -5 \
                    --chart-style solar-system --perspective heliocentric

./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 \
                    --lat 40.7128 --lon -74.0060 --timezone -5 \
                    --chart-style solar-system --perspective mars-centric
```

### 📍 Location Management

```bash
# Search for locations
./bin/horoscope_cli --search-location "New York"
./bin/horoscope_cli --search-location "Delhi"

# List all predefined locations
./bin/horoscope_cli --list-locations

# Use location by name
./bin/horoscope_cli --location "London" \
                    --date 1990-01-15 --time 14:30:00
```

### 💾 Output Formats

```bash
# JSON output for API integration
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 \
                    --lat 40.7128 --lon -74.0060 --timezone -5 \
                    --output json

# Astronomical coordinates only
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 \
                    --lat 40.7128 --lon -74.0060 --timezone -5 \
                    --astronomical

# No visual charts (data only)
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 \
                    --lat 40.7128 --lon -74.0060 --timezone -5 \
                    --no-drawing
```

---

## 🎯 PRACTICAL EXAMPLES

### 📚 Historical Research

```bash
# Buddha's possible birth dates (563 BC)
./bin/horoscope_cli --hindu-search 563BC-04-01 563BC-06-30 \
                    --hindu-search-purnima \
                    --lat 27.5 --lon 83.3  # Lumbini coordinates

# Julius Caesar's assassination (44 BC)
./bin/horoscope_cli --date -0043-03-15 --time 12:00:00 \
                    --lat 41.9028 --lon 12.4964 --timezone 1 \
                    --chart-style western

# Ancient eclipse research
./bin/horoscope_cli --eclipse-range -0500-01-01 -0400-12-31 \
                    --lat 32.5 --lon 44.4  # Babylon coordinates
```

### 🎭 Festival Planning

```bash
# Find all major Hindu festivals in 2025
./bin/horoscope_cli --hindu-search 2025-01-01 2025-12-31 \
                    --hindu-search-ekadashi --hindu-search-purnima \
                    --hindu-search-amavasya --hindu-search-logic or

# Myanmar Buddhist sabbath planning
./bin/horoscope_cli --myanmar-search 2025-07-01 2025-07-31 \
                    --myanmar-search-sabbath

# Wedding date planning (auspicious days)
./bin/horoscope_cli --hindu-search 2025-11-01 2025-12-31 \
                    --search-varna-day brahmin --hindu-search-purnima \
                    --hindu-search-logic or
```

### 🔬 Astronomical Research

```bash
# Planetary conjunctions in ancient times
./bin/horoscope_cli --conjunction-range -0100-01-01 0100-12-31 \
                    --lat 32.5 --lon 44.4 \
                    --conjunction-orb 1.0

# Eclipse cycles analysis
./bin/horoscope_cli --eclipse-range 2000-01-01 2030-12-31 \
                    --lat 0 --lon 0 \
                    --eclipse-format csv > eclipses_21st_century.csv

# Retrograde motion tracking
./bin/horoscope_cli --ephemeris-range 2025-01-01 2025-12-31 \
                    --ephemeris-interval 7 \
                    --ephemeris-colors
```

---

## 🛠️ TROUBLESHOOTING

### ❗ Common Issues

<div align="center">

| Issue | Cause | Solution |
|:------|:------|:---------|
| **Binary not found** | Not built properly | Run `make clean && make` |
| **Swiss Ephemeris error** | Missing data files | Check `./data/` directory exists |
| **BC date errors** | Wrong format | Use `-YYYY-MM-DD` or `YYYYBC-MM-DD` |
| **Timezone issues** | Wrong offset | Check local timezone at birth time |
| **Location errors** | Invalid coordinates | Lat: -90 to +90, Lon: -180 to +180 |

</div>

### 🔧 Debugging Commands

```bash
# Test with simple command
./bin/horoscope_cli --version

# Check available features
./bin/horoscope_cli --features

# Test with known location
./bin/horoscope_cli --location "New York" --date 2000-01-01 --time 12:00:00

# Verify ephemeris data
ls -la ./data/
```

### 📊 Performance Tips

```bash
# For large date ranges, use appropriate intervals
./bin/horoscope_cli --ephemeris-range 2000-01-01 2030-12-31 \
                    --ephemeris-interval 30  # Monthly instead of daily

# Use specific output formats for scripting
./bin/horoscope_cli --hindu-search 2025-01-01 2025-12-31 \
                    --hindu-search-ekadashi \
                    --hindu-search-format list | head -10

# For API integration, use JSON output
./bin/horoscope_cli --panchanga --panchanga-format json \
                    --lat 28.6139 --lon 77.2090 --timezone 5.5 \
                    --date 2025-07-25 --time 12:00:00
```

---

## 📖 APPENDICES

### 🌟 Nakshatra Reference

<div align="center">

| No. | Sanskrit Name | English | Lord | Characteristics |
|:---:|:-------------|:--------|:-----|:----------------|
| 1 | अश्विनी Ashwini | Horse Twins | Ketu | Speed, healing |
| 2 | भरणी Bharani | Bearer | Venus | Transformation |
| 3 | कृत्तिका Krittika | Cutter | Sun | Sharpness, fire |
| 4 | रोहिणी Rohini | Growing One | Moon | Beauty, growth |
| 5 | मृगशीर्ष Mrigashira | Deer Head | Mars | Searching nature |
| ... | ... | ... | ... | ... |
| 27 | रेवती Revati | Wealthy | Mercury | Completion, travel |

</div>

### 🏠 House System Details

<div align="center">

| System | Calculation Method | Strengths | Traditional Use |
|:-------|:------------------|:----------|:----------------|
| **Placidus** | Trisects diurnal arcs | Precise timing | Modern Western |
| **Koch** | Birthplace-dependent | Local accuracy | German tradition |
| **Equal House** | 30° from Ascendant | Simple, reliable | Ancient systems |
| **Whole Sign** | Sign = House | Traditional accuracy | Hellenistic |
| **Campanus** | Prime vertical | Spatial relationships | Medieval |

</div>

### 📅 Calendar System Comparison

<div align="center">

| Calendar | Year Zero | New Year | Month Count | Lunar/Solar |
|:---------|:----------|:---------|:------------|:------------|
| **Gregorian** | 1 AD | January 1 | 12 | Solar |
| **Hindu (Vikram)** | 57 BC | Chaitra | 12 | Luni-Solar |
| **Myanmar** | 639 AD | Tagu | 12-13 | Luni-Solar |
| **Buddhist** | 544 BC | Varies | Varies | Lunar |

</div>

---

## 📚 COMMAND REFERENCE QUICK GUIDE

### 🏃‍♂️ Most Common Commands

```bash
# Birth chart with Western style
./bin/horoscope_cli --date YYYY-MM-DD --time HH:MM:SS \
                    --lat LAT --lon LON --timezone TZ \
                    --chart-style western

# Hindu calendar for current month
./bin/horoscope_cli --hindu-monthly $(date +%Y-%m)

# Myanmar calendar for current month
./bin/horoscope_cli --myanmar-monthly $(date +%Y-%m)

# Today's Panchanga
./bin/horoscope_cli --panchanga --lat LAT --lon LON --timezone TZ \
                    --date $(date +%Y-%m-%d) --time 12:00:00

# Eclipse search for current year
./bin/horoscope_cli --eclipse-range $(date +%Y)-01-01 $(date +%Y)-12-31 \
                    --lat LAT --lon LON

# Find Ekadashi days this year
./bin/horoscope_cli --hindu-search $(date +%Y)-01-01 $(date +%Y)-12-31 \
                    --hindu-search-ekadashi
```

### 🎯 Short Options

<div align="center">

| Short | Long | Description |
|:------|:-----|:------------|
| `-h` | `--help` | Show help |
| `-v` | `--version` | Show version |
| `-f` | `--features` | Show features |
| `-m` | `--myanmar-monthly` | Myanmar monthly calendar |
| `-hm` | `--hindu-monthly` | Hindu monthly calendar |

</div>

---

<div align="center">

## 🌟 CONCLUSION

**Horoscope CLI** is your gateway to professional astronomical calculations with cultural authenticity. From birth charts to historical research, festival planning to scientific analysis - everything is possible with Swiss Ephemeris precision.

### 🚀 Start Your Cosmic Journey Today!

```bash
# Your first command
./bin/horoscope_cli --features

# Get help anytime
./bin/horoscope_cli --help

# Join the cosmic community
# ⭐ Star us on GitHub
# 🐛 Report issues
# 💡 Suggest features
```

---

**📧 Support:** GitHub Issues | **📖 Documentation:** README.md | **⭐ Star:** github.com/BlacMeW/horoscope_cli

**🕉️ May the stars guide your path 🌟**

</div>

---

<div align="center">

*Created with ❤️ by the Horoscope CLI team*
*Powered by Swiss Ephemeris • Licensed under MIT*

**Version 2.0.0** | Last Updated: July 2025

</div>
