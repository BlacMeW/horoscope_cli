#include "eclipse_calculator.h"
#include "ephemeris_manager.h"
#include "astro_types.h"
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

    double fromJD = fromDate.getJulianDay();
    double toJD = toDate.getJulianDay();
    double currentJD = fromJD;

    char serr[256];
    double tret[10];
    double attr[20];
    double geopos[3];
    geopos[0] = longitude;
    geopos[1] = latitude;
    geopos[2] = 0; // Sea level

    // Find all solar eclipses in the time range
    while (currentJD < toJD) {
        int32 retval = swe_sol_eclipse_when_glob(currentJD, SEFLG_SWIEPH, 0, tret, 0, serr);

        if (retval == ERR) {
            break;
        }

        if (retval > 0 && tret[0] <= toJD) {
            EclipseEvent eclipse;
            eclipse.julianDay = tret[0];
            eclipse.isVisible = true;

            // Determine eclipse type
            if (retval & SE_ECL_TOTAL) {
                eclipse.type = EclipseType::SOLAR_TOTAL;
            } else if (retval & SE_ECL_ANNULAR) {
                eclipse.type = EclipseType::SOLAR_ANNULAR;
            } else if (retval & SE_ECL_PARTIAL) {
                eclipse.type = EclipseType::SOLAR_PARTIAL;
            } else if (retval & SE_ECL_ANNULAR_TOTAL) {
                eclipse.type = EclipseType::SOLAR_TOTAL; // Hybrid treated as total
            }

            // Get magnitude for the specified location
            int32 localRet = swe_sol_eclipse_how(eclipse.julianDay, SEFLG_SWIEPH, geopos, attr, serr);
            if (localRet > 0) {
                eclipse.magnitude = attr[0];
            } else {
                eclipse.magnitude = 0.0; // Not visible from this location
            }

            // Get Sun/Moon positions
            double sunpos[6], moonpos[6];
            if (swe_calc_ut(eclipse.julianDay, SE_SUN, SEFLG_SWIEPH, sunpos, serr) >= 0) {
                eclipse.sunLongitude = sunpos[0];
            }
            if (swe_calc_ut(eclipse.julianDay, SE_MOON, SEFLG_SWIEPH, moonpos, serr) >= 0) {
                eclipse.moonLongitude = moonpos[0];
            }

            eclipse.duration = 0; // Would need more complex calculation
            eclipses.push_back(eclipse);

            // Search for next eclipse
            currentJD = tret[0] + 1.0;
        } else {
            break; // No more eclipses found
        }
    }

    // Find all lunar eclipses in the time range
    currentJD = fromJD;
    while (currentJD < toJD) {
        int32 retval = swe_lun_eclipse_when(currentJD, SEFLG_SWIEPH, 0, tret, 0, serr);

        if (retval == ERR) {
            break;
        }

        if (retval > 0 && tret[0] <= toJD) {
            EclipseEvent eclipse;
            eclipse.julianDay = tret[0];
            eclipse.isVisible = true;

            // Determine eclipse type
            if (retval & SE_ECL_TOTAL) {
                eclipse.type = EclipseType::LUNAR_TOTAL;
            } else if (retval & SE_ECL_PARTIAL) {
                eclipse.type = EclipseType::LUNAR_PARTIAL;
            } else if (retval & SE_ECL_PENUMBRAL) {
                eclipse.type = EclipseType::LUNAR_PENUMBRAL;
            }

            // Get magnitude for the specified location
            int32 localRet = swe_lun_eclipse_how(eclipse.julianDay, SEFLG_SWIEPH, geopos, attr, serr);
            if (localRet > 0) {
                eclipse.magnitude = attr[0]; // Umbral magnitude
            } else {
                eclipse.magnitude = 0.5; // Default if calculation fails
            }

            // Get Sun/Moon positions
            double sunpos[6], moonpos[6];
            if (swe_calc_ut(eclipse.julianDay, SE_SUN, SEFLG_SWIEPH, sunpos, serr) >= 0) {
                eclipse.sunLongitude = sunpos[0];
            }
            if (swe_calc_ut(eclipse.julianDay, SE_MOON, SEFLG_SWIEPH, moonpos, serr) >= 0) {
                eclipse.moonLongitude = moonpos[0];
            }

            eclipse.duration = 0; // Would need more complex calculation
            eclipses.push_back(eclipse);

            // Search for next eclipse
            currentJD = tret[0] + 1.0;
        } else {
            break; // No more eclipses found
        }
    }

    // Sort eclipses by date
    std::sort(eclipses.begin(), eclipses.end(),
              [](const EclipseEvent& a, const EclipseEvent& b) {
                  return a.julianDay < b.julianDay;
              });

    lastError = eclipses.empty() ? "No eclipses found in the specified period" : "";
    return eclipses;
}

