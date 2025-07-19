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

    // Set which planets to include in calculations
    void setPlanetsToCalculate(const std::vector<Planet>& planets);

    // Get error message
    std::string getLastError() const;

private:
    EphemerisManager& ephemerisManager;
    std::vector<Planet> planetsToCalculate;
    std::string lastError;
};

} // namespace Astro
