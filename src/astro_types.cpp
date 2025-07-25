#include "astro_types.h"
#include "swephexp.h"
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <cctype>

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

std::string PlanetPosition::getAstronomicalCoordinates() const {
    std::ostringstream oss;

    // Format latitude
    int latDeg = static_cast<int>(std::abs(latitude));
    int latMin = static_cast<int>((std::abs(latitude) - latDeg) * 60);
    oss << "Lat: " << (latitude >= 0 ? "+" : "-")
        << std::setfill('0') << std::setw(2) << latDeg << "°"
        << std::setw(2) << latMin << "'";

    // Format declination
    int decDeg = static_cast<int>(std::abs(declination));
    int decMin = static_cast<int>((std::abs(declination) - decDeg) * 60);
    oss << " | Dec: " << (declination >= 0 ? "+" : "-")
        << std::setfill('0') << std::setw(2) << decDeg << "°"
        << std::setw(2) << decMin << "'";

    // Format right ascension (in hours)
    double raHours = rightAscension / 15.0; // Convert degrees to hours
    int raH = static_cast<int>(raHours);
    int raM = static_cast<int>((raHours - raH) * 60);
    oss << " | RA: " << std::setfill('0') << std::setw(2) << raH << "h"
        << std::setw(2) << raM << "m";

    // Format inclination
    int incDeg = static_cast<int>(std::abs(inclination));
    int incMin = static_cast<int>((std::abs(inclination) - incDeg) * 60);
    oss << " | Inc: " << std::setfill('0') << std::setw(2) << incDeg << "°"
        << std::setw(2) << incMin << "'";

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

    // Handle BC/AD era display (convert from astronomical year numbering)
    if (year <= 0) {
        int bcYear = 1 - year;  // Convert: year 0 = 1 BC, year -1 = 2 BC, etc.
        oss << std::setw(2) << month << "/" << std::setw(2) << day << "/" << bcYear << " BC ";
    } else {
        oss << std::setw(2) << month << "/" << std::setw(2) << day << "/" << year << " AD ";
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
    // Support multiple date formats:
    // 1. Standard format: "1990-01-15" for 1990 AD
    // 2. BC era with minus: "-0044-03-15" for 44 BC
    // 3. BC era with suffix: "44BC-03-15" for 44 BC
    // 4. AD era with suffix: "1990AD-01-15" for 1990 AD

    std::string processStr = dateStr;
    bool isBCEra = false;

    // Check for BC/AD suffix format first
    if (processStr.find("BC-") != std::string::npos) {
        isBCEra = true;
        size_t bcPos = processStr.find("BC-");
        if (bcPos != std::string::npos) {
            // Extract year before BC and date after BC-
            std::string yearPart = processStr.substr(0, bcPos);
            std::string datePart = processStr.substr(bcPos + 3); // Skip "BC-"

            // Remove any space before BC
            if (!yearPart.empty() && yearPart.back() == ' ') {
                yearPart.pop_back();
            }

            // Reconstruct as standard format: YYYY-MM-DD
            // Pad year to 4 digits
            while (yearPart.length() < 4) {
                yearPart = "0" + yearPart;
            }

            processStr = yearPart + "-" + datePart;
        }
    } else if (processStr.find("AD-") != std::string::npos) {
        // Handle AD suffix format
        size_t adPos = processStr.find("AD-");
        if (adPos != std::string::npos) {
            std::string yearPart = processStr.substr(0, adPos);
            std::string datePart = processStr.substr(adPos + 3); // Skip "AD-"

            // Remove any space before AD
            if (!yearPart.empty() && yearPart.back() == ' ') {
                yearPart.pop_back();
            }

            // Pad year to 4 digits if needed
            while (yearPart.length() < 4) {
                yearPart = "0" + yearPart;
            }

            processStr = yearPart + "-" + datePart;
        }
    }

    // Check for BC era with minus sign (original format)
    // Only treat as BC if it starts with minus and has proper length
    if (!isBCEra && processStr.length() >= 11 && processStr[0] == '-') {
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

        // Convert to astronomical year numbering for BC dates ONLY
        // In astronomical year numbering: 1 BC = year 0, 2 BC = year -1, etc.
        // For AD dates, leave the year as-is
        if (isBCEra) {
            year = -(year - 1);
        }
        // For standard dates like "2025-07-01", year remains 2025 (no conversion)

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

std::string planetToSymbol(Planet planet) {
    switch (planet) {
        case Planet::SUN: return "☉";
        case Planet::MOON: return "☽";
        case Planet::MERCURY: return "☿";
        case Planet::VENUS: return "♀";
        case Planet::MARS: return "♂";
        case Planet::JUPITER: return "♃";
        case Planet::SATURN: return "♄";
        case Planet::URANUS: return "♅";
        case Planet::NEPTUNE: return "♆";
        case Planet::PLUTO: return "♇";
        case Planet::NORTH_NODE: return "☊";
        case Planet::SOUTH_NODE: return "☋";
        case Planet::CHIRON: return "⚷";
        case Planet::LILITH: return "⚸";
        default: return "?";
    }
}

std::string formatBCDate(int year, int month, int day) {
    std::ostringstream oss;
    oss << std::setfill('0');

    if (year <= 0) {
        int bcYear = 1 - year;  // Convert from astronomical year numbering
        oss << bcYear << " BC-" << std::setw(2) << month << "-" << std::setw(2) << day;
    } else {
        oss << year << " AD-" << std::setw(2) << month << "-" << std::setw(2) << day;
    }

    return oss.str();
}

std::string formatBCDateLong(int year, int month, int day) {
    std::ostringstream oss;
    oss << std::setfill('0');

    const std::vector<std::string> monthNames = {
        "", "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
    };

    if (year <= 0) {
        int bcYear = 1 - year;  // Convert from astronomical year numbering
        oss << monthNames[month] << " " << day << ", " << bcYear << " BC";
    } else {
        oss << monthNames[month] << " " << day << ", " << year << " AD";
    }

    return oss.str();
}

std::string getPlanetName(Planet planet) {
    switch (planet) {
        case Planet::SUN: return "Sun";
        case Planet::MOON: return "Moon";
        case Planet::MERCURY: return "Mercury";
        case Planet::VENUS: return "Venus";
        case Planet::MARS: return "Mars";
        case Planet::JUPITER: return "Jupiter";
        case Planet::SATURN: return "Saturn";
        case Planet::URANUS: return "Uranus";
        case Planet::NEPTUNE: return "Neptune";
        case Planet::PLUTO: return "Pluto";
        case Planet::NORTH_NODE: return "North Node";
        case Planet::SOUTH_NODE: return "South Node";
        case Planet::CHIRON: return "Chiron";
        case Planet::LILITH: return "Lilith";
        default: return "Unknown";
    }
}

std::string zodiacModeToString(ZodiacMode mode) {
    switch (mode) {
        case ZodiacMode::TROPICAL: return "Tropical";
        case ZodiacMode::SIDEREAL: return "Sidereal";
        default: return "Unknown";
    }
}

std::string ayanamsaTypeToString(AyanamsaType ayanamsa) {
    switch (ayanamsa) {
        case AyanamsaType::FAGAN_BRADLEY: return "Fagan-Bradley";
        case AyanamsaType::LAHIRI: return "Lahiri (Chitrapaksha)";
        case AyanamsaType::DELUCE: return "De Luce";
        case AyanamsaType::RAMAN: return "B.V. Raman";
        case AyanamsaType::USHASHASHI: return "Usha-Shashi";
        case AyanamsaType::KRISHNAMURTI: return "K.S. Krishnamurti";
        case AyanamsaType::DJWHAL_KHUL: return "Djwhal Khul";
        case AyanamsaType::YUKTESHWAR: return "Sri Yukteshwar";
        case AyanamsaType::JN_BHASIN: return "J.N. Bhasin";
        case AyanamsaType::BABYLONIAN_KUGLER1: return "Babylonian (Kugler 1)";
        case AyanamsaType::BABYLONIAN_KUGLER2: return "Babylonian (Kugler 2)";
        case AyanamsaType::BABYLONIAN_KUGLER3: return "Babylonian (Kugler 3)";
        case AyanamsaType::BABYLONIAN_HUBER: return "Babylonian (Huber)";
        case AyanamsaType::BABYLONIAN_ETPSC: return "Babylonian (Et Psc)";
        case AyanamsaType::ALDEBARAN_15TAU: return "Aldebaran at 15° Taurus";
        case AyanamsaType::HIPPARCHOS: return "Hipparchos";
        case AyanamsaType::SASSANIAN: return "Sassanian";
        case AyanamsaType::GALACTIC_CENTER: return "Galactic Center";
        case AyanamsaType::J2000: return "J2000";
        case AyanamsaType::J1900: return "J1900";
        case AyanamsaType::B1950: return "B1950";
        default: return "Unknown";
    }
}

std::string calculationFlagToString(CalculationFlag flag) {
    switch (flag) {
        case CalculationFlag::GEOCENTRIC: return "Geocentric";
        case CalculationFlag::HELIOCENTRIC: return "Heliocentric";
        case CalculationFlag::BARYCENTRIC: return "Barycentric";
        case CalculationFlag::TOPOCENTRIC: return "Topocentric";
        case CalculationFlag::APPARENT: return "Apparent Position";
        case CalculationFlag::TRUE_GEOMETRIC: return "True Geometric Position";
        case CalculationFlag::ASTROMETRIC: return "Astrometric Position";
        case CalculationFlag::STANDARD_EQUINOX: return "Standard Equinox of Date";
        case CalculationFlag::J2000_EQUINOX: return "J2000.0 Equinox";
        case CalculationFlag::MEAN_EQUINOX: return "Mean Equinox of Date";
        case CalculationFlag::HIGH_PRECISION_SPEED: return "High Precision Speed";
        case CalculationFlag::ECLIPTIC: return "Ecliptic Coordinates";
        case CalculationFlag::EQUATORIAL: return "Equatorial Coordinates";
        default: return "Unknown";
    }
}

AyanamsaType stringToAyanamsaType(const std::string& ayanamsaStr) {
    std::string lower = ayanamsaStr;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    if (lower == "fagan-bradley" || lower == "fagan_bradley" || lower == "fagan") return AyanamsaType::FAGAN_BRADLEY;
    else if (lower == "lahiri" || lower == "chitrapaksha") return AyanamsaType::LAHIRI;
    else if (lower == "deluce" || lower == "de_luce") return AyanamsaType::DELUCE;
    else if (lower == "raman" || lower == "b.v.raman" || lower == "bv_raman") return AyanamsaType::RAMAN;
    else if (lower == "ushashashi" || lower == "usha_shashi" || lower == "usha-shashi") return AyanamsaType::USHASHASHI;
    else if (lower == "krishnamurti" || lower == "kp" || lower == "k.s.krishnamurti") return AyanamsaType::KRISHNAMURTI;
    else if (lower == "djwhal_khul" || lower == "djwhal-khul" || lower == "djwhal") return AyanamsaType::DJWHAL_KHUL;
    else if (lower == "yukteshwar" || lower == "sri_yukteshwar" || lower == "sri-yukteshwar") return AyanamsaType::YUKTESHWAR;
    else if (lower == "jn_bhasin" || lower == "j.n.bhasin" || lower == "bhasin") return AyanamsaType::JN_BHASIN;
    else if (lower == "babylonian_kugler1" || lower == "babyl_kugler1" || lower == "kugler1") return AyanamsaType::BABYLONIAN_KUGLER1;
    else if (lower == "babylonian_kugler2" || lower == "babyl_kugler2" || lower == "kugler2") return AyanamsaType::BABYLONIAN_KUGLER2;
    else if (lower == "babylonian_kugler3" || lower == "babyl_kugler3" || lower == "kugler3") return AyanamsaType::BABYLONIAN_KUGLER3;
    else if (lower == "babylonian_huber" || lower == "babyl_huber" || lower == "huber") return AyanamsaType::BABYLONIAN_HUBER;
    else if (lower == "babylonian_etpsc" || lower == "babyl_etpsc" || lower == "etpsc") return AyanamsaType::BABYLONIAN_ETPSC;
    else if (lower == "aldebaran_15tau" || lower == "aldebaran" || lower == "aldebaran_15") return AyanamsaType::ALDEBARAN_15TAU;
    else if (lower == "hipparchos" || lower == "hipparcos") return AyanamsaType::HIPPARCHOS;
    else if (lower == "sassanian") return AyanamsaType::SASSANIAN;
    else if (lower == "galactic_center" || lower == "galcent" || lower == "galactic") return AyanamsaType::GALACTIC_CENTER;
    else if (lower == "j2000") return AyanamsaType::J2000;
    else if (lower == "j1900") return AyanamsaType::J1900;
    else if (lower == "b1950") return AyanamsaType::B1950;
    else return AyanamsaType::LAHIRI; // default
}

ZodiacMode stringToZodiacMode(const std::string& modeStr) {
    std::string lower = modeStr;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    if (lower == "sidereal" || lower == "sid") return ZodiacMode::SIDEREAL;
    else return ZodiacMode::TROPICAL; // default
}

CalculationFlag stringToCalculationFlag(const std::string& flagStr) {
    std::string lower = flagStr;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    // Coordinate system
    if (lower == "geocentric" || lower == "geo") return CalculationFlag::GEOCENTRIC;
    else if (lower == "heliocentric" || lower == "helio") return CalculationFlag::HELIOCENTRIC;
    else if (lower == "barycentric" || lower == "bary") return CalculationFlag::BARYCENTRIC;
    else if (lower == "topocentric" || lower == "topo") return CalculationFlag::TOPOCENTRIC;

    // Position type
    else if (lower == "apparent" || lower == "app") return CalculationFlag::APPARENT;
    else if (lower == "true_geometric" || lower == "true" || lower == "geometric") return CalculationFlag::TRUE_GEOMETRIC;
    else if (lower == "astrometric" || lower == "astro") return CalculationFlag::ASTROMETRIC;

    // Precession/Nutation
    else if (lower == "standard_equinox" || lower == "standard" || lower == "date") return CalculationFlag::STANDARD_EQUINOX;
    else if (lower == "j2000_equinox" || lower == "j2000" || lower == "no_precession") return CalculationFlag::J2000_EQUINOX;
    else if (lower == "mean_equinox" || lower == "mean" || lower == "no_nutation") return CalculationFlag::MEAN_EQUINOX;

    // Speed
    else if (lower == "high_precision_speed" || lower == "high_speed" || lower == "speed") return CalculationFlag::HIGH_PRECISION_SPEED;

    // Coordinates
    else if (lower == "ecliptic" || lower == "ecl") return CalculationFlag::ECLIPTIC;
    else if (lower == "equatorial" || lower == "equ") return CalculationFlag::EQUATORIAL;

    else return CalculationFlag::GEOCENTRIC; // default
}

int ayanamsaTypeToSwissEphId(AyanamsaType ayanamsa) {
    switch (ayanamsa) {
        case AyanamsaType::FAGAN_BRADLEY: return SE_SIDM_FAGAN_BRADLEY;
        case AyanamsaType::LAHIRI: return SE_SIDM_LAHIRI;
        case AyanamsaType::DELUCE: return SE_SIDM_DELUCE;
        case AyanamsaType::RAMAN: return SE_SIDM_RAMAN;
        case AyanamsaType::USHASHASHI: return SE_SIDM_USHASHASHI;
        case AyanamsaType::KRISHNAMURTI: return SE_SIDM_KRISHNAMURTI;
        case AyanamsaType::DJWHAL_KHUL: return SE_SIDM_DJWHAL_KHUL;
        case AyanamsaType::YUKTESHWAR: return SE_SIDM_YUKTESHWAR;
        case AyanamsaType::JN_BHASIN: return SE_SIDM_JN_BHASIN;
        case AyanamsaType::BABYLONIAN_KUGLER1: return SE_SIDM_BABYL_KUGLER1;
        case AyanamsaType::BABYLONIAN_KUGLER2: return SE_SIDM_BABYL_KUGLER2;
        case AyanamsaType::BABYLONIAN_KUGLER3: return SE_SIDM_BABYL_KUGLER3;
        case AyanamsaType::BABYLONIAN_HUBER: return SE_SIDM_BABYL_HUBER;
        case AyanamsaType::BABYLONIAN_ETPSC: return SE_SIDM_BABYL_ETPSC;
        case AyanamsaType::ALDEBARAN_15TAU: return SE_SIDM_ALDEBARAN_15TAU;
        case AyanamsaType::HIPPARCHOS: return SE_SIDM_HIPPARCHOS;
        case AyanamsaType::SASSANIAN: return SE_SIDM_SASSANIAN;
        case AyanamsaType::GALACTIC_CENTER: return SE_SIDM_GALCENT_0SAG;
        case AyanamsaType::J2000: return SE_SIDM_J2000;
        case AyanamsaType::J1900: return SE_SIDM_J1900;
        case AyanamsaType::B1950: return SE_SIDM_B1950;
        default: return SE_SIDM_LAHIRI;
    }
}

} // namespace Astro