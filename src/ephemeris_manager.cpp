#include "ephemeris_manager.h"
#include "swephexp.h"
#include <iostream>
#include <cstring>

namespace Astro {

EphemerisManager::EphemerisManager() : initialized(false) {
}

EphemerisManager::~EphemerisManager() {
    if (initialized) {
        swe_close();
    }
}

bool EphemerisManager::initialize(const std::string& ephemerisPath) {
    if (!ephemerisPath.empty()) {
        setEphemerisPath(ephemerisPath);
    }

    // Set ephemeris flags
    swe_set_ephe_path(nullptr); // Use default path if not specified

    initialized = true;
    lastError.clear();
    return true;
}

void EphemerisManager::setEphemerisPath(const std::string& path) {
    if (!path.empty()) {
        swe_set_ephe_path(const_cast<char*>(path.c_str()));
    }
}

bool EphemerisManager::calculatePlanetPosition(double julianDay, Planet planet, PlanetPosition& position) {
    if (!initialized) {
        lastError = "EphemerisManager not initialized";
        return false;
    }

    double xx[6];
    char serr[256];
    int32 iflag = SEFLG_SWIEPH | SEFLG_SPEED;
    int ipl = planetToSwissEph(planet);

    if (ipl < 0) {
        lastError = "Invalid planet";
        return false;
    }

    int32 ret = swe_calc_ut(julianDay, ipl, iflag, xx, serr);

    if (ret < 0) {
        lastError = std::string("Swiss Ephemeris error: ") + serr;
        return false;
    }

    // Fill position structure
    position.planet = planet;
    position.longitude = xx[0];
    position.latitude = xx[1];
    position.distance = xx[2];
    position.speed = xx[3];
    position.house = 0; // Will be set by HouseCalculator
    position.housePosition = 0.0;

    position.calculateSignPosition();

    return true;
}

bool EphemerisManager::calculatePlanetPosition(double julianDay, Planet planet, PlanetPosition& position,
                                             ZodiacMode zodiacMode, AyanamsaType ayanamsa,
                                             const std::vector<CalculationFlag>& flags) {
    if (!initialized) {
        lastError = "EphemerisManager not initialized";
        return false;
    }

    double xx[6];
    char serr[256];
    int32 iflag = buildSwissEphFlags(zodiacMode, flags);
    int ipl = planetToSwissEph(planet);

    if (ipl < 0) {
        lastError = "Invalid planet";
        return false;
    }

    // Set ayanamsa if using sidereal zodiac
    if (zodiacMode == ZodiacMode::SIDEREAL) {
        swe_set_sid_mode(ayanamsaTypeToSwissEphId(ayanamsa), 0, 0);
    }

    int ret = swe_calc(julianDay, ipl, iflag, xx, serr);

    if (ret < 0) {
        lastError = std::string("Swiss Ephemeris error: ") + serr;
        return false;
    }

    position.planet = planet;
    position.longitude = xx[0];
    position.latitude = xx[1];
    position.distance = xx[2];
    position.speed = xx[3];
    position.house = 0; // Will be set by HouseCalculator
    position.housePosition = 0.0;

    position.calculateSignPosition();

    return true;
}

bool EphemerisManager::calculateHouseCusps(double julianDay, double latitude, double longitude,
                                         HouseSystem system, HouseCusps& cusps) {
    if (!initialized) {
        lastError = "EphemerisManager not initialized";
        return false;
    }

    double hcusps[13];
    double ascmc[10];
    char hsys = houseSystemToSwissEph(system);

    int ret = swe_houses(julianDay, latitude, longitude, hsys, hcusps, ascmc);

    if (ret < 0) {
        lastError = "Failed to calculate house cusps";
        return false;
    }

    // Copy house cusps
    for (int i = 1; i <= 12; i++) {
        cusps.cusps[i] = hcusps[i];
    }

    cusps.ascendant = ascmc[0];
    cusps.midheaven = ascmc[1];
    cusps.vertex = ascmc[3];
    cusps.eastPoint = ascmc[4];

    return true;
}

bool EphemerisManager::calculateHouseCusps(double julianDay, double latitude, double longitude,
                                         HouseSystem system, HouseCusps& cusps,
                                         ZodiacMode zodiacMode, AyanamsaType ayanamsa) {
    if (!initialized) {
        lastError = "EphemerisManager not initialized";
        return false;
    }

    // Set ayanamsa if using sidereal zodiac
    if (zodiacMode == ZodiacMode::SIDEREAL) {
        swe_set_sid_mode(ayanamsaTypeToSwissEphId(ayanamsa), 0, 0);
    }

    double hcusps[13];
    double ascmc[10];
    char hsys = houseSystemToSwissEph(system);
    int32 iflag = (zodiacMode == ZodiacMode::SIDEREAL) ? SEFLG_SIDEREAL : 0;

    int ret = swe_houses_ex(julianDay, iflag, latitude, longitude, hsys, hcusps, ascmc);

    if (ret < 0) {
        lastError = "Failed to calculate house cusps";
        return false;
    }

    // Copy house cusps
    for (int i = 1; i <= 12; i++) {
        cusps.cusps[i] = hcusps[i];
    }

    cusps.ascendant = ascmc[0];
    cusps.midheaven = ascmc[1];
    cusps.vertex = ascmc[3];
    cusps.eastPoint = ascmc[4];

    return true;
}

std::string EphemerisManager::getLastError() const {
    return lastError;
}

bool EphemerisManager::isDateAvailable(double julianDay) const {
    // Swiss Ephemeris typically covers -3000 to +3000 CE
    // Julian Day 0 corresponds to January 1, 4713 BCE
    // Approximate range: JD 625000 to JD 2816787
    return julianDay >= 625000.0 && julianDay <= 2816787.0;
}

int EphemerisManager::planetToSwissEph(Planet planet) const {
    switch (planet) {
        case Planet::SUN: return SE_SUN;
        case Planet::MOON: return SE_MOON;
        case Planet::MERCURY: return SE_MERCURY;
        case Planet::VENUS: return SE_VENUS;
        case Planet::MARS: return SE_MARS;
        case Planet::JUPITER: return SE_JUPITER;
        case Planet::SATURN: return SE_SATURN;
        case Planet::URANUS: return SE_URANUS;
        case Planet::NEPTUNE: return SE_NEPTUNE;
        case Planet::PLUTO: return SE_PLUTO;
        case Planet::NORTH_NODE: return SE_TRUE_NODE;
        case Planet::SOUTH_NODE: return SE_TRUE_NODE; // Will need special handling
        case Planet::CHIRON: return SE_CHIRON;
        case Planet::LILITH: return SE_MEAN_APOG; // Black Moon Lilith
        default: return -1;
    }
}

char EphemerisManager::houseSystemToSwissEph(HouseSystem system) const {
    return static_cast<char>(system);
}

int EphemerisManager::buildSwissEphFlags(ZodiacMode zodiacMode, const std::vector<CalculationFlag>& flags) const {
    int32 iflag = SEFLG_SWIEPH; // Use Swiss Ephemeris by default

    // Set zodiac mode
    if (zodiacMode == ZodiacMode::SIDEREAL) {
        iflag |= SEFLG_SIDEREAL;
    }
    // TROPICAL is the default (no additional flag needed)

    // Process calculation flags
    for (const auto& flag : flags) {
        switch (flag) {
            case CalculationFlag::GEOCENTRIC:
                // Geocentric is default, no flag needed
                break;
            case CalculationFlag::HELIOCENTRIC:
                iflag |= SEFLG_HELCTR;
                break;
            case CalculationFlag::BARYCENTRIC:
                iflag |= SEFLG_BARYCTR;
                break;
            case CalculationFlag::TOPOCENTRIC:
                iflag |= SEFLG_TOPOCTR;
                break;
            case CalculationFlag::APPARENT:
                // Apparent position is default, no additional flag needed
                break;
            case CalculationFlag::TRUE_GEOMETRIC:
                iflag |= SEFLG_TRUEPOS;
                break;
            case CalculationFlag::ASTROMETRIC:
                iflag |= SEFLG_ASTROMETRIC;
                break;
            case CalculationFlag::STANDARD_EQUINOX:
                // Standard equinox of date is default, no flag needed
                break;
            case CalculationFlag::J2000_EQUINOX:
                iflag |= SEFLG_J2000;
                break;
            case CalculationFlag::MEAN_EQUINOX:
                iflag |= SEFLG_NONUT;
                break;
            case CalculationFlag::HIGH_PRECISION_SPEED:
                iflag |= SEFLG_SPEED;
                break;
            case CalculationFlag::EQUATORIAL:
                iflag |= SEFLG_EQUATORIAL;
                break;
            default:
                // Ignore unknown flags
                break;
        }
    }

    return iflag;
}

} // namespace Astro
