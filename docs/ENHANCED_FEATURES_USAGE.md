# Enhanced Horoscope CLI - Advanced Astronomical Features Usage Guide

## Overview

The horoscope CLI has been enhanced with advanced astronomical calculation features inspired by modern astronomy engines. These features provide multi-coordinate system support, sophisticated atmospheric corrections, polar region handling, and high-precision time conversions.

## New Features

### 1. Multi-Coordinate System Support

The enhanced system now provides three coordinate systems for maximum accuracy:

#### Example Usage:
```cpp
#include "hindu_calendar.h"

HinduCalendar calendar;
calendar.initialize();

// Calculate all coordinate systems for the Sun
double julianDay = 2460883.5;  // July 27, 2025
double latitude = 13.7563;     // Bangkok
double longitude = 100.5018;
double elevation = 2.0;        // 2 meters above sea level

auto coords = calendar.calculateAllCoordinates(SE_SUN, julianDay, latitude, longitude, elevation);

// Access different coordinate systems
std::cout << "Astrometric RA: " << coords.astrometric.rightAscension << "°" << std::endl;
std::cout << "Apparent RA: " << coords.apparent.rightAscension << "°" << std::endl;
std::cout << "Topocentric Azimuth: " << coords.topocentric.azimuth << "°" << std::endl;
std::cout << "Topocentric Elevation: " << coords.topocentric.elevation << "°" << std::endl;
```

### 2. Enhanced Atmospheric Modeling

#### Seasonal Atmospheric Corrections:
```cpp
// Get seasonal atmospheric parameters
auto atmosphere = calendar.getSeasonalAtmosphere(julianDay, latitude, longitude);

std::cout << "Pressure: " << atmosphere.pressure << " mbar" << std::endl;
std::cout << "Temperature: " << atmosphere.temperature << "°C" << std::endl;
std::cout << "Humidity: " << atmosphere.humidity * 100 << "%" << std::endl;

// Calculate refraction with current atmospheric conditions
double elevation = 10.0;  // 10 degrees above horizon
double refraction = calendar.calculateRefraction(elevation, atmosphere);
std::cout << "Atmospheric refraction: " << refraction * 3600 << " arcseconds" << std::endl;
```

### 3. Polar Region Handling

#### Detect and Handle Polar Conditions:
```cpp
// Test for polar conditions (e.g., northern Alaska)
double polarLatitude = 70.0;  // 70°N
double sunDeclination = 23.4; // Summer solstice

auto polar = calendar.detectPolarConditions(polarLatitude, sunDeclination, julianDay);

if (polar.isPolarDay) {
    std::cout << "Polar day condition: " << polar.description << std::endl;
    std::cout << "Duration: " << polar.continuousDays << " days" << std::endl;
} else if (polar.isPolarNight) {
    std::cout << "Polar night condition: " << polar.description << std::endl;
}
```

### 4. Advanced Rise/Set/Culmination Calculations

#### Calculate All Astronomical Events:
```cpp
// Calculate all rise/set/culmination events for a day
auto events = calendar.calculateAllEvents(julianDay, latitude, longitude, 7.0, elevation);

for (const auto& event : events) {
    if (event.isValid) {
        std::cout << event.objectName << " " << event.eventType
                  << " at " << std::setprecision(2) << event.localTime << ":00"
                  << " (Az: " << event.azimuth << "°, El: " << event.elevation << "°)"
                  << std::endl;
    }
}
```

#### Precise Rise/Set with Custom Horizon:
```cpp
// Calculate precise sunrise with custom observer height
double observerHeight = 100.0;  // 100 meters above sea level
auto sunrise = calendar.calculatePreciseRiseSet(SE_SUN, julianDay, latitude, longitude,
                                               observerHeight, 7.0);

if (sunrise.isValid) {
    std::cout << "Precise sunrise: " << sunrise.localTime << std::endl;
    std::cout << "Coordinates available in all systems:" << std::endl;
    std::cout << "  Astrometric: " << sunrise.coordinates.astrometric.rightAscension << "°" << std::endl;
    std::cout << "  Apparent: " << sunrise.coordinates.apparent.rightAscension << "°" << std::endl;
    std::cout << "  Topocentric: " << sunrise.coordinates.topocentric.azimuth << "°" << std::endl;
} else {
    std::cout << "Sunrise calculation failed: " << sunrise.notes << std::endl;
}
```

