#include "western_chart_drawer.h"
#include <sstream>
#include <cmath>
#include <algorithm>
#include <iomanip>

namespace Astro {

// Planet glyphs for ASCII art
const std::map<Planet, std::string> WesternChartDrawer::PLANET_GLYPHS = {
    {Planet::SUN, "☉"},
    {Planet::MOON, "☽"},
    {Planet::MERCURY, "☿"},
    {Planet::VENUS, "♀"},
    {Planet::MARS, "♂"},
    {Planet::JUPITER, "♃"},
    {Planet::SATURN, "♄"},
    {Planet::URANUS, "♅"},
    {Planet::NEPTUNE, "♆"},
    {Planet::PLUTO, "♇"},
    {Planet::NORTH_NODE, "☊"},
    {Planet::SOUTH_NODE, "☋"},
    {Planet::CHIRON, "⚷"},
    {Planet::LILITH, "⚸"}
};

// Zodiac sign glyphs
const std::map<ZodiacSign, std::string> WesternChartDrawer::SIGN_GLYPHS = {
    {ZodiacSign::ARIES, "♈"},
    {ZodiacSign::TAURUS, "♉"},
    {ZodiacSign::GEMINI, "♊"},
    {ZodiacSign::CANCER, "♋"},
    {ZodiacSign::LEO, "♌"},
    {ZodiacSign::VIRGO, "♍"},
    {ZodiacSign::LIBRA, "♎"},
    {ZodiacSign::SCORPIO, "♏"},
    {ZodiacSign::SAGITTARIUS, "♐"},
    {ZodiacSign::CAPRICORN, "♑"},
    {ZodiacSign::AQUARIUS, "♒"},
    {ZodiacSign::PISCES, "♓"}
};

// Aspect glyphs
const std::map<AspectType, std::string> WesternChartDrawer::ASPECT_GLYPHS = {
    {AspectType::CONJUNCTION, "☌"},
    {AspectType::SEXTILE, "⚹"},
    {AspectType::SQUARE, "□"},
    {AspectType::TRINE, "△"},
    {AspectType::OPPOSITION, "☍"}
};

WesternChartDrawer::WesternChartDrawer()
    : showAspects(true), showDegrees(true), compactMode(false) {
}

std::string WesternChartDrawer::drawChartWheel(const BirthChart& chart) const {
    const int wheelSize = 61; // Must be odd for center
    const int centerX = wheelSize / 2;
    const int centerY = wheelSize / 2;

    // Create empty wheel
    std::vector<std::string> wheel(wheelSize, std::string(wheelSize, ' '));

    // Draw the basic wheel structure
    std::string frame = drawWheelFrame(wheelSize);
    std::istringstream frameStream(frame);
    std::string line;
    int lineIndex = 0;
    while (std::getline(frameStream, line) && lineIndex < wheelSize) {
        if (line.length() > static_cast<size_t>(wheelSize)) {
            wheel[lineIndex] = line.substr(0, wheelSize);
        } else {
            wheel[lineIndex] = line;
            if (wheel[lineIndex].length() < static_cast<size_t>(wheelSize)) {
                wheel[lineIndex].resize(wheelSize, ' ');
            }
        }
        lineIndex++;
    }

    // Fill wheel with chart data
    const auto& positions = chart.getPlanetPositions();
    const auto& cusps = chart.getHouseCusps();

    fillWheelWithPlanets(wheel, positions, cusps, centerX, centerY);
    drawHouseCusps(wheel, cusps, centerX, centerY, 25);

    if (showAspects) {
        drawAspectLines(wheel, chart.getAspects(), positions, cusps, centerX, centerY);
    }

    // Convert wheel to string
    std::stringstream ss;
    ss << "\n=== WESTERN CHART WHEEL ===\n\n";
    for (const auto& row : wheel) {
        ss << row << "\n";
    }

    // Add legend
    ss << "\nChart Symbols:\n";
    ss << "─────────────\n";
    ss << "☉ Sun      ☽ Moon     ☿ Mercury  ♀ Venus    ♂ Mars     ♃ Jupiter\n";
    ss << "♄ Saturn   ♅ Uranus   ♆ Neptune  ♇ Pluto    ☊ N.Node   ☋ S.Node\n";
    if (showAspects) {
        ss << "\nAspects:\n";
        ss << "☌ Conjunction  ⚹ Sextile  □ Square  △ Trine  ☍ Opposition\n";
    }
    ss << "\n";

    return ss.str();
}

std::string WesternChartDrawer::drawRectangularChart(const BirthChart& chart) const {
    std::stringstream ss;
    ss << "\n=== WESTERN CHART - RECTANGULAR LAYOUT ===\n\n";

    const auto& positions = chart.getPlanetPositions();
    const auto& cusps = chart.getHouseCusps();

    // Create house grid (3x4 layout)
    std::vector<std::string> lines = {
        "┌──────────────┬──────────────┬──────────────┬──────────────┐",
        "│   House 10   │   House 11   │   House 12   │   House 1    │",
        "│              │              │              │              │",
        "│              │              │              │              │",
        "├──────────────┼──────────────┼──────────────┼──────────────┤",
        "│   House 9    │                            │   House 2    │",
        "│              │         CHART WHEEL         │              │",
        "│              │                            │              │",
        "├──────────────┤                            ├──────────────┤",
        "│   House 8    │                            │   House 3    │",
        "│              │                            │              │",
        "│              │                            │              │",
        "├──────────────┼──────────────┼──────────────┼──────────────┤",
        "│   House 7    │   House 6    │   House 5    │   House 4    │",
        "│              │              │              │              │",
        "│              │              │              │              │",
        "└──────────────┴──────────────┴──────────────┴──────────────┘"
    };

    // House positions in the grid
    struct HousePos { int line; int col; int width; };
    std::vector<HousePos> housePositions = {
        {0, 0, 0},         // Dummy for index 0
        {1, 46, 12},       // House 1
        {5, 46, 12},       // House 2
        {9, 46, 12},       // House 3
        {13, 46, 12},      // House 4
        {13, 32, 12},      // House 5
        {13, 16, 12},      // House 6
        {13, 2, 12},       // House 7
        {9, 2, 12},        // House 8
        {5, 2, 12},        // House 9
        {1, 2, 12},        // House 10
        {1, 16, 12},       // House 11
        {1, 32, 12}        // House 12
    };

    // Fill houses with planets
    for (int house = 1; house <= 12; house++) {
        std::vector<std::string> planetsInHouse;

        for (const auto& pos : positions) {
            if (pos.house == house) {
                std::string planetStr = getPlanetGlyph(pos.planet);
                if (showDegrees) {
                    planetStr += formatDegreeWithSign(pos.longitude);
                }
                planetsInHouse.push_back(planetStr);
            }
        }

        if (!planetsInHouse.empty()) {
            const auto& housePos = housePositions[house];

            // Format planets for display
            std::string planetsStr;
            for (size_t i = 0; i < planetsInHouse.size(); i++) {
                if (i > 0) planetsStr += " ";
                planetsStr += planetsInHouse[i];
            }

            // Truncate if too long
            if (planetsStr.length() > static_cast<size_t>(housePos.width)) {
                planetsStr = planetsStr.substr(0, housePos.width);
            }

            // Center the text
            int padding = (housePos.width - planetsStr.length()) / 2;
            std::string formatted = std::string(padding, ' ') + planetsStr +
                                  std::string(housePos.width - padding - planetsStr.length(), ' ');

            // Place in grid
            if (housePos.line + 2 < static_cast<int>(lines.size()) &&
                housePos.col + formatted.length() <= lines[housePos.line + 2].length()) {
                lines[housePos.line + 2].replace(housePos.col, formatted.length(), formatted);
            }
        }
    }

    // Output the chart
    for (const auto& line : lines) {
        ss << line << "\n";
    }

    // Add legend
    ss << "\nHouse System Information:\n";
    ss << "Ascendant (AC): " << formatDegreeWithSign(cusps.ascendant) << " in "
       << zodiacSignToString(longitudeToSign(cusps.ascendant)) << "\n";
    ss << "Midheaven (MC): " << formatDegreeWithSign(cusps.midheaven) << " in "
       << zodiacSignToString(longitudeToSign(cusps.midheaven)) << "\n\n";

    return ss.str();
}

std::string WesternChartDrawer::drawAspectGrid(const BirthChart& chart) const {
    std::stringstream ss;
    ss << "\n=== ASPECT GRID ===\n\n";

    const auto& positions = chart.getPlanetPositions();
    const auto& aspects = chart.getAspects();

    // Create grid header
    ss << "        ";
    for (const auto& pos : positions) {
        if (pos.planet == Planet::NORTH_NODE || pos.planet == Planet::SOUTH_NODE ||
            pos.planet == Planet::CHIRON || pos.planet == Planet::LILITH) continue;
        ss << std::left << std::setw(4) << getPlanetGlyph(pos.planet);
    }
    ss << "\n";

    // Create grid rows
    for (const auto& pos1 : positions) {
        if (pos1.planet == Planet::NORTH_NODE || pos1.planet == Planet::SOUTH_NODE ||
            pos1.planet == Planet::CHIRON || pos1.planet == Planet::LILITH) continue;

        ss << std::left << std::setw(8) << (getPlanetGlyph(pos1.planet) + ":");

        for (const auto& pos2 : positions) {
            if (pos2.planet == Planet::NORTH_NODE || pos2.planet == Planet::SOUTH_NODE ||
                pos2.planet == Planet::CHIRON || pos2.planet == Planet::LILITH) continue;

            std::string aspectSymbol = "    ";

            if (pos1.planet != pos2.planet) {
                // Find aspect between these planets
                for (const auto& aspect : aspects) {
                    if ((aspect.planet1 == pos1.planet && aspect.planet2 == pos2.planet) ||
                        (aspect.planet1 == pos2.planet && aspect.planet2 == pos1.planet)) {
                        aspectSymbol = getAspectGlyph(aspect.type) +
                                     std::to_string(static_cast<int>(aspect.orb)) + "°";
                        if (aspectSymbol.length() > 4) aspectSymbol = aspectSymbol.substr(0, 4);
                        break;
                    }
                }
            }

            ss << std::left << std::setw(4) << aspectSymbol;
        }
        ss << "\n";
    }

    return ss.str();
}

std::string WesternChartDrawer::drawWheelFrame(int size) const {
    std::stringstream ss;
    int center = size / 2;
    int outerRadius = center - 1;
    int innerRadius = center - 8;

    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            int dx = x - center;
            int dy = y - center;
            double distance = sqrt(dx * dx + dy * dy);

            if (distance <= innerRadius) {
                ss << " ";
            } else if (distance <= innerRadius + 1) {
                ss << "·";
            } else if (distance <= outerRadius - 1) {
                ss << " ";
            } else if (distance <= outerRadius) {
                ss << "*";
            } else {
                ss << " ";
            }
        }
        ss << "\n";
    }

    return ss.str();
}

