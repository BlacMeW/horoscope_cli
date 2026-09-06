#include <iostream>
#include <iomanip>
#include "myanmar_calendar.h"

using namespace std;
using namespace Astro;

// Helper functions to convert enums to strings
string weekdayToString(MyanmarWeekday wd) {
    switch (wd) {
        case MyanmarWeekday::SATURDAY: return "Saturday";
        case MyanmarWeekday::SUNDAY: return "Sunday";
        case MyanmarWeekday::MONDAY: return "Monday";
        case MyanmarWeekday::TUESDAY: return "Tuesday";
        case MyanmarWeekday::WEDNESDAY: return "Wednesday";
        case MyanmarWeekday::THURSDAY: return "Thursday";
        case MyanmarWeekday::FRIDAY: return "Friday";
        default: return "Unknown";
    }
}

string monthToString(MyanmarMonth month) {
    switch (month) {
        case MyanmarMonth::FIRST_WASO: return "First Waso";
        case MyanmarMonth::TAGU: return "Tagu";
        case MyanmarMonth::KASON: return "Kason";
        case MyanmarMonth::NAYON: return "Nayon";
        case MyanmarMonth::WASO: return "Waso";
        case MyanmarMonth::WAGAUNG: return "Wagaung";
        case MyanmarMonth::TAWTHALIN: return "Tawthalin";
        case MyanmarMonth::THADINGYUT: return "Thadingyut";
        case MyanmarMonth::TAZAUNGMON: return "Tazaungmon";
        case MyanmarMonth::NADAW: return "Nadaw";
        case MyanmarMonth::PYATHO: return "Pyatho";
        case MyanmarMonth::TABODWE: return "Tabodwe";
        case MyanmarMonth::TABAUNG: return "Tabaung";
        case MyanmarMonth::LATE_TAGU: return "Late Tagu";
        case MyanmarMonth::LATE_KASON: return "Late Kason";
        default: return "Unknown";
    }
}

string moonPhaseToString(MyanmarMoonPhase phase) {
    switch (phase) {
        case MyanmarMoonPhase::WAXING: return "Waxing";
        case MyanmarMoonPhase::FULL_MOON: return "Full Moon";
        case MyanmarMoonPhase::WANING: return "Waning";
        case MyanmarMoonPhase::NEW_MOON: return "New Moon";
        default: return "Unknown";
    }
}

string yearTypeToString(MyanmarYearType type) {
    switch (type) {
        case MyanmarYearType::COMMON: return "Common";
        case MyanmarYearType::LITTLE_WATAT: return "Little Watat";
        case MyanmarYearType::BIG_WATAT: return "Big Watat";
        default: return "Unknown";
    }
}

// Function to convert Gregorian date to Julian Day
double gregorianToJulian(int year, int month, int day) {
    int a = (14 - month) / 12;
    int y = year - a;
    int m = month + 12 * a - 3;
    
    if (year > 1582 || (year == 1582 && month > 10) || (year == 1582 && month == 10 && day >= 15)) {
        // Gregorian calendar
        return day + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 + 1721119;
    } else {
        // Julian calendar
        return day + (153 * m + 2) / 5 + 365 * y + y / 4 + 1721117;
    }
}

int main() {
    cout << "Myanmar Calendar Verification Test" << endl;
    cout << "=====================================\n" << endl;

    // Create Myanmar calendar instance
    MyanmarCalendar myanmarCalendar;
    if (!myanmarCalendar.initialize()) {
        cout << "Error: Failed to initialize Myanmar calendar" << endl;
        return 1;
    }

    // Test the specific date: June 11, 1976
    // Expected: 1338 ME, Nayon 15
    double jd = gregorianToJulian(1976, 6, 11);
    
    cout << "Testing: June 11, 1976" << endl;
    cout << "Expected: 1338 ME, Nayon 15" << endl;
    cout << "Julian Day: " << fixed << setprecision(1) << jd << endl;
    cout << endl;

    try {
        // Calculate Myanmar calendar data
        MyanmarCalendarData data = myanmarCalendar.calculateMyanmarCalendar(jd);
        
        cout << "RESULTS:" << endl;
        cout << "========" << endl;
        cout << "Myanmar Year: " << data.myanmarYear << " ME" << endl;
        cout << "Myanmar Month: " << monthToString(data.month) << endl;
        cout << "Day of Month: " << data.dayOfMonth << endl;
        cout << "Moon Phase: " << moonPhaseToString(data.moonPhase) << endl;
        cout << "Year Type: " << yearTypeToString(data.yearType) << endl;
        cout << "Weekday: " << weekdayToString(data.weekday) << endl;
        cout << "Fortnight Day: " << data.fortnightDay << endl;
        cout << endl;
        
        cout << "VERIFICATION:" << endl;
        cout << "=============" << endl;
        cout << "Expected Year: 1338 ME" << endl;
        cout << "Actual Year:   " << data.myanmarYear << " ME" << endl;
        cout << "Year Match: " << (data.myanmarYear == 1338 ? "✓ YES" : "✗ NO") << endl;
        cout << endl;
        
        cout << "Expected Month: Nayon" << endl;
        cout << "Actual Month:   " << monthToString(data.month) << endl;
        cout << "Month Match: " << (data.month == MyanmarMonth::NAYON ? "✓ YES" : "✗ NO") << endl;
        cout << endl;
        
        cout << "Expected Day: 15" << endl;
        cout << "Actual Day:   " << data.dayOfMonth << endl;
        cout << "Day Match: " << (data.dayOfMonth == 15 ? "✓ YES" : "✗ NO") << endl;
        cout << endl;
        
        bool fullMatch = (data.myanmarYear == 1338) && 
                        (data.month == MyanmarMonth::NAYON) && 
                        (data.dayOfMonth == 15);
        
        cout << "OVERALL RESULT: " << (fullMatch ? "✓ MATCH" : "✗ NO MATCH") << endl;
        
    } catch (const exception& e) {
        cout << "Error calculating Myanmar calendar: " << e.what() << endl;
        return 1;
    }

    return 0;
}