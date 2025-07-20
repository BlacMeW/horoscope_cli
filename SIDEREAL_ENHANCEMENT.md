# Sidereal Zodiac and Ayanamsa Enhancement Summary

## 🌟 New Features Added

### 1. Zodiac Calculation Modes
- **Tropical Zodiac** (default): Season-based, Western astrology standard
- **Sidereal Zodiac**: Star-based, Vedic astrology standard

### 2. Ayanamsa (Precession Correction) Support
Added support for 21 different ayanamsa types:

**Popular Ayanamsas:**
- `lahiri` - Lahiri/Chitrapaksha (most common in Vedic astrology)
- `fagan-bradley` - Fagan-Bradley (Western sidereal)
- `raman` - B.V. Raman
- `krishnamurti` - K.S. Krishnamurti (KP System)
- `yukteshwar` - Sri Yukteshwar
- `jn_bhasin` - J.N. Bhasin

**Traditional Ayanamsas:**
- `sassanian` - Sassanian
- `galactic_center` - Galactic Center at 0° Sagittarius
- `hipparchos` - Hipparchos

**Babylonian Systems:**
- `babylonian_kugler1`, `babylonian_kugler2`, `babylonian_kugler3`
- `babylonian_huber`, `babylonian_etpsc`

**Astronomical Reference Points:**
- `j2000`, `j1900`, `b1950`
- `aldebaran_15tau` - Aldebaran at 15° Taurus

### 3. Advanced Calculation Flags
Enhanced precision and coordinate system options:

**Coordinate Systems:**
- `geocentric` (default) - Earth-centered
- `heliocentric` - Sun-centered  
- `barycentric` - Solar system barycenter
- `topocentric` - Observer location-centered

**Position Types:**
- `apparent` (default) - Includes aberration & light deflection
- `true_geometric` - True geometric positions
- `astrometric` - No aberration or deflection corrections

**Precision Options:**
- `high_precision_speed` - High precision planetary speeds
- `j2000_equinox` - J2000.0 reference frame
- `mean_equinox` - No nutation corrections
- `equatorial` - Equatorial coordinates instead of ecliptic

## 🚀 New Command Line Options

### Basic Sidereal Usage
```bash
# Sidereal chart with default Lahiri ayanamsa
horoscope_cli --date 1990-01-15 --time 14:30:00 \
              --lat 40.7128 --lon -74.0060 --timezone -5 \
              --zodiac-mode sidereal

# Specify different ayanamsa
horoscope_cli --date 1990-01-15 --time 14:30:00 \
              --lat 40.7128 --lon -74.0060 --timezone -5 \
              --zodiac-mode sidereal --ayanamsa krishnamurti
```

### High Precision Calculations
```bash
# High precision sidereal chart
horoscope_cli --date 1990-01-15 --time 14:30:00 \
              --lat 40.7128 --lon -74.0060 --timezone -5 \
              --zodiac-mode sidereal --ayanamsa lahiri \
              --calculation-flags high_precision_speed,true_geometric

# Multiple calculation flags
horoscope_cli --date 1990-01-15 --time 14:30:00 \
              --lat 40.7128 --lon -74.0060 --timezone -5 \
              --calculation-flags heliocentric,high_precision_speed
```

### KP System Integration
```bash
# KP System chart with Krishnamurti ayanamsa
horoscope_cli --date 1990-01-15 --time 14:30:00 \
              --lat 40.7128 --lon -74.0060 --timezone -5 \
              --zodiac-mode sidereal --ayanamsa krishnamurti \
              --chart-style north-indian --kp-table
```

## 💻 API/JSON Integration
```bash
# JSON output with sidereal calculations
horoscope_cli --date 1990-01-15 --time 14:30:00 \
              --lat 40.7128 --lon -74.0060 --timezone -5 \
              --zodiac-mode sidereal --ayanamsa lahiri \
              --output json
```

## 🔧 Technical Implementation

### Core Classes Enhanced
- **`AyanamsaType`** - New enum with 21 ayanamsa types
- **`ZodiacMode`** - Tropical/Sidereal mode enum
- **`CalculationFlag`** - Advanced calculation options
- **`HoroscopeCalculator`** - Enhanced with sidereal support
- **`PlanetCalculator`** - Zodiac mode and ayanamsa integration
- **`EphemerisManager`** - Swiss Ephemeris flag integration
- **`BirthChart`** - Displays zodiac mode and ayanamsa in headers

### Swiss Ephemeris Integration
- Full integration with Swiss Ephemeris sidereal calculation system
- Automatic ayanamsa setting via `swe_set_sid_mode()`
- Support for all Swiss Ephemeris calculation flags
- High precision calculations with proper flag combinations

### Backward Compatibility
- All existing tropical calculations remain unchanged
- Default behavior preserved (tropical zodiac)
- Existing command line options work exactly as before
- No breaking changes to existing functionality

## 🎯 Usage Scenarios

### Vedic Astrology
```bash
# Traditional Vedic chart with Lahiri ayanamsa
horoscope_cli --date 1990-01-15 --time 14:30:00 \
              --location "New Delhi" \
              --zodiac-mode sidereal --ayanamsa lahiri \
              --chart-style north-indian
```

### KP System
```bash
# Krishnamurti Paddhati system
horoscope_cli --date 1990-01-15 --time 14:30:00 \
              --location "Chennai" \
              --zodiac-mode sidereal --ayanamsa krishnamurti \
              --kp-table --chart-style south-indian
```

### Western Sidereal
```bash
# Western sidereal with Fagan-Bradley ayanamsa
horoscope_cli --date 1990-01-15 --time 14:30:00 \
              --location "Los Angeles" \
              --zodiac-mode sidereal --ayanamsa fagan-bradley \
              --chart-style western
```

### Research/Academic
```bash
# High precision research calculations
horoscope_cli --date 1990-01-15 --time 14:30:00 \
              --lat 40.7128 --lon -74.0060 --timezone -5 \
              --zodiac-mode sidereal --ayanamsa lahiri \
              --calculation-flags high_precision_speed,true_geometric,j2000_equinox \
              --output json
```

## 📊 Precision & Accuracy
- Arc-second precision maintained for all calculations  
- Swiss Ephemeris provides NASA-quality accuracy
- Support for historical dates (6000 BC - 7000 AD)
- Multiple calculation methods for different precision needs
- Full integration with existing eclipse, conjunction, and ephemeris features

## 🎉 Benefits
1. **Professional Grade**: Industry-standard Swiss Ephemeris integration
2. **Comprehensive**: 21 different ayanamsa systems supported
3. **Flexible**: Multiple coordinate systems and precision levels
4. **Compatible**: Works with all existing chart styles and features
5. **Accessible**: Simple command line interface
6. **Programmable**: JSON output for software integration

This enhancement makes the horoscope CLI a complete professional tool for both Western tropical and Vedic sidereal astrology, supporting the most widely used calculation methods and precision requirements in the field.
