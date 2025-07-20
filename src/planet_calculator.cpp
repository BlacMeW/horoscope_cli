#include "planet_calculator.h"

namespace Astro {

PlanetCalculator::PlanetCalculator(EphemerisManager& ephemeris)
    : ephemerisManager(ephemeris),
      zodiacMode(ZodiacMode::TROPICAL),
      ayanamsa(AyanamsaType::LAHIRI) {
    planetsToCalculate = getStandardPlanets();
}

bool PlanetCalculator::calculateAllPlanets(const BirthData& birthData, std::vector<PlanetPosition>& positions) {
    positions.clear();
    positions.reserve(planetsToCalculate.size());

    double julianDay = birthData.getJulianDay();

    for (Planet planet : planetsToCalculate) {
        PlanetPosition position;
        if (calculatePlanet(birthData, planet, position)) {
            positions.push_back(position);
        } else {
            lastError = "Failed to calculate position for " + planetToString(planet) +
                       ": " + ephemerisManager.getLastError();
            return false;
        }
    }

    return true;
}

bool PlanetCalculator::calculatePlanet(const BirthData& birthData, Planet planet, PlanetPosition& position) {
    double julianDay = birthData.getJulianDay();

    bool success = ephemerisManager.calculatePlanetPosition(julianDay, planet, position, zodiacMode, ayanamsa, calculationFlags);

    if (!success) {
        lastError = ephemerisManager.getLastError();
        return false;
    }

    // Special handling for South Node
    if (planet == Planet::SOUTH_NODE) {
        // South Node is always 180 degrees opposite to North Node
        // We need to calculate North Node first, then add 180 degrees
        PlanetPosition northNode;
        if (ephemerisManager.calculatePlanetPosition(julianDay, Planet::NORTH_NODE, northNode, zodiacMode, ayanamsa, calculationFlags)) {
            position.longitude = normalizeAngle(northNode.longitude + 180.0);
            position.latitude = -northNode.latitude;
            position.distance = northNode.distance;
            position.speed = northNode.speed;
            position.planet = Planet::SOUTH_NODE;
            position.calculateSignPosition();
        } else {
            lastError = "Failed to calculate North Node for South Node calculation";
            return false;
        }
    }

    return true;
}std::vector<Planet> PlanetCalculator::getStandardPlanets() {
    return {
        Planet::SUN,
        Planet::MOON,
        Planet::MERCURY,
        Planet::VENUS,
        Planet::MARS,
        Planet::JUPITER,
        Planet::SATURN,
        Planet::URANUS,
        Planet::NEPTUNE,
        Planet::PLUTO,
        Planet::NORTH_NODE,
        Planet::SOUTH_NODE,
        Planet::CHIRON
    };
}

std::vector<Planet> PlanetCalculator::getAncientDatePlanets() {
    // Only include planets with reliable ephemeris data for ancient dates
    // Excludes Chiron, Lilith, and modern outer planets discovered after antiquity
    return {
        Planet::SUN,
        Planet::MOON,
        Planet::MERCURY,
        Planet::VENUS,
        Planet::MARS,
        Planet::JUPITER,
        Planet::SATURN,
        Planet::NORTH_NODE,
        Planet::SOUTH_NODE
    };
}

void PlanetCalculator::setPlanetsToCalculate(const std::vector<Planet>& planets) {
    planetsToCalculate = planets;
}

void PlanetCalculator::setZodiacMode(ZodiacMode mode) {
    zodiacMode = mode;
}

void PlanetCalculator::setAyanamsa(AyanamsaType ayanamsa) {
    this->ayanamsa = ayanamsa;
}

void PlanetCalculator::setCalculationFlags(const std::vector<CalculationFlag>& flags) {
    calculationFlags = flags;
}

std::string PlanetCalculator::getLastError() const {
    return lastError;
}

} // namespace Astro
