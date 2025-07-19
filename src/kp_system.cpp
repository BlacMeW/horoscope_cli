#include "kp_system.h"
#include "planet_calculator.h"
#include "ephemeris_manager.h"
#include "astro_types.h"
#include <swephexp.h>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <iostream>

namespace Astro {

// KP Sub Lord proportions based on Vimshottari Dasha periods
static const double KP_SUB_PROPORTIONS[9] = {
    20.0/120.0, 16.0/120.0, 7.0/120.0, 18.0/120.0, 16.0/120.0,
    19.0/120.0, 17.0/120.0, 7.0/120.0, 20.0/120.0
};

static const Planet KP_SUB_LORDS[9] = {
    Planet::JUPITER, Planet::SATURN, Planet::MERCURY, Planet::SUN, Planet::MOON,
    Planet::MARS, Planet::VENUS, Planet::NORTH_NODE, Planet::JUPITER
};

// Nakshatra data with lords and degrees
static const struct {
    int number;
    const char* name;
    Planet lord;
    double startDegree;
} NAKSHATRA_DATA[27] = {
    {1, "Ashwini", Planet::SOUTH_NODE, 0.0},
    {2, "Bharani", Planet::VENUS, 13.333333},
    {3, "Krittika", Planet::SUN, 26.666667},
    {4, "Rohini", Planet::MOON, 40.0},
    {5, "Mrigashira", Planet::MARS, 53.333333},
    {6, "Ardra", Planet::NORTH_NODE, 66.666667},
    {7, "Punarvasu", Planet::JUPITER, 80.0},
    {8, "Pushya", Planet::SATURN, 93.333333},
    {9, "Ashlesha", Planet::MERCURY, 106.666667},
    {10, "Magha", Planet::SOUTH_NODE, 120.0},
    {11, "Purva Phalguni", Planet::VENUS, 133.333333},
    {12, "Uttara Phalguni", Planet::SUN, 146.666667},
    {13, "Hasta", Planet::MOON, 160.0},
    {14, "Chitra", Planet::MARS, 173.333333},
    {15, "Swati", Planet::NORTH_NODE, 186.666667},
    {16, "Vishakha", Planet::JUPITER, 200.0},
    {17, "Anuradha", Planet::SATURN, 213.333333},
    {18, "Jyeshtha", Planet::MERCURY, 226.666667},
    {19, "Mula", Planet::SOUTH_NODE, 240.0},
    {20, "Purva Ashadha", Planet::VENUS, 253.333333},
    {21, "Uttara Ashadha", Planet::SUN, 266.666667},
    {22, "Shravana", Planet::MOON, 280.0},
    {23, "Dhanishta", Planet::MARS, 293.333333},
    {24, "Shatabhisha", Planet::NORTH_NODE, 306.666667},
    {25, "Purva Bhadrapada", Planet::JUPITER, 320.0},
    {26, "Uttara Bhadrapada", Planet::SATURN, 333.333333},
    {27, "Revati", Planet::MERCURY, 346.666667}
};

KPSystem::KPSystem() : isInitialized(false) {
}

KPSystem::~KPSystem() {
}

bool KPSystem::initialize() {
    try {
        initializeNakshatras();
        initializeSubDivisions();
        isInitialized = true;
        lastError.clear();
        return true;
    } catch (const std::exception& e) {
        lastError = std::string("Failed to initialize KP system: ") + e.what();
        isInitialized = false;
        return false;
    }
}

void KPSystem::initializeNakshatras() {
    nakshatras.clear();
    for (int i = 0; i < 27; i++) {
        Nakshatra nak;
        nak.number = NAKSHATRA_DATA[i].number;
        nak.name = NAKSHATRA_DATA[i].name;
        nak.lord = NAKSHATRA_DATA[i].lord;
        nak.startDegree = NAKSHATRA_DATA[i].startDegree;
        nak.endDegree = (i < 26) ? NAKSHATRA_DATA[i+1].startDegree : 360.0;
        nakshatras.push_back(nak);
    }
}

void KPSystem::initializeSubDivisions() {
    subDivisions.clear();
    subDivisions.resize(27); // 27 nakshatras
    
    for (int i = 0; i < 27; i++) {
        const Nakshatra& nak = nakshatras[i];
        
        // Calculate sub divisions for each level (3, 4, 5)
        for (int level = 3; level <= 5; level++) {
            auto divs = calculateSubDivisions(nak, level);
            subDivisions[i].insert(subDivisions[i].end(), divs.begin(), divs.end());
        }
    }
}

std::vector<KPSubDivision> KPSystem::calculateSubDivisions(const Nakshatra& nakshatra, int level) const {
    std::vector<KPSubDivision> divisions;
    
    double nakshatraSpan = nakshatra.endDegree - nakshatra.startDegree;
    double currentDegree = nakshatra.startDegree;
    
    // For each level, calculate the subdivisions
    int numDivisions = (level == 3) ? 9 : (level == 4) ? 81 : 729; // 9^(level-2)
    
    for (int i = 0; i < numDivisions; i++) {
        KPSubDivision div;
        div.level = level;
        
        // Calculate the proportion for this subdivision
        double proportion = 1.0 / numDivisions;
        if (level == 3) {
            // Use Vimshottari proportions for sub level
            proportion = KP_SUB_PROPORTIONS[i % 9];
        }
        
        div.startDegree = currentDegree;
        div.endDegree = currentDegree + (nakshatraSpan * proportion);
        div.lord = KP_SUB_LORDS[i % 9];
        
        divisions.push_back(div);
        currentDegree = div.endDegree;
    }
    
    return divisions;
}

Nakshatra KPSystem::findNakshatra(double longitude) const {
    longitude = normalizeKPLongitude(longitude);
    
    for (const auto& nak : nakshatras) {
        if (longitude >= nak.startDegree && longitude < nak.endDegree) {
            return nak;
        }
    }
    
    // Default to last nakshatra if not found
    return nakshatras.back();
}

Planet KPSystem::findSubLord(double longitude, const Nakshatra& nakshatra, int level) const {
    longitude = normalizeKPLongitude(longitude);
    
    // Find appropriate subdivision
    for (const auto& div : subDivisions[nakshatra.number - 1]) {
        if (div.level == level && longitude >= div.startDegree && longitude < div.endDegree) {
            return div.lord;
        }
    }
    
    // Default fallback
    return KP_SUB_LORDS[0];
}

KPPosition KPSystem::calculateKPPosition(double longitude) const {
    if (!isInitialized) {
        KPPosition empty;
        return empty;
    }
    
    KPPosition pos;
    pos.longitude = longitude;
    
    // Level 1: Sign
    pos.sign = longitudeToSign(longitude);
    pos.signLord = getSignLord(pos.sign);
    
    // Level 2: Nakshatra
    pos.nakshatra = findNakshatra(longitude);
    
    // Level 3: Sub
    pos.subLord = findSubLord(longitude, pos.nakshatra, 3);
    
    // Level 4: Sub-Sub
    pos.subSubLord = findSubLord(longitude, pos.nakshatra, 4);
    
    // Level 5: Sub-Sub-Sub
    pos.subSubSubLord = findSubLord(longitude, pos.nakshatra, 5);
    
    return pos;
}

std::vector<KPPosition> KPSystem::calculateAllKPPositions(const std::vector<PlanetPosition>& planets) const {
    std::vector<KPPosition> kpPositions;
    
    for (const auto& planet : planets) {
        KPPosition kpPos = calculateKPPosition(planet.longitude);
        kpPositions.push_back(kpPos);
    }
    
    return kpPositions;
}

std::string KPPosition::getFormattedPosition() const {
    std::ostringstream oss;
    oss << zodiacSignToString(sign) << " " 
        << nakshatra.name << " "
        << planetToString(subLord) << " "
        << planetToString(subSubLord) << " "
        << planetToString(subSubSubLord);
    return oss.str();
}

std::string KPPosition::getKPNotation() const {
    std::ostringstream oss;
    int signNum = static_cast<int>(sign) + 1;
    oss << signNum << "-" << nakshatra.number << "-" 
        << planetToShortString(subLord) << "-"
        << planetToShortString(subSubLord) << "-"
        << planetToShortString(subSubSubLord);
    return oss.str();
}

std::string KPSystem::generateKPTable(const std::vector<PlanetPosition>& planets) const {
    if (!isInitialized) {
        return "Error: KP System not initialized";
    }
    
    auto kpPositions = calculateAllKPPositions(planets);
    
    std::ostringstream table;
    table << "\n=== KP SUB LORD 5 LEVELS SYSTEM ===\n";
    table << "Based on Krishnamurti Paddhati with Vimshottari Dasha proportions\n\n";
    
    // Header
    table << std::left << std::setw(10) << "Planet"
          << std::setw(8) << "Sign"
          << std::setw(15) << "Nakshatra"
          << std::setw(8) << "Sub-L1"
          << std::setw(8) << "Sub-L2" 
          << std::setw(8) << "Sub-L3"
          << std::setw(15) << "KP Notation"
          << std::setw(12) << "Longitude\n";
    
    table << std::string(90, '-') << "\n";
    
    // Planet data
    for (size_t i = 0; i < planets.size() && i < kpPositions.size(); i++) {
        const auto& planet = planets[i];
        const auto& kpPos = kpPositions[i];
        
        table << std::left << std::setw(10) << planetToString(planet.planet)
              << std::setw(8) << zodiacSignToString(kpPos.sign)
              << std::setw(15) << kpPos.nakshatra.name
              << std::setw(8) << planetToShortString(kpPos.subLord)
              << std::setw(8) << planetToShortString(kpPos.subSubLord)
              << std::setw(8) << planetToShortString(kpPos.subSubSubLord)
              << std::setw(15) << kpPos.getKPNotation()
              << std::setw(12) << std::fixed << std::setprecision(4) << planet.longitude << "°\n";
    }
    
    table << std::string(90, '-') << "\n";
    table << "Legend: L1=Sub, L2=Sub-Sub, L3=Sub-Sub-Sub levels\n";
    table << "KP Notation: Sign-Nakshatra-SubL1-SubL2-SubL3\n";
    
    return table.str();
}

std::vector<KPTransition> KPSystem::findTransitions(const std::string& fromDate, const std::string& toDate,
                                                   Planet planet, KPLevel level) const {
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
    fromBirth.latitude = 0.0;
    fromBirth.longitude = 0.0;
    
    toBirth.hour = 23;
    toBirth.minute = 59;
    toBirth.second = 59;
    toBirth.timezone = 0.0;
    toBirth.latitude = 0.0;
    toBirth.longitude = 0.0;
    
    return findTransitions(fromBirth, toBirth, planet, level);
}

std::vector<KPTransition> KPSystem::findTransitions(const BirthData& fromDate, const BirthData& toDate,
                                                   Planet planet, KPLevel level) const {
    std::vector<KPTransition> transitions;
    
    if (!isInitialized) {
        lastError = "KP System not initialized";
        return transitions;
    }
    
    // Calculate transitions by checking positions day by day
    double fromJD = fromDate.getJulianDay();
    double toJD = toDate.getJulianDay();
    
    // Sample every 0.1 days for precision
    double step = 0.1;
    Planet previousLord = Planet::SUN; // Initialize with a default
    bool firstIteration = true;
    
    for (double jd = fromJD; jd <= toJD; jd += step) {
        // Calculate planet position for this Julian Day
        double planetLongitude = calculatePlanetLongitudeForJD(jd, planet);
        
        if (planetLongitude < 0) continue; // Skip if calculation failed
        
        // Get the appropriate lord for the requested level
        Planet currentLord;
        if (level == KPLevel::SIGN) {
            currentLord = getSignLord(longitudeToSign(planetLongitude));
        } else if (level == KPLevel::STAR) {
            Nakshatra nak = findNakshatra(planetLongitude);
            currentLord = nak.lord;
        } else {
            Nakshatra nak = findNakshatra(planetLongitude);
            currentLord = findSubLord(planetLongitude, nak, static_cast<int>(level));
        }
        
        // Check for transition
        if (!firstIteration && currentLord != previousLord) {
            KPTransition transition;
            transition.julianDay = jd;
            transition.planet = planet;
            transition.level = level;
            transition.fromLord = previousLord;
            transition.toLord = currentLord;
            transition.description = "Transition from " + planetToString(previousLord) + 
                                   " to " + planetToString(currentLord);
            
            transitions.push_back(transition);
        }
        
        previousLord = currentLord;
        firstIteration = false;
    }
    
    return transitions;
}

double KPSystem::calculatePlanetLongitudeForJD(double julianDay, Planet planet) const {
    // Simplified planet calculation - in a real implementation, 
    // this would use Swiss Ephemeris or similar
    double pos[6];
    char serr[256];
    
    int planetNum = static_cast<int>(planet);
    if (planetNum > 11) return -1; // Skip minor bodies for now
    
    int32 result = swe_calc_ut(julianDay, planetNum, SEFLG_SWIEPH, pos, serr);
    if (result < 0) {
        return -1; // Calculation failed
    }
    
    return pos[0]; // Longitude in degrees
}

std::string KPSystem::generateTransitionTable(const std::vector<KPTransition>& transitions) const {
    std::ostringstream table;
    
    table << "\n=== KP TRANSITION TABLE ===\n";
    table << "5 Levels Planetary Transitions\n\n";
    
    if (transitions.empty()) {
        table << "No transitions found in the specified period.\n";
        return table.str();
    }
    
    // Header
    table << std::left << std::setw(20) << "Date & Time"
          << std::setw(10) << "Planet"
          << std::setw(8) << "Level"
          << std::setw(12) << "From Lord"
          << std::setw(12) << "To Lord"
          << "Description\n";
    
    table << std::string(80, '-') << "\n";
    
    // Transition data
    for (const auto& trans : transitions) {
        table << std::left << std::setw(20) << trans.getDateString()
              << std::setw(10) << planetToString(trans.planet)
              << std::setw(8) << kpLevelToString(trans.level)
              << std::setw(12) << planetToString(trans.fromLord)
              << std::setw(12) << planetToString(trans.toLord)
              << trans.description << "\n";
    }
    
    table << std::string(80, '-') << "\n";
    table << "Total transitions found: " << transitions.size() << "\n";
    
    return table.str();
}

// Utility function implementations

double normalizeKPLongitude(double longitude) {
    while (longitude < 0) longitude += 360.0;
    while (longitude >= 360.0) longitude -= 360.0;
    return longitude;
}

std::string formatKPDegree(double degree) {
    int deg = static_cast<int>(degree);
    int min = static_cast<int>((degree - deg) * 60);
    int sec = static_cast<int>(((degree - deg) * 60 - min) * 60);
    
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << deg << "°"
        << std::setw(2) << min << "'"
        << std::setw(2) << sec << "\"";
    return oss.str();
}

std::string kpLevelToString(KPLevel level) {
    switch (level) {
        case KPLevel::SIGN: return "Sign";
        case KPLevel::STAR: return "Star";
        case KPLevel::SUB: return "Sub";
        case KPLevel::SUB_SUB: return "Sub-Sub";
        case KPLevel::SUB_SUB_SUB: return "Sub³";
        default: return "Unknown";
    }
}

KPLevel stringToKPLevel(const std::string& levelStr) {
    if (levelStr == "sign" || levelStr == "Sign" || levelStr == "1") return KPLevel::SIGN;
    if (levelStr == "star" || levelStr == "Star" || levelStr == "2") return KPLevel::STAR;
    if (levelStr == "sub" || levelStr == "Sub" || levelStr == "3") return KPLevel::SUB;
    if (levelStr == "sub-sub" || levelStr == "Sub-Sub" || levelStr == "4") return KPLevel::SUB_SUB;
    if (levelStr == "sub³" || levelStr == "Sub³" || levelStr == "5") return KPLevel::SUB_SUB_SUB;
    return KPLevel::SUB; // Default
}

std::string KPTransition::getDateString() const {
    // Convert Julian Day to calendar date
    int year, month, day, hour, minute;
    double second;
    swe_jdet_to_utc(julianDay, SE_GREG_CAL, &year, &month, &day, &hour, &minute, &second);
    
    std::ostringstream oss;
    if (year <= 0) {
        int bcYear = 1 - year;
        oss << bcYear << " BC-" << std::setfill('0') << std::setw(2) << month
            << "-" << std::setw(2) << day << " "
            << std::setw(2) << hour << ":" << std::setw(2) << minute;
    } else {
        oss << year << "-" << std::setfill('0') << std::setw(2) << month
            << "-" << std::setw(2) << day << " "
            << std::setw(2) << hour << ":" << std::setw(2) << minute;
    }
    return oss.str();
}

std::string KPTransition::getTransitionDescription() const {
    return planetToString(planet) + " " + kpLevelToString(level) + 
           " changes from " + planetToString(fromLord) + " to " + planetToString(toLord);
}

} // namespace Astro
