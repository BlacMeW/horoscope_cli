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
    const int wheelWidth = 63;
    const int wheelHeight = 29;
    const int centerX = wheelWidth / 2;
    const int centerY = wheelHeight / 2;

    // Create empty wheel canvas with proper terminal aspect ratio
    std::vector<std::string> wheel(wheelHeight, std::string(wheelWidth, ' '));

    // Draw the basic circular wheel structure
    std::string frame = drawWheelFrame(wheelWidth);
    std::istringstream frameStream(frame);
    std::string line;
    int lineIndex = 0;
    while (std::getline(frameStream, line) && lineIndex < wheelHeight) {
        if (line.length() > static_cast<size_t>(wheelWidth)) {
            wheel[lineIndex] = line.substr(0, wheelWidth);
        } else {
            wheel[lineIndex] = line;
            if (wheel[lineIndex].length() < static_cast<size_t>(wheelWidth)) {
                wheel[lineIndex].resize(wheelWidth, ' ');
            }
        }
        lineIndex++;
    }

    // Fill wheel with chart data
    const auto& positions = chart.getPlanetPositions();
    const auto& cusps = chart.getHouseCusps();

    fillWheelWithPlanets(wheel, positions, cusps, centerX, centerY);
    drawHouseCusps(wheel, cusps, centerX, centerY, 24);

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

static std::string getPlanetAbbr(Planet planet) {
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
        case Planet::NORTH_NODE: return "Ra";
        case Planet::SOUTH_NODE: return "Ke";
        case Planet::CHIRON: return "Ch";
        case Planet::LILITH: return "Li";
        default: return "??";
    }
}

