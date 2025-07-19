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
        "┌─────────────┬─────────────┬─────────────┬─────────────┐",
        "│             │             │             │             │",
        "│     12      │      1      │      2      │      3      │",
        "│             │             │             │             │",
        "├─────────────┼─────────────┼─────────────┼─────────────┤",
        "│             │                           │             │",
        "│     11      │                           │      4      │",
        "│             │                           │             │",
        "├─────────────┤           RASI            ├─────────────┤",
        "│             │                           │             │",
        "│     10      │                           │      5      │",
        "│             │                           │             │",
        "├─────────────┼─────────────┼─────────────┼─────────────┤",
        "│             │             │             │             │",
        "│      9      │      8      │      7      │      6      │",
        "│             │             │             │             │",
        "└─────────────┴─────────────┴─────────────┴─────────────┘"
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

    // Create the chart frame - South Indian style is a square divided into 12 triangular sections
    std::vector<std::string> lines = {
        "        ┌─────────────┬─────────────┐        ",
        "        │      2      │      3      │        ",
        "        │             │             │        ",
        "┌───────┼─────────────┼─────────────┼───────┐",
        "│   1   │                           │   4   │",
        "│       │                           │       │",
        "├───────┤           RASI            ├───────┤",
        "│  12   │                           │   5   │",
        "│       │                           │       │",
        "└───────┼─────────────┼─────────────┼───────┘",
        "        │     11      │      6      │        ",
        "        │             │             │        ",
        "        └─────────────┴─────────────┘        ",
        "        ┌─────────────┬─────────────┐        ",
        "        │     10      │      7      │        ",
        "        │             │             │        ",
        "        └─────────────┴─────────────┘        ",
        "              ┌─────────────┐              ",
        "              │      8      │              ",
        "              │             │              ",
        "              └─────────────┘              ",
        "              ┌─────────────┐              ",
        "              │      9      │              ",
        "              │             │              ",
        "              └─────────────┘              "
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

    // Create the chart frame - East Indian style is a rectangular 3x4 grid format
    std::vector<std::string> lines = {
        "┌─────────┬─────────┬─────────┐",
        "│   12    │    1    │    2    │",
        "├─────────┼─────────┼─────────┤",
        "│   11    │         │    3    │",
        "├─────────┼─────────┼─────────┤",
        "│   10    │         │    4    │",
        "├─────────┼─────────┼─────────┤",
        "│    9    │    8    │    7    │",
        "└─────────┴─────────┴─────────┘"
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
    oss << "- Houses arranged in rectangular 3x4 grid\n";
    oss << "- House 1 (Ascendant) in top-center position\n";
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
    // North Indian chart house positions in the ASCII grid
    struct HousePosition {
        int line;
        int startCol;
        int width;
    };

    std::vector<HousePosition> housePositions = {
        {0, 0, 0},      // Dummy for index 0
        {2, 14, 11},    // House 1
        {2, 28, 11},    // House 2
        {2, 42, 11},    // House 3
        {6, 42, 11},    // House 4
        {10, 42, 11},   // House 5
        {14, 42, 11},   // House 6
        {14, 28, 11},   // House 7
        {14, 14, 11},   // House 8
        {14, 0, 11},    // House 9
        {10, 0, 11},    // House 10
        {6, 0, 11},     // House 11
        {2, 0, 11}      // House 12
    };

    for (int house = 1; house <= 12; house++) {
        std::string planets = getPlanetsInHouse(positions, house);
        if (!planets.empty()) {
            const auto& pos = housePositions[house];
            std::string formatted = formatHouseContent(planets, pos.width);

            // Replace the content in the appropriate line
            if (pos.line < static_cast<int>(lines.size())) {
                std::string& line = lines[pos.line];
                if (pos.startCol + formatted.length() <= line.length()) {
                    line.replace(pos.startCol + 2, formatted.length(), formatted);
                }
            }
        }
    }
}

void EasternChartDrawer::fillSouthIndianHouses(std::vector<std::string>& lines,
                                              const std::vector<PlanetPosition>& positions) const {
    // South Indian chart house positions in the ASCII grid
    struct HousePosition {
        int line;
        int startCol;
        int width;
    };

    std::vector<HousePosition> housePositions = {
        {0, 0, 0},      // Dummy for index 0
        {4, 2, 5},      // House 1
        {1, 10, 11},    // House 2
        {1, 24, 11},    // House 3
        {4, 40, 5},     // House 4
        {7, 40, 5},     // House 5
        {10, 24, 11},   // House 6
        {14, 24, 11},   // House 7
        {18, 16, 11},   // House 8
        {22, 16, 11},   // House 9
        {14, 10, 11},   // House 10
        {10, 10, 11},   // House 11
        {7, 2, 5}       // House 12
    };

    for (int house = 1; house <= 12; house++) {
        std::string planets = getPlanetsInHouse(positions, house);
        if (!planets.empty()) {
            const auto& pos = housePositions[house];
            std::string formatted = formatHouseContent(planets, pos.width);

            // Replace the content in the appropriate line
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
    // East Indian chart house positions in the 3x4 rectangular grid
    // Create a mapping of houses to their positions in the grid
    std::map<int, std::pair<int, int>> houseToLineCol = {
        {12, {1, 1}},  // House 12 (top-left)
        {1, {1, 11}},  // House 1 (top-center)
        {2, {1, 21}},  // House 2 (top-right)
        {11, {3, 1}},  // House 11 (middle-left)
        {3, {3, 21}},  // House 3 (middle-right)
        {10, {5, 1}},  // House 10 (not visible in this simplified format)
        {4, {5, 21}},  // House 4 (not visible in this simplified format)
        {9, {7, 1}},   // House 9 (bottom-left)
        {8, {7, 11}},  // House 8 (bottom-center)
        {7, {7, 21}}   // House 7 (bottom-right)
    };

    // Fill the houses with planets
    for (int house = 1; house <= 12; house++) {
        std::string planets = getPlanetsInHouse(positions, house);
        if (!planets.empty()) {
            auto it = houseToLineCol.find(house);
            if (it != houseToLineCol.end()) {
                int lineIdx = it->second.first;
                int colStart = it->second.second;

                if (lineIdx < static_cast<int>(lines.size())) {
                    // Format content to fit in the cell (7 characters wide)
                    std::string content = planets;
                    if (content.length() > 7) {
                        content = content.substr(0, 7);
                    }

                    // Center the content
                    int padding = (7 - content.length()) / 2;
                    std::string formatted = std::string(padding, ' ') + content +
                                          std::string(7 - padding - content.length(), ' ');

                    // Replace in the line
                    if (colStart + 7 <= lines[lineIdx].length()) {
                        lines[lineIdx].replace(colStart, 7, formatted);
                    }
                }
            }
        }
    }
}

} // namespace Astro
