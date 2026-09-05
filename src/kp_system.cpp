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

struct KPSubDivisionResult {
    Planet lord;
    double startDeg;
    double endDeg;
};

static KPSubDivisionResult findNextSubDivision(double longitude, double parentStart, double parentEnd, Planet parentLord) {
    int lordIdx = getVimshottariIndex(parentLord);
    double parentSpan = parentEnd - parentStart;
    double curStart = parentStart;

    for (int i = 0; i < 9; ++i) {
        int idx = (lordIdx + i) % 9;
        double span = parentSpan * (VIMSHOTTARI_CYCLE[idx].years / 120.0);
        double curEnd = (i == 8) ? parentEnd : (curStart + span);
        if (longitude >= curStart && (longitude < curEnd || (i == 8 && longitude <= parentEnd + 1e-9))) {
            return {VIMSHOTTARI_CYCLE[idx].planet, curStart, curEnd};
        }
        curStart = curEnd;
    }
    return {parentLord, parentStart, parentEnd};
}

Planet KPSystem::findSubLord(double longitude, const Nakshatra& nakshatra, int level) const {
    longitude = normalizeKPLongitude(longitude);

    auto sub1 = findNextSubDivision(longitude, nakshatra.startDegree, nakshatra.endDegree, nakshatra.lord);
    if (level == 3 || level == static_cast<int>(KPLevel::SUB)) return sub1.lord;

    auto sub2 = findNextSubDivision(longitude, sub1.startDeg, sub1.endDeg, sub1.lord);
    if (level == 4 || level == static_cast<int>(KPLevel::SUB_SUB)) return sub2.lord;

    auto sub3 = findNextSubDivision(longitude, sub2.startDeg, sub2.endDeg, sub2.lord);
    if (level == 5 || level == static_cast<int>(KPLevel::SUB_SUB_SUB)) return sub3.lord;

    auto sub4 = findNextSubDivision(longitude, sub3.startDeg, sub3.endDeg, sub3.lord);
    if (level == 6 || level == static_cast<int>(KPLevel::SUB_4)) return sub4.lord;

    auto sub5 = findNextSubDivision(longitude, sub4.startDeg, sub4.endDeg, sub4.lord);
    if (level == 7 || level == static_cast<int>(KPLevel::SUB_5)) return sub5.lord;

    return sub1.lord;
}

KPPosition KPSystem::calculateKPPosition(double longitude) const {
    if (!isInitialized) {
        KPPosition empty{};
        return empty;
    }

    KPPosition pos{};
    pos.longitude = longitude;

    // Level 1: Sign
    pos.sign = longitudeToSign(longitude);
    pos.signLord = getSignLord(pos.sign);

    // Level 2: Nakshatra
    pos.nakshatra = findNakshatra(longitude);

    // Sub-L1
    auto sub1 = findNextSubDivision(longitude, pos.nakshatra.startDegree, pos.nakshatra.endDegree, pos.nakshatra.lord);
    pos.subLord = sub1.lord;

    // Sub-L2
    auto sub2 = findNextSubDivision(longitude, sub1.startDeg, sub1.endDeg, sub1.lord);
    pos.subSubLord = sub2.lord;

    // Sub-L3
    auto sub3 = findNextSubDivision(longitude, sub2.startDeg, sub2.endDeg, sub2.lord);
    pos.subSubSubLord = sub3.lord;

    // Sub-L4
    auto sub4 = findNextSubDivision(longitude, sub3.startDeg, sub3.endDeg, sub3.lord);
    pos.sub4Lord = sub4.lord;

    // Sub-L5
    auto sub5 = findNextSubDivision(longitude, sub4.startDeg, sub4.endDeg, sub4.lord);
    pos.sub5Lord = sub5.lord;

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
        << planetToString(subSubSubLord) << " "
        << planetToString(sub4Lord) << " "
        << planetToString(sub5Lord);
    return oss.str();
}

