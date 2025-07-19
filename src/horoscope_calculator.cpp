#include "horoscope_calculator.h"
#include <algorithm>

namespace Astro {

HoroscopeCalculator::HoroscopeCalculator()
    : planetCalculator(ephemerisManager),
      houseCalculator(ephemerisManager),
      defaultHouseSystem(HouseSystem::PLACIDUS),
      initialized(false) {
}

HoroscopeCalculator::~HoroscopeCalculator() {
}

bool HoroscopeCalculator::initialize(const std::string& ephemerisPath) {
    if (!ephemerisManager.initialize(ephemerisPath)) {
        lastError = "Failed to initialize ephemeris manager: " + ephemerisManager.getLastError();
        return false;
    }

    initialized = true;
    lastError.clear();
    return true;
}

bool HoroscopeCalculator::calculateBirthChart(const BirthData& birthData,
                                            HouseSystem houseSystem,
                                            BirthChart& chart) {
    if (!initialized) {
        lastError = "Calculator not initialized";
        return false;
    }

    if (!validateBirthData(birthData)) {
        return false;
    }

    // Set birth data in chart
    chart.setBirthData(birthData);

    // Determine which planets to use based on date
    // Use ancient planet set for dates before 650 AD to avoid ephemeris limitations
    // Julian Day 1967601.5 corresponds to approximately 650 AD
    bool isAncientDate = birthData.getJulianDay() < 1967601.5;
    if (isAncientDate) {
        planetCalculator.setPlanetsToCalculate(PlanetCalculator::getAncientDatePlanets());
    } else {
        planetCalculator.setPlanetsToCalculate(PlanetCalculator::getStandardPlanets());
    }

    // Calculate planetary positions
    std::vector<PlanetPosition> planetPositions;
    if (!planetCalculator.calculateAllPlanets(birthData, planetPositions)) {
        lastError = "Failed to calculate planetary positions: " + planetCalculator.getLastError();
        return false;
    }

    // Calculate house cusps
    HouseCusps houseCusps;
    if (!houseCalculator.calculateHouses(birthData, houseSystem, houseCusps)) {
        lastError = "Failed to calculate houses: " + houseCalculator.getLastError();
        return false;
    }

    // Assign houses to planets
    houseCalculator.assignHousesToPlanets(houseCusps, planetPositions);

    // Calculate aspects
    std::vector<Aspect> aspects = aspectCalculator.calculateAspects(planetPositions);

    // Set all data in chart
    chart.setPlanetPositions(planetPositions);
    chart.setHouseCusps(houseCusps);
    chart.setAspects(aspects);

    return true;
}

void HoroscopeCalculator::setHouseSystem(HouseSystem system) {
    defaultHouseSystem = system;
}

void HoroscopeCalculator::setPlanetsToCalculate(const std::vector<Planet>& planets) {
    planetCalculator.setPlanetsToCalculate(planets);
}

void HoroscopeCalculator::setAspectOrbs(const std::map<AspectType, double>& orbs) {
    for (const auto& pair : orbs) {
        aspectCalculator.setOrbTolerance(pair.first, pair.second);
    }
}

std::string HoroscopeCalculator::getLastError() const {
    return lastError;
}

bool HoroscopeCalculator::isInitialized() const {
    return initialized;
}

bool HoroscopeCalculator::validateBirthData(const BirthData& birthData) {
    // Validate year range
    if (birthData.year < -3000 || birthData.year > 3000) {
        lastError = "Year must be between -3000 and 3000";
        return false;
    }

    // Validate month
    if (birthData.month < 1 || birthData.month > 12) {
        lastError = "Month must be between 1 and 12";
        return false;
    }

    // Validate day
    if (birthData.day < 1 || birthData.day > 31) {
        lastError = "Day must be between 1 and 31";
        return false;
    }

    // Validate hour
    if (birthData.hour < 0 || birthData.hour > 23) {
        lastError = "Hour must be between 0 and 23";
        return false;
    }

    // Validate minute
    if (birthData.minute < 0 || birthData.minute > 59) {
        lastError = "Minute must be between 0 and 59";
        return false;
    }

    // Validate second
    if (birthData.second < 0 || birthData.second > 59) {
        lastError = "Second must be between 0 and 59";
        return false;
    }

    // Validate latitude
    if (birthData.latitude < -90.0 || birthData.latitude > 90.0) {
        lastError = "Latitude must be between -90 and 90 degrees";
        return false;
    }

    // Validate longitude
    if (birthData.longitude < -180.0 || birthData.longitude > 180.0) {
        lastError = "Longitude must be between -180 and 180 degrees";
        return false;
    }

    // Validate timezone
    if (birthData.timezone < -12.0 || birthData.timezone > 14.0) {
        lastError = "Timezone must be between -12 and +14 hours";
        return false;
    }

    // Check if date is available in ephemeris
    double julianDay = birthData.getJulianDay();
    if (!ephemerisManager.isDateAvailable(julianDay)) {
        lastError = "Date is outside available ephemeris range";
        return false;
    }

    return true;
}

} // namespace Astro
