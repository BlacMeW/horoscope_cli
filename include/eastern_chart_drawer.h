#pragma once

#include "birth_chart.h"
#include <string>
#include <vector>

namespace Astro {

class EasternChartDrawer {
public:
    EasternChartDrawer();

    // Draw Eastern/Vedic style chart in ASCII
    std::string drawEasternChart(const BirthChart& chart) const;

    // Draw North Indian style chart
    std::string drawNorthIndianChart(const BirthChart& chart) const;

    // Draw South Indian style chart
    std::string drawSouthIndianChart(const BirthChart& chart) const;

    // Draw East Indian style chart
    std::string drawEastIndianChart(const BirthChart& chart) const;

    // Set chart style
    void setChartStyle(const std::string& style);

    // Get available chart styles
    static std::vector<std::string> getAvailableStyles();

private:
    std::string chartStyle;

    // Helper functions for chart drawing
    std::string getPlanetsInHouse(const std::vector<PlanetPosition>& positions, int house) const;
    std::string getShortPlanetName(Planet planet) const;
    std::string formatHouseContent(const std::string& content, int width) const;

    // North Indian chart specific functions
    std::string drawNorthIndianFrame() const;
    void fillNorthIndianHouses(std::vector<std::string>& lines,
                              const std::vector<PlanetPosition>& positions) const;

    // South Indian chart specific functions
    std::string drawSouthIndianFrame() const;
    void fillSouthIndianHouses(std::vector<std::string>& lines,
                              const std::vector<PlanetPosition>& positions) const;

    // East Indian chart specific functions
    std::string drawEastIndianFrame() const;
    void fillEastIndianHouses(std::vector<std::string>& lines,
                             const std::vector<PlanetPosition>& positions) const;

    // Get house number mapping for different chart styles
    int getNorthIndianHousePosition(int house) const;
    int getSouthIndianHousePosition(int house) const;
    int getEastIndianHousePosition(int house) const;
};

} // namespace Astro
