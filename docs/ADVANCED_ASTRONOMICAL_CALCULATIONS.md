# Advanced Astronomical Calculations Reference

## Table of Contents
1. [Multi-Coordinate System Support](#multi-coordinate-system-support)
2. [Enhanced Atmospheric Refraction](#enhanced-atmospheric-refraction)
3. [Polar Region Handling](#polar-region-handling)
4. [High-Precision Time Systems](#high-precision-time-systems)
5. [Advanced Rise/Set/Culmination](#advanced-risesetculmination)
6. [Horizon Dip Calculations](#horizon-dip-calculations)
7. [Leap Second Handling](#leap-second-handling)
8. [Implementation Examples](#implementation-examples)

## Multi-Coordinate System Support

### Overview
Modern astronomical calculations require multiple coordinate systems to achieve maximum accuracy:

1. **Astrometric Coordinates (ICRF/J2000)**: Raw coordinates without atmospheric effects
2. **Apparent Coordinates**: Corrected for light-time, aberration, precession, nutation
3. **Topocentric Coordinates**: As seen from observer's location with atmospheric refraction

### Coordinate Systems

#### ICRF/J2000 (Astrometric)
- **Purpose**: Standard reference frame for precise positioning
- **Features**: No atmospheric corrections, geometric positions only
- **Use Case**: Scientific calculations, catalog positions
- **Accuracy**: Sub-arcsecond precision

#### True Equinox of Date (Apparent)
- **Purpose**: Coordinates corrected for Earth's motion and orientation
- **Corrections Applied**:
  - Precession (Earth's axis wobble over centuries)
  - Nutation (short-term axis variations)
  - Light-time delay
  - Stellar aberration
- **Use Case**: Navigation, precise ephemeris

#### Topocentric Refracted (Observer-Based)
- **Purpose**: What observer actually sees
- **Corrections Applied**:
  - All apparent coordinate corrections
  - Atmospheric refraction
  - Horizon dip
  - Geographic parallax
- **Use Case**: Visual observations, sunrise/sunset times

### Implementation Structure

```cpp
struct CoordinateSet {
    // Astrometric coordinates (ICRF/J2000)
    struct {
        double rightAscension;     // RA in degrees
        double declination;        // Dec in degrees
        double distance;           // Distance in AU
    } astrometric;

    // Apparent coordinates (true equinox of date)
    struct {
        double rightAscension;     // Apparent RA
        double declination;        // Apparent Dec
        double eclipticLongitude;  // Ecliptic longitude
        double eclipticLatitude;   // Ecliptic latitude
    } apparent;

    // Topocentric coordinates (observer location)
    struct {
        double rightAscension;     // Topocentric RA
        double declination;        // Topocentric Dec
        double azimuth;            // Azimuth (0°=N, 90°=E)
        double elevation;          // Elevation above horizon
        double hourAngle;          // Local hour angle
    } topocentric;
};
```

## Enhanced Atmospheric Refraction

### Refraction Models

#### Standard Atmospheric Model
```cpp
struct AtmosphericModel {
    double pressure;           // Atmospheric pressure (mbar)
    double temperature;        // Temperature (Celsius)
    double humidity;           // Relative humidity (0-1)
    double wavelength;         // Light wavelength (micrometers)
    double lapseRate;          // Temperature lapse rate
};

// Standard parameters
AtmosphericModel standardAtmosphere = {
    .pressure = 1013.25,       // Standard pressure
    .temperature = 15.0,       // Standard temperature
    .humidity = 0.5,           // 50% humidity
    .wavelength = 0.55,        // Yellow light
    .lapseRate = 0.0065        // Standard lapse rate
};
```

#### Seasonal Variations
```cpp
struct SeasonalCorrections {
    double pressureVariation;  // Seasonal pressure change
    double temperatureRange;   // Annual temperature range
    double humidityPattern;    // Seasonal humidity pattern
    double latitudeEffect;     // Latitude-dependent corrections
};

// Bangkok tropical corrections
SeasonalCorrections bangkokCorrections = {
    .pressureVariation = 7.0,  // ±7 mbar seasonal variation
    .temperatureRange = 8.0,   // 8°C annual range
    .humidityPattern = 0.3,    // 30% humidity variation
    .latitudeEffect = 0.85     // Tropical latitude factor
};
```

#### Advanced Refraction Formula
```cpp
double calculateRefraction(double elevation, const AtmosphericModel& atm) {
    if (elevation < -2.0) return 0.0;  // Below useful horizon

    // Barometric and temperature corrections
    double pressureCorrection = atm.pressure / 1013.25;
    double temperatureCorrection = 283.0 / (273.0 + atm.temperature);

    // Basic refraction formula (Bennett 1982)
    double refraction = 1.0 / tan(radians(elevation + 7.31 / (elevation + 4.4)));

    // Apply atmospheric corrections
    refraction *= pressureCorrection * temperatureCorrection;

    // Humidity correction for lower elevations
    if (elevation < 15.0) {
        double humidityCorrection = 1.0 + 0.0013 * atm.humidity * (1.0 - elevation / 15.0);
        refraction *= humidityCorrection;
    }

    return refraction / 60.0;  // Convert to degrees
}
```

## Polar Region Handling

### Challenge Overview
At extreme latitudes (>60°), celestial objects may:
- Never rise or set for extended periods
- Have extremely long twilight periods
- Experience rapid azimuth changes near horizon

### Detection and Handling

#### Polar Night/Day Detection
```cpp
struct PolarConditions {
    bool isPolarNight;         // Sun never rises
    bool isPolarDay;           // Sun never sets
    bool isExtendedTwilight;   // Extended twilight period
    double continuousDays;     // Days of continuous condition
    std::string description;   // Human-readable description
};

PolarConditions detectPolarConditions(double latitude, double declination, double julianDay) {
    PolarConditions conditions = {};

    // Calculate limiting latitude for given declination
    double limitingLatitude = 90.0 - abs(declination);

    if (abs(latitude) > limitingLatitude) {
        if ((latitude > 0 && declination > 0) || (latitude < 0 && declination < 0)) {
            conditions.isPolarDay = true;
            conditions.description = "Continuous daylight";
        } else {
            conditions.isPolarNight = true;
            conditions.description = "Continuous night";
        }

        // Calculate duration
        conditions.continuousDays = calculatePolarDuration(latitude, declination, julianDay);
    }

    return conditions;
}
```

#### Fallback Calculations
```cpp
void handlePolarRiseSet(PanchangaData& panchanga, double latitude, const PolarConditions& polar) {
    if (polar.isPolarDay) {
        // Sun/Moon continuously above horizon
        panchanga.sunriseTime = 0.0;        // Midnight
        panchanga.sunsetTime = 24.0;        // Next midnight
        panchanga.dayLength = 24.0;
        panchanga.nightLength = 0.0;
        panchanga.specialEvents.push_back("Polar Day - Continuous Sunlight");
    }
    else if (polar.isPolarNight) {
        // Sun/Moon continuously below horizon
        panchanga.sunriseTime = -1.0;       // No sunrise
        panchanga.sunsetTime = -1.0;        // No sunset
        panchanga.dayLength = 0.0;
        panchanga.nightLength = 24.0;
        panchanga.specialEvents.push_back("Polar Night - Continuous Darkness");
    }
    else {
        // Use extended search window for polar regions
        calculateWithExtendedSearch(panchanga, latitude, 72.0);  // 3-day search
    }
}
```

## High-Precision Time Systems

### Time Scale Conversions

#### Multiple Time Standards
```cpp
enum class TimeScale {
    UTC,           // Coordinated Universal Time
    UT1,           // Universal Time (Earth rotation)
    TDB,           // Barycentric Dynamical Time
    TT,            // Terrestrial Time
    TAI,           // International Atomic Time
    GPS            // GPS Time
};

struct TimeConversion {
    double utcToTdb(double jdUtc) {
        // Convert UTC to TDB for precise ephemeris calculations
        double deltaT = getDeltaT(jdUtc);
        double ttJd = jdUtc + deltaT / 86400.0;
        return ttJd + getTdbMinusTt(ttJd) / 86400.0;
    }

    double getDeltaT(double jdUtc) {
        // Delta T = TT - UT1 (in seconds)
        // Polynomial approximation for different epochs
        double year = jdToYear(jdUtc);

        if (year >= 2005 && year < 2050) {
            // Modern era (use IERS data)
            return polynomialDeltaT(year);
        } else if (year >= 1900 && year < 2005) {
            // 20th century approximation
            return historicalDeltaT(year);
        } else {
            // Long-term extrapolation
            return extrapolatedDeltaT(year);
        }
    }
};
```

#### Leap Second Management
```cpp
struct LeapSecondTable {
    std::vector<std::pair<double, int>> leapSeconds;  // JD and cumulative leap seconds

    void initializeLeapSeconds() {
        // Historical leap seconds (1972-present)
        leapSeconds = {
            {2441317.5, 10},  // 1972-01-01: +10s
            {2441499.5, 11},  // 1972-07-01: +11s
            {2441683.5, 12},  // 1973-01-01: +12s
            // ... continue with all leap seconds
            {2457754.5, 37},  // 2017-01-01: +37s (current)
        };
    }

    int getLeapSeconds(double jdUtc) {
        for (auto it = leapSeconds.rbegin(); it != leapSeconds.rend(); ++it) {
            if (jdUtc >= it->first) {
                return it->second;
            }
        }
        return 0;  // Before 1972
    }
};
```

## Advanced Rise/Set/Culmination

### Multi-Body Calculations

#### Simultaneous Object Tracking
```cpp
struct RiseSetEvent {
    std::string objectName;
    std::string eventType;    // "rise", "set", "culmination"
    double julianDay;
    double localTime;
    double azimuth;
    double elevation;
    bool isValid;
};

class AdvancedRiseSetCalculator {
private:
    static constexpr double SEARCH_PRECISION = 1.0 / 1440.0;  // 1 minute precision
    static constexpr double MAX_SEARCH_DAYS = 3.0;             // Maximum search window

public:
    std::vector<RiseSetEvent> calculateAllEvents(double jdStart, double latitude,
                                                 double longitude, double timezone) {
        std::vector<RiseSetEvent> events;

        // Calculate for Sun
        addSolarEvents(events, jdStart, latitude, longitude, timezone);

        // Calculate for Moon
        addLunarEvents(events, jdStart, latitude, longitude, timezone);

        // Calculate for planets (optional)
        addPlanetaryEvents(events, jdStart, latitude, longitude, timezone);

        // Sort chronologically
        std::sort(events.begin(), events.end(),
                 [](const RiseSetEvent& a, const RiseSetEvent& b) {
                     return a.julianDay < b.julianDay;
                 });

        return events;
    }

private:
    void addSolarEvents(std::vector<RiseSetEvent>& events, double jdStart,
                       double latitude, double longitude, double timezone) {
        // Search for sunrise
        auto sunrise = findRiseEvent(SE_SUN, jdStart, latitude, longitude, timezone);
        if (sunrise.isValid) events.push_back(sunrise);

        // Search for sunset
        auto sunset = findSetEvent(SE_SUN, jdStart, latitude, longitude, timezone);
        if (sunset.isValid) events.push_back(sunset);

        // Search for culmination (always possible)
        auto culmination = findCulminationEvent(SE_SUN, jdStart, latitude, longitude, timezone);
        events.push_back(culmination);
    }
};
```

#### Elevation-Based Calculations
```cpp
double calculateCustomHorizon(double observerElevation, double targetElevation = 0.0) {
    // Geometric horizon depression
    double geometricHorizon = -sqrt(2.0 * observerElevation / EARTH_RADIUS_KM) * 180.0 / M_PI;

    // Standard refraction correction
    double refractionCorrection = 0.5667;  // 34 arcminutes

    // Solar/lunar semidiameter
    double semidiameter = 0.2667;  // 16 arcminutes

    return geometricHorizon - refractionCorrection - semidiameter + targetElevation;
}
```

## Horizon Dip Calculations

### Geometric Horizon Depression

#### Mathematical Foundation
```cpp
class HorizonCalculator {
public:
    struct HorizonData {
        double geometricHorizon;   // Pure geometric horizon
        double apparentHorizon;    // With refraction
        double nauticalHorizon;    // Nautical definition
        double astronomicalHorizon; // Astronomical definition
    };

    HorizonData calculateHorizon(double observerHeight, double temperature = 15.0,
                                double pressure = 1013.25) {
        HorizonData horizon = {};

        // Geometric horizon (no refraction)
        horizon.geometricHorizon = calculateGeometricHorizon(observerHeight);

        // Apparent horizon (with refraction)
        double refractionCorrection = calculateRefractionCorrection(temperature, pressure);
        horizon.apparentHorizon = horizon.geometricHorizon - refractionCorrection;

        // Nautical horizon (center of object at geometric horizon)
        horizon.nauticalHorizon = horizon.geometricHorizon;

        // Astronomical horizon (upper limb at apparent horizon)
        horizon.astronomicalHorizon = horizon.apparentHorizon - 0.2667;  // Solar semidiameter

        return horizon;
    }

private:
    double calculateGeometricHorizon(double height) {
        // Height in meters, result in degrees
        return -sqrt(2.0 * height / 1000.0 / EARTH_RADIUS_KM) * 180.0 / M_PI;
    }

    double calculateRefractionCorrection(double temperature, double pressure) {
        // Standard atmospheric refraction at horizon
        double standardRefraction = 0.5667;  // 34.0 arcminutes

        // Temperature and pressure corrections
        double tempCorrection = 283.0 / (273.0 + temperature);
        double pressureCorrection = pressure / 1013.25;

        return standardRefraction * tempCorrection * pressureCorrection;
    }
};
```

### Observer Position Corrections

#### Height-Based Adjustments
```cpp
struct ObserverCorrections {
    double heightCorrection;     // Observer height effect
    double curvatureCorrection; // Earth curvature effect
    double refractionCorrection; // Atmospheric refraction
    double parallaxCorrection;   // Geocentric parallax
};

ObserverCorrections calculateObserverCorrections(double latitude, double longitude,
                                               double elevation, int celestialBody) {
    ObserverCorrections corrections = {};

    // Height above sea level correction
    corrections.heightCorrection = sqrt(2.0 * elevation / 1000.0 / EARTH_RADIUS_KM) * 180.0 / M_PI;

    // Earth curvature (already included in height correction)
    corrections.curvatureCorrection = 0.0;

    // Atmospheric refraction (altitude-dependent)
    corrections.refractionCorrection = calculateDifferentialRefraction(elevation);

    // Geocentric parallax (significant only for Moon)
    if (celestialBody == SE_MOON) {
        corrections.parallaxCorrection = calculateLunarParallax(latitude, elevation);
    }

    return corrections;
}
```

## Implementation Examples

### Complete Rise/Set Calculator
```cpp
class EnhancedAstronomicalCalculator {
public:
    struct CalculationResult {
        bool success;
        double julianDay;
        double localTime;
        CoordinateSet coordinates;
        AtmosphericModel atmosphere;
        std::string notes;
    };

    CalculationResult calculatePreciseRiseSet(int body, double jdStart,
                                            double latitude, double longitude,
                                            double elevation, double timezone) {
        CalculationResult result = {};

        try {
            // Setup atmospheric parameters
            AtmosphericModel atmosphere = getSeasonalAtmosphere(jdStart, latitude, longitude);

            // Calculate custom horizon
            double customHorizon = calculateCustomHorizon(elevation);

            // Setup Swiss Ephemeris parameters
            int sweFlags = SEFLG_SWIEPH | SEFLG_TOPOCTR | SEFLG_SPEED;
            swe_set_topo(longitude, latitude, elevation);

            // Perform rise/set calculation
            double riseSet[10];
            char errorString[AS_MAXCH];

            int sweResult = swe_rise_trans(jdStart, body, nullptr, sweFlags,
                                         SE_CALC_RISE, longitude, latitude, elevation,
                                         atmosphere.pressure, atmosphere.temperature,
                                         customHorizon, riseSet, errorString);

            if (sweResult == ERR) {
                // Handle polar conditions
                PolarConditions polar = detectPolarConditions(latitude,
                                          getCurrentDeclination(body, jdStart), jdStart);
                return handlePolarConditions(polar, jdStart, latitude);
            }

            // Calculate all coordinate systems
            result.coordinates = calculateAllCoordinates(body, riseSet[0],
                                                       latitude, longitude, elevation);

            result.success = true;
            result.julianDay = riseSet[0];
            result.localTime = (riseSet[0] - floor(riseSet[0])) * 24.0 + timezone;
            result.atmosphere = atmosphere;

        } catch (const std::exception& e) {
            result.success = false;
            result.notes = "Calculation error: " + std::string(e.what());
        }

        return result;
    }

private:
    AtmosphericModel getSeasonalAtmosphere(double jd, double latitude, double longitude) {
        AtmosphericModel model = {};

        // Extract month for seasonal corrections
        int year, month, day;
        double hour;
        swe_jdet_to_utc(jd, SE_GREG_CAL, &year, &month, &day, &hour);

        // Apply seasonal corrections based on location
        getSeasonalAtmosphericParams(month, latitude, longitude,
                                   &model.pressure, &model.temperature);

        // Set other parameters
        model.humidity = 0.6;        // 60% average humidity
        model.wavelength = 0.55;     // Yellow light
        model.lapseRate = 0.0065;    // Standard atmosphere

        return model;
    }
};
```

### Integration with Existing Code
```cpp
void HinduCalendar::calculateAdvancedSunMoonTimes(PanchangaData& panchanga,
                                                 double latitude, double longitude) const {
    EnhancedAstronomicalCalculator calculator;

    // Calculate precise sunrise with all corrections
    auto sunrise = calculator.calculatePreciseRiseSet(SE_SUN, panchanga.julianDay,
                                                     latitude, longitude, 0.0, 0.0);

    if (sunrise.success) {
        panchanga.sunriseTime = sunrise.localTime;
        // Store additional coordinate information
        panchanga.sunriseAzimuth = sunrise.coordinates.topocentric.azimuth;
        panchanga.sunriseElevation = sunrise.coordinates.topocentric.elevation;
    }

    // Similar calculations for sunset, moonrise, moonset
    // ...

    // Calculate culmination times (always possible)
    auto sunCulmination = calculateCulminationTime(SE_SUN, panchanga.julianDay, latitude);
    panchanga.sunCulminationTime = sunCulmination.localTime;
}
```

## Summary

This advanced astronomical calculation system provides:

1. **Multi-coordinate system support** for maximum accuracy
2. **Enhanced atmospheric refraction** with seasonal corrections
3. **Robust polar region handling** with appropriate fallbacks
4. **High-precision time systems** with leap second support
5. **Advanced rise/set calculations** with custom horizons
6. **Horizon dip corrections** for observer elevation
7. **Comprehensive error handling** for edge cases

The implementation maintains compatibility with your existing Hindu calendar system while adding sophisticated astronomical accuracy comparable to professional ephemeris software.
