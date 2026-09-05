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

// Vimshottari Dasha periods (total 120 years)
struct VimshottariLord {
    Planet planet;
    double years;
};

static const VimshottariLord VIMSHOTTARI_CYCLE[9] = {
    {Planet::SOUTH_NODE, 7.0},   // Ketu
    {Planet::VENUS, 20.0},       // Venus
    {Planet::SUN, 6.0},          // Sun
    {Planet::MOON, 10.0},        // Moon
    {Planet::MARS, 7.0},         // Mars
    {Planet::NORTH_NODE, 18.0},  // Rahu
    {Planet::JUPITER, 16.0},     // Jupiter
    {Planet::SATURN, 19.0},      // Saturn
    {Planet::MERCURY, 17.0}      // Mercury
};

static int getVimshottariIndex(Planet planet) {
    for (int i = 0; i < 9; ++i) {
        if (VIMSHOTTARI_CYCLE[i].planet == planet) return i;
    }
    return 0;
}

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

    // Level 3 (Sub): 9 divisions of the nakshatra starting from nakshatra lord
    int startLordIdx = getVimshottariIndex(nakshatra.lord);
    double curDegree = nakshatra.startDegree;
    double nakSpan = nakshatra.endDegree - nakshatra.startDegree;

    std::vector<KPSubDivision> level3Divs;
    for (int i = 0; i < 9; i++) {
        int idx = (startLordIdx + i) % 9;
        KPSubDivision div;
        div.level = 3;
        div.lord = VIMSHOTTARI_CYCLE[idx].planet;
        div.startDegree = curDegree;
        double span = nakSpan * (VIMSHOTTARI_CYCLE[idx].years / 120.0);
        div.endDegree = (i == 8) ? nakshatra.endDegree : (curDegree + span);
        curDegree = div.endDegree;
        level3Divs.push_back(div);
    }

    if (level == 3) {
        return level3Divs;
    }

    // Level 4 (Sub-Sub): 81 divisions, dividing each level 3 div into 9 parts starting with sub lord
    std::vector<KPSubDivision> level4Divs;
    for (const auto& sub3 : level3Divs) {
        int sub3LordIdx = getVimshottariIndex(sub3.lord);
        double sub3Cur = sub3.startDegree;
        double sub3Span = sub3.endDegree - sub3.startDegree;

        for (int j = 0; j < 9; j++) {
            int idx = (sub3LordIdx + j) % 9;
            KPSubDivision div;
            div.level = 4;
            div.lord = VIMSHOTTARI_CYCLE[idx].planet;
            div.startDegree = sub3Cur;
            double span = sub3Span * (VIMSHOTTARI_CYCLE[idx].years / 120.0);
            div.endDegree = (j == 8) ? sub3.endDegree : (sub3Cur + span);
            sub3Cur = div.endDegree;
            level4Divs.push_back(div);
        }
    }

    if (level == 4) {
        return level4Divs;
    }

    // Level 5 (Sub-Sub-Sub): 729 divisions, dividing each level 4 div into 9 parts starting with sub-sub lord
    std::vector<KPSubDivision> level5Divs;
    for (const auto& sub4 : level4Divs) {
        int sub4LordIdx = getVimshottariIndex(sub4.lord);
        double sub4Cur = sub4.startDegree;
        double sub4Span = sub4.endDegree - sub4.startDegree;

        for (int m = 0; m < 9; m++) {
            int idx = (sub4LordIdx + m) % 9;
            KPSubDivision div;
            div.level = 5;
            div.lord = VIMSHOTTARI_CYCLE[idx].planet;
            div.startDegree = sub4Cur;
            double span = sub4Span * (VIMSHOTTARI_CYCLE[idx].years / 120.0);
            div.endDegree = (m == 8) ? sub4.endDegree : (sub4Cur + span);
            sub4Cur = div.endDegree;
            level5Divs.push_back(div);
        }
    }

    return level5Divs;
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

    int nakIdx = nakshatra.number - 1;
    if (nakIdx < 0 || nakIdx >= static_cast<int>(subDivisions.size())) {
        return nakshatra.lord;
    }

    // Find appropriate subdivision
    Planet lastFound = nakshatra.lord;
    for (const auto& div : subDivisions[nakIdx]) {
        if (div.level == level) {
            lastFound = div.lord;
            if (longitude >= div.startDegree && (longitude < div.endDegree || (div.endDegree >= nakshatra.endDegree - 1e-7 && longitude <= nakshatra.endDegree + 1e-7))) {
                return div.lord;
            }
        }
    }

    return lastFound;
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
    swe_jdut1_to_utc(julianDay, SE_GREG_CAL, &year, &month, &day, &hour, &minute, &second);

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
