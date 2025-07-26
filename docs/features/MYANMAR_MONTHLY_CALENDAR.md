# 🇲🇲 Myanmar Monthly Calendar Feature

## Overview

This new feature adds Myanmar monthly calendar display to the horoscope CLI application, inspired by the design from [mmcal.blogspot.com](https://mmcal.blogspot.com/). The feature provides beautiful monthly calendar layouts that combine Gregorian and Myanmar calendar systems with astrological information.

## Features

### 📅 **Multiple Display Formats**

1. **Traditional** - Authentic Myanmar calendar with Myanmar script headers
2. **Modern** - Contemporary layout with visual elements
3. **Compact** - Space-efficient view for quick reference
4. **Blog-Style** - Inspired by mmcal.blogspot.com layout
5. **JSON** - Structured data for programming
6. **CSV** - Spreadsheet-compatible format
7. **HTML** - Web display with CSS styling

### 🌟 **Rich Calendar Information**

- **Dual Dating System**: Shows both Gregorian and Myanmar dates
- **Moon Phases**: Visual indicators for lunar phases
- **Astrological Days**: Sabbath, Yatyaza, Pyathada, Thamanyo
- **Quality Indicators**: Day quality symbols (⭐⚠️🎉⚪)
- **Festivals**: Myanmar festivals and observances
- **Monthly Statistics**: Summary of special days

### 🎨 **Visual Elements**

- **Unicode Symbols**: Moon phases, quality indicators
- **Myanmar Script**: Traditional weekday headers
- **Box Drawing**: Beautiful ASCII art calendar grids
- **Color Coding**: Different day types visually distinguished
- **Legends**: Clear explanation of symbols and indicators

## Usage

### Basic Usage

```bash
# Display current month Myanmar calendar
./bin/horoscope_cli --myanmar-monthly $(date +'%Y-%m')

# Display specific month
./bin/horoscope_cli --myanmar-monthly 2024-04

# Specify format
./bin/horoscope_cli --myanmar-monthly 2024-04 --myanmar-monthly-format traditional
```

### Command Line Options

```bash
--myanmar-monthly YYYY-MM          Generate Myanmar monthly calendar
--myanmar-monthly-format FORMAT    Choose output format
```

### Available Formats

| Format | Description | Best For |
|--------|-------------|----------|
| `traditional` | Myanmar script headers, detailed astrological info | Cultural authenticity, religious observance |
| `modern` | Clean visual design, contemporary typography | General use, easy reading |
| `compact` | Minimal space usage, essential info only | Terminals, mobile devices |
| `blog-style` | mmcal.blogspot.com inspired with highlights | Web publishing, documentation |
| `json` | Structured data format | Programming, web applications |
| `csv` | Comma-separated values | Spreadsheet analysis |
| `html` | Web display format | Web pages, documentation |

## Examples

### Traditional Format

```
╔══════════════════════════════════════════════════════════════════════════════════╗
║                          🇲🇲 MYANMAR CALENDAR 🇲🇲                                ║
╠══════════════════════════════════════════════════════════════════════════════════╣
║  April 2024 CE   |   Tagu 1386 ME                                                ║
╠══════════════════════════════════════════════════════════════════════════════════╣
║   Sun   │   Mon   │   Tue   │   Wed   │   Thu   │   Fri   │   Sat   ║
║ တနင်္ဂနွေ │ တနင်္လာ │ အင်္ဂါ  │ ဗုဒ္ဓဟူး │ ကြာသပတေး │ သောကြာ │ စနေ    ║
╠═════════╪═════════╪═════════╪═════════╪═════════╪═════════╪═════════╣
║         │  01/15 ⭐│  02/16 ⚪│  03/17 ⚪│  04/18 ⚪│  05/19 ⚪│  06/20 ⚪║
...
```

### Modern Format

```
┌─────────────────────────────────────────────────────────────────────────────────┐
│                    🌙✨ April 2024 - Tagu 1386 ME ✨🌙                    │
├─────────────────────────────────────────────────────────────────────────────────┤
│  SUN  │  MON  │  TUE  │  WED  │  THU  │  FRI  │  SAT  │
├───────┼───────┼───────┼───────┼───────┼───────┼───────┤
│       │ 01 ⭐● │ 02 ⚪◐ │ 03 ⚪◐ │ 04 ⚪◐ │ 05 ⚪◐ │ 06 ⚪◐ │
...
```

### Blog-Style Format

```
═══════════════════════════════════════════════════════════════════════════════
               Myanmar Calendar - April 2024
               Tagu 1386 ME
═══════════════════════════════════════════════════════════════════════════════

🌟 MONTH HIGHLIGHTS:
───────────────────
🎉 Thingyan Water Festival
⭐ 1: New Moon Day
⭐ 15: Full Moon Sabbath

📊 MONTHLY STATISTICS:
─────────────────────
Buddhist Sabbath Days: 4
Auspicious Days: 8
Festival Days: 5
Days to avoid: 6
```

## Implementation Details

### Architecture

- **MyanmarMonthlyCalendar** class handles monthly calendar generation
- **MyanmarMonthlyData** structure stores month data and statistics
- **Multiple format generators** for different output styles
- **Integration** with existing Myanmar calendar system

### File Structure

```
include/myanmar_monthly_calendar.h    - Header file
src/myanmar_monthly_calendar.cpp      - Implementation
demo_myanmar_monthly_calendar.sh      - Demo script
```

### Key Components

1. **Calendar Data Calculation**
   - Gregorian to Myanmar date conversion
   - Astrological day calculations
   - Festival identification
   - Quality assessment

2. **Layout Generation**
   - Grid-based calendar layout
   - Cell formatting for different styles
   - Header and legend generation
   - Statistical summaries

3. **Export Capabilities**
   - JSON for web applications
   - CSV for data analysis
   - HTML for web display
   - Multiple text formats

## Inspiration

This feature is inspired by the Myanmar calendar layouts from [mmcal.blogspot.com](https://mmcal.blogspot.com/), which provides traditional Myanmar calendar images. Our implementation:

- Maintains the aesthetic appeal of traditional Myanmar calendars
- Adds modern visual elements and symbols
- Provides multiple output formats for different use cases
- Integrates seamlessly with the existing astrological system

## Technical References

- **Myanmar Calendar Algorithm**: Based on yan9a/mmcal implementation
- **Swiss Ephemeris**: For astronomical calculations
- **Unicode Support**: For Myanmar script and symbols
- **ASCII Art**: For beautiful terminal output

## Demo

Run the demo script to see all formats:

```bash
./demo_myanmar_monthly_calendar.sh
```

This will demonstrate:
- All available output formats
- Format comparison
- Usage examples
- Export capabilities

## Future Enhancements

Potential improvements:
- Color output support
- Interactive calendar navigation
- PDF export capability
- Multiple language support
- Custom festival configuration
- Location-specific calculations

---

*Created as part of the horoscope CLI application enhancement, inspired by mmcal.blogspot.com design.*