### 5. Horizon Calculations

#### Custom Horizon Analysis:
```cpp
// Calculate different horizon definitions
double observerHeight = 50.0;  // 50 meters
double temperature = 28.0;     // Bangkok average
double pressure = 1010.0;      // Typical Bangkok pressure

auto horizon = calendar.calculateHorizon(observerHeight, temperature, pressure);

std::cout << "Geometric horizon: " << horizon.geometricHorizon << "°" << std::endl;
std::cout << "Apparent horizon: " << horizon.apparentHorizon << "°" << std::endl;
std::cout << "Nautical horizon: " << horizon.nauticalHorizon << "°" << std::endl;
std::cout << "Astronomical horizon: " << horizon.astronomicalHorizon << "°" << std::endl;

// Use custom horizon for calculations
double customHorizon = calendar.calculateCustomHorizon(observerHeight, -0.5);
std::cout << "Custom horizon for sunrise: " << customHorizon << "°" << std::endl;
```

### 6. High-Precision Time Conversions

#### Time System Conversions:
```cpp
// Convert between time systems for precise calculations
double utcJD = 2460883.5;  // UTC Julian Day
double tdbJD = calendar.utcToTdb(utcJD);  // Convert to Barycentric Dynamical Time

std::cout << "UTC JD: " << std::setprecision(9) << utcJD << std::endl;
std::cout << "TDB JD: " << std::setprecision(9) << tdbJD << std::endl;

// Get Delta T and leap seconds
double deltaT = calendar.getDeltaT(utcJD);
int leapSeconds = calendar.getLeapSeconds(utcJD);

std::cout << "Delta T: " << deltaT << " seconds" << std::endl;
std::cout << "Leap seconds: " << leapSeconds << std::endl;
```

## Integration with Existing Panchanga Calculations

### Enhanced Panchanga with Advanced Calculations:

```cpp
#include "hindu_calendar.h"
#include <iostream>
#include <iomanip>

int main() {
    // Initialize calendar
    HinduCalendar calendar;
    if (!calendar.initialize()) {
        std::cerr << "Failed to initialize calendar" << std::endl;
        return 1;
    }

    // Bangkok coordinates
    double latitude = 13.7563;
    double longitude = 100.5018;
    double elevation = 2.0;
    double timezone = 7.0;

    // Current date
    std::string dateStr = "2025-07-27";
    auto panchanga = calendar.calculatePanchanga(dateStr, latitude, longitude);

    // Display enhanced astronomical information
    std::cout << "\n=== Enhanced Astronomical Data ===" << std::endl;

    // Calculate all events for the day
    auto events = calendar.calculateAllEvents(panchanga.julianDay, latitude, longitude, timezone, elevation);

    std::cout << "\nAstronomical Events:" << std::endl;
    for (const auto& event : events) {
        if (event.isValid) {
            int hours = static_cast<int>(event.localTime);
            int minutes = static_cast<int>((event.localTime - hours) * 60);
            std::cout << "  " << event.objectName << " " << event.eventType
                      << ": " << std::setfill('0') << std::setw(2) << hours
                      << ":" << std::setw(2) << minutes
                      << " (Az: " << std::setprecision(1) << std::fixed << event.azimuth
                      << "°, El: " << event.elevation << "°)" << std::endl;
        }
    }

    // Show atmospheric conditions
    auto atmosphere = calendar.getSeasonalAtmosphere(panchanga.julianDay, latitude, longitude);
    std::cout << "\nAtmospheric Conditions:" << std::endl;
    std::cout << "  Pressure: " << atmosphere.pressure << " mbar" << std::endl;
    std::cout << "  Temperature: " << atmosphere.temperature << "°C" << std::endl;
    std::cout << "  Humidity: " << atmosphere.humidity * 100 << "%" << std::endl;

    // Show coordinate systems for the Sun
    auto sunCoords = calendar.calculateAllCoordinates(SE_SUN, panchanga.julianDay, latitude, longitude, elevation);
    std::cout << "\nSun Coordinates:" << std::endl;
    std::cout << "  Astrometric (J2000): RA " << std::setprecision(4) << sunCoords.astrometric.rightAscension
              << "°, Dec " << sunCoords.astrometric.declination << "°" << std::endl;
    std::cout << "  Apparent: RA " << sunCoords.apparent.rightAscension
              << "°, Dec " << sunCoords.apparent.declination << "°" << std::endl;
    std::cout << "  Topocentric: Az " << sunCoords.topocentric.azimuth
              << "°, El " << sunCoords.topocentric.elevation << "°" << std::endl;

    // Show horizon information
    auto horizon = calendar.calculateHorizon(elevation, atmosphere.temperature, atmosphere.pressure);
    std::cout << "\nHorizon Analysis:" << std::endl;
    std::cout << "  Geometric: " << std::setprecision(3) << horizon.geometricHorizon << "°" << std::endl;
    std::cout << "  Apparent: " << horizon.apparentHorizon << "°" << std::endl;
    std::cout << "  Astronomical: " << horizon.astronomicalHorizon << "°" << std::endl;

    return 0;
}
```

