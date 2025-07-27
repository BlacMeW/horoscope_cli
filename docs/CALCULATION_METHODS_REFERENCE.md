# Complete Calculation Methods Reference

## Table of Contents
1. [Swiss Ephemeris Core Methods](#swiss-ephemeris-core-methods)
2. [Hindu Calendar Calculations](#hindu-calendar-calculations)
3. [Astronomical Coordinate Systems](#astronomical-coordinate-systems)
4. [Time System Conversions](#time-system-conversions)
5. [Atmospheric and Refraction Models](#atmospheric-and-refraction-models)
6. [Rise/Set/Transit Calculations](#riseSettransit-calculations)
7. [Lunar Calculations](#lunar-calculations)
8. [Solar Calculations](#solar-calculations)
9. [Planetary Calculations](#planetary-calculations)
10. [Precision and Accuracy Notes](#precision-and-accuracy-notes)

## Swiss Ephemeris Core Methods

### Primary Calculation Function: `swe_calc()`
```cpp
int swe_calc(double tjd, int ipl, int32 iflag, double *xx, char *serr);
```

#### Parameters:
- **tjd**: Julian Day (Ephemeris Time/TDB)
- **ipl**: Planet identifier (SE_SUN, SE_MOON, SE_MERCURY, etc.)
- **iflag**: Calculation flags (coordinate system, precision, corrections)
- **xx**: Output array [longitude, latitude, distance, speed_long, speed_lat, speed_dist]
- **serr**: Error message buffer

#### Key Flags:
```cpp
// Coordinate Systems
SEFLG_SWIEPH     // Use Swiss Ephemeris (highest precision)
SEFLG_JPLEPH     // Use JPL ephemeris (if available)
SEFLG_MOSEPH     // Use Moshier ephemeris (medium precision)

// Reference Frames
SEFLG_HELIOCTR   // Heliocentric coordinates
SEFLG_TOPOCTR    // Topocentric coordinates (observer-based)
SEFLG_BARYCTR    // Barycentric coordinates
SEFLG_J2000      // J2000.0 coordinates
SEFLG_EQUATORIAL // Equatorial coordinates (RA/Dec)

// Corrections
SEFLG_SPEED      // Calculate velocities
SEFLG_NONUT      // No nutation
SEFLG_NOABERR    // No aberration
SEFLG_NODDEFL    // No gravitational deflection
```

### Rise/Set/Transit Function: `swe_rise_trans()`
```cpp
int swe_rise_trans(double tjd_ut, int ipl, char *starname, int32 epheflag,
                   int32 rsmi, double *geopos, double atpress, double attemp,
                   double horhgt, double *tret, char *serr);
```

#### Parameters:
- **tjd_ut**: Julian Day (Universal Time)
- **rsmi**: Rise/Set/Transit type (SE_CALC_RISE, SE_CALC_SET, SE_CALC_MTRANSIT)
- **geopos**: Geographic position [longitude, latitude, elevation]
- **atpress**: Atmospheric pressure (mbar)
- **attemp**: Temperature (Celsius)
- **horhgt**: Horizon height (degrees)

### Ayanamsa Calculation: `swe_get_ayanamsa()`
```cpp
double swe_get_ayanamsa(double tjd_et);
double swe_get_ayanamsa_ut(double tjd_ut);
```

#### Available Ayanamsas:
```cpp
SE_SIDM_FAGAN_BRADLEY    = 0   // Fagan/Bradley
SE_SIDM_LAHIRI          = 1   // Lahiri (most common)
SE_SIDM_DELUCE          = 2   // De Luce
SE_SIDM_RAMAN           = 3   // Raman
SE_SIDM_USHASHASHI      = 4   // Ushashashi
SE_SIDM_KRISHNAMURTI    = 5   // Krishnamurti
SE_SIDM_DJWHAL_KHUL     = 6   // Djwhal Khul
SE_SIDM_YUKTESHWAR      = 7   // Yukteshwar
SE_SIDM_JN_BHASIN       = 8   // J.N. Bhasin
SE_SIDM_BABYL_KUGLER1   = 9   // Babylonian Kugler 1
SE_SIDM_BABYL_KUGLER2   = 10  // Babylonian Kugler 2
SE_SIDM_BABYL_KUGLER3   = 11  // Babylonian Kugler 3
SE_SIDM_BABYL_HUBER     = 12  // Babylonian Huber
SE_SIDM_BABYL_ETPSC     = 13  // Babylonian ETPSC
SE_SIDM_ALDEBARAN_15TAU = 14  // Aldebaran at 15 Taurus
SE_SIDM_HIPPARCHOS      = 15  // Hipparchos
SE_SIDM_SASSANIAN       = 16  // Sassanian
SE_SIDM_GALCENT_0SAG    = 17  // Galactic Center at 0 Sagittarius
SE_SIDM_J2000           = 18  // J2000
SE_SIDM_J1900           = 19  // J1900
SE_SIDM_B1950           = 20  // B1950
```

## Hindu Calendar Calculations

### Tithi Calculation
```cpp
// Calculate lunar day (Tithi)
double calculateTithi(double julianDay) {
    double sun_long = getSunLongitude(julianDay);
    double moon_long = getMoonLongitude(julianDay);

    double tithi_angle = moon_long - sun_long;
    if (tithi_angle < 0) tithi_angle += 360.0;

    return tithi_angle / 12.0;  // Each tithi = 12 degrees
}

// Tithi number (1-30)
int getTithiNumber(double tithi_value) {
    return static_cast<int>(tithi_value) + 1;
}

// Tithi ending time
double getTithiEndTime(double julianDay) {
    double current_tithi = calculateTithi(julianDay);
    double next_tithi = ceil(current_tithi);

    // Binary search for exact ending time
    return findExactTransition(julianDay, current_tithi, next_tithi);
}
```

### Nakshatra Calculation
```cpp
// Calculate constellation (Nakshatra)
double calculateNakshatra(double julianDay) {
    double moon_long = getMoonLongitude(julianDay);
    double ayanamsa = swe_get_ayanamsa(julianDay);

    double sidereal_moon = moon_long - ayanamsa;
    if (sidereal_moon < 0) sidereal_moon += 360.0;

    return sidereal_moon / 13.333333;  // Each nakshatra = 13°20'
}

// Nakshatra number (1-27)
int getNakshatraNumber(double nakshatra_value) {
    return static_cast<int>(nakshatra_value) + 1;
}

// Nakshatra Pada (1-4)
int getNakshatraPada(double nakshatra_value) {
    double pada_fraction = (nakshatra_value - floor(nakshatra_value)) * 4.0;
    return static_cast<int>(pada_fraction) + 1;
}
```

### Yoga Calculation
```cpp
// Calculate Yoga (Sun + Moon longitudes)
double calculateYoga(double julianDay) {
    double sun_long = getSunLongitude(julianDay);
    double moon_long = getMoonLongitude(julianDay);

    double yoga_angle = sun_long + moon_long;
    while (yoga_angle >= 360.0) yoga_angle -= 360.0;

    return yoga_angle / 13.333333;  // Each yoga = 13°20'
}

// Yoga number (1-27)
int getYogaNumber(double yoga_value) {
    return static_cast<int>(yoga_value) + 1;
}
```

### Karana Calculation
```cpp
// Calculate Karana (half-tithi)
double calculateKarana(double julianDay) {
    double tithi = calculateTithi(julianDay);
    return tithi * 2.0;  // Each karana = 6 degrees
}

// Karana number (1-11, cyclical for moveable karanas)
int getKaranaNumber(double karana_value) {
    int karana_int = static_cast<int>(karana_value) + 1;

    // Handle fixed karanas
    if (karana_int == 1) return 1;   // Shakuni (Amavasya 2nd half)
    if (karana_int == 2) return 2;   // Chatushpada (Pratipad 1st half)
    if (karana_int == 57) return 3;  // Naga (Chaturdashi 2nd half)
    if (karana_int == 58) return 4;  // Kimstughna (Purnima 1st half)

    // Moveable karanas (cycle of 7)
    return ((karana_int - 3) % 7) + 5;  // Bava, Balava, Kaulava, Taitila, Gara, Vanija, Vishti
}
```

### Rashi (Zodiac Sign) Calculation
```cpp
// Calculate zodiac sign for a planet
int calculateRashi(double longitude, double ayanamsa) {
    double sidereal_long = longitude - ayanamsa;
    if (sidereal_long < 0) sidereal_long += 360.0;

    return static_cast<int>(sidereal_long / 30.0) + 1;  // Each rashi = 30 degrees
}

// Get rashi name
std::string getRashiName(int rashi_number) {
    const std::vector<std::string> rashi_names = {
        "Mesha", "Vrishabha", "Mithuna", "Karka", "Simha", "Kanya",
        "Tula", "Vrishchika", "Dhanu", "Makara", "Kumbha", "Meena"
    };

    if (rashi_number >= 1 && rashi_number <= 12) {
        return rashi_names[rashi_number - 1];
    }
    return "Unknown";
}
```

## Astronomical Coordinate Systems

### Ecliptic Coordinates
```cpp
struct EclipticCoordinates {
    double longitude;    // 0° to 360°
    double latitude;     // -90° to +90°
    double distance;     // AU or Earth radii
};

// Calculate ecliptic coordinates
EclipticCoordinates getEclipticCoordinates(int planet, double julianDay) {
    double xx[6];
    char serr[AS_MAXCH];

    int flags = SEFLG_SWIEPH;  // Basic ecliptic coordinates
    int result = swe_calc(julianDay, planet, flags, xx, serr);

    EclipticCoordinates coords;
    coords.longitude = xx[0];
    coords.latitude = xx[1];
    coords.distance = xx[2];

    return coords;
}
```

### Equatorial Coordinates
```cpp
struct EquatorialCoordinates {
    double rightAscension;  // 0° to 360°
    double declination;     // -90° to +90°
    double distance;        // AU or Earth radii
};

// Calculate equatorial coordinates
EquatorialCoordinates getEquatorialCoordinates(int planet, double julianDay) {
    double xx[6];
    char serr[AS_MAXCH];

    int flags = SEFLG_SWIEPH | SEFLG_EQUATORIAL;
    int result = swe_calc(julianDay, planet, flags, xx, serr);

    EquatorialCoordinates coords;
    coords.rightAscension = xx[0];
    coords.declination = xx[1];
    coords.distance = xx[2];

    return coords;
}
```

### Horizontal Coordinates
```cpp
struct HorizontalCoordinates {
    double azimuth;     // 0° to 360° (North=0°, East=90°)
    double elevation;   // -90° to +90°
    double distance;    // AU or Earth radii
};

// Calculate horizontal coordinates
HorizontalCoordinates getHorizontalCoordinates(int planet, double julianDay,
                                             double latitude, double longitude, double elevation_m) {
    // Set observer position
    swe_set_topo(longitude, latitude, elevation_m);

    double xx[6];
    char serr[AS_MAXCH];

    // Get equatorial coordinates first
    int flags = SEFLG_SWIEPH | SEFLG_TOPOCTR | SEFLG_EQUATORIAL;
    int result = swe_calc(julianDay, planet, flags, xx, serr);

    // Convert to horizontal coordinates
    double azalt[3];
    swe_azalt(julianDay, SE_EQU2HOR, longitude, latitude, elevation_m, xx[0], xx[1], azalt);

    HorizontalCoordinates coords;
    coords.azimuth = azalt[0];
    coords.elevation = azalt[1];
    coords.distance = xx[2];

    return coords;
}
```

## Time System Conversions

### Julian Day Conversions
```cpp
// Gregorian calendar to Julian Day
double gregorianToJD(int year, int month, int day, double hour) {
    return swe_julday(year, month, day, hour, SE_GREG_CAL);
}

// Julian Day to Gregorian calendar
void jdToGregorian(double jd, int &year, int &month, int &day, double &hour) {
    swe_jdut1_to_utc(jd, SE_GREG_CAL, &year, &month, &day, &hour);
}

// Current Julian Day (UTC)
double getCurrentJD() {
    return swe_julday(2025, 7, 27, 12.0, SE_GREG_CAL);  // Example current date
}
```

### Time Scale Conversions
```cpp
// UTC to Ephemeris Time (TDB)
double utcToET(double jd_utc) {
    return jd_utc + swe_deltat(jd_utc) / 86400.0;
}

// Get Delta T (ET - UT)
double getDeltaT(double jd_utc) {
    return swe_deltat(jd_utc);  // Returns seconds
}

// Local time to UTC
double localToUTC(double local_jd, double timezone_hours) {
    return local_jd - timezone_hours / 24.0;
}

// UTC to local time
double utcToLocal(double utc_jd, double timezone_hours) {
    return utc_jd + timezone_hours / 24.0;
}
```

### Sidereal Time
```cpp
// Greenwich Sidereal Time
double getGST(double jd_ut) {
    return swe_sidtime(jd_ut);  // Returns hours
}

// Local Sidereal Time
double getLST(double jd_ut, double longitude) {
    double gst = swe_sidtime(jd_ut);
    return gst + longitude / 15.0;  // Convert longitude to hours
}

// Sidereal time to mean solar time conversion
double siderealToSolar(double sidereal_hours) {
    return sidereal_hours * 0.99726957;  // Sidereal day is shorter
}
```

## Atmospheric and Refraction Models

### Standard Atmospheric Refraction
```cpp
// Calculate atmospheric refraction
double calculateRefraction(double true_elevation, double pressure_mbar, double temp_celsius) {
    if (true_elevation < -2.0) return 0.0;  // Below useful horizon

    // Basic refraction formula (accurate to ~1 arcminute)
    double h = true_elevation;
    double refraction = 1.02 / tan((h + 10.3/(h + 5.11)) * M_PI/180.0) / 60.0;

    // Atmospheric corrections
    double pressure_correction = pressure_mbar / 1013.25;
    double temp_correction = 283.0 / (273.0 + temp_celsius);

    return refraction * pressure_correction * temp_correction;
}

// Apply refraction correction
double applyRefraction(double true_elevation, double pressure, double temperature) {
    double refraction = calculateRefraction(true_elevation, pressure, temperature);
    return true_elevation + refraction;
}

// Remove refraction correction
double removeRefraction(double apparent_elevation, double pressure, double temperature) {
    // Iterative solution (refraction depends on true elevation)
    double true_elevation = apparent_elevation;
    for (int i = 0; i < 5; i++) {
        double refraction = calculateRefraction(true_elevation, pressure, temperature);
        true_elevation = apparent_elevation - refraction;
    }
    return true_elevation;
}
```

### Seasonal Atmospheric Parameters
```cpp
struct SeasonalAtmosphere {
    double pressure;     // mbar
    double temperature;  // Celsius
    double humidity;     // 0.0 to 1.0
};

// Get seasonal atmospheric parameters
SeasonalAtmosphere getSeasonalAtmosphere(int month, double latitude, double longitude) {
    SeasonalAtmosphere atm;

    // Bangkok tropical climate model
    if (abs(latitude) < 30.0) {  // Tropical zone
        // Pressure varies seasonally
        double pressure_base = 1012.0;
        double pressure_variation = 5.0 * cos((month - 1) * M_PI / 6.0);  // ±5 mbar
        atm.pressure = pressure_base + pressure_variation;

        // Temperature varies seasonally
        double temp_base = 28.0;  // Average temperature
        double temp_variation = 4.0 * cos((month - 4) * M_PI / 6.0);  // Peak in April
        atm.temperature = temp_base + temp_variation;

        // Humidity varies with monsoon
        if (month >= 5 && month <= 10) {  // Rainy season
            atm.humidity = 0.8;
        } else {  // Dry season
            atm.humidity = 0.6;
        }
    }
    else {  // Temperate/polar zones - more complex seasonal variation
        atm.pressure = 1013.25;    // Standard atmosphere
        atm.temperature = 15.0;    // Standard temperature
        atm.humidity = 0.5;        // Average humidity
    }

    return atm;
}
```

## Rise/Set/Transit Calculations

### Basic Rise/Set Calculation
```cpp
struct RiseSetTimes {
    double rise_jd;
    double set_jd;
    double transit_jd;
    bool rise_valid;
    bool set_valid;
    bool transit_valid;
};

// Calculate rise/set/transit times
RiseSetTimes calculateRiseSetTransit(int planet, double jd_start,
                                   double latitude, double longitude, double elevation,
                                   double pressure, double temperature) {
    RiseSetTimes times = {};
    double geopos[3] = {longitude, latitude, elevation};
    double tret[10];
    char serr[AS_MAXCH];
    int flags = SEFLG_SWIEPH;

    // Calculate rise
    int result = swe_rise_trans(jd_start, planet, nullptr, flags, SE_CALC_RISE,
                               geopos, pressure, temperature, 0.0, tret, serr);
    if (result != ERR) {
        times.rise_jd = tret[0];
        times.rise_valid = true;
    }

    // Calculate set
    result = swe_rise_trans(jd_start, planet, nullptr, flags, SE_CALC_SET,
                           geopos, pressure, temperature, 0.0, tret, serr);
    if (result != ERR) {
        times.set_jd = tret[0];
        times.set_valid = true;
    }

    // Calculate transit (culmination)
    result = swe_rise_trans(jd_start, planet, nullptr, flags, SE_CALC_MTRANSIT,
                           geopos, pressure, temperature, 0.0, tret, serr);
    if (result != ERR) {
        times.transit_jd = tret[0];
        times.transit_valid = true;
    }

    return times;
}
```

### Custom Horizon Calculations
```cpp
// Calculate rise/set for custom horizon
RiseSetTimes calculateRiseSetCustomHorizon(int planet, double jd_start,
                                         double latitude, double longitude, double elevation,
                                         double horizon_degrees, double pressure, double temperature) {
    RiseSetTimes times = {};
    double geopos[3] = {longitude, latitude, elevation};
    double tret[10];
    char serr[AS_MAXCH];
    int flags = SEFLG_SWIEPH;

    // Use custom horizon height
    int result = swe_rise_trans(jd_start, planet, nullptr, flags, SE_CALC_RISE,
                               geopos, pressure, temperature, horizon_degrees, tret, serr);
    if (result != ERR) {
        times.rise_jd = tret[0];
        times.rise_valid = true;
    }

    result = swe_rise_trans(jd_start, planet, nullptr, flags, SE_CALC_SET,
                           geopos, pressure, temperature, horizon_degrees, tret, serr);
    if (result != ERR) {
        times.set_jd = tret[0];
        times.set_valid = true;
    }

    return times;
}

// Calculate horizon depression for observer height
double calculateHorizonDepression(double height_meters) {
    const double earth_radius_km = 6371.0;
    double height_km = height_meters / 1000.0;

    // Geometric horizon depression in degrees
    return -sqrt(2.0 * height_km / earth_radius_km) * 180.0 / M_PI;
}
```

## Lunar Calculations

### Lunar Phases
```cpp
// Calculate lunar phase angle
double getLunarPhaseAngle(double jd) {
    double sun_long = getSunLongitude(jd);
    double moon_long = getMoonLongitude(jd);

    double phase_angle = moon_long - sun_long;
    if (phase_angle < 0) phase_angle += 360.0;

    return phase_angle;
}

// Get lunar phase name
std::string getLunarPhaseName(double phase_angle) {
    if (phase_angle < 1.0 || phase_angle > 359.0) return "New Moon";
    if (phase_angle >= 89.0 && phase_angle <= 91.0) return "First Quarter";
    if (phase_angle >= 179.0 && phase_angle <= 181.0) return "Full Moon";
    if (phase_angle >= 269.0 && phase_angle <= 271.0) return "Last Quarter";

    if (phase_angle < 90.0) return "Waxing Crescent";
    if (phase_angle < 180.0) return "Waxing Gibbous";
    if (phase_angle < 270.0) return "Waning Gibbous";
    return "Waning Crescent";
}

// Calculate next lunar phase
double getNextLunarPhase(double jd_start, double target_angle) {
    double current_angle = getLunarPhaseAngle(jd_start);
    double target_diff = target_angle - current_angle;

    if (target_diff <= 0) target_diff += 360.0;

    // Approximate lunar motion (13.2 degrees per day)
    double approx_days = target_diff / 13.2;

    // Refine with binary search
    return findExactLunarPhase(jd_start, jd_start + approx_days, target_angle);
}
```

### Lunar Distance and Size
```cpp
// Calculate lunar distance
double getLunarDistance(double jd) {
    double xx[6];
    char serr[AS_MAXCH];

    int result = swe_calc(jd, SE_MOON, SEFLG_SWIEPH, xx, serr);
    if (result == ERR) return 0.0;

    return xx[2];  // Distance in Earth radii
}

// Calculate apparent lunar diameter
double getLunarDiameter(double jd) {
    double distance = getLunarDistance(jd);
    if (distance == 0.0) return 0.0;

    // Mean lunar radius = 1737.4 km, Earth radius = 6371 km
    double angular_radius = atan(1737.4 / (distance * 6371.0)) * 180.0 / M_PI;

    return angular_radius * 2.0;  // Diameter in degrees
}

// Lunar apogee and perigee
struct LunarApsides {
    double apogee_jd;
    double perigee_jd;
    double apogee_distance;
    double perigee_distance;
};

LunarApsides getNextLunarApsides(double jd_start) {
    // Search for next apogee and perigee (approximately 27.5 day cycle)
    LunarApsides apsides = {};

    // Implementation would use numerical search to find extrema
    // of lunar distance function

    return apsides;
}
```

## Solar Calculations

### Solar Position
```cpp
// Get solar longitude
double getSunLongitude(double jd) {
    double xx[6];
    char serr[AS_MAXCH];

    int result = swe_calc(jd, SE_SUN, SEFLG_SWIEPH, xx, serr);
    if (result == ERR) return 0.0;

    return xx[0];  // Ecliptic longitude
}

// Get solar declination
double getSunDeclination(double jd) {
    double xx[6];
    char serr[AS_MAXCH];

    int result = swe_calc(jd, SE_SUN, SEFLG_SWIEPH | SEFLG_EQUATORIAL, xx, serr);
    if (result == ERR) return 0.0;

    return xx[1];  // Declination
}

// Calculate equation of time
double getEquationOfTime(double jd) {
    double sun_ra = getSunRightAscension(jd);
    double mean_sun_ra = getMeanSunRightAscension(jd);

    double eot = (mean_sun_ra - sun_ra) * 4.0;  // Convert to minutes

    // Normalize to ±20 minutes
    while (eot > 20.0) eot -= 1440.0;
    while (eot < -20.0) eot += 1440.0;

    return eot;
}
```

### Solar Seasons
```cpp
// Calculate solstices and equinoxes
struct SolarSeasons {
    double vernal_equinox;
    double summer_solstice;
    double autumnal_equinox;
    double winter_solstice;
};

SolarSeasons calculateSolarSeasons(int year) {
    SolarSeasons seasons = {};

    // Search for exact moments when solar longitude = 0°, 90°, 180°, 270°
    double march_approx = gregorianToJD(year, 3, 20, 12.0);
    seasons.vernal_equinox = findSolarLongitude(march_approx, 0.0);

    double june_approx = gregorianToJD(year, 6, 21, 12.0);
    seasons.summer_solstice = findSolarLongitude(june_approx, 90.0);

    double sept_approx = gregorianToJD(year, 9, 23, 12.0);
    seasons.autumnal_equinox = findSolarLongitude(sept_approx, 180.0);

    double dec_approx = gregorianToJD(year, 12, 22, 12.0);
    seasons.winter_solstice = findSolarLongitude(dec_approx, 270.0);

    return seasons;
}

// Solar transit calculations (Sankranti)
double calculateSankranti(double jd_start, int target_rashi) {
    double target_longitude = (target_rashi - 1) * 30.0;  // Each rashi = 30°

    return findSolarLongitude(jd_start, target_longitude);
}
```

## Planetary Calculations

### Planetary Positions
```cpp
// Get all planetary positions
struct PlanetaryPositions {
    double mercury, venus, mars, jupiter, saturn;
    double uranus, neptune, pluto;
    double north_node, south_node;
};

PlanetaryPositions getAllPlanetaryPositions(double jd) {
    PlanetaryPositions positions = {};
    double xx[6];
    char serr[AS_MAXCH];

    // Get positions with ayanamsa correction
    double ayanamsa = swe_get_ayanamsa(jd);

    // Mercury
    if (swe_calc(jd, SE_MERCURY, SEFLG_SWIEPH, xx, serr) != ERR) {
        positions.mercury = xx[0] - ayanamsa;
        if (positions.mercury < 0) positions.mercury += 360.0;
    }

    // Venus
    if (swe_calc(jd, SE_VENUS, SEFLG_SWIEPH, xx, serr) != ERR) {
        positions.venus = xx[0] - ayanamsa;
        if (positions.venus < 0) positions.venus += 360.0;
    }

    // Mars
    if (swe_calc(jd, SE_MARS, SEFLG_SWIEPH, xx, serr) != ERR) {
        positions.mars = xx[0] - ayanamsa;
        if (positions.mars < 0) positions.mars += 360.0;
    }

    // Jupiter
    if (swe_calc(jd, SE_JUPITER, SEFLG_SWIEPH, xx, serr) != ERR) {
        positions.jupiter = xx[0] - ayanamsa;
        if (positions.jupiter < 0) positions.jupiter += 360.0;
    }

    // Saturn
    if (swe_calc(jd, SE_SATURN, SEFLG_SWIEPH, xx, serr) != ERR) {
        positions.saturn = xx[0] - ayanamsa;
        if (positions.saturn < 0) positions.saturn += 360.0;
    }

    // Lunar nodes
    if (swe_calc(jd, SE_TRUE_NODE, SEFLG_SWIEPH, xx, serr) != ERR) {
        positions.north_node = xx[0] - ayanamsa;
        if (positions.north_node < 0) positions.north_node += 360.0;
        positions.south_node = positions.north_node + 180.0;
        if (positions.south_node >= 360.0) positions.south_node -= 360.0;
    }

    return positions;
}

// Planetary retrograde detection
bool isPlanetRetrograde(int planet, double jd) {
    double xx[6];
    char serr[AS_MAXCH];

    int result = swe_calc(jd, planet, SEFLG_SWIEPH | SEFLG_SPEED, xx, serr);
    if (result == ERR) return false;

    return xx[3] < 0.0;  // Negative longitude speed = retrograde
}

// Planetary aspects
struct PlanetaryAspect {
    int planet1, planet2;
    double aspect_angle;
    double orb;
    std::string aspect_name;
};

std::vector<PlanetaryAspect> findPlanetaryAspects(double jd, double max_orb = 8.0) {
    std::vector<PlanetaryAspect> aspects;
    PlanetaryPositions positions = getAllPlanetaryPositions(jd);

    // Standard aspects: 0° (conjunction), 60° (sextile), 90° (square),
    // 120° (trine), 180° (opposition)
    std::vector<double> aspect_angles = {0.0, 60.0, 90.0, 120.0, 180.0};
    std::vector<std::string> aspect_names = {"Conjunction", "Sextile", "Square", "Trine", "Opposition"};

    // Compare all planet pairs
    // Implementation would check angular differences and find close aspects

    return aspects;
}
```

## Precision and Accuracy Notes

### Swiss Ephemeris Accuracy
- **Position Accuracy**: 0.001 arcseconds for main planets
- **Time Range**: 13,000 BCE to 17,000 CE
- **Lunar Accuracy**: 0.1 arcseconds
- **Planetary Nodes**: 0.1 arcseconds
- **Asteroid Accuracy**: 1-10 arcseconds depending on object

### Calculation Precision Guidelines
```cpp
// Recommended precision for different applications
const double PRECISION_CASUAL = 0.1;        // 6 arcminutes (casual use)
const double PRECISION_STANDARD = 0.017;    // 1 arcminute (standard astrology)
const double PRECISION_HIGH = 0.003;        // 10 arcseconds (research)
const double PRECISION_SCIENTIFIC = 0.0003; // 1 arcsecond (scientific)

// Time precision recommendations
const double TIME_PRECISION_MINUTE = 1.0/1440.0;    // 1 minute
const double TIME_PRECISION_SECOND = 1.0/86400.0;   // 1 second
const double TIME_PRECISION_SUBSECOND = 1.0/864000.0; // 0.1 second
```

### Error Sources and Mitigation
1. **Atmospheric Refraction**: Use seasonal corrections and local weather data
2. **Observer Height**: Include elevation in topocentric calculations
3. **Time Zones**: Ensure proper UTC/local time conversions
4. **Calendar Systems**: Handle Julian/Gregorian transition properly
5. **Polar Regions**: Use extended search windows and fallback methods
6. **Numerical Precision**: Use double precision for all calculations

### Validation Methods
```cpp
// Validate calculation results
bool validateCalculationResult(double result, double expected, double tolerance) {
    return abs(result - expected) <= tolerance;
}

// Cross-check with multiple methods
bool crossValidateResult(double primary_result, double secondary_result, double tolerance) {
    return abs(primary_result - secondary_result) <= tolerance;
}

// Range validation
bool isResultInValidRange(double result, double min_value, double max_value) {
    return result >= min_value && result <= max_value;
}
```

This comprehensive reference covers all major calculation methods used in the enhanced horoscope CLI system, providing both theoretical background and practical implementation details for achieving maximum astronomical accuracy.
