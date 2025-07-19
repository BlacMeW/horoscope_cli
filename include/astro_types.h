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
    ZodiacSign sign;
    int house;              // House number (1-12)
    double housePosition;   // Position within house (0-30 degrees)

    // Convert longitude to sign and degree within sign
    void calculateSignPosition();

    // Get formatted position string (e.g., "25°42' Capricorn")
    std::string getFormattedPosition() const;
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
ZodiacSign longitudeToSign(double longitude);
double normalizeAngle(double angle);
double calculateAspectOrb(double angle1, double angle2, AspectType aspect);
Planet getSignLord(ZodiacSign sign);

// Date parsing with BC era support
bool parseBCDate(const std::string& dateStr, int& year, int& month, int& day);

// Date formatting with BC/AD support
std::string formatBCDate(int year, int month, int day);
std::string formatBCDateLong(int year, int month, int day);

} // namespace Astro