std::string KPPosition::getKPNotation() const {
    std::ostringstream oss;
    int signNum = static_cast<int>(sign) + 1;
    oss << signNum << "-" << nakshatra.number << "-"
        << planetToShortString(subLord) << "-"
        << planetToShortString(subSubLord) << "-"
        << planetToShortString(subSubSubLord) << "-"
        << planetToShortString(sub4Lord) << "-"
        << planetToShortString(sub5Lord);
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

    // Header with proper column widths to avoid text collision
    table << std::left
          << std::setw(12) << "Planet"
          << std::setw(14) << "Sign"
          << std::setw(20) << "Nakshatra"
          << std::setw(8)  << "Sub-L1"
          << std::setw(8)  << "Sub-L2"
          << std::setw(8)  << "Sub-L3"
          << std::setw(8)  << "Sub-L4"
          << std::setw(8)  << "Sub-L5"
          << std::setw(26) << "KP Notation"
          << "Longitude\n";

    table << std::string(118, '-') << "\n";

    // Planet data
    for (size_t i = 0; i < planets.size() && i < kpPositions.size(); i++) {
        const auto& planet = planets[i];
        const auto& kpPos = kpPositions[i];

        std::ostringstream lonStream;
        lonStream << std::fixed << std::setprecision(4) << planet.longitude << "°";

        table << std::left
              << std::setw(12) << planetToString(planet.planet)
              << std::setw(14) << zodiacSignToString(kpPos.sign)
              << std::setw(20) << kpPos.nakshatra.name
              << std::setw(8)  << planetToShortString(kpPos.subLord)
              << std::setw(8)  << planetToShortString(kpPos.subSubLord)
              << std::setw(8)  << planetToShortString(kpPos.subSubSubLord)
              << std::setw(8)  << planetToShortString(kpPos.sub4Lord)
              << std::setw(8)  << planetToShortString(kpPos.sub5Lord)
              << std::setw(26) << kpPos.getKPNotation()
              << lonStream.str() << "\n";
    }

    table << std::string(118, '-') << "\n";
    table << "Legend: L1=Sub, L2=Sub-Sub, L3=Sub-3, L4=Sub-4, L5=Sub-5 levels\n";
    table << "KP Notation: Sign-Nakshatra-SubL1-SubL2-SubL3-SubL4-SubL5\n";

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

    // Adaptive step size based on KP Level
    double step = 0.05; // ~1.2 hours for Sign, Star, Sub-L1
    if (level == KPLevel::SUB_SUB) {
        step = 0.01; // ~14 minutes
    } else if (level == KPLevel::SUB_SUB_SUB) {
        step = 0.002; // ~2.8 minutes
    } else if (level == KPLevel::SUB_4) {
        step = 0.0005; // ~43 seconds
    } else if (level == KPLevel::SUB_5) {
        step = 0.0001; // ~8.6 seconds
    }

    // Moon moves 13x faster than Sun, adapt step for Moon
    if (planet == Planet::MOON) {
        step *= 0.2;
    }

    Planet previousLord = Planet::SUN;
    bool firstIteration = true;
    const size_t MAX_TRANSITIONS = 1500;

    for (double jd = fromJD; jd <= toJD && transitions.size() < MAX_TRANSITIONS; jd += step) {
        // Calculate planet position for this Julian Day
        double planetLongitude = calculatePlanetLongitudeForJD(jd, planet);
        if (planetLongitude < 0) continue;

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
            // Binary search (bisection) to locate exact transition timestamp with millisecond precision
            double lowJD = jd - step;
            double highJD = jd;
            for (int iter = 0; iter < 16; ++iter) {
                double midJD = (lowJD + highJD) * 0.5;
                double midLon = calculatePlanetLongitudeForJD(midJD, planet);
                if (midLon >= 0) {
                    Nakshatra midNak = findNakshatra(midLon);
                    Planet midLord;
                    if (level == KPLevel::SIGN) {
                        midLord = getSignLord(longitudeToSign(midLon));
                    } else if (level == KPLevel::STAR) {
                        midLord = midNak.lord;
                    } else {
                        midLord = findSubLord(midLon, midNak, static_cast<int>(level));
                    }
                    if (midLord == previousLord) {
                        lowJD = midJD;
                    } else {
                        highJD = midJD;
                    }
                }
            }

            KPTransition transition;
            transition.julianDay = highJD;
            transition.planet = planet;
            transition.level = level;
            transition.fromLord = previousLord;
            transition.toLord = currentLord;
            transition.description = planetToString(planet) + " " + kpLevelToString(level) +
                                   ": " + planetToString(previousLord) + " -> " + planetToString(currentLord);

            transitions.push_back(transition);
        }

        previousLord = currentLord;
        firstIteration = false;
    }

    return transitions;
}

