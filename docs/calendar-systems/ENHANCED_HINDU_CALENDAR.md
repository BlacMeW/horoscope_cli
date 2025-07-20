# Enhanced Hindu Calendar System

## Overview

The Hindu calendar system in `horoscope_cli` has been significantly enhanced to provide comprehensive Panchanga calculations with configurable astronomical parameters, matching traditional systems like `pancanga3.14.pl` while maintaining modern Swiss Ephemeris accuracy.

## Key Enhancements

### 1. Ayanamsa Configuration Options
The system now supports multiple ayanamsa (precession correction) methods:

- **Lahiri/Chitrapaksha** (Default) - Most widely used in India
- **B.V. Raman** - Popular in South India
- **K.S. Krishnamurti** - Used in KP Astrology
- **Sri Yukteshwar** - Based on Yukteshwar's calculations
- **J.N. Bhasin** - Alternative traditional method
- **Sassanian** - Ancient Persian system
- **Galactic Center** - Modern astronomical alignment
- **True Chitra/Revati/Pushya** - Star-based calculations

### 2. Calculation Methods
Multiple calculation approaches are supported:

- **Drik Siddhanta** (Default) - Modern precise astronomical calculations
- **Surya Siddhanta** - Traditional Hindu astronomy text
- **Arya Siddhanta** - Ancient mathematical astronomy
- **Brahma Siddhanta** - Classical Hindu astronomy
- **Modern Mixed** - Hybrid approach combining traditional and modern methods

### 3. Calendar System Types
Three calendar system approaches:

- **Lunar-based** - Pure lunar calendar system
- **Solar-based** - Solar calendar with lunar tithis
- **Luni-Solar** (Default) - Combined system (most traditional)

### 4. Date Conversion Utilities
Bidirectional date conversion between Hindu and Gregorian calendars:

```cpp
// Hindu to Gregorian
std::string gregorianDate = calendar.hinduDateToGregorian(2081, 12, 1, false);

// Gregorian to Hindu
std::string hinduDate = calendar.gregorianDateToHindu(2024, 3, 15);
```

### 5. Multiple Year Systems
Support for different Hindu year calculation systems:

- **Vikram Samvat** (57 BC epoch) - Most common
- **Shaka Samvat** (79 AD epoch) - Official in India
- **Kali Yuga** (3102 BC epoch) - Traditional cosmic cycle

## Usage Examples

### Basic Panchanga Calculation
```bash
./bin/horoscope_cli --date 2024-03-15 --time 12:00:00 \
    --lat 28.6139 --lon 77.2090 --timezone 5.5 --panchanga
```

### JSON Format Output
```bash
./bin/horoscope_cli --date 2024-03-15 --time 12:00:00 \
    --lat 28.6139 --lon 77.2090 --timezone 5.5 \
    --panchanga --panchanga-format json
```

### Date Range Calculation
```bash
./bin/horoscope_cli --panchanga-range 2024-03-15 2024-03-20 \
    --lat 28.6139 --lon 77.2090 --timezone 5.5
```

## Enhanced Output Features

### Configuration Display
The output now shows the current calculation settings:
```
🔧 CALCULATION SETTINGS:
   Ayanamsa: Lahiri/Chitrapaksha
   Method: Drik Siddhanta (Modern Astronomical)
   System: Luni-Solar
```

### Comprehensive Panchanga Information
- **Tithi** with end time calculation
- **Vara** (weekday)
- **Nakshatra** with transition time
- **Yoga** with duration
- **Karana** with half-day calculation

### Special Event Detection
- **Sankranti** (solar transitions)
- **Ekadashi** (fasting days)
- **Purnima/Amavasya** (full/new moon)
- **Festival identification**

### Muhurta Assessment
Basic auspiciousness evaluation based on:
- Tithi quality
- Nakshatra nature
- Yoga effects
- Combined assessment

## Compatibility with Traditional Systems

The enhanced system is designed to be compatible with traditional pancanga calculation systems like `pancanga3.14.pl`:

1. **Swiss Ephemeris Integration** - Uses the most accurate modern planetary calculations
2. **Traditional Algorithms** - Implements classical Hindu astronomical methods
3. **Configurable Parameters** - Allows matching different traditional approaches
4. **Accurate Timing** - Precise calculation of element transition times

## Technical Implementation

### Core Classes
- `HinduCalendar` - Main calculation engine
- `PanchangaData` - Data structure for results
- Enhanced enums for all configuration options

### Swiss Ephemeris Integration
- Configurable ayanamsa settings
- Sidereal vs tropical coordinate systems
- High-precision planetary positions

### Date Conversion Algorithms
- Julian Day Number calculations
- Hindu calendar mathematics
- Paksha (fortnight) determination

## Testing and Validation

The system includes comprehensive test suites:

- `test_enhanced_hindu_calendar.sh` - Feature demonstration
- `test_date_conversion.cpp` - Date conversion validation
- `validate_hindu_calendar.sh` - Accuracy verification

## Future Enhancements

Planned improvements include:
- Regional calendar variations (Tamil, Bengali, etc.)
- Advanced muhurta calculations
- Eclipse and planetary event predictions
- Integration with Western astrology features
- Additional festival and observance databases

## Conclusion

The enhanced Hindu calendar system provides a comprehensive, accurate, and configurable Panchanga calculation engine suitable for both traditional and modern astrological applications. It maintains compatibility with established systems while providing the accuracy and flexibility required for contemporary use.
