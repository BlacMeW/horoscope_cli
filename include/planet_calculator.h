#pragma once

#include "astro_types.h"
#include "ephemeris_manager.h"
#include <vector>

namespace Astro {

class PlanetCalculator {
public:
    PlanetCalculator(EphemerisManager& ephemeris);

    // Calculate all planetary positions for given birth data
    bool calculateAllPlanets(const BirthData& birthData, std::vector<PlanetPosition>& positions);

    // Calculate single planet position
    bool calculatePlanet(const BirthData& birthData, Planet planet, PlanetPosition& position);

    // Get list of planets to calculate
    static std::vector<Planet> getStandardPlanets();

    // Get list of planets available for ancient dates (before 650 AD)
    static std::vector<Planet> getAncientDatePlanets();

    // Set which planets to include in calculations
    void setPlanetsToCalculate(const std::vector<Planet>& planets);

    // Set zodiac calculation mode (tropical/sidereal)
    void setZodiacMode(ZodiacMode mode);

    // Set ayanamsa for sidereal calculations
    void setAyanamsa(AyanamsaType ayanamsa);

    // Set calculation flags
    void setCalculationFlags(const std::vector<CalculationFlag>& flags);

    // Get error message
    std::string getLastError() const;

private:
    EphemerisManager& ephemerisManager;
    std::vector<Planet> planetsToCalculate;
    ZodiacMode zodiacMode;
    AyanamsaType ayanamsa;
    std::vector<CalculationFlag> calculationFlags;
    std::string lastError;
};

} // namespace Astro
