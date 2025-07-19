# API Documentation

## Overview

The Horoscope CLI is built with a modular architecture using several key classes:

## Core Classes

### `EphemerisManager`
Manages the Swiss Ephemeris library interface.

**Key Methods:**
- `initialize(ephemerisPath)` - Initialize with optional ephemeris data path
- `calculatePlanetPosition(julianDay, planet, position)` - Calculate planetary position
- `calculateHouseCusps(julianDay, lat, lon, system, cusps)` - Calculate house cusps

### `PlanetCalculator`
Calculates planetary positions for birth charts.

**Key Methods:**
- `calculateAllPlanets(birthData, positions)` - Calculate all planetary positions
- `calculatePlanet(birthData, planet, position)` - Calculate single planet
- `setPlanetsToCalculate(planets)` - Configure which planets to include

### `HouseCalculator`
Handles astrological house calculations.

**Key Methods:**
- `calculateHouses(birthData, system, cusps)` - Calculate house cusps
- `assignHousesToPlanets(cusps, positions)` - Assign house numbers to planets
- `getHouseNumber(cusps, longitude)` - Get house for longitude

### `AspectCalculator`
Calculates aspects between planets.

**Key Methods:**
- `calculateAspects(positions)` - Calculate all aspects
- `setOrbTolerance(aspect, orb)` - Set orb tolerance for aspects
- `isAspect(angle1, angle2, aspect, orb)` - Check if two angles form an aspect

### `BirthChart`
Container for complete birth chart data.

**Key Methods:**
- `setBirthData(data)` - Set birth information
- `setPlanetPositions(positions)` - Set planetary positions
- `setHouseCusps(cusps)` - Set house cusp data
- `setAspects(aspects)` - Set aspect data
- `getFormattedChart()` - Get formatted text output
- `exportToJson()` - Export as JSON

### `HoroscopeCalculator`
Main calculator class that coordinates all components.

**Key Methods:**
- `initialize(ephemerisPath)` - Initialize the calculator
- `calculateBirthChart(birthData, houseSystem, chart)` - Calculate complete chart
- `setHouseSystem(system)` - Set default house system
- `setPlanetsToCalculate(planets)` - Configure planets to include

## Data Structures

### `BirthData`
```cpp
struct BirthData {
    int year, month, day;
    int hour, minute, second;
    double latitude, longitude;
    double timezone;

    double getJulianDay() const;
    std::string getLocationString() const;
    std::string getDateTimeString() const;
};
```

### `PlanetPosition`
```cpp
struct PlanetPosition {
    Planet planet;
    double longitude, latitude;
    double distance, speed;
    ZodiacSign sign;
    int house;
    double housePosition;

    void calculateSignPosition();
    std::string getFormattedPosition() const;
};
```

### `HouseCusps`
```cpp
struct HouseCusps {
    double cusps[13];       // Houses 1-12
    double ascendant;       // 1st house cusp
    double midheaven;       // 10th house cusp
    double vertex;
    double eastPoint;

    int getHouseForLongitude(double longitude) const;
};
```

### `Aspect`
```cpp
struct Aspect {
    Planet planet1, planet2;
    AspectType type;
    double orb;
    bool isApplying;

    std::string getDescription() const;
};
```

## Enumerations

### `Planet`
- SUN, MOON, MERCURY, VENUS, MARS, JUPITER, SATURN
- URANUS, NEPTUNE, PLUTO, NORTH_NODE, SOUTH_NODE
- CHIRON, LILITH

### `ZodiacSign`
- ARIES, TAURUS, GEMINI, CANCER, LEO, VIRGO
- LIBRA, SCORPIO, SAGITTARIUS, CAPRICORN, AQUARIUS, PISCES

### `HouseSystem`
- PLACIDUS ('P'), KOCH ('K'), EQUAL ('E'), WHOLE_SIGN ('W')
- CAMPANUS ('C'), REGIOMONTANUS ('R')

### `AspectType`
- CONJUNCTION (0°), SEXTILE (60°), SQUARE (90°)
- TRINE (120°), OPPOSITION (180°)

## Usage Example

```cpp
#include "horoscope_calculator.h"

using namespace Astro;

int main() {
    // Initialize calculator
    HoroscopeCalculator calculator;
    calculator.initialize();

    // Set birth data
    BirthData birthData;
    birthData.year = 1990;
    birthData.month = 1;
    birthData.day = 15;
    birthData.hour = 14;
    birthData.minute = 30;
    birthData.second = 0;
    birthData.latitude = 40.7128;
    birthData.longitude = -74.0060;
    birthData.timezone = -5;

    // Calculate birth chart
    BirthChart chart;
    if (calculator.calculateBirthChart(birthData, HouseSystem::PLACIDUS, chart)) {
        std::cout << chart.getFormattedChart() << std::endl;
    }

    return 0;
}
```

## Error Handling

All calculation methods return boolean success/failure status. Use `getLastError()` methods to retrieve error messages when operations fail.

## Thread Safety

The classes are not thread-safe. Use separate instances or external synchronization for multi-threaded applications.
