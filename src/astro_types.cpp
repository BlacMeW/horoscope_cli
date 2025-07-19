#include "astro_types.h"
#include "swephexp.h"
#include <sstream>
#include <iomanip>
#include <cmath>

namespace Astro {

void PlanetPosition::calculateSignPosition() {
    sign = longitudeToSign(longitude);
    // Calculate position within sign (0-30 degrees)
    housePosition = fmod(longitude, 30.0);
    if (housePosition < 0) housePosition += 30.0;
}

std::string PlanetPosition::getFormattedPosition() const {
    int degrees = static_cast<int>(longitude) % 30;
    int minutes = static_cast<int>((longitude - static_cast<int>(longitude)) * 60) % 60;

    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << degrees << "°"
        << std::setw(2) << minutes << "' " << zodiacSignToString(sign);

    if (house > 0) {
        oss << " (House " << house << ")";
    }

    return oss.str();
}

std::string Aspect::getDescription() const {
    std::ostringstream oss;
    oss << planetToString(planet1) << " " << aspectTypeToString(type)
        << " " << planetToString(planet2) << " (orb: ";

    int degrees = static_cast<int>(orb);
    int minutes = static_cast<int>((orb - degrees) * 60);

    oss << degrees << "°" << std::setfill('0') << std::setw(2) << minutes << "')";

    if (isApplying) {
        oss << " [Applying]";
    } else {
        oss << " [Separating]";
    }

    return oss.str();
}

double BirthData::getJulianDay() const {
    double jd;
    int gregflag = SE_GREG_CAL; // Gregorian calendar

    // Convert local time to UTC
    double utcHour = hour - timezone;
    double utcMinute = minute;
    double utcSecond = second;

    // Handle day overflow/underflow when adjusting for timezone
    int utcDay = day;
    int utcMonth = month;
    int utcYear = year;

    if (utcHour < 0) {
        utcHour += 24;
        utcDay--;
        if (utcDay < 1) {
            utcMonth--;
            if (utcMonth < 1) {
                utcMonth = 12;
                utcYear--;
            }
            // Get days in previous month (simplified)
            if (utcMonth == 2) {
                utcDay = (utcYear % 4 == 0 && (utcYear % 100 != 0 || utcYear % 400 == 0)) ? 29 : 28;
            } else if (utcMonth == 4 || utcMonth == 6 || utcMonth == 9 || utcMonth == 11) {
                utcDay = 30;
            } else {
                utcDay = 31;
            }
        }
    } else if (utcHour >= 24) {
        utcHour -= 24;
        utcDay++;
        // Handle month overflow (simplified)
        if ((utcMonth == 2 && utcDay > 28 + (utcYear % 4 == 0 ? 1 : 0)) ||
            ((utcMonth == 4 || utcMonth == 6 || utcMonth == 9 || utcMonth == 11) && utcDay > 30) ||
            (utcDay > 31)) {
            utcDay = 1;
            utcMonth++;
            if (utcMonth > 12) {
                utcMonth = 1;
                utcYear++;
            }
        }
    }

    double utcTime = utcHour + utcMinute/60.0 + utcSecond/3600.0;
    jd = swe_julday(utcYear, utcMonth, utcDay, utcTime, gregflag);

    return jd;
}

std::string BirthData::getLocationString() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(4);
    oss << std::abs(latitude) << "°" << (latitude >= 0 ? "N" : "S") << ", ";
    oss << std::abs(longitude) << "°" << (longitude >= 0 ? "E" : "W");
    return oss.str();
}

std::string BirthData::getDateTimeString() const {
    std::ostringstream oss;
    oss << std::setfill('0');

    // Handle BC era display (convert from astronomical year numbering)
    if (year <= 0) {
        int bcYear = 1 - year;  // Convert: year 0 = 1 BC, year -1 = 2 BC, etc.
        oss << bcYear << " BC-" << std::setw(2) << month << "-" << std::setw(2) << day << " ";
    } else {
        oss << year << "-" << std::setw(2) << month << "-" << std::setw(2) << day << " ";
    }

    oss << std::setw(2) << hour << ":" << std::setw(2) << minute << ":" << std::setw(2) << second;
    oss << " UTC" << (timezone >= 0 ? "+" : "") << timezone;
    return oss.str();
}

int HouseCusps::getHouseForLongitude(double longitude) const {
    // Normalize longitude to 0-360 range
    longitude = normalizeAngle(longitude);

    for (int house = 1; house <= 12; house++) {
        double currentCusp = normalizeAngle(cusps[house]);
        double nextCusp = normalizeAngle(cusps[house == 12 ? 1 : house + 1]);

        if (nextCusp > currentCusp) {
            // Normal case: cusp doesn't cross 0°
            if (longitude >= currentCusp && longitude < nextCusp) {
                return house;
            }
        } else {
            // Cusp crosses 0° Aries
            if (longitude >= currentCusp || longitude < nextCusp) {
                return house;
            }
        }
    }

    return 1; // Default to first house
}

