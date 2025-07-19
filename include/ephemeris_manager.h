#pragma once

#include "astro_types.h"
#include <string>

namespace Astro {

class EphemerisManager {
public:
    EphemerisManager();
    ~EphemerisManager();

    // Initialize Swiss Ephemeris
    bool initialize(const std::string& ephemerisPath = "");

    // Set ephemeris path
    void setEphemerisPath(const std::string& path);

    // Calculate planet position for given Julian Day
    bool calculatePlanetPosition(double julianDay, Planet planet, PlanetPosition& position);

    // Calculate house cusps
    bool calculateHouseCusps(double julianDay, double latitude, double longitude,
                           HouseSystem system, HouseCusps& cusps);

    // Get error message if calculation fails
    std::string getLastError() const;

    // Check if ephemeris data is available for given date
    bool isDateAvailable(double julianDay) const;

private:
    std::string lastError;
    bool initialized;

    // Convert Planet enum to Swiss Ephemeris planet number
    int planetToSwissEph(Planet planet) const;

    // Convert HouseSystem enum to Swiss Ephemeris house system character
    char houseSystemToSwissEph(HouseSystem system) const;
};

} // namespace Astro