std::string WesternChartDrawer::drawRectangularChart(const BirthChart& chart) const {
    std::stringstream ss;
    ss << "\n=== WESTERN CHART - RECTANGULAR LAYOUT ===\n\n";

    const auto& positions = chart.getPlanetPositions();
    const auto& cusps = chart.getHouseCusps();

    // Create house grid (3x4 layout) using clean ASCII borders
    std::vector<std::string> lines = {
        "+--------------+--------------+--------------+--------------+",
        "|   House 10   |   House 11   |   House 12   |   House 1    |",
        "|              |              |              |              |",
        "|              |              |              |              |",
        "+--------------+--------------+--------------+--------------+",
        "|   House 9    |                             |   House 2    |",
        "|              |         CHART WHEEL         |              |",
        "|              |                             |              |",
        "+--------------+                             +--------------+",
        "|   House 8    |                             |   House 3    |",
        "|              |                             |              |",
        "|              |                             |              |",
        "+--------------+--------------+--------------+--------------+",
        "|   House 7    |   House 6    |   House 5    |   House 4    |",
        "|              |              |              |              |",
        "|              |              |              |              |",
        "+--------------+--------------+--------------+--------------+"
    };

    struct HousePos { int line; int col; int width; };
    std::vector<HousePos> housePositions = {
        {0, 0, 0},         // Dummy for index 0
        {1, 46, 14},       // House 1
        {5, 46, 14},       // House 2
        {9, 46, 14},       // House 3
        {13, 46, 14},      // House 4
        {13, 31, 14},      // House 5
        {13, 16, 14},      // House 6
        {13, 1, 14},       // House 7
        {9, 1, 14},        // House 8
        {5, 1, 14},        // House 9
        {1, 1, 14},        // House 10
        {1, 16, 14},       // House 11
        {1, 31, 14}        // House 12
    };

    // Fill houses with planets
    for (int house = 1; house <= 12; house++) {
        std::vector<std::string> planetEntries;
        for (const auto& pos : positions) {
            if (pos.house == house) {
                std::string entry = getPlanetAbbr(pos.planet);
                if (showDegrees) {
                    int deg = static_cast<int>(pos.longitude) % 30;
                    int min = static_cast<int>((pos.longitude - static_cast<int>(pos.longitude)) * 60) % 60;
                    char buf[16];
                    snprintf(buf, sizeof(buf), " %02d:%02d", deg, min);
                    entry += buf;
                }
                planetEntries.push_back(entry);
            }
        }

        if (!planetEntries.empty()) {
            const auto& hp = housePositions[house];
            if (planetEntries.size() == 1) {
                // Single planet: center on line + 2
                std::string text = planetEntries[0];
                int pad = (hp.width - static_cast<int>(text.length())) / 2;
                if (pad < 0) pad = 0;
                std::string formatted = std::string(pad, ' ') + text;
                while (formatted.length() < static_cast<size_t>(hp.width)) formatted += ' ';
                lines[hp.line + 2].replace(hp.col, hp.width, formatted);
            } else if (planetEntries.size() == 2) {
                // Two planets: one on line + 1, one on line + 2
                for (size_t p = 0; p < 2; ++p) {
                    std::string text = planetEntries[p];
                    int pad = (hp.width - static_cast<int>(text.length())) / 2;
                    if (pad < 0) pad = 0;
                    std::string formatted = std::string(pad, ' ') + text;
                    while (formatted.length() < static_cast<size_t>(hp.width)) formatted += ' ';
                    lines[hp.line + 1 + p].replace(hp.col, hp.width, formatted);
                }
            } else {
                // 3 or more planets: show comma-separated abbreviations
                std::string abbrs;
                for (size_t p = 0; p < planetEntries.size(); ++p) {
                    if (p > 0) abbrs += ",";
                    abbrs += planetEntries[p].substr(0, 2);
                }
                int pad = (hp.width - static_cast<int>(abbrs.length())) / 2;
                if (pad < 0) pad = 0;
                std::string formatted = std::string(pad, ' ') + abbrs;
                while (formatted.length() < static_cast<size_t>(hp.width)) formatted += ' ';
                lines[hp.line + 2].replace(hp.col, hp.width, formatted);
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

    // Filter main planets
    std::vector<PlanetPosition> mainPlanets;
    for (const auto& pos : positions) {
        if (pos.planet == Planet::NORTH_NODE || pos.planet == Planet::SOUTH_NODE ||
            pos.planet == Planet::CHIRON || pos.planet == Planet::LILITH) continue;
        mainPlanets.push_back(pos);
    }

    // Create grid header (5 visual columns per planet)
    ss << "     ";
    for (const auto& pos : mainPlanets) {
        ss << "  " << getPlanetGlyph(pos.planet) << "  ";
    }
    ss << "\n";

    // Create grid rows
    for (const auto& pos1 : mainPlanets) {
        ss << " " << getPlanetGlyph(pos1.planet) << ": ";

        for (const auto& pos2 : mainPlanets) {
            if (pos1.planet == pos2.planet) {
                ss << "  -  ";
                continue;
            }

            std::string cell = "     ";
            // Find aspect between these planets
            for (const auto& aspect : aspects) {
                if ((aspect.planet1 == pos1.planet && aspect.planet2 == pos2.planet) ||
                    (aspect.planet1 == pos2.planet && aspect.planet2 == pos1.planet)) {
                    int orbInt = static_cast<int>(std::round(aspect.orb));
                    std::string glyph = getAspectGlyph(aspect.type);
                    if (orbInt >= 10) {
                        cell = glyph + std::to_string(orbInt) + "° ";
                    } else {
                        cell = " " + glyph + std::to_string(orbInt) + "° ";
                    }
                    break;
                }
            }

            ss << cell;
        }
        ss << "\n";
    }

    return ss.str();
}

std::string WesternChartDrawer::drawWheelFrame(int size) const {
    std::stringstream ss;
    int width = size;
    int height = (size * 29) / 63; // Monospace aspect ratio (2:1 char cell)
    double centerX = width / 2.0;
    double centerY = height / 2.0;

    double radiusX = (width / 2.0) - 2.0;
    double radiusY = (height / 2.0) - 1.5;
    double innerRadiusX = radiusX * 0.70;
    double innerRadiusY = radiusY * 0.70;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double dx = x - centerX;
            double dy = y - centerY;

            double normOuter = (dx * dx) / (radiusX * radiusX) + (dy * dy) / (radiusY * radiusY);
            double normInner = (dx * dx) / (innerRadiusX * innerRadiusX) + (dy * dy) / (innerRadiusY * innerRadiusY);

            if (normInner < 0.88) {
                ss << " ";
            } else if (normInner <= 1.08) {
                ss << ".";
            } else if (normOuter <= 0.88) {
                ss << " ";
            } else if (normOuter <= 1.08) {
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

        if (x >= 0 && x + 1 < static_cast<int>(wheel[0].length()) &&
            y >= 0 && y < static_cast<int>(wheel.size())) {
            std::string abbr = getPlanetAbbr(pos.planet);
            wheel[y][x] = abbr[0];
            if (abbr.length() > 1) {
                wheel[y][x + 1] = abbr[1];
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
            } else {
                wheel[ny][nx] = '1';
                if (nx + 1 < static_cast<int>(wheel[0].length())) {
                    wheel[ny][nx + 1] = '0' + (house - 10);
                }
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
    // Aspect ratio compensation for monospace terminal characters (width:height ~ 1:2.1)
    double rx = radius * 1.08;
    double ry = radius * 0.50;
    int x = centerX + static_cast<int>(std::round(rx * cos(angle)));
    int y = centerY + static_cast<int>(std::round(ry * sin(angle)));
    return {x, y};
}

} // namespace Astro
