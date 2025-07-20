#include "birth_chart.h"
#include <sstream>
#include <iomanip>
#include <cmath>

namespace Astro {

BirthChart::BirthChart() : zodiacMode(ZodiacMode::TROPICAL), ayanamsa(AyanamsaType::LAHIRI) {
}

void BirthChart::setBirthData(const BirthData& data) {
    birthData = data;
}

void BirthChart::setPlanetPositions(const std::vector<PlanetPosition>& positions) {
    planetPositions = positions;
}

void BirthChart::setHouseCusps(const HouseCusps& cusps) {
    houseCusps = cusps;
}

void BirthChart::setAspects(const std::vector<Aspect>& aspects) {
    this->aspects = aspects;
}

void BirthChart::setZodiacMode(ZodiacMode mode) {
    zodiacMode = mode;
}

void BirthChart::setAyanamsa(AyanamsaType ayanamsa) {
    this->ayanamsa = ayanamsa;
}

std::string BirthChart::getFormattedChart() const {
    std::ostringstream oss;

    oss << getChartHeader() << "\n\n";
    oss << getPlanetPositionsString() << "\n\n";
    oss << getHouseCuspsString() << "\n\n";
    oss << getAspectsString();

    return oss.str();
}

std::string BirthChart::getFormattedChart(bool showAstronomicalCoordinates) const {
    std::ostringstream oss;

    oss << getChartHeader() << "\n\n";

    if (showAstronomicalCoordinates) {
        oss << getPlanetPositionsWithAstronomicalCoordinates() << "\n\n";
    } else {
        oss << getPlanetPositionsString() << "\n\n";
    }

    oss << getHouseCuspsString() << "\n\n";
    oss << getAspectsString();

    return oss.str();
}

std::string BirthChart::getPlanetPositionsString() const {
    std::ostringstream oss;
    oss << "=== PLANETARY POSITIONS ===\n";

    for (const auto& pos : planetPositions) {
        oss << std::left << std::setw(12) << (planetToString(pos.planet) + ":");
        oss << pos.getFormattedPosition() << "\n";
    }

    return oss.str();
}

std::string BirthChart::getPlanetPositionsWithAstronomicalCoordinates() const {
    std::ostringstream oss;
    oss << "=== PLANETARY POSITIONS & ASTRONOMICAL COORDINATES ===\n";

    for (const auto& pos : planetPositions) {
        oss << std::left << std::setw(12) << (planetToString(pos.planet) + ":");
        oss << pos.getFormattedPosition() << "\n";
        oss << std::left << std::setw(12) << "";
        oss << pos.getAstronomicalCoordinates() << "\n\n";
    }

    return oss.str();
}

std::string BirthChart::getHouseCuspsString() const {
    std::ostringstream oss;
    oss << "=== HOUSE CUSPS ===\n";

    for (int house = 1; house <= 12; house++) {
        double cusp = houseCusps.cusps[house];
        ZodiacSign sign = longitudeToSign(cusp);

        // Create proper ordinal suffix
        std::string suffix;
        if (house == 1) suffix = "st";
        else if (house == 2) suffix = "nd";
        else if (house == 3) suffix = "rd";
        else suffix = "th";

        oss << std::left << std::setw(12) << (std::to_string(house) + suffix + " House:");
        oss << formatDegreeMinute(cusp) << " " << zodiacSignToString(sign) << "\n";
    }

    oss << "\nAngles:\n";
    oss << std::left << std::setw(12) << "Ascendant:";
    oss << formatDegreeMinute(houseCusps.ascendant) << " "
        << zodiacSignToString(longitudeToSign(houseCusps.ascendant)) << "\n";

    oss << std::left << std::setw(12) << "Midheaven:";
    oss << formatDegreeMinute(houseCusps.midheaven) << " "
        << zodiacSignToString(longitudeToSign(houseCusps.midheaven)) << "\n";

    return oss.str();
}

std::string BirthChart::getAspectsString() const {
    std::ostringstream oss;
    oss << "=== MAJOR ASPECTS ===\n";

    if (aspects.empty()) {
        oss << "No significant aspects found.\n";
        return oss.str();
    }

    for (const auto& aspect : aspects) {
        oss << aspect.getDescription() << "\n";
    }

    return oss.str();
}

std::string BirthChart::getChartHeader() const {
    std::ostringstream oss;
    oss << "Birth Chart for " << birthData.getDateTimeString() << "\n";
    oss << "Location: " << birthData.getLocationString() << "\n";
    oss << "Zodiac System: " << zodiacModeToString(zodiacMode);
    if (zodiacMode == ZodiacMode::SIDEREAL) {
        oss << " (Ayanamsa: " << ayanamsaTypeToString(ayanamsa) << ")";
    }
    return oss.str();
}

std::string BirthChart::exportToJson() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"birthData\": {\n";
    oss << "    \"date\": \"" << birthData.year << "-"
        << std::setfill('0') << std::setw(2) << birthData.month << "-"
        << std::setw(2) << birthData.day << "\",\n";
    oss << "    \"time\": \"" << std::setw(2) << birthData.hour << ":"
        << std::setw(2) << birthData.minute << ":"
        << std::setw(2) << birthData.second << "\",\n";
    oss << "    \"latitude\": " << std::fixed << std::setprecision(6) << birthData.latitude << ",\n";
    oss << "    \"longitude\": " << birthData.longitude << ",\n";
    oss << "    \"timezone\": " << birthData.timezone << "\n";
    oss << "  },\n";