## Command Line Usage

### Enhanced CLI Options:

```bash
# Basic panchanga with enhanced calculations
./bin/horoscope_cli --panchanga --date 2025-07-27 --lat 13.7563 --lon 100.5018 --timezone 7

# With custom observer elevation
./bin/horoscope_cli --panchanga --date 2025-07-27 --lat 13.7563 --lon 100.5018 --timezone 7 --elevation 100

# Test polar conditions (high latitude)
./bin/horoscope_cli --panchanga --date 2025-07-27 --lat 70.0 --lon 0.0 --timezone 0

# Test extended search for extreme latitudes
./bin/horoscope_cli --panchanga --date 2025-12-21 --lat 80.0 --lon 0.0 --timezone 0
```

## Advanced Features for Developers

### Custom Atmospheric Models:

```cpp
// Create custom atmospheric model
HinduCalendar::AtmosphericModel customAtmosphere;
customAtmosphere.pressure = 950.0;      // Lower pressure (high altitude)
customAtmosphere.temperature = 5.0;     // Cold temperature
customAtmosphere.humidity = 0.3;        // Low humidity
customAtmosphere.wavelength = 0.55;     // Yellow light
customAtmosphere.lapseRate = 0.0080;    // Modified lapse rate

// Use in refraction calculations
double refraction = calendar.calculateRefraction(15.0, customAtmosphere);
```

### Event Filtering:

```cpp
// Filter events by type
auto allEvents = calendar.calculateAllEvents(julianDay, latitude, longitude, timezone);
std::vector<HinduCalendar::RiseSetEvent> sunEvents;

std::copy_if(allEvents.begin(), allEvents.end(), std::back_inserter(sunEvents),
    [](const HinduCalendar::RiseSetEvent& event) {
        return event.objectName == "Sun";
    });
```

### Coordinate Transformations:

```cpp
// Access different coordinate systems
auto coords = calendar.calculateAllCoordinates(SE_MOON, julianDay, latitude, longitude);

// Convert to other formats as needed
double raHours = coords.apparent.rightAscension / 15.0;  // RA in hours
double hourAngle = coords.topocentric.hourAngle;         // Hour angle

// Calculate additional derived values
double zenithDistance = 90.0 - coords.topocentric.elevation;
double airmass = 1.0 / cos(coords.topocentric.elevation * M_PI / 180.0);
```

## Error Handling

The enhanced system includes comprehensive error handling:

```cpp
// All methods include try-catch blocks
auto result = calendar.calculatePreciseRiseSet(SE_SUN, julianDay, latitude, longitude, elevation, timezone);

if (!result.isValid) {
    std::cout << "Calculation failed: " << result.notes << std::endl;

    // Check for polar conditions
    auto polar = calendar.detectPolarConditions(latitude, 0.0, julianDay);
    if (polar.isPolarDay || polar.isPolarNight) {
        std::cout << "Polar condition detected: " << polar.description << std::endl;
    }
}
```

## Performance Considerations

1. **Coordinate Calculations**: Computing all three coordinate systems is more expensive than single system
2. **Event Calculations**: Multiple rise/set/culmination calculations per day
3. **Atmospheric Modeling**: Seasonal calculations add minimal overhead
4. **Polar Handling**: Extended search windows increase calculation time

## Best Practices

1. **Cache Results**: Store coordinate calculations for repeated use
2. **Choose Appropriate Precision**: Use simpler calculations when high precision isn't needed
3. **Handle Edge Cases**: Always check for polar conditions at high latitudes
4. **Monitor Performance**: Profile code when calculating many events

This enhanced system provides professional-grade astronomical accuracy while maintaining compatibility with your existing Hindu calendar calculations.
