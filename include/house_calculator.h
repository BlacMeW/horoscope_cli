#pragma once

#include "astro_types.h"
#include "ephemeris_manager.h"

namespace Astro {

class HouseCalculator {
public:
    HouseCalculator(EphemerisManager& ephemeris);

    // Calculate house cusps for given birth data
    bool calculateHouses(const BirthData& birthData, HouseSystem system, HouseCusps& cusps);

    // Calculate house cusps with zodiac mode and ayanamsa
    bool calculateHouses(const BirthData& birthData, HouseSystem system, HouseCusps& cusps,
                        ZodiacMode zodiacMode, AyanamsaType ayanamsa);

    // Assign house numbers to planet positions
    void assignHousesToPlanets(const HouseCusps& cusps, std::vector<PlanetPosition>& positions);

    // Get house position for a given longitude
    static int getHouseNumber(const HouseCusps& cusps, double longitude);

    // Get position within house (0-30 degrees)
    static double getHousePosition(const HouseCusps& cusps, double longitude, int house);

    // Get error message
    std::string getLastError() const;

private:
    EphemerisManager& ephemerisManager;
    std::string lastError;
};

} // namespace Astro
