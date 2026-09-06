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

int main() {
    MyanmarCalendar calendar;
    if (!calendar.initialize()) {
        cout << "Error: Failed to initialize Myanmar calendar" << endl;
        return 1;
    }
    
    cout << "Myanmar Calendar Verification Test" << endl;
    cout << "===================================" << endl;
    
    // Test specific dates that are known references
    struct TestDate {
        double jd;
        const char* gregorian;
        const char* description;
    };
    
    TestDate testDates[] = {
        {2299161.0, "1582-10-15", "Gregorian calendar adoption"},
        {2299160.0, "1582-10-04", "Last Julian calendar day"},
        {2415021.0, "1900-01-01", "Start of 20th century"},
        {2451545.0, "2000-01-01", "J2000 epoch reference"},
        {2460001.0, "2023-01-01", "Recent reference"},
        {2460416.0, "2024-04-15", "Test from demo"}
    };
    
    for (int i = 0; i < 6; i++) {
        cout << "\n" << testDates[i].description << endl;
        cout << "Gregorian: " << testDates[i].gregorian << endl;
        cout << "Julian Day: " << fixed << setprecision(1) << testDates[i].jd << endl;
        
        try {
            MyanmarCalendarData data = calendar.calculateMyanmarCalendar(testDates[i].jd);
            
            cout << "Myanmar Year: " << data.myanmarYear << " ME" << endl;
            cout << "Myanmar Month: " << monthToString(data.month) << endl;
            cout << "Day of Month: " << data.dayOfMonth << endl;
            cout << "Moon Phase: " << moonPhaseToString(data.moonPhase) << endl;
            cout << "Year Type: " << yearTypeToString(data.yearType) << endl;
            cout << "Fortnight Day: " << data.fortnightDay << endl;
            cout << "Weekday: " << weekdayToString(data.weekday) << endl;
        } catch (const exception& e) {
            cout << "Error: " << e.what() << endl;
        }
    }
    
    return 0;
}
