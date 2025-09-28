# Swiss Ephemeris Complete Reference Documentation

## Table of Contents
1. [Overview](#overview)
2. [Ephemeris Types](#ephemeris-types)
3. [Core Functions](#core-functions)
4. [Calculation Methods](#calculation-methods)
5. [Rise/Transit/Set Calculations](#risetransitset-calculations)
6. [Atmospheric Refraction](#atmospheric-refraction)
7. [Coordinate Systems](#coordinate-systems)
8. [Error Handling](#error-handling)
9. [Best Practices](#best-practices)
10. [Implementation Examples](#implementation-examples)

## Overview

The Swiss Ephemeris is a highly accurate astronomical calculation library based on JPL Ephemeris DE431, providing planetary positions with precision better than 1 milli-arcsecond. Current version supports calculations from 13,000 BCE to 17,000 CE.

### Key Features
- **Precision**: Agrees with JPL Ephemeris to 1 milli-arcsecond (0.001")
- **Time Range**: 13,000 BCE to 17,000 CE (based on DE431)
- **Storage**: 99 MB compressed vs 2.8 GB original JPL data
- **Performance**: High-speed calculations with sophisticated compression
- **Standards**: Follows IAU resolutions and Astronomical Almanac standards

## Ephemeris Types

### 1. Swiss Ephemeris (Recommended)
- **Basis**: Compressed JPL Ephemeris DE431
- **Precision**: < 0.001" deviation from JPL
- **Time Range**: 13,000 BCE to 17,000 CE
- **Storage**: 99 MB compressed files
- **Use Case**: Primary choice for all applications

### 2. Moshier Ephemeris
- **Basis**: Semi-analytical approximation of JPL DE404
- **Precision**: < 1" for planets, few arcsec for Moon
- **Time Range**: 3000 BCE to 3000 CE
- **Storage**: No external files (built-in)
- **Use Case**: When storage is extremely limited

### 3. Full JPL Ephemeris
- **Basis**: Original JPL DE431 files
- **Precision**: Full JPL precision
- **Time Range**: 13,002 BCE to 17,000 CE
- **Storage**: 2.8 GB uncompressed
- **Use Case**: When absolute maximum precision required

## Core Functions

### Primary Calculation Functions

```c
// Planetary positions with Ephemeris Time (ET/TT)
int32 swe_calc(double tjd_et, int ipl, int32 iflag, double *xx, char *serr);

// Planetary positions with Universal Time (UT)
int32 swe_calc_ut(double tjd_ut, int ipl, int32 iflag, double *xx, char *serr);

// Fixed stars with ET
int32 swe_fixstar(char *star, double tjd_et, int32 iflag, double *xx, char *serr);

// Fixed stars with UT
int32 swe_fixstar_ut(char *star, double tjd_ut, int32 iflag, double *xx, char *serr);
```

### Time Conversion Functions

```c
// Delta T calculation: ET = UT + Delta_T
double swe_deltat(double tjd_ut);

// Julian Day from calendar date
double swe_julday(int year, int month, int day, double hour, int gregflag);

// Calendar date from Julian Day
void swe_revjul(double tjd, int gregflag, int *year, int *month, int *day, double *hour);

// UTC to Julian Day
int32 swe_utc_to_jd(int32 iyear, int32 imonth, int32 iday, int32 ihour, int32 imin, double dsec, int32 gregflag, double *dret, char *serr);

// Julian Day to UTC
int32 swe_jdet_to_utc(double tjd_et, int32 gregflag, int32 *iyear, int32 *imonth, int32 *iday, int32 *ihour, int32 *imin, double *dsec);
```

## Calculation Methods

### Calculation Flags (iflag parameter)

```c
// Ephemeris selection
#define SEFLG_JPLEPH        1       // Use JPL ephemeris
#define SEFLG_SWIEPH        2       // Use Swiss Ephemeris (default)
#define SEFLG_MOSEPH        4       // Use Moshier ephemeris

// Coordinate system
#define SEFLG_HELIOCTR      8       // Heliocentric coordinates
#define SEFLG_TOPOCTR       32      // Topocentric coordinates
#define SEFLG_BARYCTR       64      // Barycentric coordinates

// Reference frame
#define SEFLG_J2000         256     // J2000 coordinates
#define SEFLG_NONUT         512     // No nutation
#define SEFLG_SPEED3        1024    // High precision speed
#define SEFLG_SPEED         2       // Include speed vectors

// Light effects
#define SEFLG_TRUEPOS       1024    // True positions (no light-time)
#define SEFLG_NOABERR       2048    // No aberration
#define SEFLG_NOGDEFL       4096    // No gravitational deflection

// Astrometric vs apparent
#define SEFLG_ASTROMETRIC   (SEFLG_NOABERR | SEFLG_NOGDEFL)
```

### Planet IDs

```c
#define SE_SUN              0
#define SE_MOON             1
#define SE_MERCURY          2
#define SE_VENUS            3
#define SE_MARS             4
#define SE_JUPITER          5
#define SE_SATURN           6
#define SE_URANUS           7
#define SE_NEPTUNE          8
#define SE_PLUTO            9
#define SE_MEAN_NODE        10
#define SE_TRUE_NODE        11
#define SE_MEAN_APOG        12
#define SE_OSCU_APOG        13
#define SE_INTP_APOG        14
#define SE_INTP_PERG        15
```

## Rise/Transit/Set Calculations

### swe_rise_trans() Function

```c
int32 swe_rise_trans(double tjd_start, int32 ipl, char *starname,
                     int32 epheflag, int32 rsmi, double *geopos,
                     double atpress, double attemp,
                     double *tret, char *serr);
```

#### Parameters:
- **tjd_start**: Julian Day for start of search
- **ipl**: Planet ID (SE_SUN, SE_MOON, etc.)
- **starname**: Fixed star name (or NULL for planets)
- **epheflag**: Ephemeris flags (SEFLG_SWIEPH | SEFLG_TOPOCTR recommended)
- **rsmi**: Rise/Set/Transit flags
- **geopos**: [longitude, latitude, altitude] array
- **atpress**: Atmospheric pressure in mbar
- **attemp**: Atmospheric temperature in °C
- **tret**: Returned Julian Day of event
- **serr**: Error string buffer

#### Rise/Set/Transit Flags (rsmi):

```c
#define SE_CALC_RISE        1       // Calculate rise time
#define SE_CALC_SET         2       // Calculate set time
#define SE_CALC_MTRANSIT    4       // Upper meridian transit
#define SE_CALC_ITRANSIT    8       // Lower meridian transit

// Disc definition flags
#define SE_BIT_DISC_CENTER  256     // Center of disc
#define SE_BIT_DISC_BOTTOM  512     // Bottom of disc (horizon)
#define SE_BIT_NO_REFRACTION 1024   // No atmospheric refraction
#define SE_BIT_CIVIL_TWILIGHT 2048  // Civil twilight (6° below)
#define SE_BIT_NAUTIC_TWILIGHT 4096 // Nautical twilight (12° below)
#define SE_BIT_ASTRO_TWILIGHT 8192  // Astronomical twilight (18° below)
#define SE_BIT_FIXED_DISC_SIZE 16384 // Use fixed disc size
```

## Atmospheric Refraction

### Standard Conditions
- **Pressure**: 1013.25 mbar (sea level)
- **Temperature**: 10°C
- **Humidity**: Not directly specified in Swiss Ephemeris

### Tropical Climate Adjustments
For locations like Bangkok (tropical climate):
- **Pressure**: ~1012 mbar (slightly lower due to heat)
- **Temperature**: ~28°C (higher average temperature)
- **Effect**: Reduces refraction slightly compared to standard

### Refraction Formula
The Swiss Ephemeris uses Bennett's formula for atmospheric refraction:
```
R = (1/tan(h + 7.31/(h + 4.4))) / 3600
```
Where:
- R = refraction in degrees
- h = apparent altitude in degrees

### Seasonal Variations
Atmospheric conditions vary significantly by season:
- **Winter**: Higher pressure, lower temperature → stronger refraction
- **Summer**: Lower pressure, higher temperature → weaker refraction
- **Effect**: Can cause 30-60 minute variations in sunrise/sunset times

## Coordinate Systems

### Ecliptic Coordinates
- **Default**: Ecliptic longitude and latitude
- **Reference**: Mean ecliptic and equinox of date
- **Units**: Degrees

### Equatorial Coordinates
- **Format**: Right ascension and declination
- **Reference**: Mean equator and equinox of date (or J2000)
- **Units**: Hours/degrees

### Coordinate Transformation

```c
// Transform between coordinate systems
void swe_cotrans(double *xpo, double *xpn, double eps);
void swe_cotrans_sp(double *xpo, double *xpn, double eps);
```

## Error Handling

### Return Values
- **>= 0**: Success (flags indicating calculation method used)
- **< 0**: Error occurred

### Common Error Codes
- **-1**: Date out of range
- **-2**: File not found
- **-3**: Invalid planet number
- **-4**: No rise/set found
- **-5**: Invalid geographical position

### Error String Analysis
Always check the `serr` parameter for detailed error information.

## Best Practices

### 1. Initialization
```c
// Set ephemeris path (if needed)
swe_set_ephe_path("/path/to/ephemeris/files");

// Set calculation parameters
swe_set_sid_mode(SE_SIDM_LAHIRI, 0, 0); // For sidereal calculations
```

### 2. Memory Management
```c
// Close Swiss Ephemeris when done
swe_close();
```

### 3. Precision Considerations
- Use `SEFLG_SWIEPH | SEFLG_TOPOCTR` for highest precision
- Include `SEFLG_SPEED` for velocity calculations
- Use appropriate atmospheric parameters for location

### 4. Time Handling
- Always specify whether input time is UT or ET/TT
- Use `swe_deltat()` for UT to ET conversion
- Consider timezone conversions separately

## Implementation Examples

### 1. Basic Planetary Position

```c
double tjd_et = 2451545.0; // J2000.0
double xx[6];
char serr[256];
int32 iflag = SEFLG_SWIEPH | SEFLG_SPEED;

int32 result = swe_calc(tjd_et, SE_SUN, iflag, xx, serr);
if (result >= 0) {
    printf("Sun longitude: %.6f°\n", xx[0]);
    printf("Sun latitude: %.6f°\n", xx[1]);
    printf("Sun distance: %.9f AU\n", xx[2]);
    printf("Sun speed lon: %.6f°/day\n", xx[3]);
}
```

### 2. Sunrise/Sunset Calculation

```c
double tjd_start = floor(tjd_ut) + 0.5; // Start of day
double geopos[3] = {longitude, latitude, altitude};
double tret;
char serr[256];

// Sunrise calculation
int32 result = swe_rise_trans(tjd_start, SE_SUN, NULL,
                             SEFLG_SWIEPH | SEFLG_TOPOCTR,
                             SE_CALC_RISE | SE_BIT_DISC_BOTTOM,
                             geopos, 1013.25, 10.0,
                             &tret, serr);

if (result >= 0) {
    // Convert to local time
    double sunrise_ut = tret;
    double sunrise_local = sunrise_ut + timezone_offset/24.0;
}
```

### 3. Topocentric Moon Position

```c
double geopos[3] = {longitude, latitude, altitude};
double xx[6];
char serr[256];

// Set topocentric position
swe_set_topo(longitude, latitude, altitude);

int32 result = swe_calc(tjd_et, SE_MOON,
                       SEFLG_SWIEPH | SEFLG_TOPOCTR | SEFLG_SPEED,
                       xx, serr);

if (result >= 0) {
    printf("Moon topocentric longitude: %.6f°\n", xx[0]);
    printf("Moon topocentric latitude: %.6f°\n", xx[1]);
}
```

### 4. High Precision Calculation with Error Handling

```c
double calculatePlanetPosition(double tjd_ut, int planet, double *longitude) {
    double tjd_et = tjd_ut + swe_deltat(tjd_ut)/86400.0;
    double xx[6];
    char serr[256];

    int32 iflag = SEFLG_SWIEPH | SEFLG_SPEED | SEFLG_TOPOCTR;
    int32 result = swe_calc(tjd_et, planet, iflag, xx, serr);

    if (result < 0) {
        fprintf(stderr, "Swiss Ephemeris error: %s\n", serr);
        return -1;
    }

    *longitude = xx[0];
    return 0; // Success
}
```

### 5. Seasonal Atmospheric Correction Example

```c
double getSeasonalAtmosphericParams(double tjd_ut, double latitude,
                                   double *pressure, double *temperature) {
    // Get month from Julian Day
    int year, month, day;
    double hour;
    swe_revjul(tjd_ut, SE_GREG_CAL, &year, &month, &day, &hour);

    // Seasonal adjustments for tropical climate (Bangkok example)
    if (latitude > 10.0 && latitude < 20.0) { // Tropical zone
        if (month >= 3 && month <= 5) {
            // Hot season
            *pressure = 1010.0;
            *temperature = 32.0;
        } else if (month >= 6 && month <= 10) {
            // Rainy season
            *pressure = 1008.0;
            *temperature = 28.0;
        } else {
            // Cool season
            *pressure = 1015.0;
            *temperature = 24.0;
        }
    } else {
        // Standard conditions
        *pressure = 1013.25;
        *temperature = 10.0;
    }

    return 0;
}
```

## Validation and Testing

### Comparison with JPL Horizons
The Swiss Ephemeris should match JPL Horizons to within:
- **Planetary positions**: < 1 milli-arcsecond
- **Rise/set times**: < 1 second
- **Lunar positions**: < 1 arcsecond

### Comparison with Astronomical Almanac
Should reproduce Astronomical Almanac values exactly for:
- Sample calculations in AA2006, AA2011, AA2013
- Standard epoch positions (J2000.0, etc.)

### Testing Procedures
1. **Precision tests**: Compare with JPL Horizons using astrometric positions
2. **Apparent position tests**: Verify aberration and light deflection
3. **Rise/set tests**: Validate atmospheric refraction calculations
4. **Long-term tests**: Check precession models for ancient dates

## References

1. **Swiss Ephemeris Documentation**: https://www.astro.com/swisseph/swisseph.htm
2. **JPL Horizons System**: https://ssd.jpl.nasa.gov/horizons.cgi
3. **Astronomical Almanac**: Published annually by USNO/HMNAO
4. **IAU Resolutions**: https://www.iau.org/science/scientific_bodies/commissions/a1/
5. **Meeus, J.**: "Astronomical Algorithms", 2nd Edition
6. **Explanatory Supplement to the Astronomical Almanac**: 3rd Edition

---

**Note**: This reference is based on Swiss Ephemeris version 2.10+ and should be updated as new versions are released. Always consult the official documentation for the most current information.
