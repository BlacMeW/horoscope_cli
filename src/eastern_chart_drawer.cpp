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

    fillSouthIndianHouses(lines, positions, chart.getHouseCusps().ascendant);

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
        {3, 16, 11},    // House 1
        {3, 30, 11},    // House 2
        {3, 44, 11},    // House 3
        {7, 44, 11},    // House 4
        {11, 44, 11},   // House 5
        {15, 44, 11},   // House 6
        {15, 30, 11},   // House 7
        {15, 16, 11},   // House 8
        {15, 2, 11},    // House 9
        {11, 2, 11},    // House 10
        {7, 2, 11},     // House 11
        {3, 2, 11}      // House 12
    };

    for (int house = 1; house <= 12; house++) {
        const auto& pos = housePositions[house];

        // Indicate Ascendant in House 1 on the top line
        if (house == 1 && pos.line - 1 >= 0 && pos.line - 1 < static_cast<int>(lines.size())) {
            std::string ascStr = formatHouseContent("[ASC]", pos.width);
            lines[pos.line - 1].replace(pos.startCol, ascStr.length(), ascStr);
        }

        std::string planets = getPlanetsInHouse(positions, house);
        if (!planets.empty()) {
            std::string formatted = formatHouseContent(planets, pos.width);
            // Put planets on line below the house number so house number is not overwritten
            if (pos.line + 1 < static_cast<int>(lines.size())) {
                lines[pos.line + 1].replace(pos.startCol, formatted.length(), formatted);
            }
        }
    }
}

void EasternChartDrawer::fillSouthIndianHouses(std::vector<std::string>& lines,
                                              const std::vector<PlanetPosition>& positions,
                                              double ascendantLongitude) const {
    struct HousePosition {
        int line;
        int startCol;
        int width;
    };

    // South Indian chart fixed signs: 1 (Aries) to 12 (Pisces)
    std::vector<HousePosition> housePositions = {
        {0, 0, 0},      // Dummy for index 0
        {3, 16, 11},    // Sign 1 (Aries / Mesha)
        {3, 30, 11},    // Sign 2 (Taurus / Vrishabha)
        {3, 44, 11},    // Sign 3 (Gemini / Mithuna)
        {7, 44, 11},    // Sign 4 (Cancer / Karka)
        {11, 44, 11},   // Sign 5 (Leo / Simha)
        {15, 44, 11},   // Sign 6 (Virgo / Kanya)
        {15, 30, 11},   // Sign 7 (Libra / Tula)
        {15, 16, 11},   // Sign 8 (Scorpio / Vrishchika)
        {15, 2, 11},    // Sign 9 (Sagittarius / Dhanu)
        {11, 2, 11},    // Sign 10 (Capricorn / Makara)
        {7, 2, 11},     // Sign 11 (Aquarius / Kumbha)
        {3, 2, 11}      // Sign 12 (Pisces / Meena)
    };

    int ascSignNum = (static_cast<int>(ascendantLongitude) / 30) + 1;
    if (ascSignNum < 1) ascSignNum = 1;
    if (ascSignNum > 12) ascSignNum = 12;

    for (int signNum = 1; signNum <= 12; signNum++) {
        const auto& pos = housePositions[signNum];
        ZodiacSign sign = static_cast<ZodiacSign>(signNum - 1);

        // If this sign is the Ascendant, show [ASC] on line pos.line - 1
        if (signNum == ascSignNum && pos.line - 1 >= 0 && pos.line - 1 < static_cast<int>(lines.size())) {
            std::string ascStr = formatHouseContent("[ASC]", pos.width);
            lines[pos.line - 1].replace(pos.startCol, ascStr.length(), ascStr);
        }

        // Find all planets in this sign
        std::vector<std::string> planets;
        for (const auto& p : positions) {
            if (p.sign == sign) {
                planets.push_back(getShortPlanetName(p.planet));
            }
        }

        if (!planets.empty()) {
            std::string pStr;
            for (size_t i = 0; i < planets.size(); i++) {
                if (i > 0) pStr += ",";
                pStr += planets[i];
            }
            std::string formatted = formatHouseContent(pStr, pos.width);
            // Place planets on line below the sign label
            if (pos.line + 1 < static_cast<int>(lines.size())) {
                lines[pos.line + 1].replace(pos.startCol, formatted.length(), formatted);
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
        {3, 44, 11},    // House 1
        {7, 44, 11},    // House 2
        {11, 44, 11},   // House 3
        {15, 44, 11},   // House 4
        {15, 30, 11},   // House 5
        {15, 16, 11},   // House 6
        {15, 2, 11},    // House 7
        {11, 2, 11},    // House 8
        {7, 2, 11},     // House 9
        {3, 2, 11},     // House 10
        {3, 16, 11},    // House 11
        {3, 30, 11}     // House 12
    };

    for (int house = 1; house <= 12; house++) {
        const auto& pos = housePositions[house];

        if (house == 1 && pos.line - 1 >= 0 && pos.line - 1 < static_cast<int>(lines.size())) {
            std::string ascStr = formatHouseContent("[ASC]", pos.width);
            lines[pos.line - 1].replace(pos.startCol, ascStr.length(), ascStr);
        }

        std::string planets = getPlanetsInHouse(positions, house);
        if (!planets.empty()) {
            std::string formatted = formatHouseContent(planets, pos.width);
            if (pos.line + 1 < static_cast<int>(lines.size())) {
                lines[pos.line + 1].replace(pos.startCol, formatted.length(), formatted);
            }
        }
    }
}

} // namespace Astro
