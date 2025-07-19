#include "solar_system_drawer.h"
#include <sstream>
#include <cmath>
#include <algorithm>
#include <iomanip>

namespace Astro {

// Orbital radii for ASCII representation (not to scale, for visual clarity)
const std::map<Planet, int> SolarSystemDrawer::ORBITAL_RADII = {
    {Planet::MERCURY, 3},
    {Planet::VENUS, 5},
    {Planet::MARS, 9},
    {Planet::JUPITER, 13},
    {Planet::SATURN, 17},
    {Planet::URANUS, 21},
    {Planet::NEPTUNE, 25},
    {Planet::PLUTO, 29}
};

// Planet symbols for ASCII art
const std::map<Planet, std::string> SolarSystemDrawer::PLANET_SYMBOLS = {
    {Planet::SUN, "O"},
    {Planet::MOON, "D"},
    {Planet::MERCURY, "H"},
    {Planet::VENUS, "V"},
    {Planet::MARS, "M"},
    {Planet::JUPITER, "J"},
    {Planet::SATURN, "S"},
    {Planet::URANUS, "U"},
    {Planet::NEPTUNE, "N"},
    {Planet::PLUTO, "P"}
};

SolarSystemDrawer::SolarSystemDrawer()
    : showOrbits(true), showPlanetNames(true), showDistances(false), scale(1.0) {
}

std::string SolarSystemDrawer::drawSolarSystem(const BirthChart& chart) const {
    return drawSolarSystemWithPositions(chart.getPlanetPositions());
}

std::string SolarSystemDrawer::drawOrbitalPaths() const {
    const int width = 80;
    const int height = 60;
    const int centerX = width / 2;
    const int centerY = height / 2;

    // Create empty canvas
    std::vector<std::string> canvas(height, std::string(width, ' '));

    // Draw orbital paths
    if (showOrbits) {
        for (const auto& pair : ORBITAL_RADII) {
            int radius = static_cast<int>(pair.second * scale);
            if (radius < height/2 && radius < width/2) {
                drawOrbitOnCanvas(canvas, centerX, centerY, radius);
            }
        }
    }

    // Place Sun at center
    canvas[centerY][centerX] = 'O';

    // Convert canvas to string
    std::stringstream ss;
    ss << "\n┌─ Solar System Orbital Paths ─┐\n";
    for (const auto& row : canvas) {
        ss << "│" << row << "│\n";
    }
    ss << "└─────────────────────────────────┘\n\n";

    if (showPlanetNames) {
        ss << addLegend("");
    }

    return ss.str();
}

std::string SolarSystemDrawer::drawSolarSystemWithPositions(const std::vector<PlanetPosition>& positions) const {
    const int width = 80;
    const int height = 60;
    const int centerX = width / 2;
    const int centerY = height / 2;

    // Create empty canvas
    std::vector<std::string> canvas(height, std::string(width, ' '));

    // Draw orbital paths first
    if (showOrbits) {
        for (const auto& pair : ORBITAL_RADII) {
            int radius = static_cast<int>(pair.second * scale);
            if (radius < height/2 && radius < width/2) {
                drawOrbitOnCanvas(canvas, centerX, centerY, radius);
            }
        }
    }

    // Place Sun at center
    canvas[centerY][centerX] = 'O';

    // Place planets based on their positions
    for (const auto& pos : positions) {
        if (pos.planet == Planet::SUN) continue; // Sun is already at center

        auto planetPos = calculatePlanetPosition(pos, centerX, centerY);
        int x = planetPos.first;
        int y = planetPos.second;

        if (x >= 0 && x < width && y >= 0 && y < height) {
            placePlanetOnCanvas(canvas, x, y, pos.planet);
        }
    }

    // Convert canvas to string
    std::stringstream ss;
    ss << "\n┌─ Solar System Current Positions ─┐\n";
    for (const auto& row : canvas) {
        ss << "│" << row << "│\n";
    }
    ss << "└──────────────────────────────────┘\n\n";

    if (showPlanetNames) {
        ss << addLegend("");
    }

    // Add position details if requested
    if (showDistances) {
        ss << "\nPlanet Positions:\n";
        ss << "─────────────────\n";
        for (const auto& pos : positions) {
            if (pos.planet == Planet::MOON || pos.planet == Planet::NORTH_NODE ||
                pos.planet == Planet::SOUTH_NODE || pos.planet == Planet::CHIRON ||
                pos.planet == Planet::LILITH) continue;

            ss << std::left << std::setw(10) << getPlanetName(pos.planet)
               << " " << getPlanetSymbol(pos.planet)
               << "  " << pos.getFormattedPosition() << "\n";
        }
    }

    return ss.str();
}

std::string SolarSystemDrawer::getPlanetSymbol(Planet planet) const {
    auto it = PLANET_SYMBOLS.find(planet);
    if (it != PLANET_SYMBOLS.end()) {
        return it->second;
    }
    return "•";
}

std::string SolarSystemDrawer::getPlanetName(Planet planet) const {
    return planetToString(planet);
}

int SolarSystemDrawer::getOrbitRadius(Planet planet) const {
    auto it = ORBITAL_RADII.find(planet);
    if (it != ORBITAL_RADII.end()) {
        return static_cast<int>(it->second * scale);
    }
    return 5; // Default radius
}

std::pair<int, int> SolarSystemDrawer::calculatePlanetPosition(const PlanetPosition& position, int centerX, int centerY) const {
    Planet planet = position.planet;

    // Skip Moon and nodes as they're Earth-centric
    if (planet == Planet::MOON || planet == Planet::NORTH_NODE ||
        planet == Planet::SOUTH_NODE || planet == Planet::CHIRON ||
        planet == Planet::LILITH) {
        return {-1, -1}; // Invalid position
    }

    int radius = getOrbitRadius(planet);

    // Convert longitude to radians (0° = right, 90° = up)
    double angleRad = (position.longitude * M_PI / 180.0) - (M_PI / 2.0);

    // Calculate position on orbit
    int x = centerX + static_cast<int>(radius * cos(angleRad));
    int y = centerY + static_cast<int>(radius * sin(angleRad));

    return {x, y};
}

void SolarSystemDrawer::placePlanetOnCanvas(std::vector<std::string>& canvas, int x, int y, Planet planet) const {
    if (y >= 0 && y < static_cast<int>(canvas.size()) &&
        x >= 0 && x < static_cast<int>(canvas[y].length())) {

        std::string symbol = getPlanetSymbol(planet);
        if (!symbol.empty()) {
            canvas[y][x] = symbol[0]; // Use first character of symbol
        }
    }
}

void SolarSystemDrawer::drawOrbitOnCanvas(std::vector<std::string>& canvas, int centerX, int centerY, int radius) const {
    int height = canvas.size();
    int width = canvas[0].length();

    // Draw orbit as a circle using ASCII characters
    for (int angle = 0; angle < 360; angle += 5) {
        double angleRad = angle * M_PI / 180.0;
        int x = centerX + static_cast<int>(radius * cos(angleRad));
        int y = centerY + static_cast<int>(radius * sin(angleRad));

        if (x >= 0 && x < width && y >= 0 && y < height) {
            if (canvas[y][x] == ' ') { // Don't overwrite planets
                canvas[y][x] = '.';
            }
        }
    }
}

std::string SolarSystemDrawer::addLegend(const std::string& /* canvas */) const {
    std::stringstream ss;
    ss << "Planet Symbols:\n";
    ss << "──────────────\n";

    std::vector<Planet> planets = {
        Planet::SUN, Planet::MERCURY, Planet::VENUS, Planet::MARS,
        Planet::JUPITER, Planet::SATURN, Planet::URANUS, Planet::NEPTUNE, Planet::PLUTO
    };

    for (Planet planet : planets) {
        ss << getPlanetSymbol(planet) << " " << std::left << std::setw(10) << getPlanetName(planet) << "  ";
        if (planet == Planet::MARS || planet == Planet::PLUTO) ss << "\n";
    }
    ss << "\n\n";

    if (showOrbits) {
        ss << ". = Orbital paths\n";
        ss << "O = Sun (center)\n\n";
    }

    return ss.str();
}

} // namespace Astro
