#pragma once

#include "birth_chart.h"
#include "astro_types.h"
#include <string>
#include <vector>
#include <map>

namespace Astro {

// Perspective for viewing the solar system
enum class SolarSystemPerspective {
    HELIOCENTRIC,  // Sun-centered (default)
    GEOCENTRIC,    // Earth-centered
    MARS_CENTRIC,  // Mars-centered
    JUPITER_CENTRIC, // Jupiter-centered
    SATURN_CENTRIC   // Saturn-centered
};

class SolarSystemDrawer {
public:
    SolarSystemDrawer();

    // Draw solar system in ASCII art with planet positions
    std::string drawSolarSystem(const BirthChart& chart) const;

    // Draw simplified solar system with just orbital paths
    std::string drawOrbitalPaths() const;

    // Draw solar system with planet positions at specific date
    std::string drawSolarSystemWithPositions(const std::vector<PlanetPosition>& positions) const;

    // Draw solar system from different planetary perspectives
    std::string drawGeocentricView(const std::vector<PlanetPosition>& positions) const;
    std::string drawPlanetCentricView(const std::vector<PlanetPosition>& positions, Planet centerPlanet) const;

    // Set drawing options
    void setShowOrbits(bool show) { showOrbits = show; }
    void setShowPlanetNames(bool show) { showPlanetNames = show; }
    void setShowDistances(bool show) { showDistances = show; }
    void setScale(double scale) { this->scale = scale; }
    void setPerspective(SolarSystemPerspective perspective) { this->perspective = perspective; }

private:
    bool showOrbits;
    bool showPlanetNames;
    bool showDistances;
    double scale;
    SolarSystemPerspective perspective;

    // Helper methods
    std::string getPlanetSymbol(Planet planet) const;
    std::string getPlanetName(Planet planet) const;
    int getOrbitRadius(Planet planet) const;
    std::pair<int, int> calculatePlanetPosition(const PlanetPosition& position, int centerX, int centerY) const;
    std::pair<int, int> calculateRelativePosition(const PlanetPosition& position, const PlanetPosition& centerPosition, int centerX, int centerY) const;
    std::string createEmptyCanvas(int width, int height) const;
    void placePlanetOnCanvas(std::vector<std::string>& canvas, int x, int y, Planet planet) const;
    void drawOrbitOnCanvas(std::vector<std::string>& canvas, int centerX, int centerY, int radius) const;
    std::string addLegend(const std::string& canvas) const;
    std::string getPerspectiveName(SolarSystemPerspective perspective) const;
    PlanetPosition findPlanetPosition(const std::vector<PlanetPosition>& positions, Planet planet) const;

    // Solar system constants (relative distances, not to scale)
    static const std::map<Planet, int> ORBITAL_RADII;
    static const std::map<Planet, std::string> PLANET_SYMBOLS;
};

} // namespace Astro
