#include "eastern_chart_drawer.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <map>

namespace Astro {

EasternChartDrawer::EasternChartDrawer() : chartStyle("north-indian") {
}

std::string EasternChartDrawer::drawEasternChart(const BirthChart& chart) const {
    if (chartStyle == "south-indian") {
        return drawSouthIndianChart(chart);
    } else if (chartStyle == "east-indian") {
        return drawEastIndianChart(chart);
    } else {
        return drawNorthIndianChart(chart);
    }
}

std::string EasternChartDrawer::drawNorthIndianChart(const BirthChart& chart) const {
    std::ostringstream oss;

    oss << "\n=== NORTH INDIAN VEDIC CHART ===\n\n";

    const auto& positions = chart.getPlanetPositions();

    // Create the chart frame
    std::vector<std::string> lines = {
        "+-------------+-------------+-------------+-------------+",
        "|             |             |             |             |",
        "|     12      |      1      |      2      |      3      |",
        "|             |             |             |             |",
        "+-------------+-------------+-------------+-------------+",
        "|             |                           |             |",
        "|     11      |                           |      4      |",
        "|             |                           |             |",
        "+-------------+           RASI            +-------------+",
        "|             |                           |             |",
        "|     10      |                           |      5      |",
        "|             |                           |             |",
        "+-------------+-------------+-------------+-------------+",
        "|             |             |             |             |",
        "|      9      |      8      |      7      |      6      |",
        "|             |             |             |             |",
        "+-------------+-------------+-------------+-------------+"
    };

    fillNorthIndianHouses(lines, positions);

    for (const auto& line : lines) {
        oss << line << "\n";
    }

    // Add legend
    oss << "\nPlanet Symbols:\n";
    oss << "Su=Sun, Mo=Moon, Me=Mercury, Ve=Venus, Ma=Mars\n";
    oss << "Ju=Jupiter, Sa=Saturn, Ra=Rahu, Ke=Ketu\n";
    oss << "Ur=Uranus, Ne=Neptune, Pl=Pluto\n";

    return oss.str();
}

std::string EasternChartDrawer::drawSouthIndianChart(const BirthChart& chart) const {
    std::ostringstream oss;

    oss << "\n=== SOUTH INDIAN VEDIC CHART ===\n\n";

    const auto& positions = chart.getPlanetPositions();

    // Create the chart frame - South Indian style 4x4 perimeter grid
    std::vector<std::string> lines = {
        "+-------------+-------------+-------------+-------------+",
        "|             |             |             |             |",
        "|     12      |      1      |      2      |      3      |",
        "|             |             |             |             |",
        "+-------------+-------------+-------------+-------------+",
        "|             |                           |             |",
        "|     11      |                           |      4      |",
        "|             |                           |             |",
        "+-------------+           RASI            +-------------+",
        "|             |                           |             |",
        "|     10      |                           |      5      |",
        "|             |                           |             |",
        "+-------------+-------------+-------------+-------------+",
        "|             |             |             |             |",
        "|      9      |      8      |      7      |      6      |",
        "|             |             |             |             |",
        "+-------------+-------------+-------------+-------------+"
    };

    fillSouthIndianHouses(lines, positions);

    for (const auto& line : lines) {
        oss << line << "\n";
    }

    // Add legend
    oss << "\nPlanet Symbols:\n";
    oss << "Su=Sun, Mo=Moon, Me=Mercury, Ve=Venus, Ma=Mars\n";
    oss << "Ju=Jupiter, Sa=Saturn, Ra=Rahu, Ke=Ketu\n";
    oss << "Ur=Uranus, Ne=Neptune, Pl=Pluto\n";

    return oss.str();
}

std::string EasternChartDrawer::drawEastIndianChart(const BirthChart& chart) const {
    std::ostringstream oss;

    oss << "\n=== EAST INDIAN VEDIC CHART ===\n\n";

    const auto& positions = chart.getPlanetPositions();

    // Create the chart frame - East Indian style 4x4 grid with all 12 houses
    std::vector<std::string> lines = {
        "+-------------+-------------+-------------+-------------+",
        "|             |             |             |             |",
        "|     10      |     11      |     12      |      1      |",
        "|             |             |             |             |",
        "+-------------+-------------+-------------+-------------+",
        "|             |                           |             |",
        "|      9      |                           |      2      |",
        "|             |                           |             |",
        "+-------------+           RASI            +-------------+",
        "|             |                           |             |",
        "|      8      |                           |      3      |",
        "|             |                           |             |",
        "+-------------+-------------+-------------+-------------+",
        "|             |             |             |             |",
        "|      7      |      6      |      5      |      4      |",
        "|             |             |             |             |",
        "+-------------+-------------+-------------+-------------+"
    };

    fillEastIndianHouses(lines, positions);

    for (const auto& line : lines) {
        oss << line << "\n";
    }

    // Add legend
    oss << "\nPlanet Symbols:\n";
    oss << "Su=Sun, Mo=Moon, Me=Mercury, Ve=Venus, Ma=Mars\n";
    oss << "Ju=Jupiter, Sa=Saturn, Ra=Rahu, Ke=Ketu\n";
    oss << "Ur=Uranus, Ne=Neptune, Pl=Pluto\n";
    oss << "\nEast Indian Chart Layout:\n";
    oss << "- Houses arranged in rectangular grid (all 12 houses)\n";
    oss << "- House 1 (Ascendant) in top-right position\n";
    oss << "- Traditional Bengali/Oriya astrology format\n";

    return oss.str();
}

void EasternChartDrawer::setChartStyle(const std::string& style) {
    chartStyle = style;
}

std::vector<std::string> EasternChartDrawer::getAvailableStyles() {
    return {"north-indian", "south-indian", "east-indian"};
}

std::string EasternChartDrawer::getPlanetsInHouse(const std::vector<PlanetPosition>& positions, int house) const {
    std::vector<std::string> planetsInHouse;

    for (const auto& pos : positions) {
        if (pos.house == house) {
            planetsInHouse.push_back(getShortPlanetName(pos.planet));
        }
    }

    std::ostringstream oss;
    for (size_t i = 0; i < planetsInHouse.size(); i++) {
        if (i > 0) oss << ",";
        oss << planetsInHouse[i];
    }

    return oss.str();
}

std::string EasternChartDrawer::getShortPlanetName(Planet planet) const {
    switch (planet) {
        case Planet::SUN: return "Su";
        case Planet::MOON: return "Mo";
        case Planet::MERCURY: return "Me";
        case Planet::VENUS: return "Ve";
        case Planet::MARS: return "Ma";
        case Planet::JUPITER: return "Ju";
        case Planet::SATURN: return "Sa";
        case Planet::URANUS: return "Ur";
        case Planet::NEPTUNE: return "Ne";
        case Planet::PLUTO: return "Pl";
        case Planet::NORTH_NODE: return "Ra"; // Rahu
        case Planet::SOUTH_NODE: return "Ke"; // Ketu
        case Planet::CHIRON: return "Ch";
        case Planet::LILITH: return "Li";
        default: return "??";
    }
}

std::string EasternChartDrawer::formatHouseContent(const std::string& content, int width) const {
    if (content.length() >= static_cast<size_t>(width)) {
        return content.substr(0, width - 1);
    }

    int padding = width - content.length();
    int leftPad = padding / 2;
    int rightPad = padding - leftPad;

    return std::string(leftPad, ' ') + content + std::string(rightPad, ' ');
}

void EasternChartDrawer::fillNorthIndianHouses(std::vector<std::string>& lines,
                                              const std::vector<PlanetPosition>& positions) const {
    struct HousePosition {
        int line;
        int startCol;
        int width;
    };

    std::vector<HousePosition> housePositions = {
        {0, 0, 0},      // Dummy for index 0
        {2, 16, 11},    // House 1
        {2, 30, 11},    // House 2
        {2, 44, 11},    // House 3
        {6, 44, 11},    // House 4
        {10, 44, 11},   // House 5
        {14, 44, 11},   // House 6
        {14, 30, 11},   // House 7
        {14, 16, 11},   // House 8
        {14, 2, 11},    // House 9
        {10, 2, 11},    // House 10
        {6, 2, 11},     // House 11
        {2, 2, 11}      // House 12
    };

    for (int house = 1; house <= 12; house++) {
        std::string planets = getPlanetsInHouse(positions, house);
        if (!planets.empty()) {
            const auto& pos = housePositions[house];
            std::string formatted = formatHouseContent(planets, pos.width);

            if (pos.line < static_cast<int>(lines.size())) {
                std::string& line = lines[pos.line];
                if (pos.startCol + formatted.length() <= line.length()) {
                    line.replace(pos.startCol, formatted.length(), formatted);
                }
            }
        }
    }
}

void EasternChartDrawer::fillSouthIndianHouses(std::vector<std::string>& lines,
                                              const std::vector<PlanetPosition>& positions) const {
    struct HousePosition {
        int line;
        int startCol;
        int width;
    };

    // South Indian chart house positions in the 4x4 grid (Pisces top-left to Aquarius mid-left)
    std::vector<HousePosition> housePositions = {
        {0, 0, 0},      // Dummy for index 0
        {2, 16, 11},    // House 1 (Aries)
        {2, 30, 11},    // House 2 (Taurus)
        {2, 44, 11},    // House 3 (Gemini)
        {6, 44, 11},    // House 4 (Cancer)
        {10, 44, 11},   // House 5 (Leo)
        {14, 44, 11},   // House 6 (Virgo)
        {14, 30, 11},   // House 7 (Libra)
        {14, 16, 11},   // House 8 (Scorpio)
        {14, 2, 11},    // House 9 (Sagittarius)
        {10, 2, 11},    // House 10 (Capricorn)
        {6, 2, 11},     // House 11 (Aquarius)
        {2, 2, 11}      // House 12 (Pisces)
    };

    for (int house = 1; house <= 12; house++) {
        std::string planets = getPlanetsInHouse(positions, house);
        if (!planets.empty()) {
            const auto& pos = housePositions[house];
            std::string formatted = formatHouseContent(planets, pos.width);

            if (pos.line < static_cast<int>(lines.size())) {
                std::string& line = lines[pos.line];
                if (pos.startCol + formatted.length() <= line.length()) {
                    line.replace(pos.startCol, formatted.length(), formatted);
                }
            }
        }
    }
}

void EasternChartDrawer::fillEastIndianHouses(std::vector<std::string>& lines,
                                             const std::vector<PlanetPosition>& positions) const {
    struct HousePosition {
        int line;
        int startCol;
        int width;
    };

    // East Indian chart house positions in 4x4 grid (House 1 at top-right, clockwise)
    std::vector<HousePosition> housePositions = {
        {0, 0, 0},      // Dummy for index 0
        {2, 44, 11},    // House 1
        {6, 44, 11},    // House 2
        {10, 44, 11},   // House 3
        {14, 44, 11},   // House 4
        {14, 30, 11},   // House 5
        {14, 16, 11},   // House 6
        {14, 2, 11},    // House 7
        {10, 2, 11},    // House 8
        {6, 2, 11},     // House 9
        {2, 2, 11},     // House 10
        {2, 16, 11},    // House 11
        {2, 30, 11}     // House 12
    };

    for (int house = 1; house <= 12; house++) {
        std::string planets = getPlanetsInHouse(positions, house);
        if (!planets.empty()) {
            const auto& pos = housePositions[house];
            std::string formatted = formatHouseContent(planets, pos.width);

            if (pos.line < static_cast<int>(lines.size())) {
                std::string& line = lines[pos.line];
                if (pos.startCol + formatted.length() <= line.length()) {
                    line.replace(pos.startCol, formatted.length(), formatted);
                }
            }
        }
    }
}

} // namespace Astro