std::vector<EclipseEvent> EclipseCalculator::findEclipses(const std::string& fromDate, const std::string& toDate,
                                                          double latitude, double longitude) const {
    // Convert string dates to BirthData
    BirthData fromBirth, toBirth;

    // Parse date strings with BC era support
    parseBCDate(fromDate, fromBirth.year, fromBirth.month, fromBirth.day);
    parseBCDate(toDate, toBirth.year, toBirth.month, toBirth.day);

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
    EclipseEvent eclipse;
    eclipse.isVisible = false; // Default to not visible

    if (!isInitialized) {
        lastError = "Eclipse calculator not initialized";
        return eclipse;
    }

    double startJD = afterDate.getJulianDay();
    double tret[10]; // Array to store eclipse times
    double attr[20]; // Array to store eclipse attributes
    char serr[256];

    // Use Swiss Ephemeris to find the next solar eclipse globally
    int32 retval = swe_sol_eclipse_when_glob(startJD, SEFLG_SWIEPH, 0, tret, 0, serr);

    if (retval == ERR) {
        lastError = std::string("Swiss Ephemeris error: ") + serr;
        return eclipse;
    }

    if (retval > 0) {
        eclipse.julianDay = tret[0]; // Time of maximum eclipse
        eclipse.isVisible = true;

        // Determine eclipse type from return flags
        if (retval & SE_ECL_TOTAL) {
            eclipse.type = EclipseType::SOLAR_TOTAL;
        } else if (retval & SE_ECL_ANNULAR) {
            eclipse.type = EclipseType::SOLAR_ANNULAR;
        } else if (retval & SE_ECL_PARTIAL) {
            eclipse.type = EclipseType::SOLAR_PARTIAL;
        } else if (retval & SE_ECL_ANNULAR_TOTAL) {
            eclipse.type = EclipseType::SOLAR_TOTAL; // Hybrid eclipse treated as total
        }

        // Get more detailed information for this eclipse location
        double geopos[3];
        geopos[0] = longitude;
        geopos[1] = latitude;
        geopos[2] = 0; // Sea level

        int32 localRet = swe_sol_eclipse_how(eclipse.julianDay, SEFLG_SWIEPH, geopos, attr, serr);

        if (localRet > 0) {
            eclipse.magnitude = attr[0]; // Magnitude at the given location
            eclipse.duration = 0; // Duration calculation would need more detailed implementation

            // Calculate Sun and Moon positions for additional info
            double sunpos[6], moonpos[6];
            if (swe_calc_ut(eclipse.julianDay, SE_SUN, SEFLG_SWIEPH, sunpos, serr) >= 0 &&
                swe_calc_ut(eclipse.julianDay, SE_MOON, SEFLG_SWIEPH, moonpos, serr) >= 0) {
                eclipse.sunLongitude = sunpos[0];
                eclipse.moonLongitude = moonpos[0];
            }
        } else {
            // Eclipse not visible from this location, but still valid
            eclipse.magnitude = 0.0;

            // Get positions anyway
            double sunpos[6], moonpos[6];
            if (swe_calc_ut(eclipse.julianDay, SE_SUN, SEFLG_SWIEPH, sunpos, serr) >= 0 &&
                swe_calc_ut(eclipse.julianDay, SE_MOON, SEFLG_SWIEPH, moonpos, serr) >= 0) {
                eclipse.sunLongitude = sunpos[0];
                eclipse.moonLongitude = moonpos[0];
            }
        }
    } else {
        lastError = "No solar eclipse found";
    }

    return eclipse;
}

EclipseEvent EclipseCalculator::findNextLunarEclipse(const BirthData& afterDate,
                                                   double latitude, double longitude) const {
    EclipseEvent eclipse;
    eclipse.isVisible = false; // Default to not visible

    if (!isInitialized) {
        lastError = "Eclipse calculator not initialized";
        return eclipse;
    }

    double startJD = afterDate.getJulianDay();
    double tret[10]; // Array to store eclipse times
    double attr[20]; // Array to store eclipse attributes
    char serr[256];

    // Use Swiss Ephemeris to find the next lunar eclipse
    int32 retval = swe_lun_eclipse_when(startJD, SEFLG_SWIEPH, 0, tret, 0, serr);

    if (retval == ERR) {
        lastError = std::string("Swiss Ephemeris error: ") + serr;
        return eclipse;
    }

    if (retval > 0) {
        eclipse.julianDay = tret[0]; // Time of maximum eclipse
        eclipse.isVisible = true;

        // Determine eclipse type from return flags
        if (retval & SE_ECL_TOTAL) {
            eclipse.type = EclipseType::LUNAR_TOTAL;
        } else if (retval & SE_ECL_PARTIAL) {
            eclipse.type = EclipseType::LUNAR_PARTIAL;
        } else if (retval & SE_ECL_PENUMBRAL) {
            eclipse.type = EclipseType::LUNAR_PENUMBRAL;
        }

        // Get more detailed information for this eclipse location
        double geopos[3];
        geopos[0] = longitude;
        geopos[1] = latitude;
        geopos[2] = 0; // Sea level

        int32 localRet = swe_lun_eclipse_how(eclipse.julianDay, SEFLG_SWIEPH, geopos, attr, serr);

        if (localRet > 0) {
            eclipse.magnitude = attr[0]; // Umbral magnitude at the given location
            eclipse.duration = 0; // Duration calculation would need more detailed implementation
        } else {
            eclipse.magnitude = 0.5; // Default magnitude if calculation fails
        }

        // Calculate Sun and Moon positions
        double sunpos[6], moonpos[6];
        if (swe_calc_ut(eclipse.julianDay, SE_SUN, SEFLG_SWIEPH, sunpos, serr) >= 0 &&
            swe_calc_ut(eclipse.julianDay, SE_MOON, SEFLG_SWIEPH, moonpos, serr) >= 0) {
            eclipse.sunLongitude = sunpos[0];
            eclipse.moonLongitude = moonpos[0];
        }
    } else {
        lastError = "No lunar eclipse found";
    }

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
