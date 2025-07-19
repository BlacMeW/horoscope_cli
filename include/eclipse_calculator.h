#pragma once

#include "astro_types.h"
#include <string>
#include <vector>

namespace Astro {

// Eclipse types
enum class EclipseType {
    SOLAR_TOTAL,
    SOLAR_PARTIAL,
    SOLAR_ANNULAR,
    LUNAR_TOTAL,
    LUNAR_PARTIAL,
    LUNAR_PENUMBRAL
};

// Eclipse event structure
struct EclipseEvent {
    EclipseType type;
    double julianDay;       // Julian day of maximum eclipse
    double magnitude;       // Eclipse magnitude (0.0 - 1.0+)
    double duration;        // Duration in minutes
    double moonLongitude;   // Moon's longitude at eclipse
    double sunLongitude;    // Sun's longitude at eclipse
    std::string location;   // Visible from location (if applicable)
    bool isVisible;         // Visible from birth location

    // Convert Julian Day to readable date
    std::string getDateString() const;
    std::string getTypeString() const;
    std::string getDescription() const;
};

class EclipseCalculator {
public:
    EclipseCalculator();
    ~EclipseCalculator();

    // Initialize with ephemeris data
    bool initialize(const std::string& ephePath = "");

    // Find eclipses in a given time range
    std::vector<EclipseEvent> findEclipses(const BirthData& fromDate, const BirthData& toDate,
                                          double latitude = 0.0, double longitude = 0.0) const;

    // Find eclipses by date strings (convenience method)
    std::vector<EclipseEvent> findEclipses(const std::string& fromDate, const std::string& toDate,
                                          double latitude = 0.0, double longitude = 0.0) const;

    // Find next solar eclipse after given date
    EclipseEvent findNextSolarEclipse(const BirthData& afterDate,
                                     double latitude = 0.0, double longitude = 0.0) const;

    // Find next lunar eclipse after given date
    EclipseEvent findNextLunarEclipse(const BirthData& afterDate,
                                     double latitude = 0.0, double longitude = 0.0) const;

    // Find eclipses near birth date (within +/- years)
    std::vector<EclipseEvent> findEclipsesNearBirth(const BirthData& birthData,
                                                   int yearsBefore = 1, int yearsAfter = 1) const;

    // Check if there's an eclipse on a specific date
    std::vector<EclipseEvent> checkEclipseOnDate(const BirthData& date,
                                                double latitude = 0.0, double longitude = 0.0) const;

    // Generate formatted eclipse report
    std::string generateEclipseReport(const std::vector<EclipseEvent>& eclipses) const;

    // Print eclipse event to console
    void printEclipseEvent(const EclipseEvent& eclipse) const;

    // Get error message if initialization failed
    std::string getLastError() const { return lastError; }

private:
    bool isInitialized;
    mutable std::string lastError;

    // Helper functions for Swiss Ephemeris eclipse calculations
    EclipseEvent calculateSolarEclipse(double julianDay, double latitude, double longitude) const;
    EclipseEvent calculateLunarEclipse(double julianDay, double latitude, double longitude) const;

    // Convert eclipse flags to EclipseType
    EclipseType getEclipseTypeFromFlags(int flags) const;

    // Calculate eclipse visibility from location
    bool isEclipseVisible(const EclipseEvent& eclipse, double latitude, double longitude) const;

    // Format eclipse magnitude
    std::string formatMagnitude(double magnitude) const;

    // Format eclipse duration
    std::string formatDuration(double minutes) const;
};

} // namespace Astro
