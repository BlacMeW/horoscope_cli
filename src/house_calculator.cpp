#include "house_calculator.h"
#include <cmath>

namespace Astro {

HouseCalculator::HouseCalculator(EphemerisManager& ephemeris)
    : ephemerisManager(ephemeris) {
}

bool HouseCalculator::calculateHouses(const BirthData& birthData, HouseSystem system, HouseCusps& cusps) {
    double julianDay = birthData.getJulianDay();

    bool success = ephemerisManager.calculateHouseCusps(julianDay, birthData.latitude,
                                                       birthData.longitude, system, cusps);

    if (!success) {
        lastError = ephemerisManager.getLastError();
        return false;
    }

    return true;
}

bool HouseCalculator::calculateHouses(const BirthData& birthData, HouseSystem system, HouseCusps& cusps,
                                     ZodiacMode zodiacMode, AyanamsaType ayanamsa) {
    double julianDay = birthData.getJulianDay();

    bool success = ephemerisManager.calculateHouseCusps(julianDay, birthData.latitude,
                                                       birthData.longitude, system, cusps,
                                                       zodiacMode, ayanamsa);

    if (!success) {
        lastError = ephemerisManager.getLastError();
        return false;
    }

    return true;
}

void HouseCalculator::assignHousesToPlanets(const HouseCusps& cusps, std::vector<PlanetPosition>& positions) {
    for (auto& position : positions) {
        position.house = getHouseNumber(cusps, position.longitude);
        position.housePosition = getHousePosition(cusps, position.longitude, position.house);
    }
}

int HouseCalculator::getHouseNumber(const HouseCusps& cusps, double longitude) {
    return cusps.getHouseForLongitude(longitude);
}

double HouseCalculator::getHousePosition(const HouseCusps& cusps, double longitude, int house) {
    if (house < 1 || house > 12) return 0.0;

    double currentCusp = normalizeAngle(cusps.cusps[house]);
    double nextCusp = normalizeAngle(cusps.cusps[house == 12 ? 1 : house + 1]);
    longitude = normalizeAngle(longitude);

    double houseSize;
    double positionInHouse;

    if (nextCusp > currentCusp) {
        // Normal case: cusp doesn't cross 0°
        houseSize = nextCusp - currentCusp;
        positionInHouse = longitude - currentCusp;
    } else {
        // Cusp crosses 0° Aries
        houseSize = (360.0 - currentCusp) + nextCusp;
        if (longitude >= currentCusp) {
            positionInHouse = longitude - currentCusp;
        } else {
            positionInHouse = (360.0 - currentCusp) + longitude;
        }
    }

    // Convert to 0-30 degree scale (as if each house were 30 degrees)
    return (positionInHouse / houseSize) * 30.0;
}

std::string HouseCalculator::getLastError() const {
    return lastError;
}

} // namespace Astro
