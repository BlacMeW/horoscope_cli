#pragma once

#define Uses_TApplication
#define Uses_TKeys
#define Uses_TRect
#define Uses_TMenuBar
#define Uses_TSubMenu
#define Uses_TMenuItem
#define Uses_TStatusLine
#define Uses_TStatusItem
#define Uses_TStatusDef
#define Uses_TDeskTop
#define Uses_TDialog
#define Uses_TStaticText
#define Uses_TButton
#define Uses_TInputLine
#define Uses_TLabel
#define Uses_TRadioButtons
#define Uses_TSItem
#define Uses_TCheckBoxes
#define Uses_TEvent
#define Uses_TWindow
#define Uses_MsgBox
#include <tvision/tv.h>

#include "astro_types.h"
#include "horoscope_calculator.h"
#include "location_manager.h"
#include <string>

namespace AstroTui {

// Command IDs
const ushort cmNewChart        = 200;
const ushort cmCityPreset      = 201;
const ushort cmExportFile      = 202;
const ushort cmPlanetPositions  = 216;
const ushort cmHinduPanchang   = 220;
const ushort cmHinduMonth      = 221;
const ushort cmMyanmarCalendar = 222;
const ushort cmMyanmarMonth    = 223;
const ushort cmChineseCalendar = 224;
const ushort cmAspectGrid      = 230;
const ushort cmConjunctions    = 231;
const ushort cmEclipses        = 232;
const ushort cmKPTable         = 233;
const ushort cmKPTransitions   = 234;
const ushort cmEphemerisTable  = 235;
const ushort cmAstroCoordinates = 236;
const ushort cmEphemerisMonthly = 237;
const ushort cmEphemerisYearly  = 238;
const ushort cmEphemerisTransits= 239;
const ushort cmTileWindows     = 240;
const ushort cmCascadeWindows  = 241;
const ushort cmAstroCalendarDay = 242;
const ushort cmAstroCalendarMonth = 243;
const ushort cmHelpAbout       = 250;
const ushort cmHelpLegend      = 251;
const ushort cmHelpShortcuts   = 252;
const ushort cmThemeDialog      = 260;
const ushort cmThemeTurboCpp     = 261;
const ushort cmThemeDark        = 262;
const ushort cmThemeBW          = 263;
const ushort cmEphemerisDialog  = 270;
const ushort cmCalendarDialog   = 271;
const ushort cmMonthlyDialog    = 272;
const ushort cmTransitDialog    = 273;

class HoroscopeTuiApp : public TApplication {
public:
    Astro::BirthData currentBirthData;
    std::string currentCityName;
    Astro::ZodiacMode currentZodiacMode;
    Astro::AyanamsaType currentAyanamsa;
    Astro::HouseSystem currentHouseSystem;
    int currentTheme;

    HoroscopeTuiApp(int initialTheme = 0);
    virtual ~HoroscopeTuiApp();

    virtual void handleEvent(TEvent& event) override;
    virtual TPalette& getPalette() const override;
    static TMenuBar* initMenuBar(TRect r);
    static TStatusLine* initStatusLine(TRect r);

    // Theme methods
    void setTheme(int themeId);
    void showThemeDialog();

    // Interactive Dialogs
    void showNewChartDialog();
    void showCityPresetDialog();
    void showExportDialog();
    void showEphemerisDialog();
    void showMonthlyDialog();
    void showCalendarQueryDialog();
    void showTransitDialog();
    bool promptDateLocation(const char* title, Astro::BirthData& data, std::string& cityName, bool showTime = true);
    bool promptYearMonth(const char* title, int& year, int& month);
    bool promptDateRange(const char* title, Astro::BirthData& start, int& days);

    // Ephemeris, Calendar & Table Views
    void showPlanetaryPositions();
    void showHinduPanchang();
    void showHinduMonth();
    void showMyanmarCalendar();
    void showMyanmarMonth();
    void showChineseCalendar();
    void showAspectGrid();
    void showConjunctions();
    void showEclipses();
    void showKPTable();
    void showKPTransitions();
    void showEphemerisTable();
    void showAstroCoordinates();
    void showEphemerisMonthly();
    void showEphemerisYearly();
    void showEphemerisTransits();
    void showAstroCalendarDay();
    void showAstroCalendarMonth();

    // Help Dialogs
    void showHelpAbout();
    void showHelpLegend();
    void showHelpShortcuts();

    // Helper to open a scrollable text window
    void openWindow(const std::string& title, const std::string& content, TRect bounds = TRect(0, 0, 0, 0));

private:
    int windowCount;
    std::string lastExportText;
    std::string lastExportTitle;
};

// Global launcher function
int runTuiApplication(int initialTheme = 0);

} // namespace AstroTui
