#pragma once

#include "birth_chart.h"
#include "astro_types.h"
#include <string>
#include <vector>
#include <map>

namespace Astro {

class WesternChartDrawer {
public:
    WesternChartDrawer();

    // Draw traditional Western chart wheel in ASCII
    std::string drawChartWheel(const BirthChart& chart) const;

    // Draw simplified rectangular chart
    std::string drawRectangularChart(const BirthChart& chart) const;

    // Draw aspect grid
    std::string drawAspectGrid(const BirthChart& chart) const;

    // Set drawing options
    void setShowAspects(bool show) { showAspects = show; }
    void setShowDegrees(bool show) { showDegrees = show; }
    void setCompactMode(bool compact) { compactMode = compact; }

private:
    bool showAspects;
    bool showDegrees;
    bool compactMode;

    // Helper methods
    std::string getPlanetGlyph(Planet planet) const;
    std::string getSignGlyph(ZodiacSign sign) const;
    std::string getAspectGlyph(AspectType aspect) const;
    std::string formatHouseNumber(int house) const;
    std::string formatDegreeWithSign(double longitude) const;
    void placeTextOnWheel(std::vector<std::string>& wheel, int centerX, int centerY,
                         double angle, int radius, const std::string& text) const;
    std::pair<int, int> getWheelPosition(int centerX, int centerY, double angle, int radius) const;

    // Chart drawing components
    std::string drawWheelFrame(int size) const;
    void fillWheelWithPlanets(std::vector<std::string>& wheel, const std::vector<PlanetPosition>& positions,
                             const HouseCusps& cusps, int centerX, int centerY) const;
    void drawHouseCusps(std::vector<std::string>& wheel, const HouseCusps& cusps,
                       int centerX, int centerY, int radius) const;
    void drawAspectLines(std::vector<std::string>& wheel, const std::vector<Aspect>& aspects,
                        const std::vector<PlanetPosition>& positions, const HouseCusps& cusps,
                        int centerX, int centerY) const;

    // Static symbol maps
    static const std::map<Planet, std::string> PLANET_GLYPHS;
    static const std::map<ZodiacSign, std::string> SIGN_GLYPHS;
    static const std::map<AspectType, std::string> ASPECT_GLYPHS;
};

} // namespace Astro