std::string WesternChartDrawer::getPlanetGlyph(Planet planet) const {
    auto it = PLANET_GLYPHS.find(planet);
    return (it != PLANET_GLYPHS.end()) ? it->second : "?";
}

std::string WesternChartDrawer::getSignGlyph(ZodiacSign sign) const {
    auto it = SIGN_GLYPHS.find(sign);
    return (it != SIGN_GLYPHS.end()) ? it->second : "?";
}

std::string WesternChartDrawer::getAspectGlyph(AspectType aspect) const {
    auto it = ASPECT_GLYPHS.find(aspect);
    return (it != ASPECT_GLYPHS.end()) ? it->second : "?";
}

std::string WesternChartDrawer::formatHouseNumber(int house) const {
    std::string suffix;
    if (house == 1) suffix = "st";
    else if (house == 2) suffix = "nd";
    else if (house == 3) suffix = "rd";
    else suffix = "th";

    return std::to_string(house) + suffix;
}

std::string WesternChartDrawer::formatDegreeWithSign(double longitude) const {
    int degrees = static_cast<int>(longitude) % 30;
    int minutes = static_cast<int>((longitude - static_cast<int>(longitude)) * 60) % 60;
    ZodiacSign sign = longitudeToSign(longitude);

    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << degrees << "°" << std::setw(2) << minutes << "'";
    oss << getSignGlyph(sign);
    return oss.str();
}

