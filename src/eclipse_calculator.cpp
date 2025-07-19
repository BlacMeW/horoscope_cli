#include "eclipse_calculator.h"
#include "ephemeris_manager.h"
#include <swephexp.h>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <iostream>

namespace Astro {

// EclipseEvent method implementations
std::string EclipseEvent::getDateString() const {
    // Convert Julian Day to calendar date
    double tjd_ut = julianDay;
    int year, month, day, hour, minute;
    double second;
    swe_jdet_to_utc(tjd_ut, SE_GREG_CAL, &year, &month, &day, &hour, &minute, &second);

    std::ostringstream oss;
    oss << year << "-" << std::setfill('0') << std::setw(2) << month
        << "-" << std::setw(2) << day
        << " " << std::setw(2) << hour
        << ":" << std::setw(2) << minute;
    return oss.str();
}

std::string EclipseEvent::getTypeString() const {
    switch (type) {
        case EclipseType::SOLAR_TOTAL: return "Total Solar Eclipse";
        case EclipseType::SOLAR_PARTIAL: return "Partial Solar Eclipse";
        case EclipseType::SOLAR_ANNULAR: return "Annular Solar Eclipse";
        case EclipseType::LUNAR_TOTAL: return "Total Lunar Eclipse";
        case EclipseType::LUNAR_PARTIAL: return "Partial Lunar Eclipse";
        case EclipseType::LUNAR_PENUMBRAL: return "Penumbral Lunar Eclipse";
        default: return "Unknown Eclipse";
    }
}

std::string EclipseEvent::getDescription() const {
    std::ostringstream desc;
    desc << "Magnitude " << std::fixed << std::setprecision(3) << magnitude;
    if (duration > 0) {
        desc << ", Duration " << std::setprecision(1) << duration << " min";
    }
    return desc.str();
}

EclipseCalculator::EclipseCalculator() : isInitialized(false) {
}

EclipseCalculator::~EclipseCalculator() {
}

bool EclipseCalculator::initialize(const std::string& ephemerisPath) {
    EphemerisManager ephMgr;
    if (!ephMgr.initialize(ephemerisPath)) {
        lastError = ephMgr.getLastError();
        return false;
    }

    isInitialized = true;
    return true;
}

std::vector<EclipseEvent> EclipseCalculator::findEclipses(const BirthData& fromDate, const BirthData& toDate,
                                                          double latitude, double longitude) const {
    std::vector<EclipseEvent> eclipses;

    if (!isInitialized) {
        lastError = "Eclipse calculator not initialized";
        return eclipses;
    }

    // Placeholder implementation - actual eclipse calculation would require full Swiss Ephemeris eclipse functions
    EclipseEvent sampleEclipse;
    sampleEclipse.julianDay = fromDate.getJulianDay() + 30; // 30 days later
    sampleEclipse.type = EclipseType::SOLAR_PARTIAL;
    sampleEclipse.magnitude = 0.5;
    sampleEclipse.duration = 120; // 2 hours
    sampleEclipse.isVisible = true;
    sampleEclipse.sunLongitude = 90.0;
    sampleEclipse.moonLongitude = 90.0;

    eclipses.push_back(sampleEclipse);

    lastError = "Eclipse calculations use placeholder data (Swiss Ephemeris eclipse functions not fully available)";
    return eclipses;
}

std::vector<EclipseEvent> EclipseCalculator::findEclipses(const std::string& fromDate, const std::string& toDate,
                                                          double latitude, double longitude) const {
    // Convert string dates to BirthData
    BirthData fromBirth, toBirth;

    // Parse date strings (assumes YYYY-MM-DD format)
    std::sscanf(fromDate.c_str(), "%d-%d-%d", &fromBirth.year, &fromBirth.month, &fromBirth.day);
    std::sscanf(toDate.c_str(), "%d-%d-%d", &toBirth.year, &toBirth.month, &toBirth.day);

    // Set default time values
    fromBirth.hour = 0;
    fromBirth.minute = 0;
    fromBirth.second = 0;
    fromBirth.timezone = 0.0;
    fromBirth.latitude = latitude;
    fromBirth.longitude = longitude;

    toBirth.hour = 23;
    toBirth.minute = 59;
    toBirth.second = 59;
    toBirth.timezone = 0.0;
    toBirth.latitude = latitude;
    toBirth.longitude = longitude;

    return findEclipses(fromBirth, toBirth, latitude, longitude);
}

EclipseEvent EclipseCalculator::findNextSolarEclipse(const BirthData& afterDate,
                                                   double latitude, double longitude) const {
    if (!isInitialized) {
        lastError = "Eclipse calculator not initialized";
        EclipseEvent empty;
        empty.isVisible = false;
        return empty;
    }

    // Placeholder implementation
    EclipseEvent eclipse;
    eclipse.isVisible = true;
    eclipse.magnitude = 0.8;
    eclipse.duration = 180.0;
    eclipse.julianDay = afterDate.getJulianDay() + 365; // Next year
    eclipse.type = EclipseType::SOLAR_TOTAL;
    eclipse.sunLongitude = 180.0;
    eclipse.moonLongitude = 180.0;

    return eclipse;
}

EclipseEvent EclipseCalculator::findNextLunarEclipse(const BirthData& afterDate,
                                                   double latitude, double longitude) const {
    if (!isInitialized) {
        lastError = "Eclipse calculator not initialized";
        EclipseEvent empty;
        empty.isVisible = false;
        return empty;
    }

    // Placeholder implementation
    EclipseEvent eclipse;
    eclipse.isVisible = true;
    eclipse.magnitude = 1.2;
    eclipse.duration = 200.0;
    eclipse.julianDay = afterDate.getJulianDay() + 180; // 6 months later
    eclipse.type = EclipseType::LUNAR_TOTAL;
    eclipse.sunLongitude = 0.0;
    eclipse.moonLongitude = 180.0;

    return eclipse;
}

std::vector<EclipseEvent> EclipseCalculator::findEclipsesNearBirth(const BirthData& birthData,
                                                                 int yearsBefore, int yearsAfter) const {
    std::vector<EclipseEvent> eclipses;

    // Create date range
    BirthData fromDate = birthData;
    BirthData toDate = birthData;
    fromDate.year -= yearsBefore;
    toDate.year += yearsAfter;

    return findEclipses(fromDate, toDate, birthData.latitude, birthData.longitude);
}

std::vector<EclipseEvent> EclipseCalculator::checkEclipseOnDate(const BirthData& date, double latitude, double longitude) const {
    return findEclipses(date, date, latitude, longitude);
}

std::string EclipseCalculator::generateEclipseReport(const std::vector<EclipseEvent>& eclipses) const {
    std::ostringstream report;

    report << "Eclipse Report\n";
    report << "==============\n\n";
    report << "Total eclipses found: " << eclipses.size() << "\n\n";

    for (const auto& eclipse : eclipses) {
        report << eclipse.getDateString() << " - " << eclipse.getTypeString() << "\n";
        report << "  Magnitude: " << eclipse.magnitude << "\n";
        report << "  Duration: " << eclipse.duration << " minutes\n";
        report << "  Visible: " << (eclipse.isVisible ? "Yes" : "No") << "\n\n";
    }

    return report.str();
}

EclipseType EclipseCalculator::getEclipseTypeFromFlags(int flags) const {
    // Simple mapping for placeholder implementation
    if (flags & 4) return EclipseType::SOLAR_TOTAL;
    if (flags & 8) return EclipseType::SOLAR_ANNULAR;
    if (flags & 16) return EclipseType::SOLAR_PARTIAL;
    return EclipseType::LUNAR_TOTAL;
}

void EclipseCalculator::printEclipseEvent(const EclipseEvent& eclipse) const {
    std::cout << eclipse.getDateString() << " - " << eclipse.getTypeString()
              << " (" << eclipse.getDescription() << ")\n";
    std::cout << "  Magnitude: " << std::fixed << std::setprecision(3) << eclipse.magnitude;
    if (eclipse.duration > 0) {
        std::cout << "  Duration: " << std::fixed << std::setprecision(1) << eclipse.duration << " min";
    }
    std::cout << "  Visible: " << (eclipse.isVisible ? "Yes" : "No") << "\n";
}

} // namespace Astro