double KPSystem::calculatePlanetLongitudeForJD(double julianDay, Planet planet) const {
    double pos[6];
    char serr[256];

    swe_set_sid_mode(SE_SIDM_KRISHNAMURTI, 0, 0);

    int planetNum = static_cast<int>(planet);
    if (planet == Planet::NORTH_NODE) {
        planetNum = SE_TRUE_NODE;
    } else if (planet == Planet::SOUTH_NODE) {
        planetNum = SE_TRUE_NODE;
    } else if (planetNum > 11) {
        return -1; // Skip minor bodies for now
    }

    int32 result = swe_calc_ut(julianDay, planetNum, SEFLG_SWIEPH | SEFLG_SIDEREAL, pos, serr);
    if (result < 0) {
        return -1; // Calculation failed
    }

    double lon = pos[0];
    if (planet == Planet::SOUTH_NODE) {
        lon = fmod(lon + 180.0, 360.0);
    }
    return normalizeKPLongitude(lon);
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
    table << std::left
          << std::setw(25) << "Date & Time (ms)"
          << std::setw(12) << "Planet"
          << std::setw(10) << "Level"
          << std::setw(13) << "From Lord"
          << std::setw(13) << "To Lord"
          << "Description\n";

    table << std::string(105, '-') << "\n";

    // Transition data
    for (const auto& trans : transitions) {
        table << std::left
              << std::setw(25) << trans.getDateString()
              << std::setw(12) << planetToString(trans.planet)
              << std::setw(10) << kpLevelToString(trans.level)
              << std::setw(13) << planetToString(trans.fromLord)
              << std::setw(13) << planetToString(trans.toLord)
              << trans.description << "\n";
    }

    table << std::string(105, '-') << "\n";
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
        case KPLevel::SUB: return "Sub-L1";
        case KPLevel::SUB_SUB: return "Sub-L2";
        case KPLevel::SUB_SUB_SUB: return "Sub-L3";
        case KPLevel::SUB_4: return "Sub-L4";
        case KPLevel::SUB_5: return "Sub-L5";
        default: return "Unknown";
    }
}

KPLevel stringToKPLevel(const std::string& levelStr) {
    if (levelStr == "sign" || levelStr == "Sign" || levelStr == "1") return KPLevel::SIGN;
    if (levelStr == "star" || levelStr == "Star" || levelStr == "2") return KPLevel::STAR;
    if (levelStr == "sub" || levelStr == "Sub" || levelStr == "sub-l1" || levelStr == "Sub-L1" || levelStr == "3") return KPLevel::SUB;
    if (levelStr == "sub-sub" || levelStr == "Sub-Sub" || levelStr == "sub-l2" || levelStr == "Sub-L2" || levelStr == "4") return KPLevel::SUB_SUB;
    if (levelStr == "sub³" || levelStr == "Sub³" || levelStr == "sub-l3" || levelStr == "Sub-L3" || levelStr == "5") return KPLevel::SUB_SUB_SUB;
    if (levelStr == "sub-l4" || levelStr == "Sub-L4" || levelStr == "sub4" || levelStr == "6") return KPLevel::SUB_4;
    if (levelStr == "sub-l5" || levelStr == "Sub-L5" || levelStr == "sub5" || levelStr == "7") return KPLevel::SUB_5;
    return KPLevel::SUB; // Default
}

std::string KPTransition::getDateString() const {
    // Convert Julian Day to calendar date with seconds and milliseconds
    int year, month, day, hour, minute;
    double second;
    swe_jdut1_to_utc(julianDay, SE_GREG_CAL, &year, &month, &day, &hour, &minute, &second);

    int isec = static_cast<int>(second);
    if (isec < 0) isec = 0;
    if (isec > 59) isec = 59;
    int millis = static_cast<int>((second - isec) * 1000.0);
    if (millis < 0) millis = 0;
    if (millis > 999) millis = 999;

    std::ostringstream oss;
    oss << std::setfill('0');
    if (year <= 0) {
        int bcYear = 1 - year;
        oss << bcYear << " BC-" << std::setw(2) << month
            << "-" << std::setw(2) << day << " "
            << std::setw(2) << hour << ":" << std::setw(2) << minute << ":"
            << std::setw(2) << isec << "." << std::setw(3) << millis;
    } else {
        oss << year << "-" << std::setw(2) << month
            << "-" << std::setw(2) << day << " "
            << std::setw(2) << hour << ":" << std::setw(2) << minute << ":"
            << std::setw(2) << isec << "." << std::setw(3) << millis;
    }
    return oss.str();
}

std::string KPTransition::getTransitionDescription() const {
    return planetToString(planet) + " " + kpLevelToString(level) +
           " changes from " + planetToString(fromLord) + " to " + planetToString(toLord);
}

} // namespace Astro
