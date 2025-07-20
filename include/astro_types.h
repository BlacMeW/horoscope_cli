#pragma once

#include <string>
#include <vector>

namespace Astro {

// Zodiac signs
enum class ZodiacSign {
    ARIES = 0, TAURUS, GEMINI, CANCER, LEO, VIRGO,
    LIBRA, SCORPIO, SAGITTARIUS, CAPRICORN, AQUARIUS, PISCES
};

// Planets
enum class Planet {
    SUN = 0, MOON, MERCURY, VENUS, MARS, JUPITER, SATURN,
    URANUS, NEPTUNE, PLUTO, NORTH_NODE, SOUTH_NODE,
    CHIRON, LILITH
};

// House systems
enum class HouseSystem {
    PLACIDUS = 'P',
    KOCH = 'K',
    EQUAL = 'E',
    WHOLE_SIGN = 'W',
    CAMPANUS = 'C',
    REGIOMONTANUS = 'R'
};

// Zodiac calculation mode
enum class ZodiacMode {
    TROPICAL = 0,    // Tropical zodiac (season-based, default)
    SIDEREAL = 1     // Sidereal zodiac (star-based)
};

// Ayanamsa (Precession correction) types for sidereal calculations
enum class AyanamsaType {
    FAGAN_BRADLEY = 0,   // Fagan-Bradley
    LAHIRI = 1,          // Lahiri/Chitrapaksha (most common in India)
    DELUCE = 2,          // De Luce
    RAMAN = 3,           // B.V. Raman
    USHASHASHI = 4,      // Usha-Shashi
    KRISHNAMURTI = 5,    // K.S. Krishnamurti (KP System)
    DJWHAL_KHUL = 6,     // Djwhal Khul
    YUKTESHWAR = 7,      // Sri Yukteshwar
    JN_BHASIN = 8,       // J.N. Bhasin
    BABYLONIAN_KUGLER1 = 9,   // Babylonian (Kugler 1)
    BABYLONIAN_KUGLER2 = 10,  // Babylonian (Kugler 2)
    BABYLONIAN_KUGLER3 = 11,  // Babylonian (Kugler 3)
    BABYLONIAN_HUBER = 12,    // Babylonian (Huber)
    BABYLONIAN_ETPSC = 13,    // Babylonian (Et Psc)
    ALDEBARAN_15TAU = 14,     // Aldebaran at 15° Taurus
    HIPPARCHOS = 15,          // Hipparchos
    SASSANIAN = 16,           // Sassanian
    GALACTIC_CENTER = 17,     // Galactic Center at 0° Sagittarius
    J2000 = 18,              // J2000
    J1900 = 19,              // J1900
    B1950 = 20               // B1950
};

// Calculation flags for Swiss Ephemeris
enum class CalculationFlag {
    // Coordinate system flags
    GEOCENTRIC = 1,          // Geocentric (default behavior when no flag set)
    HELIOCENTRIC = 2,        // Heliocentric
    BARYCENTRIC = 3,         // Barycentric
    TOPOCENTRIC = 4,         // Topocentric (requires observer coordinates)

    // Position type flags
    APPARENT = 10,           // Apparent position (default, includes aberration/deflection)
    TRUE_GEOMETRIC = 11,     // True/geometric position
    ASTROMETRIC = 12,        // Astrometric position (no aberration/deflection)

    // Precession/Nutation flags
    STANDARD_EQUINOX = 20,   // Standard equinox of date (default)
    J2000_EQUINOX = 21,      // J2000.0 equinox (no precession)
    MEAN_EQUINOX = 22,       // Mean equinox of date (no nutation)

    // Speed calculation flags
    HIGH_PRECISION_SPEED = 30,  // High precision speed calculation

    // Coordinate format flags
    ECLIPTIC = 40,           // Ecliptic coordinates (default)
    EQUATORIAL = 41          // Equatorial coordinates
};

// Aspect types
enum class AspectType {
    CONJUNCTION = 0,    // 0°
    SEXTILE = 60,      // 60°
    SQUARE = 90,       // 90°
    TRINE = 120,       // 120°
    OPPOSITION = 180   // 180°
};

// Planet position structure
struct PlanetPosition {
    Planet planet;
    double longitude;        // Ecliptic longitude in degrees
    double latitude;         // Ecliptic latitude in degrees
    double distance;         // Distance from Earth in AU
    double speed;           // Daily motion in degrees
    double declination;      // Declination in degrees (celestial coordinate)
    double rightAscension;   // Right ascension in degrees (celestial coordinate)
    double inclination;      // Orbital inclination in degrees
    ZodiacSign sign;
    int house;              // House number (1-12)
    double housePosition;   // Position within house (0-30 degrees)

    // Convert longitude to sign and degree within sign
    void calculateSignPosition();

    // Get formatted position string (e.g., "25°42' Capricorn")
    std::string getFormattedPosition() const;

    // Get formatted astronomical coordinates
    std::string getAstronomicalCoordinates() const;
};

// Aspect between two planets
struct Aspect {
    Planet planet1;
    Planet planet2;
    AspectType type;
    double orb;             // Difference from exact aspect in degrees
    bool isApplying;        // True if aspect is applying, false if separating

    std::string getDescription() const;
};

// Birth data
struct BirthData {
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    double latitude;
    double longitude;
    double timezone;        // Offset from UTC in hours

    // Convert to Julian Day
    double getJulianDay() const;

    // Get formatted location string
    std::string getLocationString() const;

    // Get formatted date/time string
    std::string getDateTimeString() const;
};

// House cusp data
struct HouseCusps {
    double cusps[13];       // Houses 1-12 (index 0 unused)
    double ascendant;       // Same as cusp[1]
    double midheaven;       // Same as cusp[10]
    double vertex;
    double eastPoint;

    // Get house number for a given longitude
    int getHouseForLongitude(double longitude) const;
};

// Helper functions
std::string zodiacSignToString(ZodiacSign sign);
std::string planetToString(Planet planet);
std::string planetToShortString(Planet planet);
std::string aspectTypeToString(AspectType aspect);
std::string zodiacModeToString(ZodiacMode mode);
std::string ayanamsaTypeToString(AyanamsaType ayanamsa);
std::string calculationFlagToString(CalculationFlag flag);
ZodiacSign longitudeToSign(double longitude);
double normalizeAngle(double angle);
double calculateAspectOrb(double angle1, double angle2, AspectType aspect);
Planet getSignLord(ZodiacSign sign);
std::string getPlanetName(Planet planet);
AyanamsaType stringToAyanamsaType(const std::string& ayanamsaStr);
ZodiacMode stringToZodiacMode(const std::string& modeStr);
CalculationFlag stringToCalculationFlag(const std::string& flagStr);
int ayanamsaTypeToSwissEphId(AyanamsaType ayanamsa);

// Date parsing with BC era support
bool parseBCDate(const std::string& dateStr, int& year, int& month, int& day);

// Date formatting with BC/AD support
std::string formatBCDate(int year, int month, int day);
std::string formatBCDateLong(int year, int month, int day);

} // namespace Astro
