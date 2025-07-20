#pragma once

#include "astro_types.h"
#include <vector>
#include <string>

namespace Astro {

class BirthChart {
public:
    BirthChart();

    // Set birth data
    void setBirthData(const BirthData& data);

    // Set planetary positions
    void setPlanetPositions(const std::vector<PlanetPosition>& positions);

    // Set house cusps
    void setHouseCusps(const HouseCusps& cusps);

    // Set aspects
    void setAspects(const std::vector<Aspect>& aspects);

    // Set zodiac mode and ayanamsa
    void setZodiacMode(ZodiacMode mode);
    void setAyanamsa(AyanamsaType ayanamsa);

    // Get formatted chart as string
    std::string getFormattedChart() const;

    // Get planet positions section
    std::string getPlanetPositionsString() const;

    // Get house cusps section
    std::string getHouseCuspsString() const;

    // Get aspects section
    std::string getAspectsString() const;

    // Get chart header
    std::string getChartHeader() const;

    // Export to JSON format
    std::string exportToJson() const;

    // Getters
    const BirthData& getBirthData() const { return birthData; }
    const std::vector<PlanetPosition>& getPlanetPositions() const { return planetPositions; }
    const HouseCusps& getHouseCusps() const { return houseCusps; }
    const std::vector<Aspect>& getAspects() const { return aspects; }
    ZodiacMode getZodiacMode() const { return zodiacMode; }
    AyanamsaType getAyanamsa() const { return ayanamsa; }

private:
    BirthData birthData;
    std::vector<PlanetPosition> planetPositions;
    HouseCusps houseCusps;
    std::vector<Aspect> aspects;
    ZodiacMode zodiacMode;
    AyanamsaType ayanamsa;

    // Helper functions for formatting
    std::string formatDegreeMinute(double degrees) const;
    std::string formatCoordinate(double coord, bool isLatitude) const;
};

} // namespace Astro
