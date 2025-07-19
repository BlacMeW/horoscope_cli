#include "astro_calendar.h"
#include "hindu_calendar.h"
#include "myanmar_calendar.h"
#include "ephemeris_manager.h"
#include "planet_calculator.h"
#include "swephexp.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <memory>
#include <cstdint>

namespace Astro {

// Month names array
const std::array<std::string, 12> AstroCalendar::monthNames = {{
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
}};

// PlanetaryTransition methods
std::string PlanetaryTransition::getDescription() const {
    std::stringstream ss;
    switch (type) {
        case TransitionType::SIGN_CHANGE:
            ss << getPlanetName(planet) << " enters new sign";
            break;
        case TransitionType::RETROGRADE_START:
            ss << getPlanetName(planet) << " goes retrograde";
            break;
        case TransitionType::RETROGRADE_END:
            ss << getPlanetName(planet) << " goes direct";
            break;
        case TransitionType::CONJUNCTION:
            ss << getPlanetName(planet) << " conjunct " << getPlanetName(secondPlanet);
            break;
        case TransitionType::ECLIPSE:
            ss << description;
            break;
        case TransitionType::NEW_MOON:
            ss << "New Moon";
            break;
        case TransitionType::FULL_MOON:
            ss << "Full Moon";
            break;
    }
    return ss.str();
}

std::string PlanetaryTransition::getFormattedDate() const {
    int year, month, day;
    AstroCalendar::julianDayToGregorian(julianDay, year, month, day);
    return AstroCalendar::formatDate(year, month, day);
}

// AstroCalendarDay methods
std::string AstroCalendarDay::getShortSummary() const {
    std::stringstream ss;
    if (hasPanchangaData) {
        ss << "T:" << static_cast<int>(panchangaData.tithi) << " ";
        ss << "N:" << static_cast<int>(panchangaData.nakshatra) << " ";
    }
    if (hasMyanmarData) {
        ss << "ME:" << myanmarData.myanmarYear << " ";
    }
    if (isAuspicious) ss << "✅";
    else if (isInauspicious) ss << "⚠️";
    return ss.str();
}

std::string AstroCalendarDay::getDetailedDescription() const {
    std::stringstream ss;
    ss << "═══ " << gregorianDateStr << " ═══\n";

    if (hasPanchangaData) {
        ss << "🕉️ Hindu: Tithi " << static_cast<int>(panchangaData.tithi);
        ss << ", Nakshatra " << static_cast<int>(panchangaData.nakshatra) << "\n";
    }

    if (hasMyanmarData) {
        ss << "🇲🇲 Myanmar: " << myanmarData.myanmarYear << " ME, ";
        ss << "Day " << myanmarData.dayOfMonth << "\n";
    }

    if (!allFestivals.empty()) {
        ss << "🎊 Festivals: " << getFestivalsSummary() << "\n";
    }

    if (!planetaryTransitions.empty()) {
        ss << "🪐 Transits: " << getPlanetaryTransitionsSummary() << "\n";
    }

    ss << "Quality: " << dayQualityDescription;
    return ss.str();
}

std::string AstroCalendarDay::getFestivalsSummary() const {
    std::stringstream ss;
    for (size_t i = 0; i < allFestivals.size() && i < 3; ++i) {
        if (i > 0) ss << ", ";
        ss << allFestivals[i];
    }
    if (allFestivals.size() > 3) {
        ss << " +" << (allFestivals.size() - 3) << " more";
    }
    return ss.str();
}

std::string AstroCalendarDay::getPlanetaryTransitionsSummary() const {
    std::stringstream ss;
    for (size_t i = 0; i < planetaryTransitions.size() && i < 2; ++i) {
        if (i > 0) ss << ", ";
        ss << planetaryTransitions[i].getDescription();
    }
    if (planetaryTransitions.size() > 2) {
        ss << " +" << (planetaryTransitions.size() - 2) << " more";
    }
    return ss.str();
}

// AstroCalendarMonth methods
std::string AstroCalendarMonth::getMonthSummary() const {
    std::stringstream ss;
    ss << monthName << " " << year << " - ";
    ss << auspiciousDays << " auspicious, ";
    ss << inauspiciousDays << " inauspicious, ";
    ss << neutralDays << " neutral days";
    return ss.str();
}

// AstroCalendar implementation
AstroCalendar::AstroCalendar() {
    hinduCalendar = std::make_unique<HinduCalendar>();
    myanmarCalendar = std::make_unique<MyanmarCalendar>();
    ephemerisManager = std::make_unique<EphemerisManager>();
    // PlanetCalculator will be initialized after ephemerisManager is ready
}

AstroCalendar::~AstroCalendar() = default;

bool AstroCalendar::initialize(double lat, double lon) {
    latitude = lat;
    longitude = lon;

    try {
        // Initialize all calendar systems
        if (!hinduCalendar->initialize()) {
            lastError = "Failed to initialize Hindu Calendar: " + hinduCalendar->getLastError();
            return false;
        }

        if (!myanmarCalendar->initialize()) {
            lastError = "Failed to initialize Myanmar Calendar: " + myanmarCalendar->getLastError();
            return false;
        }

        if (!ephemerisManager->initialize()) {
            lastError = "Failed to initialize Ephemeris Manager: " + ephemerisManager->getLastError();
            return false;
        }

        // Now initialize PlanetCalculator with the ephemeris manager
        planetCalculator = std::make_unique<PlanetCalculator>(*ephemerisManager);

        initialized = true;
        return true;

    } catch (const std::exception& e) {
        lastError = "Exception during initialization: " + std::string(e.what());
        return false;
    }
}

AstroCalendarDay AstroCalendar::calculateAstroCalendarDay(const BirthData& birthData) const {
    return calculateAstroCalendarDay(birthData.year, birthData.month, birthData.day);
}

AstroCalendarDay AstroCalendar::calculateAstroCalendarDay(int year, int month, int day) const {
    if (!initialized) {
        throw std::runtime_error("AstroCalendar not initialized");
    }

    double jd = gregorianToJulianDay(year, month, day);
    return calculateDayData(jd, year, month, day);
}

AstroCalendarMonth AstroCalendar::calculateAstroCalendarMonth(int year, int month) const {
    if (!initialized) {
        throw std::runtime_error("AstroCalendar not initialized");
    }

    AstroCalendarMonth monthData;
    monthData.year = year;
    monthData.month = month;
    monthData.monthName = getGregorianMonthName(month);

    int daysInMonth = getDaysInMonth(year, month);
    monthData.days.reserve(daysInMonth);

    // Calculate data for each day
    for (int day = 1; day <= daysInMonth; ++day) {
        try {
            AstroCalendarDay dayData = calculateAstroCalendarDay(year, month, day);
            monthData.days.push_back(dayData);

            // Update statistics
            if (dayData.isAuspicious) {
                monthData.auspiciousDays++;
            } else if (dayData.isInauspicious) {
                monthData.inauspiciousDays++;
            } else {
                monthData.neutralDays++;
            }

        } catch (const std::exception& e) {
            lastError = "Error calculating day " + std::to_string(day) + ": " + e.what();
        }
    }

    // Calculate monthly transitions if requested
    if (includePlanetaryTransitions) {
        monthData.monthlyTransitions = findMonthlyTransitions(year, month);
    }

    return monthData;
}

AstroCalendarDay AstroCalendar::calculateDayData(double julianDay, int gregYear, int gregMonth, int gregDay) const {
    AstroCalendarDay day;

    // Basic Gregorian data
    day.gregorianYear = gregYear;
    day.gregorianMonth = gregMonth;
    day.gregorianDay = gregDay;
    day.gregorianDateStr = formatDate(gregYear, gregMonth, gregDay);
    day.julianDay = julianDay;

    try {
        // Calculate Hindu Calendar (Panchanga) data
        day.panchangaData = hinduCalendar->calculatePanchanga(julianDay, latitude, longitude);
        day.hasPanchangaData = true;

    } catch (const std::exception& e) {
        lastError = "Hindu calendar calculation failed: " + std::string(e.what());
        day.hasPanchangaData = false;
    }

    try {
        // Calculate Myanmar Calendar data
        day.myanmarData = myanmarCalendar->calculateMyanmarCalendar(julianDay);
        day.hasMyanmarData = true;

    } catch (const std::exception& e) {
        lastError = "Myanmar calendar calculation failed: " + std::string(e.what());
        day.hasMyanmarData = false;
    }

    // Calculate planetary transitions if requested
    if (includePlanetaryTransitions) {
        day.planetaryTransitions = calculatePlanetaryTransitions(julianDay);
    }

    // Compile festivals and events
    compileFestivalsAndEvents(day);

    // Calculate overall day quality
    calculateDayQuality(day);

    return day;
}

std::vector<PlanetaryTransition> AstroCalendar::calculatePlanetaryTransitions(double julianDay) const {
    std::vector<PlanetaryTransition> transitions;

    try {
        // Check for lunar phases
        double moonPhase[6];
        // Calculate moon phase using ephemeris
        if (swe_calc_ut(julianDay, SE_MOON, SEFLG_SWIEPH, moonPhase, nullptr) >= 0) {
            double sunPos[6];
            if (swe_calc_ut(julianDay, SE_SUN, SEFLG_SWIEPH, sunPos, nullptr) >= 0) {
                double phaseDiff = std::fmod(moonPhase[0] - sunPos[0] + 360.0, 360.0);

                if (std::abs(phaseDiff) < 1.0) { // New Moon
                    PlanetaryTransition newMoon;
                    newMoon.type = TransitionType::NEW_MOON;
                    newMoon.planet = Planet::MOON;
                    newMoon.julianDay = julianDay;
                    newMoon.description = "New Moon - New beginnings";
                    newMoon.isAuspicious = true;
                    newMoon.recommendations = "Good time for setting intentions and starting new projects";
                    transitions.push_back(newMoon);
                }

                if (std::abs(phaseDiff - 180.0) < 1.0) { // Full Moon
                    PlanetaryTransition fullMoon;
                    fullMoon.type = TransitionType::FULL_MOON;
                    fullMoon.planet = Planet::MOON;
                    fullMoon.julianDay = julianDay;
                    fullMoon.description = "Full Moon - Peak energy";
                    fullMoon.isAuspicious = true;
                    fullMoon.recommendations = "Time for completion, manifestation, and releasing";
                    transitions.push_back(fullMoon);
                }
            }
        }

    } catch (const std::exception& e) {
        lastError = "Error calculating planetary transitions: " + std::string(e.what());
    }

    return transitions;
}

std::vector<PlanetaryTransition> AstroCalendar::findMonthlyTransitions(int year, int month) const {
    std::vector<PlanetaryTransition> transitions;

    // This is a placeholder - would need more sophisticated ephemeris calculations
    // to find actual planetary sign changes, retrogrades, etc.

    return transitions;
}

void AstroCalendar::compileFestivalsAndEvents(AstroCalendarDay& day) const {
    day.allFestivals.clear();
    day.allEvents.clear();

    // Add Hindu festivals
    if (day.hasPanchangaData) {
        for (const auto& festival : day.panchangaData.festivals) {
            day.allFestivals.push_back("🕉️ " + festival);
        }
        for (const auto& event : day.panchangaData.specialEvents) {
            day.allEvents.push_back("🕉️ " + event);
        }
    }

    // Add Myanmar festivals
    if (day.hasMyanmarData) {
        for (const auto& festival : day.myanmarData.festivals) {
            day.allFestivals.push_back("🇲🇲 " + festival);
        }
    }
}

void AstroCalendar::calculateDayQuality(AstroCalendarDay& day) const {
    int score = 5; // Neutral starting point

    // Hindu calendar factors
    if (day.hasPanchangaData) {
        if (day.panchangaData.isShubhaMuhurta) {
            score += 2;
            day.astrologicalRecommendations.push_back("Auspicious Hindu muhurta");
        }
        if (day.panchangaData.isAshubhaMuhurta) {
            score -= 2;
            day.astrologicalRecommendations.push_back("Inauspicious Hindu muhurta");
        }
        if (day.panchangaData.isEkadashi) {
            score += 1;
            day.astrologicalRecommendations.push_back("Ekadashi - spiritual practices favored");
        }
    }

    // Myanmar calendar factors
    if (day.hasMyanmarData) {
        if (day.myanmarData.isThamanyo) {
            score += 2;
            day.astrologicalRecommendations.push_back("Thamanyo - auspicious day");
        }
        if (day.myanmarData.isYatyaza) {
            score -= 2;
            day.astrologicalRecommendations.push_back("Yatyaza - avoid new ventures");
        }
        if (day.myanmarData.isSabbath) {
            score += 1;
            day.astrologicalRecommendations.push_back("Buddhist Sabbath - meditation favored");
        }
    }

    // Planetary transition factors
    for (const auto& transition : day.planetaryTransitions) {
        if (transition.isAuspicious) {
            score += 1;
        } else {
            score -= 1;
        }
    }

    // Clamp score to 0-10 range
    day.auspiciousScore = std::max(0, std::min(10, score));

    if (day.auspiciousScore >= 7) {
        day.isAuspicious = true;
        day.dayQualityDescription = "Highly Auspicious Day";
    } else if (day.auspiciousScore <= 3) {
        day.isInauspicious = true;
        day.dayQualityDescription = "Inauspicious Day - Exercise Caution";
    } else {
        day.dayQualityDescription = "Neutral Day";
    }
}

// Output formatting methods
std::string AstroCalendar::generateDayCalendar(const AstroCalendarDay& day, const std::string& format) const {
    if (format == "json") {
        return generateJSON(day);
    }

    std::stringstream ss;
    ss << "═══════════════════════════════════════════════════════════════════\n";
    ss << "               🌟 ASTRO CALENDAR - " << day.gregorianDateStr << " 🌟\n";
    ss << "═══════════════════════════════════════════════════════════════════\n\n";

    // Quality indicator
    ss << "📊 DAY QUALITY: " << day.dayQualityDescription << " (Score: " << day.auspiciousScore << "/10)\n";
    if (day.isAuspicious) ss << "   Status: ✅ AUSPICIOUS DAY\n";
    else if (day.isInauspicious) ss << "   Status: ⚠️ INAUSPICIOUS DAY\n";
    else ss << "   Status: ⚪ NEUTRAL DAY\n";
    ss << "\n";

    // Hindu Calendar section
    if (day.hasPanchangaData) {
        ss << "🕉️ HINDU CALENDAR (PANCHANGA):\n";
        ss << "   Tithi: " << static_cast<int>(day.panchangaData.tithi) << " ";
        ss << (day.panchangaData.isShukla ? "(Shukla)" : "(Krishna)") << "\n";
        ss << "   Vara: " << static_cast<int>(day.panchangaData.vara) << "\n";
        ss << "   Nakshatra: " << static_cast<int>(day.panchangaData.nakshatra) << "\n";
        ss << "   Yoga: " << static_cast<int>(day.panchangaData.yoga) << "\n";
        ss << "   Karana: " << static_cast<int>(day.panchangaData.karana) << "\n";
        ss << "\n";
    }

    // Myanmar Calendar section
    if (day.hasMyanmarData) {
        ss << "🇲🇲 MYANMAR CALENDAR:\n";
        ss << "   Myanmar Year: " << day.myanmarData.myanmarYear << " ME\n";
        ss << "   Sasana Year: " << day.myanmarData.sasanaYear << " SE\n";
        ss << "   Month: " << static_cast<int>(day.myanmarData.month) << "\n";
        ss << "   Day: " << day.myanmarData.dayOfMonth << "\n";
        ss << "   Moon Phase: " << static_cast<int>(day.myanmarData.moonPhase) << "\n";
        ss << "\n";
    }

    // Festivals and Events
    if (!day.allFestivals.empty()) {
        ss << "🎊 FESTIVALS:\n";
        for (const auto& festival : day.allFestivals) {
            ss << "   • " << festival << "\n";
        }
        ss << "\n";
    }

    // Planetary Transitions
    if (!day.planetaryTransitions.empty()) {
        ss << "🪐 PLANETARY EVENTS:\n";
        for (const auto& transition : day.planetaryTransitions) {
            ss << "   • " << transition.getDescription() << "\n";
            if (!transition.recommendations.empty()) {
                ss << "     " << transition.recommendations << "\n";
            }
        }
        ss << "\n";
    }

    // Recommendations
    if (!day.astrologicalRecommendations.empty()) {
        ss << "💡 RECOMMENDATIONS:\n";
        for (const auto& rec : day.astrologicalRecommendations) {
            ss << "   • " << rec << "\n";
        }
        ss << "\n";
    }

    ss << "═══════════════════════════════════════════════════════════════════\n";

    return ss.str();
}

std::string AstroCalendar::generateMonthlyCalendar(const AstroCalendarMonth& monthData, const std::string& format) const {
    if (format == "json") {
        return generateJSON(monthData);
    } else if (format == "csv") {
        return generateCSV(monthData);
    } else if (format == "table") {
        return generateTable(monthData);
    } else {
        return generateCalendarLayout(monthData);
    }
}

std::string AstroCalendar::generateJSON(const AstroCalendarDay& day) const {
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"gregorianDate\": \"" << day.gregorianDateStr << "\",\n";
    ss << "  \"julianDay\": " << std::fixed << std::setprecision(6) << day.julianDay << ",\n";
    ss << "  \"dayQuality\": {\n";
    ss << "    \"score\": " << day.auspiciousScore << ",\n";
    ss << "    \"description\": \"" << day.dayQualityDescription << "\",\n";
    ss << "    \"isAuspicious\": " << (day.isAuspicious ? "true" : "false") << ",\n";
    ss << "    \"isInauspicious\": " << (day.isInauspicious ? "true" : "false") << "\n";
    ss << "  },\n";

    if (day.hasPanchangaData) {
        ss << "  \"hinduCalendar\": {\n";
        ss << "    \"tithi\": " << static_cast<int>(day.panchangaData.tithi) << ",\n";
        ss << "    \"vara\": " << static_cast<int>(day.panchangaData.vara) << ",\n";
        ss << "    \"nakshatra\": " << static_cast<int>(day.panchangaData.nakshatra) << ",\n";
        ss << "    \"yoga\": " << static_cast<int>(day.panchangaData.yoga) << ",\n";
        ss << "    \"karana\": " << static_cast<int>(day.panchangaData.karana) << ",\n";
        ss << "    \"isShukla\": " << (day.panchangaData.isShukla ? "true" : "false") << "\n";
        ss << "  },\n";
    }

    if (day.hasMyanmarData) {
        ss << "  \"myanmarCalendar\": {\n";
        ss << "    \"myanmarYear\": " << day.myanmarData.myanmarYear << ",\n";
        ss << "    \"sasanaYear\": " << day.myanmarData.sasanaYear << ",\n";
        ss << "    \"month\": " << static_cast<int>(day.myanmarData.month) << ",\n";
        ss << "    \"day\": " << day.myanmarData.dayOfMonth << ",\n";
        ss << "    \"moonPhase\": " << static_cast<int>(day.myanmarData.moonPhase) << "\n";
        ss << "  },\n";
    }

    // Festivals
    ss << "  \"festivals\": [";
    for (size_t i = 0; i < day.allFestivals.size(); ++i) {
        if (i > 0) ss << ",";
        ss << "\"" << day.allFestivals[i] << "\"";
    }
    ss << "],\n";

    // Planetary transitions
    ss << "  \"planetaryTransitions\": [";
    for (size_t i = 0; i < day.planetaryTransitions.size(); ++i) {
        if (i > 0) ss << ",";
        ss << "{\"description\":\"" << day.planetaryTransitions[i].getDescription() << "\"}";
    }
    ss << "]\n";

    ss << "}";
    return ss.str();
}

std::string AstroCalendar::generateJSON(const AstroCalendarMonth& monthData) const {
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"year\": " << monthData.year << ",\n";
    ss << "  \"month\": " << monthData.month << ",\n";
    ss << "  \"monthName\": \"" << monthData.monthName << "\",\n";
    ss << "  \"summary\": \"" << monthData.getMonthSummary() << "\",\n";
    ss << "  \"statistics\": {\n";
    ss << "    \"auspiciousDays\": " << monthData.auspiciousDays << ",\n";
    ss << "    \"inauspiciousDays\": " << monthData.inauspiciousDays << ",\n";
    ss << "    \"neutralDays\": " << monthData.neutralDays << "\n";
    ss << "  },\n";
    ss << "  \"days\": [\n";

    for (size_t i = 0; i < monthData.days.size(); ++i) {
        if (i > 0) ss << ",\n";
        ss << "    " << generateJSON(monthData.days[i]);
    }

    ss << "\n  ]\n";
    ss << "}";
    return ss.str();
}

std::string AstroCalendar::generateCalendarLayout(const AstroCalendarMonth& monthData) const {
    std::stringstream ss;

    ss << "═══════════════════════════════════════════════════════════════════\n";
    ss << "           🌟 ASTRO CALENDAR - " << monthData.monthName << " " << monthData.year << " 🌟\n";
    ss << "═══════════════════════════════════════════════════════════════════\n";
    ss << "           🕉️ Hindu  🇲🇲 Myanmar  🪐 Planetary  🎊 Festivals\n";
    ss << "═══════════════════════════════════════════════════════════════════\n\n";

    ss << monthData.getMonthSummary() << "\n\n";

    // Calendar header
    ss << " Sun    Mon    Tue    Wed    Thu    Fri    Sat\n";
    ss << "─────  ─────  ─────  ─────  ─────  ─────  ─────\n";

    // Find first day of month and day of week
    double firstDayJD = gregorianToJulianDay(monthData.year, monthData.month, 1);
    int firstDayOfWeek = (static_cast<int>(firstDayJD + 1.5)) % 7; // 0=Sunday

    // Print leading spaces for first week
    for (int i = 0; i < firstDayOfWeek; ++i) {
        ss << "       ";
    }

    // Print calendar days
    for (size_t dayIdx = 0; dayIdx < monthData.days.size(); ++dayIdx) {
        const auto& day = monthData.days[dayIdx];
        int dayOfWeek = (firstDayOfWeek + dayIdx) % 7;

        // Day number
        ss << std::setw(2) << day.gregorianDay;

        // Quality indicator
        if (day.isAuspicious) ss << "✅";
        else if (day.isInauspicious) ss << "⚠️";
        else ss << "  ";

        // Festival indicator
        if (!day.allFestivals.empty()) ss << "🎊";
        else ss << " ";

        // Planetary event indicator
        if (!day.planetaryTransitions.empty()) ss << "🪐";
        else ss << " ";

        // End of week - new line
        if (dayOfWeek == 6) {
            ss << "\n";
        } else {
            ss << " ";
        }
    }

    ss << "\n\n";

    // Legend
    ss << "Legend:\n";
    ss << "✅ Auspicious day    ⚠️ Inauspicious day    🎊 Festival    🪐 Planetary event\n\n";

    // Significant days summary
    if (!monthData.significantDays.empty()) {
        ss << "Significant Days:\n";
        for (const auto& [day, description] : monthData.significantDays) {
            ss << "  " << std::setw(2) << day << ": " << description << "\n";
        }
        ss << "\n";
    }

    ss << "═══════════════════════════════════════════════════════════════════\n";

    return ss.str();
}

// Utility methods
std::string AstroCalendar::getGregorianMonthName(int month) const {
    if (month >= 1 && month <= 12) {
        return monthNames[month - 1];
    }
    return "Unknown";
}

int AstroCalendar::getDaysInMonth(int year, int month) const {
    static const int daysPerMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (month == 2 && isLeapYear(year)) {
        return 29;
    }

    if (month >= 1 && month <= 12) {
        return daysPerMonth[month - 1];
    }

    return 30; // default
}

bool AstroCalendar::isLeapYear(int year) const {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

double AstroCalendar::gregorianToJulianDay(int year, int month, int day) {
    return swe_julday(year, month, day, 12.0, SE_GREG_CAL);
}

void AstroCalendar::julianDayToGregorian(double jd, int& year, int& month, int& day) {
    int32 y, m, d, hour, min;
    double sec;
    swe_jdet_to_utc(jd, SE_GREG_CAL, &y, &m, &d, &hour, &min, &sec);
    year = y;
    month = m;
    day = d;
}

std::string AstroCalendar::formatDate(int year, int month, int day) {
    std::stringstream ss;
    ss << year << "-" << std::setfill('0') << std::setw(2) << month << "-" << std::setw(2) << day;
    return ss.str();
}

std::string AstroCalendar::generateCSV(const AstroCalendarMonth& monthData) const {
    std::stringstream ss;

    // CSV header
    ss << "Date,Day_Quality_Score,Is_Auspicious,Hindu_Tithi,Hindu_Nakshatra,Myanmar_Year,Myanmar_Day,";
    ss << "Festivals,Planetary_Events,Recommendations\n";

    for (const auto& day : monthData.days) {
        ss << day.gregorianDateStr << ",";
        ss << day.auspiciousScore << ",";
        ss << (day.isAuspicious ? "Yes" : "No") << ",";

        if (day.hasPanchangaData) {
            ss << static_cast<int>(day.panchangaData.tithi) << ",";
            ss << static_cast<int>(day.panchangaData.nakshatra) << ",";
        } else {
            ss << ",,";
        }

        if (day.hasMyanmarData) {
            ss << day.myanmarData.myanmarYear << ",";
            ss << day.myanmarData.dayOfMonth << ",";
        } else {
            ss << ",,";
        }

        // Festivals (quoted and comma-separated)
        ss << "\"";
        for (size_t i = 0; i < day.allFestivals.size(); ++i) {
            if (i > 0) ss << "; ";
            ss << day.allFestivals[i];
        }
        ss << "\",";

        // Planetary events
        ss << "\"";
        for (size_t i = 0; i < day.planetaryTransitions.size(); ++i) {
            if (i > 0) ss << "; ";
            ss << day.planetaryTransitions[i].getDescription();
        }
        ss << "\",";

        // Recommendations
        ss << "\"";
        for (size_t i = 0; i < day.astrologicalRecommendations.size(); ++i) {
            if (i > 0) ss << "; ";
            ss << day.astrologicalRecommendations[i];
        }
        ss << "\"\n";
    }

    return ss.str();
}

std::string AstroCalendar::generateTable(const AstroCalendarMonth& monthData) const {
    std::stringstream ss;

    ss << "═══════════════════════════════════════════════════════════════════\n";
    ss << "           ASTRO CALENDAR TABLE - " << monthData.monthName << " " << monthData.year << "\n";
    ss << "═══════════════════════════════════════════════════════════════════\n";

    // Table header
    ss << std::left << std::setw(12) << "Date"
       << std::setw(8) << "Quality"
       << std::setw(10) << "Hindu"
       << std::setw(12) << "Myanmar"
       << std::setw(20) << "Festivals"
       << std::setw(15) << "Planetary" << "\n";

    ss << std::string(77, '-') << "\n";

    for (const auto& day : monthData.days) {
        ss << std::left << std::setw(12) << day.gregorianDateStr;

        // Quality
        std::string quality = (day.isAuspicious ? "✅" : (day.isInauspicious ? "⚠️" : "⚪"));
        quality += std::to_string(day.auspiciousScore);
        ss << std::setw(8) << quality;

        // Hindu info
        std::string hindu = "";
        if (day.hasPanchangaData) {
            hindu = "T" + std::to_string(static_cast<int>(day.panchangaData.tithi));
        }
        ss << std::setw(10) << hindu;

        // Myanmar info
        std::string myanmar = "";
        if (day.hasMyanmarData) {
            myanmar = std::to_string(day.myanmarData.myanmarYear) + "ME";
        }
        ss << std::setw(12) << myanmar;

        // Festivals (abbreviated)
        std::string festivals = day.getFestivalsSummary();
        if (festivals.length() > 18) {
            festivals = festivals.substr(0, 15) + "...";
        }
        ss << std::setw(20) << festivals;

        // Planetary (abbreviated)
        std::string planetary = day.getPlanetaryTransitionsSummary();
        if (planetary.length() > 13) {
            planetary = planetary.substr(0, 10) + "...";
        }
        ss << std::setw(15) << planetary;

        ss << "\n";
    }

    ss << "═══════════════════════════════════════════════════════════════════\n";
    ss << monthData.getMonthSummary() << "\n";
    ss << "═══════════════════════════════════════════════════════════════════\n";

    return ss.str();
}

// Utility functions
std::string formatAstroCalendarDate(const AstroCalendarDay& day) {
    return day.gregorianDateStr;
}

std::string getSeasonName(int month) {
    if (month >= 3 && month <= 5) return "Spring";
    if (month >= 6 && month <= 8) return "Summer";
    if (month >= 9 && month <= 11) return "Autumn";
    return "Winter";
}

std::string getMoonPhaseName(double lunarPhase) {
    if (lunarPhase < 1.0 || lunarPhase > 359.0) return "New Moon";
    if (lunarPhase >= 89.0 && lunarPhase <= 91.0) return "First Quarter";
    if (lunarPhase >= 179.0 && lunarPhase <= 181.0) return "Full Moon";
    if (lunarPhase >= 269.0 && lunarPhase <= 271.0) return "Last Quarter";
    if (lunarPhase < 90.0) return "Waxing Crescent";
    if (lunarPhase < 180.0) return "Waxing Gibbous";
    if (lunarPhase < 270.0) return "Waning Gibbous";
    return "Waning Crescent";
}

std::string getPlanetaryWeather(const std::vector<PlanetaryTransition>& transitions) {
    int auspiciousCount = 0;
    int inauspiciousCount = 0;

    for (const auto& transition : transitions) {
        if (transition.isAuspicious) auspiciousCount++;
        else inauspiciousCount++;
    }

    if (auspiciousCount > inauspiciousCount) return "Favorable planetary weather";
    if (inauspiciousCount > auspiciousCount) return "Challenging planetary weather";
    return "Neutral planetary weather";
}

} // namespace Astro