    oss << "  \"planets\": [\n";
    for (size_t i = 0; i < planetPositions.size(); i++) {
        const auto& pos = planetPositions[i];
        oss << "    {\n";
        oss << "      \"name\": \"" << planetToString(pos.planet) << "\",\n";
        oss << "      \"longitude\": " << std::fixed << std::setprecision(6) << pos.longitude << ",\n";
        oss << "      \"latitude\": " << pos.latitude << ",\n";
        oss << "      \"sign\": \"" << zodiacSignToString(pos.sign) << "\",\n";
        oss << "      \"house\": " << pos.house << ",\n";
        oss << "      \"speed\": " << pos.speed << "\n";
        oss << "    }";
        if (i < planetPositions.size() - 1) oss << ",";
        oss << "\n";
    }
    oss << "  ],\n";

    oss << "  \"houses\": {\n";
    for (int house = 1; house <= 12; house++) {
        oss << "    \"" << house << "\": " << std::fixed << std::setprecision(6)
            << houseCusps.cusps[house];
        if (house < 12) oss << ",";
        oss << "\n";
    }
    oss << "  },\n";

    oss << "  \"aspects\": [\n";
    for (size_t i = 0; i < aspects.size(); i++) {
        const auto& aspect = aspects[i];
        oss << "    {\n";
        oss << "      \"planet1\": \"" << planetToString(aspect.planet1) << "\",\n";
        oss << "      \"planet2\": \"" << planetToString(aspect.planet2) << "\",\n";
        oss << "      \"type\": \"" << aspectTypeToString(aspect.type) << "\",\n";
        oss << "      \"orb\": " << std::fixed << std::setprecision(2) << aspect.orb << ",\n";
        oss << "      \"applying\": " << (aspect.isApplying ? "true" : "false") << "\n";
        oss << "    }";
        if (i < aspects.size() - 1) oss << ",";
        oss << "\n";
    }
    oss << "  ]\n";
    oss << "}";

    return oss.str();
}

std::string BirthChart::formatDegreeMinute(double degrees) const {
    int deg = static_cast<int>(degrees) % 30;
    int min = static_cast<int>((degrees - static_cast<int>(degrees)) * 60);

    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << deg << "°" << std::setw(2) << min << "'";
    return oss.str();
}

std::string BirthChart::formatCoordinate(double coord, bool isLatitude) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(4) << std::abs(coord) << "°";
    if (isLatitude) {
        oss << (coord >= 0 ? "N" : "S");
    } else {
        oss << (coord >= 0 ? "E" : "W");
    }
    return oss.str();
}

} // namespace Astro
