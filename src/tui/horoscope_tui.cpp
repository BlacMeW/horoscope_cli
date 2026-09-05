#include "tui/horoscope_tui.h"
#include "tui/chart_view.h"
#include "birth_chart.h"
#include "horoscope_calculator.h"
#include "western_chart_drawer.h"
#include "eastern_chart_drawer.h"
#include "solar_system_drawer.h"
#include "hindu_calendar.h"
#include "hindu_monthly_calendar.h"
#include "myanmar_calendar.h"
#include "myanmar_monthly_calendar.h"
#include "chinese_calendar.h"
#include "kp_system.h"
#include "conjunction_calculator.h"
#include "eclipse_calculator.h"
#include "ephemeris_table.h"
#include "location_manager.h"

#include <sstream>
#include <fstream>
#include <iomanip>
#include <cstdio>

namespace AstroTui {

HoroscopeTuiApp::HoroscopeTuiApp() :
    TProgInit(&HoroscopeTuiApp::initStatusLine,
              &HoroscopeTuiApp::initMenuBar,
              &HoroscopeTuiApp::initDeskTop),
    currentCityName("Yangon, Myanmar"),
    currentZodiacMode(Astro::ZodiacMode::TROPICAL),
    currentAyanamsa(Astro::AyanamsaType::LAHIRI),
    currentHouseSystem(Astro::HouseSystem::PLACIDUS),
    windowCount(0)
{
    // Default birth data (Yangon, 2000-01-01 12:00:00)
    currentBirthData.year = 2000;
    currentBirthData.month = 1;
    currentBirthData.day = 1;
    currentBirthData.hour = 12;
    currentBirthData.minute = 0;
    currentBirthData.second = 0;
    currentBirthData.latitude = 16.8661;
    currentBirthData.longitude = 96.1951;
    currentBirthData.timezone = 6.5;

    // Show initial welcome chart
    showWesternWheel();
}

HoroscopeTuiApp::~HoroscopeTuiApp() {
}

TMenuBar* HoroscopeTuiApp::initMenuBar(TRect r) {
    r.b.y = r.a.y + 1;
    return new TMenuBar(r,
        *new TSubMenu("~F~ile", kbAltF) +
            *new TMenuItem("~N~ew Birth Chart...", cmNewChart, kbF2, hcNoContext, "F2") +
            *new TMenuItem("~L~ocation Presets...", cmCityPreset, kbF3, hcNoContext, "F3") +
            *new TMenuItem("~E~xport Active View...", cmExportFile, kbCtrlS, hcNoContext, "Ctrl+S") +
            newLine() +
            *new TMenuItem("E~x~it", cmQuit, kbAltX, hcNoContext, "Alt+X") +
        *new TSubMenu("~C~harts", kbAltC) +
            *new TMenuItem("~W~estern Wheel Chart", cmWesternWheel, kbNoKey) +
            *new TMenuItem("Western ~R~ectangular Chart", cmWesternRect, kbNoKey) +
            newLine() +
            *new TMenuItem("~N~orth Indian Vedic Chart", cmVedicNorth, kbNoKey) +
            *new TMenuItem("~S~outh Indian Vedic Chart", cmVedicSouth, kbNoKey) +
            *new TMenuItem("~E~ast Indian Vedic Chart", cmVedicEast, kbNoKey) +
            newLine() +
            *new TMenuItem("Solar S~y~stem 2D Orbit View", cmSolarSystem, kbNoKey) +
        *new TSubMenu("C~a~lendars", kbAltA) +
            *new TMenuItem("~H~indu Daily Panchang", cmHinduPanchang, kbNoKey) +
            *new TMenuItem("Hindu ~M~onthly Calendar", cmHinduMonth, kbNoKey) +
            newLine() +
            *new TMenuItem("M~y~anmar Daily Calendar", cmMyanmarCalendar, kbNoKey) +
            *new TMenuItem("Myanmar Mo~n~thly Calendar", cmMyanmarMonth, kbNoKey) +
            newLine() +
            *new TMenuItem("~C~hinese Sexagenary Calendar", cmChineseCalendar, kbNoKey) +
        *new TSubMenu("~A~nalysis", kbAltN) +
            *new TMenuItem("Planetary ~A~spect Grid", cmAspectGrid, kbCtrlA, hcNoContext, "Ctrl+A") +
            *new TMenuItem("Planetary ~C~onjunctions (1 Year)", cmConjunctions, kbNoKey) +
            *new TMenuItem("Solar & Lunar ~E~clipses (2 Years)", cmEclipses, kbNoKey) +
            newLine() +
            *new TMenuItem("~K~P Sub-Lord 5-Levels Table", cmKPTable, kbF5, hcNoContext, "F5") +
            *new TMenuItem("KP Planetary ~T~ransitions", cmKPTransitions, kbNoKey) +
            *new TMenuItem("E~p~hemeris 30-Day Table", cmEphemerisTable, kbNoKey) +
        *new TSubMenu("~W~indow", kbAltW) +
            *new TMenuItem("~T~ile Windows", cmTileWindows, kbShiftF5, hcNoContext, "Shift+F5") +
            *new TMenuItem("~C~ascade Windows", cmCascadeWindows, kbShiftF4, hcNoContext, "Shift+F4") +
            *new TMenuItem("Cl~o~se Active Window", cmClose, kbAltF3, hcNoContext, "Alt+F3") +
            *new TMenuItem("~S~ize / Move", cmResize, kbCtrlF5, hcNoContext, "Ctrl+F5") +
            *new TMenuItem("~Z~oom / Maximize", cmZoom, kbF6, hcNoContext, "F6") +
        *new TSubMenu("~H~elp", kbAltH) +
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
            *new TStatusItem("~F2~ New Chart", kbF2, cmNewChart) +
            *new TStatusItem("~F3~ Presets", kbF3, cmCityPreset) +
            *new TStatusItem("~F5~ KP Table", kbF5, cmKPTable) +
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
            case cmWesternWheel:
                showWesternWheel();
                clearEvent(event);
                break;
            case cmWesternRect:
                showWesternRect();
                clearEvent(event);
                break;
            case cmVedicNorth:
                showVedicNorth();
                clearEvent(event);
                break;
            case cmVedicSouth:
                showVedicSouth();
                clearEvent(event);
                break;
            case cmVedicEast:
                showVedicEast();
                clearEvent(event);
                break;
            case cmSolarSystem:
                showSolarSystem();
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
        int offset = (windowCount % 6) * 2;
        int w = std::min(82, extent.b.x - 4);
        int h = std::min(24, extent.b.y - 4);
        int x1 = std::max(1, extent.a.x + 2 + offset);
        int y1 = std::max(1, extent.a.y + 1 + offset);
        int x2 = std::min(extent.b.x - 1, x1 + w);
        int y2 = std::min(extent.b.y - 1, y1 + h);
        bounds = TRect(x1, y1, x2, y2);
        windowCount++;
    }

    TTextWindow* win = new TTextWindow(bounds, title.c_str(), content);
    deskTop->insert(win);
}

void HoroscopeTuiApp::showNewChartDialog() {
    TDialog* d = new TDialog(TRect(12, 3, 68, 20), "Enter Birth Chart Data");

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

    TInputLine* inDate = new TInputLine(TRect(20, 2, 40, 3), 20);
    inDate->setData(dateBuf);
    d->insert(inDate);
    d->insert(new TLabel(TRect(3, 2, 19, 3), "~D~ate (Y-M-D):", inDate));

    TInputLine* inTime = new TInputLine(TRect(20, 4, 40, 5), 20);
    inTime->setData(timeBuf);
    d->insert(inTime);
    d->insert(new TLabel(TRect(3, 4, 19, 5), "~T~ime (H:M:S):", inTime));

    TInputLine* inLat = new TInputLine(TRect(20, 6, 40, 7), 20);
    inLat->setData(latBuf);
    d->insert(inLat);
    d->insert(new TLabel(TRect(3, 6, 19, 7), "~L~atitude (°):", inLat));

    TInputLine* inLon = new TInputLine(TRect(20, 8, 40, 9), 20);
    inLon->setData(lonBuf);
    d->insert(inLon);
    d->insert(new TLabel(TRect(3, 8, 19, 9), "L~o~ngitude (°):", inLon));

    TInputLine* inTz = new TInputLine(TRect(20, 10, 40, 11), 20);
    inTz->setData(tzBuf);
    d->insert(inTz);
    d->insert(new TLabel(TRect(3, 10, 19, 11), "Time~z~one:", inTz));

    TInputLine* inCity = new TInputLine(TRect(20, 12, 52, 13), 50);
    inCity->setData(cityBuf);
    d->insert(inCity);
    d->insert(new TLabel(TRect(3, 12, 19, 13), "~C~ity Name:", inCity));

    d->insert(new TButton(TRect(15, 14, 28, 16), "~C~alculate", cmOK, bfDefault));
    d->insert(new TButton(TRect(30, 14, 43, 16), "Cancel", cmCancel, bfNormal));

    if (deskTop->execView(d) == cmOK) {
        inDate->getData(dateBuf);
        inTime->getData(timeBuf);
        inLat->getData(latBuf);
        inLon->getData(lonBuf);
        inTz->getData(tzBuf);
        inCity->getData(cityBuf);

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

        showWesternWheel();
        showKPTable();
    }
    destroy(d);
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
            showWesternWheel();
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

void HoroscopeTuiApp::showWesternWheel() {
    Astro::BirthChart chart = computeCurrentChart(currentBirthData, currentHouseSystem, currentZodiacMode, currentAyanamsa);

    Astro::WesternChartDrawer drawer;
    drawer.setShowAspects(true);
    std::string text = drawer.drawChartWheel(chart);

    openWindow("Western Wheel Chart - " + currentCityName, text);
}

void HoroscopeTuiApp::showWesternRect() {
    Astro::BirthChart chart = computeCurrentChart(currentBirthData, currentHouseSystem, currentZodiacMode, currentAyanamsa);

    Astro::WesternChartDrawer drawer;
    std::string text = drawer.drawRectangularChart(chart);

    openWindow("Western Rectangular Layout - " + currentCityName, text);
}

void HoroscopeTuiApp::showVedicNorth() {
    Astro::BirthChart chart = computeCurrentChart(currentBirthData, currentHouseSystem, Astro::ZodiacMode::SIDEREAL, currentAyanamsa);

    Astro::EasternChartDrawer drawer;
    drawer.setChartStyle("north-indian");
    std::string text = drawer.drawEasternChart(chart);

    openWindow("North Indian Vedic Chart - " + currentCityName, text);
}

void HoroscopeTuiApp::showVedicSouth() {
    Astro::BirthChart chart = computeCurrentChart(currentBirthData, currentHouseSystem, Astro::ZodiacMode::SIDEREAL, currentAyanamsa);

    Astro::EasternChartDrawer drawer;
    drawer.setChartStyle("south-indian");
    std::string text = drawer.drawEasternChart(chart);

    openWindow("South Indian Vedic Chart - " + currentCityName, text);
}

void HoroscopeTuiApp::showVedicEast() {
    Astro::BirthChart chart = computeCurrentChart(currentBirthData, currentHouseSystem, Astro::ZodiacMode::SIDEREAL, currentAyanamsa);

    Astro::EasternChartDrawer drawer;
    drawer.setChartStyle("east-indian");
    std::string text = drawer.drawEasternChart(chart);

    openWindow("East Indian Vedic Chart - " + currentCityName, text);
}

void HoroscopeTuiApp::showSolarSystem() {
    Astro::BirthChart chart = computeCurrentChart(currentBirthData, currentHouseSystem, currentZodiacMode, currentAyanamsa);

    Astro::SolarSystemDrawer drawer;
    drawer.setShowOrbits(true);
    drawer.setShowPlanetNames(true);
    drawer.setShowDistances(true);
    std::string text = drawer.drawSolarSystem(chart);

    openWindow("Solar System 2D Orbit View", text);
}

void HoroscopeTuiApp::showHinduPanchang() {
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
    Astro::HinduMonthlyCalendar::DisplayOptions opts = Astro::HinduMonthlyCalendar::getDefaultDisplayOptions();
    opts.timeZoneOffset = currentBirthData.timezone;

    Astro::HinduMonthlyCalendar monthly(currentBirthData.latitude, currentBirthData.longitude, opts);
    monthly.initialize();
    std::string text = monthly.generateCalendar(currentBirthData.year, currentBirthData.month);
    openWindow("Hindu Monthly Calendar", text);
}

void HoroscopeTuiApp::showMyanmarCalendar() {
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
    Astro::MyanmarMonthlyCalendar monthly;
    if (monthly.initialize()) {
        auto data = monthly.calculateMonthlyData(currentBirthData.year, currentBirthData.month,
                                               currentBirthData.latitude, currentBirthData.longitude);
        std::string text = monthly.generateTraditionalMyanmarCalendar(data);
        openWindow("Myanmar Monthly Calendar", text);
    } else {
        messageBox(mfError | mfOKButton, "Failed to initialize Myanmar Monthly Calendar.");
    }
}

void HoroscopeTuiApp::showChineseCalendar() {
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
    Astro::BirthChart chart = computeCurrentChart(currentBirthData, currentHouseSystem, currentZodiacMode, currentAyanamsa);

    Astro::WesternChartDrawer drawer;
    std::string text = drawer.drawAspectGrid(chart);

    openWindow("Planetary Aspect Grid", text);
}

void HoroscopeTuiApp::showConjunctions() {
    Astro::ConjunctionCalculator conj;
    conj.initialize();
    Astro::BirthData toDate = currentBirthData;
    toDate.year += 1; // 1 year search

    auto list = conj.findConjunctions(currentBirthData, toDate, 3.0);
    std::string text = conj.generateConjunctionReport(list);

    openWindow("Planetary Conjunctions (1-Year Window)", text);
}

void HoroscopeTuiApp::showEclipses() {
    Astro::EclipseCalculator ecl;
    ecl.initialize();
    Astro::BirthData toDate = currentBirthData;
    toDate.year += 2; // 2 years search

    auto list = ecl.findEclipses(currentBirthData, toDate);
    std::string text = ecl.generateEclipseReport(list);

    openWindow("Solar & Lunar Eclipses (2-Year Window)", text);
}

void HoroscopeTuiApp::showKPTable() {
    Astro::BirthChart chart = computeCurrentChart(currentBirthData, currentHouseSystem, Astro::ZodiacMode::SIDEREAL, Astro::AyanamsaType::KRISHNAMURTI);

    Astro::KPSystem kp;
    kp.initialize();
    std::string text = kp.generateKPTable(chart.getPlanetPositions());

    openWindow("KP System 5-Levels Sub-Lord Analysis", text);
}

void HoroscopeTuiApp::showKPTransitions() {
    Astro::KPSystem kp;
    kp.initialize();
    Astro::BirthData toDate = currentBirthData;
    toDate.day += 7; // 7 days window

    auto transitions = kp.findTransitions(currentBirthData, toDate, Astro::Planet::SUN, Astro::KPLevel::SUB);
    std::string text = kp.generateTransitionTable(transitions);

    openWindow("KP Planetary Transitions (Sun Sub-Lord)", text);
}

void HoroscopeTuiApp::showEphemerisTable() {
    Astro::EphemerisTable ephTable;
    ephTable.initialize();
    Astro::EphemerisConfig cfg;
    cfg.startDate = currentBirthData;
    cfg.endDate = currentBirthData;
    cfg.endDate.day += 14; // 14 days table
    cfg.intervalDays = 1.0;

    std::string text = ephTable.generateTable(cfg);

    openWindow("14-Day Planetary Ephemeris Table", text);
}

void HoroscopeTuiApp::showHelpAbout() {
    messageBox(mfInformation | mfOKButton,
        "Horoscope CLI & TVision TUI v2.1.0\n\n"
        "Professional Multi-Calendar & Astrological Engine\n"
        "Powered by Swiss Ephemeris & Turbo Vision 2.0\n\n"
        "Features:\n"
        "• Western, Vedic (North, South, East) & Solar System Charts\n"
        "• KP System 5-Levels Sub-Lord Analysis\n"
        "• Hindu Panchang, Myanmar & Chinese Calendars\n"
        "• Conjunctions, Eclipses & Aspect Grids\n\n"
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

int runTuiApplication() {
    HoroscopeTuiApp app;
    app.run();
    return 0;
}

} // namespace AstroTui
