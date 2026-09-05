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
const ushort cmWesternWheel    = 210;
const ushort cmWesternRect     = 211;
const ushort cmVedicNorth      = 212;
const ushort cmVedicSouth      = 213;
const ushort cmVedicEast       = 214;
const ushort cmSolarSystem     = 215;
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
const ushort cmTileWindows     = 240;
const ushort cmCascadeWindows  = 241;
const ushort cmHelpAbout       = 250;
const ushort cmHelpLegend      = 251;
const ushort cmHelpShortcuts   = 252;

class HoroscopeTuiApp : public TApplication {
public:
    Astro::BirthData currentBirthData;
    std::string currentCityName;
    Astro::ZodiacMode currentZodiacMode;
    Astro::AyanamsaType currentAyanamsa;
    Astro::HouseSystem currentHouseSystem;

    HoroscopeTuiApp();
    virtual ~HoroscopeTuiApp();

    virtual void handleEvent(TEvent& event) override;
    static TMenuBar* initMenuBar(TRect r);
    static TStatusLine* initStatusLine(TRect r);

    // Interactive Dialogs
    void showNewChartDialog();
    void showCityPresetDialog();
    void showExportDialog();

    // Chart & Table Views
    void showWesternWheel();
    void showWesternRect();
    void showVedicNorth();
    void showVedicSouth();
    void showVedicEast();
    void showSolarSystem();
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
int runTuiApplication();

} // namespace AstroTui
