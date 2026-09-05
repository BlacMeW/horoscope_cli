#include "tui/horoscope_tui.h"
#include "tui/chart_view.h"
#include "birth_chart.h"
#include "horoscope_calculator.h"
#include "western_chart_drawer.h"
#include "hindu_calendar.h"
#include "hindu_monthly_calendar.h"
#include "myanmar_calendar.h"
#include "myanmar_monthly_calendar.h"
#include "chinese_calendar.h"
#include "kp_system.h"
#include "conjunction_calculator.h"
#include "eclipse_calculator.h"
#include "ephemeris_table.h"
#include "astro_calendar.h"
#include "location_manager.h"
#include "swephexp.h"

#include <sstream>
#include <fstream>
#include <iomanip>
#include <cstdio>

namespace AstroTui {

// 135-byte theme palettes for Turbo Vision
// 1. Turbo C++ 3.0 IDE Classic: Authentic Borland Deep Dark Navy Blue (ရိုးရိုး အပြာရင့်) editor & desktop, light gray menu/status
static const char cpTurboCpp[] =
    "\x11\x70\x78\x74\x1F\x18\x1E\x17\x1F\x1E\x1F\x17\x1F\x10\x1F" /* 1-15: Desktop (solid dark blue), StatusLine, MenuBar, BlueWindow */
    "\x17\x1F\x1E\x1F\x1F\x10\x1F\x1F\x70\x7F\x7E\x1F\x1F\x70\x7F\x7E" /* 16-31: Win2 (Deep Blue), GrayWin */
    "\x70\x7F\x74\x1F\x1F\x70\x70\x7F\x74\x1F\x1E\x1E\x70\x1F\x74\x70" /* 32-47: Dialogs & Buttons */
    "\x1F\x1E\x0F\x1F\x1F\x70\x74\x70\x70\x70\x1F\x1E\x70\x1F\x78\x00" /* 48-63: Controls, InputLines */
    "\x17\x1F\x1E\x71\x71\x1E\x17\x1F\x1E\x1F\x1E\x1F\x78\x1E\x10\x10" /* 64-79 */
    "\x1F\x1E\x70\x1F\x7A\x70\x12\x70\x70\x70\x1F\x1E\x70\x1F\x78\x00" /* 80-95 */
    "\x17\x1F\x1A\x1F\x1F\x1E\x10\x1F\x1E\x1F\x1E\x1F\x78\x1E\x10\x70" /* 96-111 */
    "\x7F\x7E\x1F\x1F\x1A\x70\x12\x70\x71\x70\x1F\x7E\x71\x1F\x78\x00" /* 112-127 */
    "\x17\x1F\x1A\x1F\x1F\x10\x1E\x1E";                                 /* 128-135 */

// 2. Modern Dark Slate: High contrast dark theme, zero muddy colors
static const char cpModernDark[] =
    "\x08\x07\x08\x0E\x1F\x18\x1E\x08\x0B\x0E\x08\x0B\x0F\x30\x0F" /* 1-15: Desktop, Menu/Status, Win1 */
    "\x08\x0B\x0F\x08\x08\x0F\x0E\x0B\x08\x0F\x0E\x08\x08\x0F\x0F\x0E" /* 16-31: Win2, Win3 */
    "\x08\x0F\x0E\x08\x08\x07\x08\x0F\x0E\x1F\x1B\x1F\x08\x1E\x07\x10" /* 32-47: Dialog */
    "\x1F\x1E\x0F\x1F\x0B\x10\x71\x08\x08\x10\x1F\x1E\x08\x08\x08\x00" /* 48-63: Controls */
    "\x08\x0F\x0B\x07\x07\x0F\x08\x0F\x0E\x1F\x1B\x1F\x08\x1E\x10\x10" /* 64-79 */
    "\x1F\x1E\x07\x1F\x0A\x10\x12\x08\x08\x10\x1F\x1E\x08\x08\x08\x00" /* 80-95 */
    "\x07\x0F\x0B\x08\x08\x0E\x10\x1F\x1E\x1F\x1B\x1F\x08\x1E\x10\x07" /* 96-111 */
    "\x0F\x0E\x0F\x1F\x0A\x10\x12\x08\x07\x08\x1F\x0E\x07\x08\x08\x00" /* 112-127 */
    "\x07\x0F\x0B\x08\x08\x10\x1E\x0E";                                 /* 128-135 */

HoroscopeTuiApp::HoroscopeTuiApp(int initialTheme) :
    TProgInit(&HoroscopeTuiApp::initStatusLine,
              &HoroscopeTuiApp::initMenuBar,
              &HoroscopeTuiApp::initDeskTop),
    currentCityName("Yangon, Myanmar"),
    currentZodiacMode(Astro::ZodiacMode::TROPICAL),
    currentAyanamsa(Astro::AyanamsaType::LAHIRI),
    currentHouseSystem(Astro::HouseSystem::PLACIDUS),
    currentTheme(initialTheme),
    windowCount(0)
{
    if (currentTheme == 2) {
        appPalette = apBlackWhite;
    } else {
        appPalette = apColor;
    }
    // Initialize with real-time system clock
    std::time_t now = std::time(nullptr);
    std::tm* lt = std::localtime(&now);
    if (lt) {
        currentBirthData.year = lt->tm_year + 1900;
        currentBirthData.month = lt->tm_mon + 1;
        currentBirthData.day = lt->tm_mday;
        currentBirthData.hour = lt->tm_hour;
        currentBirthData.minute = lt->tm_min;
        currentBirthData.second = lt->tm_sec;
    } else {
        currentBirthData.year = 2026;
        currentBirthData.month = 9;
        currentBirthData.day = 6;
        currentBirthData.hour = 12;
        currentBirthData.minute = 0;
        currentBirthData.second = 0;
    }
    currentBirthData.latitude = 16.8661;
    currentBirthData.longitude = 96.1951;
    currentBirthData.timezone = 6.5;

    // Queue New Chart dialog on startup so the user fills in their data first
    TEvent ev;
    ev.what = evCommand;
    ev.message.command = cmNewChart;
    putEvent(ev);
}

HoroscopeTuiApp::~HoroscopeTuiApp() {
}

TMenuBar* HoroscopeTuiApp::initMenuBar(TRect r) {
    r.b.y = r.a.y + 1;
    return new TMenuBar(r,
        *new TSubMenu("~F~ile", kbAltF) +
            *new TMenuItem("~N~ew Date & Time...", cmNewChart, kbF2, hcNoContext, "F2") +
            *new TMenuItem("~L~ocation Presets...", cmCityPreset, kbF3, hcNoContext, "F3") +
            *new TMenuItem("~E~xport Active View...", cmExportFile, kbCtrlS, hcNoContext, "Ctrl+S") +
            newLine() +
            *new TMenuItem("E~x~it", cmQuit, kbAltX, hcNoContext, "Alt+X") +
        *new TSubMenu("~E~phemeris", kbAltE) +
            *new TMenuItem("~C~ustom Ephemeris Generator Form...", cmEphemerisDialog, kbNoKey) +
            newLine() +
            *new TMenuItem("~P~lanetary Positions & Houses Table", cmPlanetPositions, kbF4, hcNoContext, "F4") +
            *new TMenuItem("~M~onthly Ephemeris (30-Day Daily Table)", cmEphemerisMonthly, kbNoKey) +
            *new TMenuItem("~Y~early Ephemeris (12-Month Table)", cmEphemerisYearly, kbNoKey) +
            *new TMenuItem("~1~4-Day Ephemeris Table", cmEphemerisTable, kbNoKey) +
            newLine() +
            *new TMenuItem("~T~ransit Planets vs Natal Query...", cmTransitDialog, kbNoKey) +
            *new TMenuItem("Astronomical ~D~etails & Coordinates", cmAstroCoordinates, kbNoKey) +
        *new TSubMenu("~C~alendars", kbAltC) +
            *new TMenuItem("~Q~uery Calendar for Date & City...", cmCalendarDialog, kbNoKey) +
            *new TMenuItem("~M~onthly Calendar Query Form...", cmMonthlyDialog, kbNoKey) +
            newLine() +
            *new TMenuItem("~U~nified Astro-Calendar (Daily View)", cmAstroCalendarDay, kbNoKey) +
            *new TMenuItem("Unified Monthly ~A~stro-Calendar", cmAstroCalendarMonth, kbNoKey) +
            newLine() +
            *new TMenuItem("~H~indu Daily Panchang", cmHinduPanchang, kbNoKey) +
            *new TMenuItem("Hindu Monthly ~P~anchang Calendar", cmHinduMonth, kbNoKey) +
            newLine() +
            *new TMenuItem("M~y~anmar Daily Calendar (မြန်မာပြက္ခဒိန်)", cmMyanmarCalendar, kbNoKey) +
            *new TMenuItem("Myanmar Mo~n~thly Calendar", cmMyanmarMonth, kbNoKey) +
            newLine() +
            *new TMenuItem("Chinese ~S~exagenary Calendar (中国农历)", cmChineseCalendar, kbNoKey) +
        *new TSubMenu("~A~nalysis", kbAltA) +
            *new TMenuItem("Planetary ~A~spect Grid", cmAspectGrid, kbCtrlA, hcNoContext, "Ctrl+A") +
            *new TMenuItem("Planetary ~C~onjunctions (1 Year)", cmConjunctions, kbNoKey) +
            *new TMenuItem("Solar & Lunar ~E~clipses (2 Years)", cmEclipses, kbNoKey) +
            newLine() +
            *new TMenuItem("~K~P Sub-Lord 5-Levels Table", cmKPTable, kbF5, hcNoContext, "F5") +
            *new TMenuItem("KP Planetary ~T~ransitions", cmKPTransitions, kbNoKey) +
        *new TSubMenu("~W~indow", kbAltW) +
            *new TMenuItem("~T~ile Windows", cmTileWindows, kbShiftF5, hcNoContext, "Shift+F5") +
            *new TMenuItem("~C~ascade Windows", cmCascadeWindows, kbShiftF4, hcNoContext, "Shift+F4") +
            *new TMenuItem("Cl~o~se Active Window", cmClose, kbAltF3, hcNoContext, "Alt+F3") +
            *new TMenuItem("~S~ize / Move", cmResize, kbCtrlF5, hcNoContext, "Ctrl+F5") +
            *new TMenuItem("~Z~oom / Maximize", cmZoom, kbF6, hcNoContext, "F6") +
        *new TSubMenu("~T~heme", kbAltT) +
            *new TMenuItem("~1~ Turbo C++ 3.0 Classic", cmThemeTurboCpp, kbNoKey) +
            *new TMenuItem("~2~ Modern Dark Slate", cmThemeDark, kbNoKey) +
            *new TMenuItem("~3~ Monochrome High Contrast B&W", cmThemeBW, kbNoKey) +
            newLine() +
            *new TMenuItem("~P~alette Selector...", cmThemeDialog, kbF9, hcNoContext, "F9") +
        *new TSubMenu("Hel~p~", kbAltP) +
            *new TMenuItem("~A~bout Horoscope TUI", cmHelpAbout, kbNoKey) +
            *new TMenuItem("Astrological ~L~egend", cmHelpLegend, kbNoKey) +
            *new TMenuItem("Keyboard & Mouse ~S~hortcuts", cmHelpShortcuts, kbF1, hcNoContext, "F1")
    );
}

TStatusLine* HoroscopeTuiApp::initStatusLine(TRect r) {
    r.a.y = r.b.y - 1;
    return new TStatusLine(r,
        *new TStatusDef(0, 0xFFFF) +
            *new TStatusItem("~Alt+X~ Exit", kbAltX, cmQuit) +
            *new TStatusItem("~F2~ Date", kbF2, cmNewChart) +
            *new TStatusItem("~F4~ Planets", kbF4, cmPlanetPositions) +
            *new TStatusItem("~F5~ KP Table", kbF5, cmKPTable) +
            *new TStatusItem("~F9~ Theme", kbF9, cmThemeDialog) +
            *new TStatusItem("~Ctrl+A~ Aspects", kbCtrlA, cmAspectGrid) +
            *new TStatusItem("~F10~ Menu", kbF10, cmMenu) +
            *new TStatusItem("~Alt+F3~ Close", kbAltF3, cmClose)
    );
}

void HoroscopeTuiApp::handleEvent(TEvent& event) {
    TApplication::handleEvent(event);
    if (event.what == evCommand) {
        switch (event.message.command) {
            case cmNewChart:
                showNewChartDialog();
                clearEvent(event);
                break;
            case cmCityPreset:
                showCityPresetDialog();
                clearEvent(event);
                break;
            case cmExportFile:
                showExportDialog();
                clearEvent(event);
                break;
            case cmEphemerisDialog:
                showEphemerisDialog();
                clearEvent(event);
                break;
            case cmCalendarDialog:
                showCalendarQueryDialog();
                clearEvent(event);
                break;
            case cmMonthlyDialog:
                showMonthlyDialog();
                clearEvent(event);
                break;
            case cmTransitDialog:
                showTransitDialog();
                clearEvent(event);
                break;
            case cmEphemerisMonthly:
                showEphemerisMonthly();
                clearEvent(event);
                break;
            case cmEphemerisYearly:
                showEphemerisYearly();
                clearEvent(event);
                break;
            case cmEphemerisTransits:
                showEphemerisTransits();
                clearEvent(event);
                break;
            case cmAstroCalendarDay:
                showAstroCalendarDay();
                clearEvent(event);
                break;
            case cmAstroCalendarMonth:
                showAstroCalendarMonth();
                clearEvent(event);
                break;
            case cmPlanetPositions:
                showPlanetaryPositions();
                clearEvent(event);
                break;
            case cmHinduPanchang:
                showHinduPanchang();
                clearEvent(event);
                break;
            case cmHinduMonth:
                showHinduMonth();
                clearEvent(event);
                break;
            case cmMyanmarCalendar:
                showMyanmarCalendar();
                clearEvent(event);
                break;
            case cmMyanmarMonth:
                showMyanmarMonth();
                clearEvent(event);
                break;
            case cmChineseCalendar:
                showChineseCalendar();
                clearEvent(event);
                break;
            case cmAspectGrid:
                showAspectGrid();
                clearEvent(event);
                break;
            case cmConjunctions:
                showConjunctions();
                clearEvent(event);
                break;
            case cmEclipses:
                showEclipses();
                clearEvent(event);
                break;
            case cmKPTable:
                showKPTable();
                clearEvent(event);
                break;
            case cmKPTransitions:
                showKPTransitions();
                clearEvent(event);
                break;
            case cmEphemerisTable:
                showEphemerisTable();
                clearEvent(event);
                break;
            case cmAstroCoordinates:
                showAstroCoordinates();
                clearEvent(event);
                break;
            case cmTileWindows:
                if (deskTop) deskTop->tile(deskTop->getExtent());
                clearEvent(event);
                break;
            case cmCascadeWindows:
                if (deskTop) deskTop->cascade(deskTop->getExtent());
                clearEvent(event);
                break;
            case cmHelpAbout:
                showHelpAbout();
                clearEvent(event);
                break;
            case cmHelpLegend:
                showHelpLegend();
                clearEvent(event);
                break;
            case cmHelpShortcuts:
                showHelpShortcuts();
                clearEvent(event);
                break;
            case cmThemeTurboCpp:
                setTheme(0);
                clearEvent(event);
                break;
            case cmThemeDark:
                setTheme(1);
                clearEvent(event);
                break;
            case cmThemeBW:
                setTheme(2);
                clearEvent(event);
                break;
            case cmThemeDialog:
                showThemeDialog();
                clearEvent(event);
                break;
            default:
                break;
        }
    }
}

void HoroscopeTuiApp::openWindow(const std::string& title, const std::string& content, TRect bounds) {
    if (!deskTop) return;

    lastExportTitle = title;
    lastExportText = content;

    TRect extent = deskTop->getExtent();
    if (bounds.a.x == 0 && bounds.b.x == 0) {
        int deskW = extent.b.x - extent.a.x;
        int deskH = extent.b.y - extent.a.y;

        // Generous sizing so full chart wheels and tables fit comfortably without clipping
        int targetW = std::max(84, (deskW * 88) / 100);
        int targetH = std::max(28, (deskH * 88) / 100);
        int w = std::min(deskW - 2, targetW);
        int h = std::min(deskH - 2, targetH);

        // Center on screen with slight offset for multiple windows
        int x1 = extent.a.x + std::max(0, (deskW - w) / 2);
        int y1 = extent.a.y + std::max(0, (deskH - h) / 2);
        if (windowCount > 0) {
            int offset = (windowCount % 5) * 2;
            x1 = std::min(extent.b.x - w - 1, x1 + offset);
            y1 = std::min(extent.b.y - h - 1, y1 + offset);
        }
        int x2 = std::min(extent.b.x, x1 + w);
        int y2 = std::min(extent.b.y, y1 + h);
        bounds = TRect(x1, y1, x2, y2);
        windowCount++;
    }

    TTextWindow* win = new TTextWindow(bounds, title.c_str(), content);
    deskTop->insert(win);
}

void HoroscopeTuiApp::showNewChartDialog() {
    TDialog* d = new TDialog(TRect(10, 2, 70, 22), "Enter Birth & Calculation Data");

    char dateBuf[32];
    snprintf(dateBuf, sizeof(dateBuf), "%04d-%02d-%02d",
             currentBirthData.year, currentBirthData.month, currentBirthData.day);
    char timeBuf[32];
    snprintf(timeBuf, sizeof(timeBuf), "%02d:%02d:%02d",
             currentBirthData.hour, currentBirthData.minute, currentBirthData.second);
    char latBuf[32];
    snprintf(latBuf, sizeof(latBuf), "%.4f", currentBirthData.latitude);
    char lonBuf[32];
    snprintf(lonBuf, sizeof(lonBuf), "%.4f", currentBirthData.longitude);
    char tzBuf[32];
    snprintf(tzBuf, sizeof(tzBuf), "%.1f", currentBirthData.timezone);
    char cityBuf[64];
    snprintf(cityBuf, sizeof(cityBuf), "%s", currentCityName.c_str());

    TInputLine* inDate = new TInputLine(TRect(22, 2, 44, 3), 20);
    inDate->setData(dateBuf);
    d->insert(inDate);
    d->insert(new TLabel(TRect(3, 2, 21, 3), "~D~ate (Y-M-D):", inDate));

    TInputLine* inTime = new TInputLine(TRect(22, 4, 44, 5), 20);
    inTime->setData(timeBuf);
    d->insert(inTime);
    d->insert(new TLabel(TRect(3, 4, 21, 5), "~T~ime (H:M:S):", inTime));

    TInputLine* inCity = new TInputLine(TRect(22, 6, 56, 7), 50);
    inCity->setData(cityBuf);
    d->insert(inCity);
    d->insert(new TLabel(TRect(3, 6, 21, 7), "~C~ity / Place:", inCity));

    TInputLine* inLat = new TInputLine(TRect(22, 8, 35, 9), 12);
    inLat->setData(latBuf);
    d->insert(inLat);
    d->insert(new TLabel(TRect(3, 8, 21, 9), "~L~atitude (°):", inLat));

    TInputLine* inLon = new TInputLine(TRect(44, 8, 56, 9), 12);
    inLon->setData(lonBuf);
    d->insert(inLon);
    d->insert(new TLabel(TRect(38, 8, 43, 9), "L~o~n:", inLon));

    TInputLine* inTz = new TInputLine(TRect(22, 10, 35, 11), 12);
    inTz->setData(tzBuf);
    d->insert(inTz);
    d->insert(new TLabel(TRect(3, 10, 21, 11), "Time~z~one:", inTz));

    TSItem* zItems = new TSItem("Tropical (Western)",
                     new TSItem("Sidereal (Vedic Lahiri)", nullptr));
    TRadioButtons* rbZodiac = new TRadioButtons(TRect(22, 12, 56, 14), zItems);
    ushort zVal = (currentZodiacMode == Astro::ZodiacMode::SIDEREAL) ? 1 : 0;
    rbZodiac->setData(&zVal);
    d->insert(rbZodiac);
    d->insert(new TLabel(TRect(3, 12, 21, 13), "~Z~odiac Mode:", rbZodiac));

    TSItem* hItems = new TSItem("Placidus",
                     new TSItem("Whole Sign",
                     new TSItem("Koch",
                     new TSItem("Equal", nullptr))));
    TRadioButtons* rbHouse = new TRadioButtons(TRect(22, 15, 56, 17), hItems);
    ushort hVal = 0;
    if (currentHouseSystem == Astro::HouseSystem::WHOLE_SIGN) hVal = 1;
    else if (currentHouseSystem == Astro::HouseSystem::KOCH) hVal = 2;
    else if (currentHouseSystem == Astro::HouseSystem::EQUAL) hVal = 3;
    rbHouse->setData(&hVal);
    d->insert(rbHouse);
    d->insert(new TLabel(TRect(3, 15, 21, 16), "~H~ouse System:", rbHouse));

    d->insert(new TButton(TRect(15, 18, 32, 20), "~C~alculate", cmOK, bfDefault));
    d->insert(new TButton(TRect(36, 18, 49, 20), "Cancel", cmCancel, bfNormal));

    if (deskTop->execView(d) == cmOK) {
        inDate->getData(dateBuf);
        inTime->getData(timeBuf);
        inLat->getData(latBuf);
        inLon->getData(lonBuf);
        inTz->getData(tzBuf);
        inCity->getData(cityBuf);
        rbZodiac->getData(&zVal);
        rbHouse->getData(&hVal);

        int y = 2000, m = 1, day = 1, h = 12, min = 0, s = 0;
        sscanf(dateBuf, "%d-%d-%d", &y, &m, &day);
        sscanf(timeBuf, "%d:%d:%d", &h, &min, &s);
        double lat = atof(latBuf);
        double lon = atof(lonBuf);
        double tz = atof(tzBuf);

        currentBirthData.year = y;
        currentBirthData.month = m;
        currentBirthData.day = day;
        currentBirthData.hour = h;
        currentBirthData.minute = min;
        currentBirthData.second = s;
        currentBirthData.latitude = lat;
        currentBirthData.longitude = lon;
        currentBirthData.timezone = tz;
        currentCityName = cityBuf;

        currentZodiacMode = (zVal == 1) ? Astro::ZodiacMode::SIDEREAL : Astro::ZodiacMode::TROPICAL;
        if (hVal == 1) currentHouseSystem = Astro::HouseSystem::WHOLE_SIGN;
        else if (hVal == 2) currentHouseSystem = Astro::HouseSystem::KOCH;
        else if (hVal == 3) currentHouseSystem = Astro::HouseSystem::EQUAL;
        else currentHouseSystem = Astro::HouseSystem::PLACIDUS;

        showPlanetaryPositions();
    }
    destroy(d);
}

bool HoroscopeTuiApp::promptDateLocation(const char* title, Astro::BirthData& data, std::string& cityName, bool showTime) {
    TDialog* d = new TDialog(TRect(12, 4, 68, showTime ? 18 : 16), title);

    char dateBuf[32];
    snprintf(dateBuf, sizeof(dateBuf), "%04d-%02d-%02d", data.year, data.month, data.day);
    char timeBuf[32];
    snprintf(timeBuf, sizeof(timeBuf), "%02d:%02d:%02d", data.hour, data.minute, data.second);
    char cityBuf[64];
    snprintf(cityBuf, sizeof(cityBuf), "%s", cityName.c_str());
    char latBuf[32];
    snprintf(latBuf, sizeof(latBuf), "%.4f", data.latitude);
    char lonBuf[32];
    snprintf(lonBuf, sizeof(lonBuf), "%.4f", data.longitude);

    int yPos = 2;
    TInputLine* inDate = new TInputLine(TRect(22, yPos, 44, yPos + 1), 20);
    inDate->setData(dateBuf);
    d->insert(inDate);
    d->insert(new TLabel(TRect(3, yPos, 21, yPos + 1), "~D~ate (Y-M-D):", inDate));
    yPos += 2;

    TInputLine* inTime = nullptr;
    if (showTime) {
        inTime = new TInputLine(TRect(22, yPos, 44, yPos + 1), 20);
        inTime->setData(timeBuf);
        d->insert(inTime);
        d->insert(new TLabel(TRect(3, yPos, 21, yPos + 1), "~T~ime (H:M:S):", inTime));
        yPos += 2;
    }

    TInputLine* inCity = new TInputLine(TRect(22, yPos, 52, yPos + 1), 45);
    inCity->setData(cityBuf);
    d->insert(inCity);
    d->insert(new TLabel(TRect(3, yPos, 21, yPos + 1), "~C~ity / Place:", inCity));
    yPos += 2;

    TInputLine* inLat = new TInputLine(TRect(22, yPos, 36, yPos + 1), 12);
    inLat->setData(latBuf);
    d->insert(inLat);
    d->insert(new TLabel(TRect(3, yPos, 21, yPos + 1), "~L~atitude (°):", inLat));

    TInputLine* inLon = new TInputLine(TRect(44, yPos, 54, yPos + 1), 12);
    inLon->setData(lonBuf);
    d->insert(inLon);
    d->insert(new TLabel(TRect(38, yPos, 43, yPos + 1), "L~o~n:", inLon));
    yPos += 2;

    d->insert(new TButton(TRect(14, yPos + 1, 28, yPos + 3), "~V~iew", cmOK, bfDefault));
    d->insert(new TButton(TRect(32, yPos + 1, 44, yPos + 3), "Cancel", cmCancel, bfNormal));

    bool ok = (deskTop->execView(d) == cmOK);
    if (ok) {
        inDate->getData(dateBuf);
        if (inTime) inTime->getData(timeBuf);
        inCity->getData(cityBuf);
        inLat->getData(latBuf);
        inLon->getData(lonBuf);

        int y = 2000, m = 1, day = 1, h = 12, min = 0, s = 0;
        sscanf(dateBuf, "%d-%d-%d", &y, &m, &day);
        if (showTime) sscanf(timeBuf, "%d:%d:%d", &h, &min, &s);

        data.year = y; data.month = m; data.day = day;
        if (showTime) { data.hour = h; data.minute = min; data.second = s; }
        cityName = cityBuf;
        data.latitude = atof(latBuf);
        data.longitude = atof(lonBuf);
    }
    destroy(d);
    return ok;
}

bool HoroscopeTuiApp::promptYearMonth(const char* title, int& year, int& month) {
    TDialog* d = new TDialog(TRect(15, 6, 65, 17), title);

    char yearBuf[16];
    snprintf(yearBuf, sizeof(yearBuf), "%d", year);
    char monthBuf[16];
    snprintf(monthBuf, sizeof(monthBuf), "%d", month);

    TInputLine* inYear = new TInputLine(TRect(24, 2, 38, 3), 10);
    inYear->setData(yearBuf);
    d->insert(inYear);
    d->insert(new TLabel(TRect(4, 2, 22, 3), "~Y~ear (YYYY):", inYear));

    TInputLine* inMonth = new TInputLine(TRect(24, 4, 32, 5), 5);
    inMonth->setData(monthBuf);
    d->insert(inMonth);
    d->insert(new TLabel(TRect(4, 4, 22, 5), "~M~onth (1-12):", inMonth));

    d->insert(new TButton(TRect(12, 7, 26, 9), "~G~enerate", cmOK, bfDefault));
    d->insert(new TButton(TRect(28, 7, 40, 9), "Cancel", cmCancel, bfNormal));

    bool ok = (deskTop->execView(d) == cmOK);
    if (ok) {
        inYear->getData(yearBuf);
        inMonth->getData(monthBuf);
        int y = atoi(yearBuf);
        int m = atoi(monthBuf);
        if (y > 0) year = y;
        if (m >= 1 && m <= 12) month = m;
    }
    destroy(d);
    return ok;
}

bool HoroscopeTuiApp::promptDateRange(const char* title, Astro::BirthData& start, int& days) {
    TDialog* d = new TDialog(TRect(14, 5, 66, 17), title);

    char dateBuf[32];
    snprintf(dateBuf, sizeof(dateBuf), "%04d-%02d-%02d", start.year, start.month, start.day);
    char daysBuf[16];
    snprintf(daysBuf, sizeof(daysBuf), "%d", days);

    TInputLine* inDate = new TInputLine(TRect(26, 2, 46, 3), 20);
    inDate->setData(dateBuf);
    d->insert(inDate);
    d->insert(new TLabel(TRect(3, 2, 25, 3), "~S~tart Date (Y-M-D):", inDate));

    TInputLine* inDays = new TInputLine(TRect(26, 4, 38, 5), 10);
    inDays->setData(daysBuf);
    d->insert(inDays);
    d->insert(new TLabel(TRect(3, 4, 25, 5), "Duration (~D~ays):", inDays));

    d->insert(new TButton(TRect(12, 7, 26, 9), "~C~alculate", cmOK, bfDefault));
    d->insert(new TButton(TRect(29, 7, 41, 9), "Cancel", cmCancel, bfNormal));

    bool ok = (deskTop->execView(d) == cmOK);
    if (ok) {
        inDate->getData(dateBuf);
        inDays->getData(daysBuf);
        int y = 2000, m = 1, day = 1;
        sscanf(dateBuf, "%d-%d-%d", &y, &m, &day);
        int dCount = atoi(daysBuf);
        if (y > 0) start.year = y;
        if (m >= 1 && m <= 12) start.month = m;
        if (day >= 1 && day <= 31) start.day = day;
        if (dCount > 0) days = dCount;
    }
    destroy(d);
    return ok;
}

void HoroscopeTuiApp::showCityPresetDialog() {
    struct CityPreset {
        const char* name;
        double lat;
        double lon;
        double tz;
    };

    static const CityPreset presets[] = {
        {"Yangon, Myanmar", 16.8661, 96.1951, 6.5},
        {"Mandalay, Myanmar", 21.9588, 96.0891, 6.5},
        {"Bangkok, Thailand", 13.7563, 100.5018, 7.0},
        {"Kolkata, India", 22.5726, 88.3639, 5.5},
        {"New Delhi, India", 28.6139, 77.2090, 5.5},
        {"Bengaluru, India", 12.9716, 77.5946, 5.5},
        {"Tokyo, Japan", 35.6762, 139.6503, 9.0},
        {"London, UK", 51.5074, -0.1278, 0.0},
        {"New York, USA", 40.7128, -74.0060, -5.0},
        {"Los Angeles, USA", 34.0522, -118.2437, -8.0}
    };

    TDialog* d = new TDialog(TRect(15, 3, 65, 19), "Select City Preset");

    TSItem* items = nullptr;
    for (int i = 9; i >= 0; --i) {
        items = new TSItem(presets[i].name, items);
    }

    TRadioButtons* rb = new TRadioButtons(TRect(4, 2, 45, 12), items);
    d->insert(rb);

    d->insert(new TButton(TRect(10, 13, 23, 15), "~A~pply", cmOK, bfDefault));
    d->insert(new TButton(TRect(27, 13, 40, 15), "Cancel", cmCancel, bfNormal));

    if (deskTop->execView(d) == cmOK) {
        ushort val = 0;
        rb->getData(&val);
        if (val < 10) {
            const auto& p = presets[val];
            currentCityName = p.name;
            currentBirthData.latitude = p.lat;
            currentBirthData.longitude = p.lon;
            currentBirthData.timezone = p.tz;
            showPlanetaryPositions();
        }
    }
    destroy(d);
}

void HoroscopeTuiApp::showExportDialog() {
    TDialog* d = new TDialog(TRect(15, 6, 65, 15), "Export View to File");

    char fnameBuf[64] = "horoscope_export.txt";
    TInputLine* inFname = new TInputLine(TRect(18, 2, 45, 3), 40);
    inFname->setData(fnameBuf);
    d->insert(inFname);
    d->insert(new TLabel(TRect(3, 2, 17, 3), "~F~ilename:", inFname));

    d->insert(new TButton(TRect(10, 5, 23, 7), "~S~ave", cmOK, bfDefault));
    d->insert(new TButton(TRect(26, 5, 39, 7), "Cancel", cmCancel, bfNormal));

    if (deskTop->execView(d) == cmOK) {
        inFname->getData(fnameBuf);
        std::ofstream out(fnameBuf);
        if (out) {
            out << "=== " << lastExportTitle << " ===\n\n";
            out << lastExportText << "\n";
            out.close();
            messageBox(mfInformation | mfOKButton, "Successfully exported to %s", fnameBuf);
        } else {
            messageBox(mfError | mfOKButton, "Failed to write file %s", fnameBuf);
        }
    }
    destroy(d);
}

static Astro::BirthChart computeCurrentChart(const Astro::BirthData& data,
                                            Astro::HouseSystem houseSystem,
                                            Astro::ZodiacMode zodiacMode,
                                            Astro::AyanamsaType ayanamsa) {
    Astro::HoroscopeCalculator calc;
    calc.initialize();
    Astro::BirthChart chart;
    calc.calculateBirthChart(data, houseSystem, zodiacMode, ayanamsa, chart);
    return chart;
}

static Astro::BirthData addCalendarDays(const Astro::BirthData& start, double days) {
    double h_dec = start.hour + start.minute / 60.0 + start.second / 3600.0;
    double jd = swe_julday(start.year, start.month, start.day, h_dec, SE_GREG_CAL) + days;
    int y, m, d;
    double h;
    swe_revjul(jd, SE_GREG_CAL, &y, &m, &d, &h);
    Astro::BirthData res = start;
    res.year = y;
    res.month = m;
    res.day = d;
    return res;
}

void HoroscopeTuiApp::showEphemerisDialog() {
    TDialog* d = new TDialog(TRect(10, 3, 70, 21), "Generate Custom Ephemeris Table");

    char startBuf[32];
    snprintf(startBuf, sizeof(startBuf), "%04d-%02d-%02d",
             currentBirthData.year, currentBirthData.month, currentBirthData.day);

    Astro::BirthData endDef = addCalendarDays(currentBirthData, 14.0);
    char endBuf[32];
    snprintf(endBuf, sizeof(endBuf), "%04d-%02d-%02d",
             endDef.year, endDef.month, endDef.day);

    char stepBuf[16] = "1";

    TInputLine* inStart = new TInputLine(TRect(26, 2, 46, 3), 20);
    inStart->setData(startBuf);
    d->insert(inStart);
    d->insert(new TLabel(TRect(3, 2, 25, 3), "~S~tart Date (Y-M-D):", inStart));

    TInputLine* inEnd = new TInputLine(TRect(26, 4, 46, 5), 20);
    inEnd->setData(endBuf);
    d->insert(inEnd);
    d->insert(new TLabel(TRect(3, 4, 25, 5), "~E~nd Date (Y-M-D):", inEnd));

    TInputLine* inStep = new TInputLine(TRect(26, 6, 36, 7), 10);
    inStep->setData(stepBuf);
    d->insert(inStep);
    d->insert(new TLabel(TRect(3, 6, 25, 7), "Interval / ~S~tep (days):", inStep));

    TSItem* zItems = new TSItem("Tropical Zodiac",
                     new TSItem("Sidereal (Lahiri Ayanamsa)", nullptr));
    TRadioButtons* rbZodiac = new TRadioButtons(TRect(26, 8, 58, 10), zItems);
    ushort zVal = (currentZodiacMode == Astro::ZodiacMode::SIDEREAL) ? 1 : 0;
    rbZodiac->setData(&zVal);
    d->insert(rbZodiac);
    d->insert(new TLabel(TRect(3, 8, 25, 9), "~Z~odiac System:", rbZodiac));

    TSItem* optItems = new TSItem("Show Daily Motion / Speed",
                       new TSItem("Show Declination Coordinates",
                       new TSItem("Show Retrograde Status", nullptr)));
    TCheckBoxes* cbOpts = new TCheckBoxes(TRect(26, 11, 58, 14), optItems);
    ushort optVal = 0x05;
    cbOpts->setData(&optVal);
    d->insert(cbOpts);
    d->insert(new TLabel(TRect(3, 11, 25, 12), "~O~ptions:", cbOpts));

    d->insert(new TButton(TRect(15, 15, 32, 17), "~G~enerate", cmOK, bfDefault));
    d->insert(new TButton(TRect(35, 15, 48, 17), "Cancel", cmCancel, bfNormal));

    if (deskTop->execView(d) == cmOK) {
        inStart->getData(startBuf);
        inEnd->getData(endBuf);
        inStep->getData(stepBuf);
        rbZodiac->getData(&zVal);
        cbOpts->getData(&optVal);

        int sy = 2000, sm = 1, sd = 1;
        int ey = 2000, em = 1, ed = 1;
        sscanf(startBuf, "%d-%d-%d", &sy, &sm, &sd);
        sscanf(endBuf, "%d-%d-%d", &ey, &em, &ed);
        int step = atoi(stepBuf);
        if (step <= 0) step = 1;

        Astro::EphemerisTable eph;
        if (eph.initialize()) {
            Astro::EphemerisConfig cfg;
            cfg.startDate = currentBirthData;
            cfg.startDate.year = sy; cfg.startDate.month = sm; cfg.startDate.day = sd;
            cfg.endDate = currentBirthData;
            cfg.endDate.year = ey; cfg.endDate.month = em; cfg.endDate.day = ed;
            cfg.intervalDays = step;
            cfg.zodiacMode = (zVal == 1) ? Astro::ZodiacMode::SIDEREAL : Astro::ZodiacMode::TROPICAL;
            cfg.ayanamsa = currentAyanamsa;
            cfg.useColors = false;
            cfg.showSiderealTime = true;
            cfg.showSpeed = (optVal & 0x01) != 0;
            cfg.showDeclination = (optVal & 0x02) != 0;
            cfg.showRetrograde = (optVal & 0x04) != 0;

            std::string text = eph.generateTable(cfg);
            std::ostringstream title;
            title << "Custom Ephemeris (" << startBuf << " to " << endBuf << ", step=" << step << "d)";
            openWindow(title.str(), text);
        } else {
            messageBox(mfError | mfOKButton, "Failed to initialize Ephemeris Table.");
        }
    }
    destroy(d);
}

void HoroscopeTuiApp::showCalendarQueryDialog() {
    TDialog* d = new TDialog(TRect(10, 3, 70, 21), "Calendar & Panchang Query Form");

    char dateBuf[32];
    snprintf(dateBuf, sizeof(dateBuf), "%04d-%02d-%02d",
             currentBirthData.year, currentBirthData.month, currentBirthData.day);
    char timeBuf[32];
    snprintf(timeBuf, sizeof(timeBuf), "%02d:%02d:%02d",
             currentBirthData.hour, currentBirthData.minute, currentBirthData.second);
    char cityBuf[64];
    snprintf(cityBuf, sizeof(cityBuf), "%s", currentCityName.c_str());

    TInputLine* inDate = new TInputLine(TRect(26, 2, 46, 3), 20);
    inDate->setData(dateBuf);
    d->insert(inDate);
    d->insert(new TLabel(TRect(3, 2, 25, 3), "~D~ate (Y-M-D):", inDate));

    TInputLine* inTime = new TInputLine(TRect(26, 4, 46, 5), 20);
    inTime->setData(timeBuf);
    d->insert(inTime);
    d->insert(new TLabel(TRect(3, 4, 25, 5), "~T~ime (H:M:S):", inTime));

    TInputLine* inCity = new TInputLine(TRect(26, 6, 56, 7), 50);
    inCity->setData(cityBuf);
    d->insert(inCity);
    d->insert(new TLabel(TRect(3, 6, 25, 7), "~C~ity / Place:", inCity));

    TSItem* calItems = new TSItem("Unified Astro-Calendar (Daily View)",
                       new TSItem("Hindu Daily Panchanga (Panchang Details)",
                       new TSItem("Myanmar Daily Calendar (မြန်မာပြက္ခဒိန်)",
                       new TSItem("Chinese Sexagenary Calendar (中国农历)", nullptr))));
    TRadioButtons* rbCal = new TRadioButtons(TRect(26, 8, 62, 12), calItems);
    ushort calVal = 0;
    rbCal->setData(&calVal);
    d->insert(rbCal);
    d->insert(new TLabel(TRect(3, 8, 25, 9), "Calendar ~S~ystem:", rbCal));

    d->insert(new TButton(TRect(15, 14, 30, 16), "~V~iew", cmOK, bfDefault));
    d->insert(new TButton(TRect(34, 14, 47, 16), "Cancel", cmCancel, bfNormal));

    if (deskTop->execView(d) == cmOK) {
        inDate->getData(dateBuf);
        inTime->getData(timeBuf);
        inCity->getData(cityBuf);
        rbCal->getData(&calVal);

        int y = 2000, m = 1, day = 1, h = 12, min = 0, s = 0;
        sscanf(dateBuf, "%d-%d-%d", &y, &m, &day);
        sscanf(timeBuf, "%d:%d:%d", &h, &min, &s);

        currentBirthData.year = y; currentBirthData.month = m; currentBirthData.day = day;
        currentBirthData.hour = h; currentBirthData.minute = min; currentBirthData.second = s;
        currentCityName = cityBuf;

        switch (calVal) {
            case 0: showAstroCalendarDay(); break;
            case 1: showHinduPanchang(); break;
            case 2: showMyanmarCalendar(); break;
            case 3: showChineseCalendar(); break;
            default: showAstroCalendarDay(); break;
        }
    }
    destroy(d);
}

void HoroscopeTuiApp::showMonthlyDialog() {
    TDialog* d = new TDialog(TRect(12, 4, 68, 19), "Monthly Calendar & Ephemeris Query");

    char yearBuf[16];
    snprintf(yearBuf, sizeof(yearBuf), "%d", currentBirthData.year);
    char monthBuf[16];
    snprintf(monthBuf, sizeof(monthBuf), "%d", currentBirthData.month);

    TInputLine* inYear = new TInputLine(TRect(24, 2, 36, 3), 10);
    inYear->setData(yearBuf);
    d->insert(inYear);
    d->insert(new TLabel(TRect(3, 2, 22, 3), "~Y~ear (YYYY):", inYear));

    TInputLine* inMonth = new TInputLine(TRect(24, 4, 32, 5), 5);
    inMonth->setData(monthBuf);
    d->insert(inMonth);
    d->insert(new TLabel(TRect(3, 4, 22, 5), "~M~onth (1-12):", inMonth));

    TSItem* mItems = new TSItem("30-Day Planetary Ephemeris Table",
                     new TSItem("Unified Monthly Astro-Calendar",
                     new TSItem("Hindu Monthly Panchang Calendar",
                     new TSItem("Myanmar Monthly Traditional Calendar", nullptr))));
    TRadioButtons* rbMonth = new TRadioButtons(TRect(24, 6, 62, 10), mItems);
    ushort mVal = 0;
    rbMonth->setData(&mVal);
    d->insert(rbMonth);
    d->insert(new TLabel(TRect(3, 6, 22, 7), "~V~iew Mode:", rbMonth));

    d->insert(new TButton(TRect(15, 12, 30, 14), "~G~enerate", cmOK, bfDefault));
    d->insert(new TButton(TRect(33, 12, 46, 14), "Cancel", cmCancel, bfNormal));

    if (deskTop->execView(d) == cmOK) {
        inYear->getData(yearBuf);
        inMonth->getData(monthBuf);
        rbMonth->getData(&mVal);

        int y = atoi(yearBuf);
        int m = atoi(monthBuf);
        if (y < 100) y = currentBirthData.year;
        if (m < 1 || m > 12) m = currentBirthData.month;

        currentBirthData.year = y;
        currentBirthData.month = m;

        switch (mVal) {
            case 0: showEphemerisMonthly(); break;
            case 1: showAstroCalendarMonth(); break;
            case 2: showHinduMonth(); break;
            case 3: showMyanmarMonth(); break;
            default: showEphemerisMonthly(); break;
        }
    }
    destroy(d);
}

void HoroscopeTuiApp::showTransitDialog() {
    TDialog* d = new TDialog(TRect(12, 4, 68, 18), "Planetary Transits Query");

    char natalBuf[32];
    snprintf(natalBuf, sizeof(natalBuf), "%04d-%02d-%02d",
             currentBirthData.year, currentBirthData.month, currentBirthData.day);

    Astro::BirthData nowData = currentBirthData;
    char transitBuf[32];
    snprintf(transitBuf, sizeof(transitBuf), "%04d-%02d-%02d",
             nowData.year, nowData.month, nowData.day);

    char daysBuf[16] = "30";

    TInputLine* inNatal = new TInputLine(TRect(26, 2, 46, 3), 20);
    inNatal->setData(natalBuf);
    d->insert(inNatal);
    d->insert(new TLabel(TRect(3, 2, 25, 3), "~N~atal Date (Y-M-D):", inNatal));

    TInputLine* inTransit = new TInputLine(TRect(26, 4, 46, 5), 20);
    inTransit->setData(transitBuf);
    d->insert(inTransit);
    d->insert(new TLabel(TRect(3, 4, 25, 5), "~T~ransit Start Date:", inTransit));

    TInputLine* inDays = new TInputLine(TRect(26, 6, 36, 7), 10);
    inDays->setData(daysBuf);
    d->insert(inDays);
    d->insert(new TLabel(TRect(3, 6, 25, 7), "Range (~D~ays):", inDays));

    d->insert(new TButton(TRect(15, 10, 32, 12), "~C~alculate", cmOK, bfDefault));
    d->insert(new TButton(TRect(35, 10, 48, 12), "Cancel", cmCancel, bfNormal));

    if (deskTop->execView(d) == cmOK) {
        inNatal->getData(natalBuf);
        inTransit->getData(transitBuf);
        inDays->getData(daysBuf);

        int ny = 2000, nm = 1, nd = 1;
        int ty = 2000, tm = 1, td = 1;
        sscanf(natalBuf, "%d-%d-%d", &ny, &nm, &nd);
        sscanf(transitBuf, "%d-%d-%d", &ty, &tm, &td);
        int days = atoi(daysBuf);
        if (days <= 0) days = 30;

        Astro::BirthData natal = currentBirthData;
        natal.year = ny; natal.month = nm; natal.day = nd;

        Astro::BirthData tStart = currentBirthData;
        tStart.year = ty; tStart.month = tm; tStart.day = td;
        Astro::BirthData tEnd = addCalendarDays(tStart, (double)days);

        Astro::EphemerisTable eph;
        if (eph.initialize()) {
            std::string text = eph.generateTransitTable(natal, tStart, tEnd, 1);
            std::ostringstream title;
            title << "Planetary Transits (" << transitBuf << " for " << days << " days)";
            openWindow(title.str(), text);
        } else {
            messageBox(mfError | mfOKButton, "Failed to initialize Ephemeris Table.");
        }
    }
    destroy(d);
}

void HoroscopeTuiApp::showPlanetaryPositions() {
    if (!promptDateLocation("Planetary Positions & Houses - Query Date", currentBirthData, currentCityName, true)) return;
    Astro::BirthChart chart = computeCurrentChart(currentBirthData, currentHouseSystem, currentZodiacMode, currentAyanamsa);
    std::string text = chart.getFormattedChart();
    openWindow("Planetary Positions & House Cusps - " + currentCityName, text);
}

void HoroscopeTuiApp::showAstroCoordinates() {
    if (!promptDateLocation("Astronomical Details & Coordinates - Query Date", currentBirthData, currentCityName, true)) return;
    Astro::BirthChart chart = computeCurrentChart(currentBirthData, currentHouseSystem, currentZodiacMode, currentAyanamsa);
    std::string text = chart.getFormattedChart(true);
    openWindow("Planetary Coordinates & Astronomical Details - " + currentCityName, text);
}

void HoroscopeTuiApp::showHinduPanchang() {
    if (!promptDateLocation("Hindu Daily Panchanga - Date Query", currentBirthData, currentCityName, true)) return;

    Astro::HinduCalendar hindu;
    if (hindu.initialize()) {
        Astro::PanchangaData p = hindu.calculatePanchanga(currentBirthData);
        std::string text = hindu.generatePanchangaTable(p);
        openWindow("Hindu Daily Panchang - " + currentCityName, text);
    } else {
        messageBox(mfError | mfOKButton, "Failed to initialize Hindu Calendar.");
    }
}

void HoroscopeTuiApp::showHinduMonth() {
    int y = currentBirthData.year, m = currentBirthData.month;
    if (!promptYearMonth("Hindu Monthly Panchang - Year & Month Query", y, m)) return;
    currentBirthData.year = y; currentBirthData.month = m;

    Astro::HinduMonthlyCalendar::DisplayOptions opts = Astro::HinduMonthlyCalendar::getDefaultDisplayOptions();
    opts.timeZoneOffset = currentBirthData.timezone;
    opts.colorOutput = false; // Crisp plain text in TVision
    opts.showTithi = true;
    opts.showNakshatra = true;
    opts.showYoga = true;
    opts.showKarana = true;
    opts.showFestivals = true;
    opts.showSpecialDays = true;
    opts.showMuhurta = true;

    Astro::HinduMonthlyCalendar monthly(currentBirthData.latitude, currentBirthData.longitude, opts);
    monthly.initialize();
    std::string text = monthly.generateCalendar(currentBirthData.year, currentBirthData.month);
    std::ostringstream title;
    title << "Hindu Monthly Calendar (" << y << "-" << std::setw(2) << std::setfill('0') << m << ") - " << currentCityName;
    openWindow(title.str(), text);
}

void HoroscopeTuiApp::showMyanmarCalendar() {
    if (!promptDateLocation("Myanmar Calendar - Date Query (မြန်မာပြက္ခဒိန်)", currentBirthData, currentCityName, false)) return;

    Astro::MyanmarCalendar myanmar;
    if (myanmar.initialize()) {
        Astro::MyanmarCalendarData d = myanmar.calculateMyanmarCalendar(currentBirthData);
        std::string text = myanmar.generateMyanmarCalendarTable(d);
        openWindow("Myanmar Daily Calendar (မြန်မာပြက္ခဒိန်)", text);
    } else {
        messageBox(mfError | mfOKButton, "Failed to initialize Myanmar Calendar.");
    }
}

void HoroscopeTuiApp::showMyanmarMonth() {
    int y = currentBirthData.year, m = currentBirthData.month;
    if (!promptYearMonth("Myanmar Monthly Calendar Query (လဆန်း/လဆုတ် ဥပုသ်နေ့များ)", y, m)) return;
    currentBirthData.year = y; currentBirthData.month = m;

    Astro::MyanmarMonthlyCalendar monthly;
    if (monthly.initialize()) {
        auto data = monthly.calculateMonthlyData(currentBirthData.year, currentBirthData.month,
                                               currentBirthData.latitude, currentBirthData.longitude);
        std::string text = monthly.generateTraditionalMyanmarCalendar(data);
        std::ostringstream title;
        title << "Myanmar Monthly Calendar (" << y << "-" << std::setw(2) << std::setfill('0') << m << ") - မြန်မာပြက္ခဒိန်";
        openWindow(title.str(), text);
    } else {
        messageBox(mfError | mfOKButton, "Failed to initialize Myanmar Monthly Calendar.");
    }
}

void HoroscopeTuiApp::showChineseCalendar() {
    if (!promptDateLocation("Chinese Calendar - Date Query (中国农历)", currentBirthData, currentCityName, true)) return;

    Astro::ChineseCalendar chinese;
    if (chinese.initialize()) {
        double jd = chinese.gregorianToJulian(currentBirthData.year, currentBirthData.month, currentBirthData.day, currentBirthData.hour);
        auto data = chinese.calculateChineseCalendar(jd);
        std::string text = chinese.generateTable(data);
        openWindow("Chinese Sexagenary Calendar (中国农历)", text);
    } else {
        messageBox(mfError | mfOKButton, "Failed to initialize Chinese Calendar.");
    }
}

void HoroscopeTuiApp::showAspectGrid() {
    if (!promptDateLocation("Planetary Aspect Grid - Query Date", currentBirthData, currentCityName, true)) return;
    Astro::BirthChart chart = computeCurrentChart(currentBirthData, currentHouseSystem, currentZodiacMode, currentAyanamsa);

    Astro::WesternChartDrawer drawer;
    std::string text = drawer.drawAspectGrid(chart);

    openWindow("Planetary Aspect Grid - " + currentCityName, text);
}

void HoroscopeTuiApp::showConjunctions() {
    int days = 365;
    if (!promptDateRange("Planetary Conjunctions Search Window", currentBirthData, days)) return;

    Astro::ConjunctionCalculator conj;
    conj.initialize();
    Astro::BirthData toDate = addCalendarDays(currentBirthData, (double)days);

    auto list = conj.findConjunctions(currentBirthData, toDate, 3.0);
    std::string text = conj.generateConjunctionReport(list);

    std::ostringstream title;
    title << "Planetary Conjunctions (" << days << " Days Window)";
    openWindow(title.str(), text);
}

void HoroscopeTuiApp::showEclipses() {
    int days = 730;
    if (!promptDateRange("Solar & Lunar Eclipses Search Window", currentBirthData, days)) return;

    Astro::EclipseCalculator ecl;
    ecl.initialize();
    Astro::BirthData toDate = addCalendarDays(currentBirthData, (double)days);

    auto list = ecl.findEclipses(currentBirthData, toDate);
    std::string text = ecl.generateEclipseReport(list);

    std::ostringstream title;
    title << "Solar & Lunar Eclipses (" << days << " Days Window)";
    openWindow(title.str(), text);
}

void HoroscopeTuiApp::showKPTable() {
    if (!promptDateLocation("KP Sub-Lord Analysis - Query Date", currentBirthData, currentCityName, true)) return;
    Astro::BirthChart chart = computeCurrentChart(currentBirthData, currentHouseSystem, Astro::ZodiacMode::SIDEREAL, Astro::AyanamsaType::KRISHNAMURTI);

    Astro::KPSystem kp;
    kp.initialize();
    std::string text = kp.generateKPTable(chart.getPlanetPositions());

    openWindow("KP System 5-Levels Sub-Lord Analysis - " + currentCityName, text);
}

void HoroscopeTuiApp::showKPTransitions() {
    int days = 7;
    if (!promptDateRange("KP Planetary Transitions Search Window", currentBirthData, days)) return;

    Astro::KPSystem kp;
    kp.initialize();
    Astro::BirthData toDate = addCalendarDays(currentBirthData, (double)days);

    auto transitions = kp.findTransitions(currentBirthData, toDate, Astro::Planet::SUN, Astro::KPLevel::SUB);
    std::string text = kp.generateTransitionTable(transitions);

    std::ostringstream title;
    title << "KP Planetary Transitions (" << days << " Days Window)";
    openWindow(title.str(), text);
}

void HoroscopeTuiApp::showEphemerisTable() {
    showEphemerisDialog();
}

void HoroscopeTuiApp::showEphemerisMonthly() {
    int y = currentBirthData.year, m = currentBirthData.month;
    if (!promptYearMonth("Monthly Ephemeris Table Query", y, m)) return;
    currentBirthData.year = y; currentBirthData.month = m;

    Astro::EphemerisTable ephTable;
    if (ephTable.initialize()) {
        int daysInMonth = 31;
        if (m == 2) {
            daysInMonth = (y % 4 == 0 && (y % 100 != 0 || y % 400 == 0)) ? 29 : 28;
        } else if (m == 4 || m == 6 || m == 9 || m == 11) {
            daysInMonth = 30;
        }

        Astro::EphemerisConfig cfg;
        cfg.startDate = currentBirthData;
        cfg.startDate.year = y; cfg.startDate.month = m; cfg.startDate.day = 1;
        cfg.startDate.hour = 0; cfg.startDate.minute = 0; cfg.startDate.second = 0;
        cfg.endDate = currentBirthData;
        cfg.endDate.year = y; cfg.endDate.month = m; cfg.endDate.day = daysInMonth;
        cfg.endDate.hour = 23; cfg.endDate.minute = 59; cfg.endDate.second = 59;
        cfg.intervalDays = 1;
        cfg.planets = {
            Astro::Planet::SUN, Astro::Planet::MOON, Astro::Planet::MERCURY,
            Astro::Planet::VENUS, Astro::Planet::MARS, Astro::Planet::JUPITER,
            Astro::Planet::SATURN, Astro::Planet::URANUS, Astro::Planet::NEPTUNE,
            Astro::Planet::PLUTO, Astro::Planet::NORTH_NODE, Astro::Planet::SOUTH_NODE
        };
        cfg.zodiacMode = currentZodiacMode;
        cfg.ayanamsa = currentAyanamsa;
        cfg.useColors = false;
        cfg.showRetrograde = true;
        cfg.showSiderealTime = true;
        cfg.showDayNames = true;

        std::string text = ephTable.generateTable(cfg);
        std::ostringstream title;
        title << "Monthly Ephemeris (" << y << "-"
              << std::setw(2) << std::setfill('0') << m << ") - Daily Positions ("
              << (currentZodiacMode == Astro::ZodiacMode::SIDEREAL ? "Sidereal" : "Tropical") << ")";
        openWindow(title.str(), text);
    } else {
        messageBox(mfError | mfOKButton, "Failed to initialize Ephemeris Table.");
    }
}

void HoroscopeTuiApp::showEphemerisYearly() {
    int y = currentBirthData.year, m = 1;
    if (!promptYearMonth("Yearly Ephemeris Table Query", y, m)) return;
    currentBirthData.year = y;

    Astro::EphemerisTable ephTable;
    if (ephTable.initialize()) {
        std::vector<Astro::Planet> planets = {
            Astro::Planet::SUN, Astro::Planet::MOON, Astro::Planet::MERCURY,
            Astro::Planet::VENUS, Astro::Planet::MARS, Astro::Planet::JUPITER,
            Astro::Planet::SATURN, Astro::Planet::URANUS, Astro::Planet::NEPTUNE,
            Astro::Planet::PLUTO, Astro::Planet::NORTH_NODE, Astro::Planet::SOUTH_NODE
        };

        Astro::EphemerisConfig cfg;
        cfg.startDate = currentBirthData;
        cfg.startDate.year = y; cfg.startDate.month = 1; cfg.startDate.day = 1;
        cfg.startDate.hour = 0; cfg.startDate.minute = 0; cfg.startDate.second = 0;
        cfg.endDate = currentBirthData;
        cfg.endDate.year = y; cfg.endDate.month = 12; cfg.endDate.day = 31;
        cfg.endDate.hour = 23; cfg.endDate.minute = 59; cfg.endDate.second = 59;
        cfg.intervalDays = 30;
        cfg.planets = planets;
        cfg.zodiacMode = currentZodiacMode;
        cfg.ayanamsa = currentAyanamsa;
        cfg.useColors = false;
        cfg.showRetrograde = true;
        cfg.showSiderealTime = true;
        cfg.showDayNames = true;

        std::string text = ephTable.generateTable(cfg);
        std::ostringstream title;
        title << "Yearly Ephemeris (" << y << ") - 12-Month Table ("
              << (currentZodiacMode == Astro::ZodiacMode::SIDEREAL ? "Sidereal" : "Tropical") << ")";
        openWindow(title.str(), text);
    } else {
        messageBox(mfError | mfOKButton, "Failed to initialize Ephemeris Table.");
    }
}

void HoroscopeTuiApp::showEphemerisTransits() {
    showTransitDialog();
}

void HoroscopeTuiApp::showAstroCalendarDay() {
    if (!promptDateLocation("Unified Astro-Calendar - Date Query", currentBirthData, currentCityName, true)) return;

    Astro::AstroCalendar astro;
    if (astro.initialize(currentBirthData.latitude, currentBirthData.longitude)) {
        astro.setIncludePlanetaryTransitions(true);
        astro.setIncludeAllFestivals(true);
        astro.setIncludeKPTransitions(true);
        Astro::AstroCalendarDay dayData = astro.calculateAstroCalendarDay(currentBirthData);
        std::string text = astro.generateDayCalendar(dayData, "calendar");
        openWindow("Unified Astro-Calendar (Daily) - " + currentCityName, text);
    } else {
        messageBox(mfError | mfOKButton, "Failed to initialize Astro Calendar.");
    }
}

void HoroscopeTuiApp::showAstroCalendarMonth() {
    int y = currentBirthData.year, m = currentBirthData.month;
    if (!promptYearMonth("Unified Monthly Astro-Calendar Query", y, m)) return;
    currentBirthData.year = y; currentBirthData.month = m;

    Astro::AstroCalendar astro;
    if (astro.initialize(currentBirthData.latitude, currentBirthData.longitude)) {
        astro.setIncludePlanetaryTransitions(true);
        astro.setIncludeAllFestivals(true);
        Astro::AstroCalendarMonth monthData = astro.calculateAstroCalendarMonth(currentBirthData.year, currentBirthData.month);
        std::string text = astro.generateMonthlyCalendar(monthData, "calendar");
        std::ostringstream title;
        title << "Unified Monthly Astro-Calendar (" << y << "-" << std::setw(2) << std::setfill('0') << m << ") - " << currentCityName;
        openWindow(title.str(), text);
    } else {
        messageBox(mfError | mfOKButton, "Failed to initialize Astro Calendar.");
    }
}

void HoroscopeTuiApp::showHelpAbout() {
    messageBox(mfInformation | mfOKButton,
        "Horoscope CLI & TVision TUI v2.1.0\n\n"
        "Professional Ephemeris & Multi-Calendar Engine\n"
        "Powered by Swiss Ephemeris & Turbo Vision 2.0\n\n"
        "Features:\n"
        "• Professional Ephemeris Tables (Daily, Monthly, Yearly, Transits)\n"
        "• Multi-Calendar Integration (Unified, Hindu Panchang, Myanmar, Chinese)\n"
        "• KP System 5-Levels Sub-Lord Analysis & Transitions\n"
        "• Planetary Conjunctions, Eclipses & Aspect Grids\n\n"
        "Built with Turbo Vision by magiblot"
    );
}

void HoroscopeTuiApp::showHelpLegend() {
    std::string legend =
        "Astrological & Astronomical Symbols\n"
        "====================================\n\n"
        "Planets:\n"
        "  ☉ Sun       (Su)    ☽ Moon     (Mo)    ☿ Mercury  (Me)\n"
        "  ♀ Venus     (Ve)    ♂ Mars     (Ma)    ♃ Jupiter  (Ju)\n"
        "  ♄ Saturn    (Sa)    ♅ Uranus   (Ur)    ♆ Neptune  (Ne)\n"
        "  ♇ Pluto     (Pl)    ☊ Rahu     (Ra)    ☋ Ketu     (Ke)\n\n"
        "Major Aspects:\n"
        "  ☌ Conjunction (0°)    ⚹ Sextile (60°)   □ Square (90°)\n"
        "  △ Trine (120°)       ☍ Opposition (180°)\n\n"
        "Zodiac Signs:\n"
        "  ♈ Aries        ♉ Taurus       ♊ Gemini       ♋ Cancer\n"
        "  ♌ Leo          ♍ Virgo        ♎ Libra        ♏ Scorpio\n"
        "  ♐ Sagittarius  ♑ Capricorn    ♒ Aquarius     ♓ Pisces\n\n"
        "KP Krishnamurti Paddhati Notation:\n"
        "  Sign - Star - Sub - SubSub - Sub³\n";

    openWindow("Astrological Legend & Symbols", legend);
}

void HoroscopeTuiApp::showHelpShortcuts() {
    std::string shortcuts =
        "Keyboard & Mouse Navigation Guide\n"
        "=================================\n\n"
        "Global Shortcuts:\n"
        "  Alt+X          Exit Application\n"
        "  F2             New Birth Chart Dialog\n"
        "  F3             Location Presets Dialog\n"
        "  F5             KP Sub-Lord 5-Levels Table\n"
        "  F10            Activate Main Menu\n"
        "  Ctrl+A         Open Planetary Aspect Grid\n"
        "  Ctrl+S         Export Active View to File\n\n"
        "Window Management:\n"
        "  Alt+F3         Close Active Window\n"
        "  F6             Zoom / Maximize Window\n"
        "  Ctrl+F5        Size / Move Active Window (Arrow Keys)\n"
        "  Shift+F5       Tile All Windows\n"
        "  Shift+F4       Cascade All Windows\n"
        "  Tab / Shift+Tab Switch Between Open Windows\n\n"
        "Mouse Support:\n"
        "  Click menu items, status bar, or window buttons [■][▲][X]\n"
        "  Drag window title bar to move\n"
        "  Drag bottom-right corner to resize\n"
        "  Use mouse wheel to scroll text up and down\n";

    openWindow("Keyboard & Mouse Shortcuts", shortcuts);
}

void HoroscopeTuiApp::setTheme(int themeId) {
    if (themeId < 0 || themeId > 2) return;
    currentTheme = themeId;
    appPalette = (themeId == 2) ? apBlackWhite : apColor;
    setState(sfExposed, False);
    setState(sfExposed, True);
    redraw();
}

static TPalette createTurboCppPalette() {
    TColorAttr attrs[sizeof(cpTurboCpp) - 1];
    // Authentic Borland Deep Dark Navy Blue (ရိုးရိုး အပြာရင့်)
    const TColorRGB darkNavyBlue(0x00, 0x1A, 0x70);       // Rich Deep Dark Navy Blue (#001A70)
    const TColorRGB desktopNavyBlue(0x00, 0x10, 0x48);    // Deep solid dark blue for desktop (#001048)
    const TColorRGB brightWhite(0xFF, 0xFF, 0xFF);        // Crisp High Contrast White
    const TColorRGB borlandYellow(0xFF, 0xFF, 0x55);      // Borland Title / hotkey yellow
    const TColorRGB borlandLightGray(0xC0, 0xC0, 0xC0);   // Menu & status bar light gray
    const TColorRGB borlandDarkGray(0x55, 0x55, 0x55);    // Inactive / border dark gray
    const TColorRGB borlandGreen(0x55, 0xFF, 0x55);       // Window control green
    const TColorRGB borlandRed(0xFF, 0x55, 0x55);         // Alert / shortcut red

    for (size_t i = 0; i < sizeof(cpTurboCpp) - 1; ++i) {
        uchar b = (uchar)cpTurboCpp[i];
        uchar fgIdx = b & 0x0F;
        uchar bgIdx = (b >> 4) & 0x0F;

        // Map background: index 1 is Blue -> map to darkNavyBlue (or desktopNavyBlue for desktop)
        TColor bg;
        if (bgIdx == 1) {
            bg = (i == 0) ? TColor(desktopNavyBlue) : TColor(darkNavyBlue);
        } else if (bgIdx == 7) {
            bg = TColor(borlandLightGray);
        } else if (bgIdx == 0) {
            bg = TColor(TColorRGB(0x00, 0x00, 0x00));
        } else {
            bg = TColor((char)bgIdx);
        }

        // Map foreground: replace any cyan/light blue (3, B) with bright white or yellow
        TColor fg;
        if (fgIdx == 0xF) {
            fg = TColor(brightWhite);
        } else if (fgIdx == 0xE) {
            fg = TColor(borlandYellow);
        } else if (fgIdx == 0x7) {
            fg = TColor(borlandLightGray);
        } else if (fgIdx == 0x8) {
            fg = TColor(borlandDarkGray);
        } else if (fgIdx == 0x4 || fgIdx == 0xC) {
            fg = TColor(borlandRed);
        } else if (fgIdx == 0x2 || fgIdx == 0xA) {
            fg = TColor(borlandGreen);
        } else if (fgIdx == 0x0) {
            fg = TColor(TColorRGB(0x00, 0x00, 0x00));
        } else if (fgIdx == 0x1) {
            fg = TColor(desktopNavyBlue);
        } else if (fgIdx == 0x3 || fgIdx == 0xB) {
            fg = TColor(brightWhite);
        } else {
            fg = TColor((char)fgIdx);
        }

        attrs[i] = TColorAttr(fg, bg);
    }
    return TPalette(attrs, sizeof(cpTurboCpp) - 1);
}

TPalette& HoroscopeTuiApp::getPalette() const {
    static TPalette turboPal = createTurboCppPalette();
    static TPalette darkPal(cpModernDark, sizeof(cpModernDark) - 1);
    static TPalette bwPal(cpAppBlackWhite, sizeof(cpAppBlackWhite) - 1);

    static TPalette* palettes[] = {
        &turboPal,
        &darkPal,
        &bwPal
    };

    if (currentTheme >= 0 && currentTheme < 3) {
        return *(palettes[currentTheme]);
    }
    return turboPal;
}

void HoroscopeTuiApp::showThemeDialog() {
    TDialog* d = new TDialog(TRect(20, 6, 68, 16), "Select Color Theme");
    d->options |= ofCentered;

    TSItem* items =
        new TSItem("1. Turbo C++ 3.0 Classic (Recommended)",
        new TSItem("2. Modern Dark Slate",
        new TSItem("3. Monochrome High Contrast B&W", nullptr)));
    TRadioButtons* rb = new TRadioButtons(TRect(3, 2, 44, 5), items);
    d->insert(rb);
    ushort val = (ushort)currentTheme;
    rb->setData(&val);

    d->insert(new TButton(TRect(8, 6, 20, 8), "O~K~", cmOK, bfDefault));
    d->insert(new TButton(TRect(24, 6, 36, 8), "Cancel", cmCancel, bfNormal));

    if (validView(d)) {
        if (deskTop->execView(d) == cmOK) {
            ushort selected = 0;
            rb->getData(&selected);
            setTheme(selected);
        }
        destroy(d);
    }
}

int runTuiApplication(int initialTheme) {
    HoroscopeTuiApp app(initialTheme);
    app.run();
    return 0;
}

} // namespace AstroTui