std::string zodiacSignToString(ZodiacSign sign) {
    static const std::string signs[] = {
        "Aries", "Taurus", "Gemini", "Cancer", "Leo", "Virgo",
        "Libra", "Scorpio", "Sagittarius", "Capricorn", "Aquarius", "Pisces"
    };
    return signs[static_cast<int>(sign)];
}

std::string planetToString(Planet planet) {
    static const std::string planets[] = {
        "Sun", "Moon", "Mercury", "Venus", "Mars", "Jupiter", "Saturn",
        "Uranus", "Neptune", "Pluto", "North Node", "South Node",
        "Chiron", "Lilith"
    };
    return planets[static_cast<int>(planet)];
}

std::string aspectTypeToString(AspectType aspect) {
    switch (aspect) {
        case AspectType::CONJUNCTION: return "conjunct";
        case AspectType::SEXTILE: return "sextile";
        case AspectType::SQUARE: return "square";
        case AspectType::TRINE: return "trine";
        case AspectType::OPPOSITION: return "opposite";
        default: return "unknown";
    }
}

ZodiacSign longitudeToSign(double longitude) {
    // Normalize to 0-360 range
    longitude = normalizeAngle(longitude);
    return static_cast<ZodiacSign>(static_cast<int>(longitude / 30.0));
}

double normalizeAngle(double angle) {
    while (angle < 0) angle += 360.0;
    while (angle >= 360.0) angle -= 360.0;
    return angle;
}

double calculateAspectOrb(double angle1, double angle2, AspectType aspect) {
    double diff = std::abs(angle1 - angle2);
    if (diff > 180.0) diff = 360.0 - diff;

    double aspectAngle = static_cast<double>(aspect);
    return std::abs(diff - aspectAngle);
}

bool parseBCDate(const std::string& dateStr, int& year, int& month, int& day) {
    // Support BC era dates: "-0500-03-15" for 500 BC
    // Standard format: "1990-01-15" for 1990 AD
    
    std::string processStr = dateStr;
    bool isBCEra = false;
    
    // Check for BC era (negative year)
    if (processStr.length() >= 11 && processStr[0] == '-') {
        isBCEra = true;
        processStr = processStr.substr(1); // Remove leading minus
    }
    
    if (processStr.length() != 10 || processStr[4] != '-' || processStr[7] != '-') {
        return false;
    }

    try {
        year = std::stoi(processStr.substr(0, 4));
        month = std::stoi(processStr.substr(5, 2));
        day = std::stoi(processStr.substr(8, 2));
        
        // Convert to astronomical year numbering for BC dates
        // In astronomical year numbering: 1 BC = year 0, 2 BC = year -1, etc.
        if (isBCEra) {
            year = -(year - 1);
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

Planet getSignLord(ZodiacSign sign) {
    switch (sign) {
        case ZodiacSign::ARIES: return Planet::MARS;
        case ZodiacSign::TAURUS: return Planet::VENUS;
        case ZodiacSign::GEMINI: return Planet::MERCURY;
        case ZodiacSign::CANCER: return Planet::MOON;
        case ZodiacSign::LEO: return Planet::SUN;
        case ZodiacSign::VIRGO: return Planet::MERCURY;
        case ZodiacSign::LIBRA: return Planet::VENUS;
        case ZodiacSign::SCORPIO: return Planet::MARS;
        case ZodiacSign::SAGITTARIUS: return Planet::JUPITER;
        case ZodiacSign::CAPRICORN: return Planet::SATURN;
        case ZodiacSign::AQUARIUS: return Planet::SATURN;
        case ZodiacSign::PISCES: return Planet::JUPITER;
        default: return Planet::SUN;
    }
}

std::string planetToShortString(Planet planet) {
    switch (planet) {
        case Planet::SUN: return "Su";
        case Planet::MOON: return "Mo";
        case Planet::MERCURY: return "Me";
        case Planet::VENUS: return "Ve";
        case Planet::MARS: return "Ma";
        case Planet::JUPITER: return "Ju";
        case Planet::SATURN: return "Sa";
        case Planet::URANUS: return "Ur";
        case Planet::NEPTUNE: return "Ne";
        case Planet::PLUTO: return "Pl";
        case Planet::NORTH_NODE: return "NN";
        case Planet::SOUTH_NODE: return "SN";
        case Planet::CHIRON: return "Ch";
        case Planet::LILITH: return "Li";
        default: return "??";
    }
}

} // namespace Astro