void WesternChartDrawer::fillWheelWithPlanets(std::vector<std::string>& wheel,
                                            const std::vector<PlanetPosition>& positions,
                                            const HouseCusps& cusps, int centerX, int centerY) const {
    const int planetRadius = 20;

    for (const auto& pos : positions) {
        // Skip some less important points for cleaner display
        if (pos.planet == Planet::NORTH_NODE || pos.planet == Planet::SOUTH_NODE ||
            pos.planet == Planet::CHIRON || pos.planet == Planet::LILITH) continue;

        // Calculate position on wheel
        double angle = pos.longitude * M_PI / 180.0;
        angle = angle - M_PI / 2.0; // Adjust so 0° is at top (like traditional charts)

        auto wheelPos = getWheelPosition(centerX, centerY, angle, planetRadius);
        int x = wheelPos.first;
        int y = wheelPos.second;

        if (x >= 0 && x < static_cast<int>(wheel[0].length()) &&
            y >= 0 && y < static_cast<int>(wheel.size())) {
            std::string glyph = getPlanetGlyph(pos.planet);
            if (!glyph.empty() && glyph != "?") {
                wheel[y][x] = glyph[0]; // Use first character if multi-byte
            }
        }
    }
}

void WesternChartDrawer::drawHouseCusps(std::vector<std::string>& wheel, const HouseCusps& cusps,
                                      int centerX, int centerY, int radius) const {
    // Draw house cusp lines
    for (int house = 1; house <= 12; house++) {
        double cuspLongitude = cusps.cusps[house];
        double angle = cuspLongitude * M_PI / 180.0 - M_PI / 2.0;

        // Draw line from center to edge
        for (int r = 10; r <= radius; r += 2) {
            auto pos = getWheelPosition(centerX, centerY, angle, r);
            int x = pos.first;
            int y = pos.second;

            if (x >= 0 && x < static_cast<int>(wheel[0].length()) &&
                y >= 0 && y < static_cast<int>(wheel.size()) &&
                wheel[y][x] == ' ') {
                wheel[y][x] = '|';
            }
        }

        // Place house number
        auto numberPos = getWheelPosition(centerX, centerY, angle, radius - 3);
        int nx = numberPos.first;
        int ny = numberPos.second;

        if (nx >= 0 && nx < static_cast<int>(wheel[0].length()) &&
            ny >= 0 && ny < static_cast<int>(wheel.size())) {
            if (house < 10) {
                wheel[ny][nx] = '0' + house;
            }
        }
    }
}

void WesternChartDrawer::drawAspectLines(std::vector<std::string>& wheel, const std::vector<Aspect>& aspects,
                                       const std::vector<PlanetPosition>& positions, const HouseCusps& cusps,
                                       int centerX, int centerY) const {
    // This is complex to implement properly in ASCII, so we'll skip aspect lines for now
    // In a full implementation, you'd draw lines between aspecting planets
}

std::pair<int, int> WesternChartDrawer::getWheelPosition(int centerX, int centerY, double angle, int radius) const {
    int x = centerX + static_cast<int>(radius * cos(angle));
    int y = centerY + static_cast<int>(radius * sin(angle));
    return {x, y};
}

} // namespace Astro
