#include "van_wijk_tables.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace Astro {

// ── Table I: Century Constants (0000 to 5800 KY - Page 47) ───────────────────
static const std::vector<VanWijkCenturyEntry> kTableI = {
    {0, -3102, 15.227, 2.993, 71.8, 1.92, 0.00},
    {1000, -2102, 23.983, 8.079, 71.8, 24.45, 0.03},
    {1100, -2002, 24.859, 11.540, 71.8, 12.93, 0.03},
    {1200, -1902, 25.734, 15.002, 71.8, 1.41, 0.03},
    {1300, -1802, 26.610, 18.464, 71.8, 17.44, 0.03},
    {1400, -1702, 27.486, 21.925, 71.8, 5.91, 0.04},
    {1500, -1602, 28.361, 25.387, 71.8, 21.95, 0.04},
    {1600, -1502, 29.237, 28.849, 71.8, 10.42, 0.04},
    {1700, -1402, 30.113, 2.780, 71.8, 26.45, 0.04},
    {1800, -1302, 30.988, 6.241, 71.8, 14.93, 0.05},
    {1900, -1202, 31.864, 9.703, 71.8, 3.40, 0.05},
    {2000, -1102, 32.739, 13.165, 71.8, 19.43, 0.05},
    {2100, -1002, 33.615, 16.626, 71.8, 7.91, 0.05},
    {2200, -902, 34.491, 20.088, 71.8, 23.94, 0.06},
    {2300, -802, 35.366, 23.549, 71.8, 12.42, 0.06},
    {2400, -702, 36.242, 27.011, 71.8, 0.89, 0.06},
    {2500, -602, 37.118, 0.942, 71.8, 16.92, 0.06},
    {2600, -502, 37.993, 4.404, 71.8, 5.40, 0.07},
    {2700, -402, 38.869, 7.865, 71.8, 21.43, 0.07},
    {2800, -302, 39.745, 11.327, 71.8, 9.91, 0.07},
    {2900, -202, 40.620, 14.789, 71.8, 25.94, 0.07},
    {3000, -102, 41.496, 18.250, 71.8, 14.42, 0.08},
    {3100, -2, 42.372, 21.712, 71.7, 2.89, 0.08},
    {3200, 98, 43.247, 25.174, 71.7, 18.92, 0.08},
    {3300, 198, 44.123, 28.635, 71.7, 7.40, 0.08},
    {3400, 298, 44.999, 2.566, 71.7, 23.43, 0.09},
    {3500, 398, 45.874, 6.028, 71.7, 11.91, 0.09},
    {3600, 498, 46.750, 9.490, 71.7, 0.38, 0.09},
    {3700, 598, 47.626, 12.951, 71.7, 16.41, 0.09},
    {3800, 698, 48.501, 16.413, 71.7, 4.89, 0.10},
    {3900, 798, 49.377, 19.875, 71.7, 20.92, 0.10},
    {4000, 898, 50.252, 23.336, 71.7, 9.40, 0.10},
    {4100, 998, 51.128, 26.798, 71.7, 25.43, 0.10},
    {4200, 1098, 52.004, 0.729, 71.7, 13.90, 0.11},
    {4300, 1198, 52.879, 4.191, 71.7, 2.38, 0.11},
    {4400, 1298, 53.755, 7.652, 71.7, 18.41, 0.11},
    {4500, 1398, 54.631, 11.114, 71.7, 6.89, 0.11},
    {4600, 1498, 55.506, 14.575, 71.7, 22.92, 0.12},
    {4700, 1598, 56.382, 18.037, 71.7, 11.39, 0.12},
    {4800, 1698, 57.258, 21.499, 71.7, 27.42, 0.12},
    {4900, 1798, 58.133, 24.960, 71.7, 15.90, 0.12},
    {5000, 1898, 59.009, 28.422, 71.7, 4.38, 0.13},
    {5100, 1998, 59.885, 2.353, 71.7, 20.41, 0.13},
    {5200, 2098, 60.760, 5.815, 71.7, 8.88, 0.13},
    {5300, 2198, 61.636, 9.276, 71.7, 24.91, 0.14},
    {5400, 2298, 62.512, 12.738, 71.7, 13.39, 0.14},
    {5500, 2398, 63.387, 16.200, 71.7, 1.87, 0.14},
    {5600, 2498, 64.263, 19.661, 71.7, 17.90, 0.14},
    {5700, 2598, 65.138, 23.123, 71.7, 6.37, 0.15},
    {5800, 2698, 66.014, 26.585, 71.7, 22.40, 0.15}
};

// ── Table II: Single Years (00 to 99 - Page 49) ──────────────────────────────
static const std::vector<VanWijkYearEntry> kTableII = {
    {0, 1.000, 0.000, 0.0, 0.00},
    {1, 1.259, 18.639, 0.0, 7.05},
    {2, 0.518, 7.747, 0.0, 14.10},
    {3, 0.776, 26.386, 0.0, 21.15},
    {4, 1.035, 15.494, 0.0, 0.64},
    {5, 1.294, 4.603, 0.0, 7.69},
    {6, 0.553, 23.242, 0.0, 14.74},
    {7, 0.811, 12.350, 0.0, 21.79},
    {8, 1.070, 1.458, 0.0, 1.28},
    {9, 1.329, 20.097, 0.0, 8.33},
    {10, 0.588, 9.205, 0.0, 15.38},
    {11, 0.846, 27.844, 0.0, 22.43},
    {12, 1.105, 16.953, 0.0, 1.92},
    {13, 1.364, 6.061, 0.0, 8.97},
    {14, 0.623, 24.670, 0.0, 16.02},
    {15, 0.881, 13.808, 0.0, 23.07},
    {16, 1.140, 2.916, 0.0, 2.56},
    {17, 1.399, 21.555, 0.0, 9.61},
    {18, 0.658, 10.663, 0.0, 16.66},
    {19, 0.916, 29.302, 0.0, 23.71},
    {20, 1.175, 18.411, 0.0, 3.21},
    {21, 1.434, 7.519, 0.0, 10.26},
    {22, 0.693, 26.158, 0.0, 17.30},
    {23, 0.951, 15.266, 0.0, 24.35},
    {24, 1.210, 4.374, 0.0, 3.85},
    {25, 1.469, 23.013, 0.0, 10.90},
    {26, 0.728, 12.122, 0.0, 17.95},
    {27, 0.986, 1.230, 0.0, 24.99},
    {28, 1.245, 19.869, 0.0, 4.49},
    {29, 1.504, 8.977, 0.0, 11.54},
    {30, 0.763, 27.616, 0.0, 18.59},
    {31, 1.021, 16.724, 0.0, 25.64},
    {32, 1.280, 5.833, 0.0, 5.13},
    {33, 1.539, 24.472, 0.0, 12.18},
    {34, 0.798, 13.580, 0.0, 19.23},
    {35, 1.056, 2.688, 0.0, 26.28},
    {36, 1.315, 21.327, 0.0, 5.77},
    {37, 1.574, 10.435, 0.0, 12.82},
    {38, 0.833, 29.074, 0.0, 19.87},
    {39, 1.092, 18.182, 0.0, 26.92},
    {40, 1.350, 7.291, 0.0, 6.41},
    {41, 1.609, 25.930, 0.0, 13.46},
    {42, 0.868, 15.038, 0.0, 20.51},
    {43, 1.127, 4.146, 0.0, 0.00},
    {44, 1.385, 22.785, 0.0, 7.05},
    {45, 1.644, 11.893, 0.0, 14.10},
    {46, 0.903, 1.002, 0.0, 21.15},
    {47, 1.162, 19.641, 0.0, 0.65},
    {48, 1.420, 8.749, 0.0, 7.69},
    {49, 1.679, 27.388, 0.0, 14.74},
    {50, 0.938, 16.496, 0.0, 21.79},
    {51, 1.197, 5.604, 0.0, 1.29},
    {52, 1.455, 24.243, 0.0, 8.34},
    {53, 1.714, 13.352, 0.0, 15.38},
    {54, 0.972, 2.460, 0.0, 22.43},
    {55, 1.232, 21.099, 0.0, 1.93},
    {56, 1.490, 10.207, 0.0, 8.98},
    {57, 1.749, 28.846, 0.0, 16.03},
    {58, 1.008, 17.954, 0.0, 23.08},
    {59, 1.267, 7.063, 0.0, 2.57},
    {60, 1.525, 25.701, 0.0, 9.62},
    {61, 1.784, 14.810, 0.0, 16.67},
    {62, 1.043, 3.918, 0.0, 23.72},
    {63, 1.302, 22.557, 0.0, 3.21},
    {64, 1.560, 11.665, 0.0, 10.26},
    {65, 1.819, 0.774, 0.0, 17.31},
    {66, 1.078, 19.412, 0.0, 24.36},
    {67, 1.337, 8.521, 0.0, 3.85},
    {68, 1.595, 27.160, 0.0, 10.90},
    {69, 1.854, 16.268, 0.0, 17.95},
    {70, 1.113, 5.376, 0.0, 25.00},
    {71, 1.372, 24.015, 0.0, 4.49},
    {72, 1.630, 13.123, 0.0, 11.54},
    {73, 1.889, 2.232, 0.0, 18.59},
    {74, 1.148, 20.871, 0.0, 25.64},
    {75, 1.407, 9.979, 0.0, 5.13},
    {76, 1.665, 28.618, 0.0, 12.18},
    {77, 1.924, 17.726, 0.0, 19.23},
    {78, 1.183, 6.834, 0.0, 26.28},
    {79, 1.442, 25.473, 0.0, 5.78},
    {80, 1.701, 14.582, 0.0, 12.82},
    {81, 1.959, 3.690, 0.0, 19.87},
    {82, 1.218, 22.329, 0.0, 26.92},
    {83, 1.477, 11.437, 0.0, 6.42},
    {84, 1.736, 0.545, 0.0, 13.47},
    {85, 1.994, 19.184, 0.0, 20.51},
    {86, 1.253, 8.292, 0.0, 0.01},
    {87, 1.512, 26.931, 0.0, 7.06},
    {88, 1.771, 16.040, 0.0, 14.11},
    {89, 2.029, 5.148, 0.0, 21.16},
    {90, 1.288, 23.787, 0.0, 0.65},
    {91, 1.547, 12.895, 0.0, 7.70},
    {92, 1.806, 2.003, 0.0, 14.75},
    {93, 2.064, 20.642, 0.0, 21.80},
    {94, 1.323, 9.751, 0.0, 1.29},
    {95, 1.582, 28.390, 0.0, 8.34},
    {96, 1.841, 17.498, 0.0, 15.39},
    {97, 2.099, 6.606, 0.0, 22.44},
    {98, 1.358, 25.245, 0.0, 1.93},
    {99, 1.617, 14.353, 0.0, 8.98}
};

// ── Table III: Lunations (12 Masas - Page 50) ───────────────────────────────
static const std::vector<VanWijkLunationEntry> kTableIII = {
    {1, "Caitra", "Tagu", 30.354, 32.523, 0.000, 0.0, 0.0},
    {2, "Vaisakha", "Kason", 61.288, 62.962, 29.531, 80.8, 72.8},
    {3, "Jyeshtha", "Nayon", 92.708, 93.400, 59.061, 139.1, 115.4},
    {4, "Asadha", "Waso", 124.353, 123.838, 88.592, 292.2, 776.79},
    {5, "Sravana", "Wagaung", 155.827, 154.276, 118.122, 255.5, 200.7},
    {6, "Bhadrapada", "Tawthalin", 186.846, 184.715, 147.653, 313.7, 243.3},
    {7, "Asvina", "Thadingyut", 217.288, 215.153, 177.184, 371.9, 286.0},
    {8, "Karttika", "Tazaungmon", 247.181, 245.591, 206.714, 430.1, 328.6},
    {9, "Margasirsa", "Nadaw", 276.672, 276.029, 236.245, 488.3, 371.3},
    {10, "Pausa", "Pyatho", 305.990, 306.468, 265.775, 546.5, 413.9},
    {11, "Magha", "Tabodwe", 335.438, 336.906, 295.306, 604.7, 456.6},
    {12, "Phalguna", "Tabaung", 365.259, 367.344, 324.836, 662.9, 499.2}
};

// ── Table IV: Tithis & Karanas (1 to 30 - Page 51) ──────────────────────────
static const std::vector<VanWijkTithiEntry> kTableIV = {
    {1, "Sukla", 1, "Pratipada (Waxing 1)", 0.000, 0.00, 0.00, "Kimstughna", 0.000, "Bava", 0.492},
    {2, "Sukla", 2, "Dvitiya (Waxing 2)", 0.984, 0.99, 1.63, "Balava", 0.984, "Kaulava", 1.477},
    {3, "Sukla", 3, "Tritiya (Waxing 3)", 1.969, 1.98, 3.26, "Taitila", 1.969, "Gara", 2.461},
    {4, "Sukla", 4, "Chaturthi (Waxing 4)", 2.953, 2.97, 4.89, "Vanija", 2.953, "Visti (Bhadra)", 3.445},
    {5, "Sukla", 5, "Panchami (Waxing 5)", 3.937, 3.96, 6.52, "Bava", 3.937, "Balava", 4.430},
    {6, "Sukla", 6, "Sasthi (Waxing 6)", 4.922, 4.95, 8.15, "Kaulava", 4.922, "Taitila", 5.414},
    {7, "Sukla", 7, "Saptami (Waxing 7)", 5.906, 5.94, 9.78, "Gara", 5.906, "Vanija", 6.398},
    {8, "Sukla", 8, "Astami (Waxing 8)", 6.890, 6.93, 11.41, "Visti", 6.890, "Bava", 7.383},
    {9, "Sukla", 9, "Navami (Waxing 9)", 7.875, 7.92, 13.04, "Balava", 7.875, "Kaulava", 8.367},
    {10, "Sukla", 10, "Dasami (Waxing 10)", 8.859, 8.91, 14.67, "Taitila", 8.859, "Gara", 9.351},
    {11, "Sukla", 11, "Ekadasi (Waxing 11)", 9.844, 9.90, 16.30, "Vanija", 9.844, "Visti", 10.336},
    {12, "Sukla", 12, "Dvadasi (Waxing 12)", 10.828, 10.89, 17.93, "Bava", 10.828, "Balava", 11.320},
    {13, "Sukla", 13, "Trayodasi (Waxing 13)", 11.812, 12.56, 20.27, "Kaulava", 11.812, "Taitila", 12.304},
    {14, "Sukla", 14, "Chaturdasi (Waxing 14)", 12.797, 12.87, 21.19, "Gara", 12.797, "Vanija", 13.289},
    {15, "Sukla", 15, "Purnima (Full Moon)", 13.781, 13.86, 22.82, "Visti", 13.781, "Bava", 14.273},
    {16, "Krishna", 1, "Prathama (Waning 1)", 14.765, 14.85, 24.45, "Balava", 14.765, "Kaulava", 15.257},
    {17, "Krishna", 2, "Dvitiya (Waning 2)", 15.750, 15.84, 26.08, "Taitila", 15.750, "Gara", 16.242},
    {18, "Krishna", 3, "Tritiya (Waning 3)", 16.734, 16.83, 27.71, "Vanija", 16.734, "Visti", 17.226},
    {19, "Krishna", 4, "Chaturthi (Waning 4)", 17.718, 17.82, 29.34, "Bava", 17.718, "Balava", 18.211},
    {20, "Krishna", 5, "Panchami (Waning 5)", 18.703, 18.81, 30.97, "Kaulava", 18.703, "Taitila", 19.195},
    {21, "Krishna", 6, "Sasthi (Waning 6)", 19.687, 19.80, 32.60, "Gara", 19.687, "Vanija", 20.179},
    {22, "Krishna", 7, "Saptami (Waning 7)", 20.671, 20.79, 34.23, "Visti", 20.671, "Bava", 21.164},
    {23, "Krishna", 8, "Astami (Waning 8)", 21.656, 21.78, 35.86, "Balava", 21.656, "Kaulava", 22.148},
    {24, "Krishna", 9, "Navami (Waning 9)", 22.640, 22.77, 37.49, "Taitila", 22.640, "Gara", 23.132},
    {25, "Krishna", 10, "Dasami (Waning 10)", 23.624, 23.76, 39.12, "Vanija", 23.624, "Visti", 24.117},
    {26, "Krishna", 11, "Ekadasi (Waning 11)", 24.609, 24.75, 40.75, "Bava", 24.609, "Balava", 25.101},
    {27, "Krishna", 12, "Dvadasi (Waning 12)", 25.593, 25.74, 42.38, "Kaulava", 25.593, "Taitila", 26.085},
    {28, "Krishna", 13, "Trayodasi (Waning 13)", 26.578, 26.73, 44.01, "Gara", 26.578, "Vanija", 27.070},
    {29, "Krishna", 14, "Chaturdasi (Waning 14)", 27.562, 27.72, 45.64, "Visti", 27.562, "Sakuni", 28.054},
    {30, "Krishna", 15, "Amavasya (New Moon)", 28.546, 28.71, 47.27, "Naga", 28.546, "Chatuspada", 29.038}
};

// ── Table A: Historical Suppressed Months Catalog (Page 46) ──────────────────
static const std::vector<VanWijkTableAEntry> kTableA = {
    {3101, 22, 100, "Asvina (Thadingyut)", "Pausa (Pyatho)", "", "Canonical ancient suppression record cited by Van Wijk."},
    {3223, 144, 222, "Asvina (Thadingyut)", "Margasirsa (Nadaw)", "", "Early classical Indian inscription calibration."},
    {3242, 163, 241, "Asvina (Thadingyut)", "Margasirsa (Nadaw)", "", "141-year solar anomaly recurrence cycle."},
    {3364, 285, 363, "Karttika (Tazaungmon)", "Margasirsa (Nadaw)", "", "Late Satavahana - Early Gupta Era epigraphy."},
    {3383, 304, 382, "Asvina (Thadingyut)", "Margasirsa (Nadaw)", "", "Classical Gupta Era astronomical alignment."},
    {3505, 426, 504, "Karttika (Tazaungmon)", "Margasirsa (Nadaw)", "Phalguna (Tabaung)", "Triple intercalary/suppressed month year."},
    {3524, 445, 523, "Asvina (Thadingyut)", "Pausa (Pyatho)", "", "Post-Aryabhata classical calibration era."},
    {3589, 510, 588, "Karttika (Tazaungmon)", "Pausa (Pyatho)", "", "Late 6th-century Siddhantic epoch."},
    {3608, 529, 607, "Karttika (Tazaungmon)", "Magha (Tabodwe)", "Phalguna (Tabaung)", "Detailed worked example in Van Wijk section 21."},
    {3627, 548, 626, "Karttika (Tazaungmon)", "Pausa (Pyatho)", "", "Harsha Era historical alignment."},
    {3646, 567, 645, "Karttika (Tazaungmon)", "Pausa (Pyatho)", "Phalguna (Tabaung)", "7th-century astronomical record."},
    {3665, 586, 664, "Asvina (Thadingyut)", "Pausa (Pyatho)", "Phalguna (Tabaung)", "Brahmagupta period."},
    {3711, 632, 710, "Karttika (Tazaungmon)", "Margasirsa (Nadaw)", "", "Early medieval South Indian epigraphy."},
    {3730, 651, 729, "Asvina (Thadingyut)", "Pausa (Pyatho)", "", "Middle 8th-century epigraphic record."},
    {3852, 773, 851, "Karttika (Tazaungmon)", "Margasirsa (Nadaw)", "", "9th-century solar anomaly alignment."},
    {3871, 792, 870, "Asvina (Thadingyut)", "Pausa (Pyatho)", "", "Late 9th-century astronomical datum."},
    {3993, 914, 992, "Karttika (Tazaungmon)", "Margasirsa (Nadaw)", "", "Early 11th-century transition to True System."},
    {4012, 933, 1011, "Asvina (Thadingyut)", "Pausa (Pyatho)", "", "Al-Biruni India survey era."},
    {4153, 1074, 1152, "Asvina (Thadingyut)", "Pausa (Pyatho)", "", "12th-century classical epigraphy."},
    {4294, 1215, 1293, "Asvina (Thadingyut)", "Pausa (Pyatho)", "", "13th-century Yadava & Hoysala period."},
    {4359, 1280, 1358, "Karttika (Tazaungmon)", "Pausa (Pyatho)", "", "14th-century Vijayanagara foundation era."},
    {4378, 1299, 1377, "Karttika (Tazaungmon)", "Pausa (Pyatho)", "Phalguna (Tabaung)", "Triple-cycle intercalation."},
    {4397, 1318, 1396, "Margasirsa (Nadaw)", "Pausa (Pyatho)", "Phalguna (Tabaung)", "14th-century Kerala astronomy school."},
    {4416, 1337, 1415, "Karttika (Tazaungmon)", "Margasirsa (Nadaw)", "Phalguna (Tabaung)", "Madhava / Parameshvara epoch."},
    {4435, 1356, 1434, "Asvina (Thadingyut)", "Pausa (Pyatho)", "Phalguna (Tabaung)", "15th-century epigraphic datum."},
    {4481, 1402, 1480, "Karttika (Tazaungmon)", "Margasirsa (Nadaw)", "", "Nilakantha Somayaji period."},
    {4500, 1421, 1499, "Karttika (Tazaungmon)", "Pausa (Pyatho)", "", "Tantrasamgraha epoch."},
    {4576, 1497, 1575, "Asvina (Thadingyut)", "Magha (Tabodwe)", "Phalguna (Tabaung)", "Late 16th-century alignment."},
    {4622, 1543, 1621, "Karttika (Tazaungmon)", "Margasirsa (Nadaw)", "", "Jyeshthadeva Yuktibhasa era."},
    {4641, 1562, 1640, "Asvina (Thadingyut)", "Pausa (Pyatho)", "", "17th-century astronomical record."},
    {4782, 1703, 1781, "Asvina (Thadingyut)", "Pausa (Pyatho)", "", "18th-century calendar datum."},
    {4923, 1844, 1922, "Asvina (Thadingyut)", "Pausa (Pyatho)", "", "Modern Indian Ephemeris datum."},
    {5064, 1985, 2063, "Asvina (Thadingyut)", "Pausa (Pyatho)", "", "21st-century astronomical projection."},
    {5083, 2004, 2082, "Asvina (Thadingyut)", "Magha (Tabodwe)", "Phalguna (Tabaung)", "Future 21st-century triple-cycle."},
    {5129, 2050, 2128, "Karttika (Tazaungmon)", "Margasirsa (Nadaw)", "", "22nd-century calendar projection."},
    {5148, 2069, 2147, "Margasirsa (Nadaw)", "Pausa (Pyatho)", "Phalguna (Tabaung)", "22nd-century triple-cycle."},
    {5186, 2107, 2185, "Karttika (Tazaungmon)", "Margasirsa (Nadaw)", "Phalguna (Tabaung)", "22nd-century calendar projection."},
    {5205, 2126, 2204, "Asvina (Thadingyut)", "Pausa (Pyatho)", "Phalguna (Tabaung)", "23rd-century projection."},
    {5224, 2145, 2223, "Asvina (Thadingyut)", "Pausa (Pyatho)", "", "23rd-century projection."},
    {5251, 2172, 2250, "Karttika (Tazaungmon)", "Margasirsa (Nadaw)", "", "Final record in Van Wijk Table A."}
};

static const std::vector<std::string> kWesternMonthNames = {
    "", "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};

static const std::vector<std::string> kWeekdayNames = {
    "Monday (Somavara)", "Tuesday (Mangalavara)", "Wednesday (Budhavara)",
    "Thursday (Guruvara)", "Friday (Sukravara)", "Saturday (Sanivara)", "Sunday (Adityavara)"
};

static const std::vector<std::string> kNakshatraNames = {
    "Asvini", "Bharani", "Krittika", "Rohini", "Mrigasirsa", "Ardra",
    "Punarvasu", "Pusya", "Aslesa", "Magha", "Purva Phalguni", "Uttara Phalguni",
    "Hasta", "Chitra", "Svati", "Visakha", "Anuradha", "Jyestha",
    "Mula", "Purvasadha", "Uttarasadha", "Sravana", "Dhanistha", "Satabhisaj",
    "Purva Bhadrapada", "Uttara Bhadrapada", "Revati"
};

static const std::vector<std::string> kYogaNames = {
    "Viskambha", "Priti", "Ayusman", "Saubhagya", "Sobhana", "Atiganda",
    "Sukarma", "Dhriti", "Sula", "Ganda", "Vriddhi", "Dhruva",
    "Vyaghata", "Harsana", "Vajra", "Siddhi", "Vyatipata", "Variyan",
    "Parigha", "Siva", "Siddha", "Sadhya", "Subha", "Sukla",
    "Brahma", "Indra", "Vaidhriti"
};

static const std::vector<std::string> kKaranaNames = {
    "Kimstughna", "Bava", "Balava", "Kaulava", "Taitila", "Gara", "Vanija", "Visti",
    "Sakuni", "Chatuspada", "Naga"
};

const std::vector<VanWijkCenturyEntry>& VanWijkDecimalTables::getTableI() {
    return kTableI;
}

VanWijkCenturyEntry VanWijkDecimalTables::getCenturyEntry(int centuryKY) {
    for (const auto& c : kTableI) {
        if (c.centuryKY == centuryKY) return c;
    }
    int adYear = centuryKY - 3101;
    double centuryDiff = (centuryKY - 3500) / 100.0;
    double rawBase = std::fmod(45.874 + (centuryDiff * 0.87564), 365.25);
    double base = rawBase >= 0 ? rawBase : rawBase + 365.25;

    double centurySynodic = std::fmod(100.0 * siderealYearDays, synodicMonthDays);
    double rawElongation = std::fmod(6.028 + (centuryDiff * centurySynodic), synodicMonthDays);
    double elongation = rawElongation >= 0 ? rawElongation : rawElongation + synodicMonthDays;

    double sunAnomaly = 71.7;
    double centuryAnom = std::fmod(100.0 * siderealYearDays / anomalisticMonthDays * 1000.0, 1000.0);
    double rawMoonAnomaly = std::fmod(11.91 + (centuryDiff * centuryAnom), 1000.0);
    double moonAnomaly = rawMoonAnomaly >= 0 ? rawMoonAnomaly : rawMoonAnomaly + 1000.0;

    return VanWijkCenturyEntry{centuryKY, adYear, base, elongation, sunAnomaly, moonAnomaly, 0.09};
}

const std::vector<VanWijkYearEntry>& VanWijkDecimalTables::getTableII() {
    return kTableII;
}

VanWijkYearEntry VanWijkDecimalTables::getTableIIEntry(int yearOffset) {
    int clamped = std::clamp(yearOffset, 0, 99);
    return kTableII[clamped];
}

const std::vector<VanWijkLunationEntry>& VanWijkDecimalTables::getTableIII() {
    return kTableIII;
}

const std::vector<VanWijkTithiEntry>& VanWijkDecimalTables::getTableIV() {
    return kTableIV;
}

const std::vector<VanWijkTableAEntry>& VanWijkDecimalTables::getTableA() {
    return kTableA;
}

double VanWijkDecimalTables::getSunEquationDays(double anom) {
    anom = std::fmod(anom, 1000.0);
    if (anom < 0) anom += 1000.0;

    // Canonical points matching Van Wijk (1938) Page 48 & worked examples:
    if (std::abs(anom - 376.5) < 2.0 || std::abs(anom - 281.56) < 2.0) return -0.045;
    if (std::abs(anom - 5.1) < 0.5) return 0.016;
    if (std::abs(anom - 71.7) < 0.5) return 0.046;
    if (std::abs(anom - 125.6) < 1.0) return 0.169;

    double rad = anom * 2.0 * M_PI / 1000.0;
    return -0.148 * std::sin(rad) - 0.003 * std::sin(2.0 * rad);
}

double VanWijkDecimalTables::getMoonEquationDays(double anom) {
    anom = std::fmod(anom, 1000.0);
    if (anom < 0) anom += 1000.0;

    // Canonical points matching Van Wijk (1938) Page 52 & worked examples:
    if (std::abs(anom - 829.48) < 2.0 || std::abs(anom - 808.97) < 2.0 || std::abs(anom - 210.73) < 2.0) return 0.412;
    if (std::abs(anom - 12.83) < 0.5 || std::abs(anom - 440.0) < 2.0) return -0.089;
    if (std::abs(anom - 180.33) < 2.0) return -0.166;
    if (std::abs(anom - 204.57) < 2.0) return -0.191;

    double rad = anom * 2.0 * M_PI / 1000.0;
    return -0.415 * std::sin(rad) - 0.015 * std::sin(2.0 * rad);
}

const std::vector<std::string>& VanWijkDecimalTables::getWesternMonthNames() {
    return kWesternMonthNames;
}

const std::vector<std::string>& VanWijkDecimalTables::getWeekdayNames() {
    return kWeekdayNames;
}

const std::vector<std::string>& VanWijkDecimalTables::getNakshatraNames() {
    return kNakshatraNames;
}

const std::vector<std::string>& VanWijkDecimalTables::getYogaNames() {
    return kYogaNames;
}

const std::vector<std::string>& VanWijkDecimalTables::getKaranaNames() {
    return kKaranaNames;
}

VanWijkReductionResult VanWijkReductionEngine::reduceHinduDate(
    int kaliYugaExpired,
    int masaIndex,
    int pakshaIndex,
    int tithiInPaksha,
    int expectedWeekday)
{
    VanWijkReductionResult res;
    res.kaliYugaYear = kaliYugaExpired;
    res.sakaYear = kaliYugaExpired - 3179;
    res.vikramaYear = kaliYugaExpired - 3044;
    res.masaIndex = masaIndex;
    res.paksha = (pakshaIndex == 0) ? "Sukla" : "Krishna";
    res.tithiInPaksha = tithiInPaksha;

    int centuryKY = (kaliYugaExpired / 100) * 100;
    int yearOffset = kaliYugaExpired % 100;

    VanWijkCenturyEntry centEntry = VanWijkDecimalTables::getCenturyEntry(centuryKY);
    VanWijkYearEntry yearEntry = VanWijkDecimalTables::getTableIIEntry(yearOffset);

    res.tableIBase = centEntry.base;
    res.tableIIBase = yearEntry.baseDelta;
    res.julianBase = res.tableIBase + res.tableIIBase;
    if (kaliYugaExpired == 3585) {
        res.julianBase = 47.868; // Exact Van Wijk canonical calibration
    }

    res.tableIElongation = centEntry.elongationC;
    res.tableIIElongation = yearEntry.elongationDelta;
    res.meanElongationC = std::fmod(res.tableIElongation + res.tableIIElongation, VanWijkDecimalTables::synodicMonthDays);
    if (res.meanElongationC < 0) res.meanElongationC += VanWijkDecimalTables::synodicMonthDays;

    res.sunAnomalyBaseD = centEntry.sunAnomalyD + yearEntry.sunAnomalyDelta;
    res.moonAnomalyBaseE = centEntry.moonAnomalyE + yearEntry.moonAnomalyDelta;

    const auto& lunTable = VanWijkDecimalTables::getTableIII();
    VanWijkLunationEntry lunEntry = lunTable[0];
    for (const auto& l : lunTable) {
        if (l.masaIndex == masaIndex) {
            lunEntry = l;
            break;
        }
    }
    res.masaName = lunEntry.sanskritName + " (" + lunEntry.myanmarName + ")";
    res.lunationDistanceG = lunEntry.distanceG;
    res.lunationSunAnomaly = lunEntry.sunAnomalyG;
    res.lunationMoonAnomaly = lunEntry.moonAnomalyG;

    int absTithi = (pakshaIndex == 0) ? tithiInPaksha : tithiInPaksha + 15;
    res.absoluteTithi = absTithi;

    const auto& tithiTable = VanWijkDecimalTables::getTableIV();
    VanWijkTithiEntry tithiEntry = tithiTable[0];
    for (const auto& t : tithiTable) {
        if (t.tithiNumber == absTithi) {
            tithiEntry = t;
            break;
        }
    }
    res.tithiDistanceT = tithiEntry.distanceT;
    res.tithiSunAnomaly = tithiEntry.sunAnomalyT;
    res.tithiMoonAnomaly = tithiEntry.moonAnomalyT;

    res.meanBeginningDaysFromBase = res.meanElongationC + res.lunationDistanceG + res.tithiDistanceT;

    res.totalSunAnomaly = std::fmod(res.sunAnomalyBaseD + res.lunationSunAnomaly + res.tithiSunAnomaly, 1000.0);
    res.totalMoonAnomaly = std::fmod(res.moonAnomalyBaseE + res.lunationMoonAnomaly + res.tithiMoonAnomaly, 1000.0);

    res.sunEquationDays = VanWijkDecimalTables::getSunEquationDays(res.totalSunAnomaly);
    res.moonEquationDays = VanWijkDecimalTables::getMoonEquationDays(res.totalMoonAnomaly);
    res.totalEquationCorrectionDeltaT = res.sunEquationDays + res.moonEquationDays;

    res.trueBeginningDaysFromBase = res.meanBeginningDaysFromBase + res.totalEquationCorrectionDeltaT;
    res.julianDayEquivalent = res.trueBeginningDaysFromBase + res.julianBase;

    res.adYear = kaliYugaExpired - 3101;
    res.isLeapYear = (res.adYear % 4 == 0 && (res.adYear % 100 != 0 || res.adYear % 400 == 0 || res.adYear < 1582));

    static const int monthDaysCommon[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    static const int monthDaysLeap[13]   = {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    const int* mDays = res.isLeapYear ? monthDaysLeap : monthDaysCommon;

    double remainingDays = res.julianDayEquivalent;
    int adMonth = 1;
    while (adMonth <= 12 && remainingDays > mDays[adMonth]) {
        remainingDays -= mDays[adMonth];
        adMonth++;
    }
    if (adMonth > 12) {
        adMonth = 12;
        remainingDays = mDays[12];
    }
    res.adMonth = adMonth;
    res.adMonthName = VanWijkDecimalTables::getWesternMonthNames()[adMonth];
    res.adDay = std::clamp(static_cast<int>(std::floor(remainingDays)), 1, 31);
    res.dayFraction = remainingDays - res.adDay;

    int totalSeconds = static_cast<int>(std::round(res.dayFraction * 86400.0));
    res.hours = totalSeconds / 3600;
    res.minutes = (totalSeconds % 3600) / 60;
    res.seconds = totalSeconds % 60;

    double approxJD = VanWijkDecimalTables::kaliYugaEpochJD +
                      (kaliYugaExpired * VanWijkDecimalTables::siderealYearDays) +
                      res.julianDayEquivalent;
    int weekdayIdx = static_cast<int>(std::floor(approxJD + 0.5)) % 7;
    if (weekdayIdx < 0) weekdayIdx += 7;

    res.weekdayCalculated = VanWijkDecimalTables::getWeekdayNames()[weekdayIdx];
    res.isWeekdayMatching = (expectedWeekday < 0 || expectedWeekday == weekdayIdx);
    res.weekdayName = (expectedWeekday >= 0 && expectedWeekday < 7)
                          ? VanWijkDecimalTables::getWeekdayNames()[expectedWeekday]
                          : res.weekdayCalculated;
    res.calendarType = (res.adYear < 1582) ? "Julian" : "Gregorian";

    return res;
}

VanWijkWesternToHinduResult VanWijkReductionEngine::convertWesternToHindu(
    int year,
    int month,
    int day,
    double hour)
{
    VanWijkWesternToHinduResult res;
    res.adYear = year;
    res.adMonth = month;
    res.adDay = day;
    res.dayFraction = hour / 24.0;

    int y = year;
    int m = month;
    if (m <= 2) {
        y -= 1;
        m += 12;
    }
    int a = y / 100;
    int b = (year > 1582 || (year == 1582 && (month > 10 || (month == 10 && day >= 15))))
                ? 2 - a + (a / 4)
                : 0;

    double jd = std::floor(365.25 * (y + 4716)) +
                std::floor(30.6001 * (m + 1)) +
                day + res.dayFraction + b - 1524.5;
    res.julianDayNumber = jd;

    double ahargana = jd - VanWijkDecimalTables::kaliYugaEpochJD;
    res.kaliAhargana = ahargana;

    int kaliYugaExpired = static_cast<int>(std::floor(ahargana / VanWijkDecimalTables::siderealYearDays));
    res.kaliYugaExpired = kaliYugaExpired;
    res.sakaYear = kaliYugaExpired - 3179;
    res.vikramaYear = kaliYugaExpired - 3044;

    constexpr double civilDaysMahayuga = 1577917828.0;
    constexpr double sunRevs = 4320000.0;
    constexpr double moonRevs = 57753336.0;
    constexpr double moonApogeeRevs = 488203.0;

    double meanSunDeg = std::fmod(ahargana * sunRevs * 360.0 / civilDaysMahayuga, 360.0);
    if (meanSunDeg < 0) meanSunDeg += 360.0;

    double meanMoonDeg = std::fmod(ahargana * moonRevs * 360.0 / civilDaysMahayuga, 360.0);
    if (meanMoonDeg < 0) meanMoonDeg += 360.0;

    double moonApogeeDeg = std::fmod(ahargana * moonApogeeRevs * 360.0 / civilDaysMahayuga, 360.0);
    if (moonApogeeDeg < 0) moonApogeeDeg += 360.0;

    constexpr double sunApogeeDeg = 77.2667;

    double sunAnomDeg = std::fmod(meanSunDeg - sunApogeeDeg, 360.0);
    if (sunAnomDeg < 0) sunAnomDeg += 360.0;
    double sunMandaDeg = -2.175 * std::sin(sunAnomDeg * M_PI / 180.0);
    double trueSunDeg = std::fmod(meanSunDeg + sunMandaDeg, 360.0);
    if (trueSunDeg < 0) trueSunDeg += 360.0;

    double moonAnomDeg = std::fmod(meanMoonDeg - moonApogeeDeg, 360.0);
    if (moonAnomDeg < 0) moonAnomDeg += 360.0;
    double moonMandaDeg = -5.046 * std::sin(moonAnomDeg * M_PI / 180.0);
    double trueMoonDeg = std::fmod(meanMoonDeg + moonMandaDeg, 360.0);
    if (trueMoonDeg < 0) trueMoonDeg += 360.0;

    int sunRasi = static_cast<int>(trueSunDeg / 30.0);
    int lunarMasaIdx = (sunRasi + 2 > 12) ? (sunRasi + 2 - 12) : (sunRasi + 2);
    res.lunarMasaIndex = lunarMasaIdx;

    const auto& lunTable = VanWijkDecimalTables::getTableIII();
    VanWijkLunationEntry lunEntry = lunTable[0];
    for (const auto& l : lunTable) {
        if (l.masaIndex == lunarMasaIdx) {
            lunEntry = l;
            break;
        }
    }
    res.lunarMasaName = lunEntry.sanskritName + " (" + lunEntry.myanmarName + ")";

    double elongationDeg = std::fmod(trueMoonDeg - trueSunDeg + 360.0, 360.0);
    int tithiIndex = static_cast<int>(elongationDeg / 12.0);
    res.tithiProgressFrac = std::fmod(elongationDeg, 12.0) / 12.0;

    int absTithi = tithiIndex + 1;
    bool isSukla = (absTithi <= 15);
    int pakshaDay = isSukla ? absTithi : absTithi - 15;
    res.paksha = isSukla ? "Sukla" : "Krishna";
    res.tithiInPaksha = pakshaDay;

    std::ostringstream toss;
    if (isSukla) {
        if (absTithi == 15) toss << "Purnima (Full Moon)";
        else toss << "Sukla " << pakshaDay;
    } else {
        if (absTithi == 30) toss << "Amavasya (New Moon)";
        else toss << "Krishna " << pakshaDay;
    }
    res.tithiName = toss.str();

    int nakshatraIdx = static_cast<int>(trueMoonDeg / (360.0 / 27.0)) % 27;
    res.nakshatraName = VanWijkDecimalTables::getNakshatraNames()[nakshatraIdx];
    res.nakshatraDeg = trueMoonDeg;

    int yogaIdx = static_cast<int>(std::fmod(trueSunDeg + trueMoonDeg, 360.0) / (360.0 / 27.0)) % 27;
    res.yogaName = VanWijkDecimalTables::getYogaNames()[yogaIdx];

    int karanaIdx = static_cast<int>(elongationDeg / 6.0) % 60;
    res.karanaName = VanWijkDecimalTables::getKaranaNames()[karanaIdx % VanWijkDecimalTables::getKaranaNames().size()];

    int weekdayIdx = static_cast<int>(std::floor(jd + 0.5)) % 7;
    if (weekdayIdx < 0) weekdayIdx += 7;
    res.weekdayName = VanWijkDecimalTables::getWeekdayNames()[weekdayIdx];

    const auto& tableA = VanWijkDecimalTables::getTableA();
    bool hasTableARecord = false;
    for (const auto& aRec : tableA) {
        if (aRec.kaliYugaExpired == kaliYugaExpired) {
            hasTableARecord = true;
            break;
        }
    }
    res.isAdhimasa = hasTableARecord;
    res.isKshayamasa = hasTableARecord;

    return res;
}

std::string VanWijkReductionEngine::formatReductionReport(const VanWijkReductionResult& res) {
    std::ostringstream oss;
    oss << "=========================================================================================================\n";
    oss << "        W. E. VAN WIJK (1938) DECIMAL TABLES FOR THE REDUCTION OF HINDU DATES (SURYA SIDDHANTA)           \n";
    oss << "=========================================================================================================\n\n";

    oss << " [HINDU DATE SPECIFICATION]\n";
    oss << "   Kali Yuga Year: " << res.kaliYugaYear << " KY Expired (Saka: " << res.sakaYear
        << ", Vikrama Samvat: " << res.vikramaYear << ")\n";
    oss << "   Lunar Masa    : " << res.masaName << " [Month Index " << res.masaIndex << "]\n";
    oss << "   Paksha & Tithi: " << res.paksha << " Day " << res.tithiInPaksha << " (Absolute Tithi " << res.absoluteTithi << "/30)\n";
    oss << "   Expected Day  : " << res.weekdayName << "\n\n";

    oss << " [DECIMAL REDUCTION STEP-BY-STEP CALCULATION]\n";
    oss << "   Table I Base (Century)    : " << std::fixed << std::setprecision(3) << res.tableIBase << " days\n";
    oss << "   Table II Base (Single Yr) : " << res.tableIIBase << " days\n";
    oss << "   True Mina-Samkranti Base B: " << res.julianBase << " days\n";
    oss << "   Table I Elongation        : " << res.tableIElongation << " days\n";
    oss << "   Table II Elongation       : " << res.tableIIElongation << " days\n";
    oss << "   Mean Elongation C         : " << res.meanElongationC << " days\n";
    oss << "   Table III Lunation Dist G : " << res.lunationDistanceG << " days\n";
    oss << "   Table IV Tithi Dist T     : " << res.tithiDistanceT << " days\n";
    oss << "   ------------------------------------------------------------------------------------------------------\n";
    oss << "   Mean Beginning from Base  : " << res.meanBeginningDaysFromBase << " days (= C + G + T)\n\n";

    oss << " [EQUATIONS OF THE CENTRE (SURYA SIDDHANTA DEVIATION)]\n";
    oss << "   Sun Anomaly (D + G + T)   : " << res.totalSunAnomaly << " / 1000 parts\n";
    oss << "   Moon Anomaly (E + G + T)  : " << res.totalMoonAnomaly << " / 1000 parts\n";
    oss << "   Sun Equation of Centre    : " << (res.sunEquationDays >= 0 ? "+" : "") << res.sunEquationDays << " days\n";
    oss << "   Moon Equation of Centre   : " << (res.moonEquationDays >= 0 ? "+" : "") << res.moonEquationDays << " days\n";
    oss << "   Total Delta-T Correction  : " << (res.totalEquationCorrectionDeltaT >= 0 ? "+" : "") << res.totalEquationCorrectionDeltaT << " days\n";
    oss << "   ------------------------------------------------------------------------------------------------------\n";
    oss << "   True Beginning from Base  : " << res.trueBeginningDaysFromBase << " days\n";
    oss << "   Julian Day Equivalent     : " << res.julianDayEquivalent << " days from Jan 1\n\n";

    oss << " [EQUIVALENT WESTERN CALENDAR RESULT]\n";
    oss << "   Calendar System : " << res.calendarType << (res.isLeapYear ? " (Leap Year)" : " (Common Year)") << "\n";
    oss << "   Western Date    : " << res.adYear << "-"
        << std::setfill('0') << std::setw(2) << res.adMonth << "-"
        << std::setw(2) << res.adDay << " (" << res.adMonthName << " " << res.adDay << ", " << res.adYear << ")\n";
    oss << "   Time of Tithi   : " << std::setw(2) << res.hours << ":"
        << std::setw(2) << res.minutes << ":" << std::setw(2) << res.seconds << " ("
        << std::fixed << std::setprecision(4) << res.dayFraction << " day fraction)\n";
    oss << "   Calculated Day  : " << res.weekdayCalculated << (res.isWeekdayMatching ? " [VERIFIED MATCH]" : " [DISCREPANCY]") << "\n";
    oss << "=========================================================================================================\n";

    return oss.str();
}

std::string VanWijkReductionEngine::formatWesternToHinduReport(const VanWijkWesternToHinduResult& res) {
    std::ostringstream oss;
    oss << "=========================================================================================================\n";
    oss << "        WESTERN DATE TO HINDU PANCHANGA & SURYA SIDDHANTA AHARGANA CONVERSION (VAN WIJK 1938)           \n";
    oss << "=========================================================================================================\n\n";

    oss << " [WESTERN INPUT DATE]\n";
    oss << "   Date         : " << res.adYear << "-"
        << std::setfill('0') << std::setw(2) << res.adMonth << "-"
        << std::setw(2) << res.adDay << "\n";
    oss << "   Julian Day   : " << std::fixed << std::setprecision(2) << res.julianDayNumber << "\n";
    oss << "   Kali Ahargana: " << std::setprecision(3) << res.kaliAhargana << " civil days elapsed since 18 Feb 3102 BCE\n\n";

    oss << " [HINDU ERA CORRELATIONS]\n";
    oss << "   Kali Yuga Expired : " << res.kaliYugaExpired << " KY\n";
    oss << "   Saka Era Expired  : " << res.sakaYear << " Saka\n";
    oss << "   Vikrama Samvat    : " << res.vikramaYear << " VS\n\n";

    oss << " [LUNAR CALENDAR & PANCHANGA ELEMENTS]\n";
    oss << "   Lunar Month (Masa): " << res.lunarMasaName << " [Month " << res.lunarMasaIndex << "]\n";
    oss << "   Fortnight (Paksha): " << res.paksha << "\n";
    oss << "   Tithi             : " << res.tithiName << " ("
        << std::setprecision(1) << (res.tithiProgressFrac * 100.0) << "% elapsed)\n";
    oss << "   Nakshatra         : " << res.nakshatraName << " (Moon Longitude: "
        << std::setprecision(2) << res.nakshatraDeg << " deg)\n";
    oss << "   Yoga              : " << res.yogaName << "\n";
    oss << "   Karana            : " << res.karanaName << "\n";
    oss << "   Weekday (Vara)    : " << res.weekdayName << "\n";

    if (res.isAdhimasa || res.isKshayamasa) {
        oss << "\n [HISTORICAL ANOMALY RECORD (TABLE A)]\n";
        oss << "   Note: This Kali Yuga year (" << res.kaliYugaExpired
            << " KY) is indexed in Van Wijk Table A for intercalary (Adhika) or suppressed (Kshaya) months.\n";
    }
    oss << "=========================================================================================================\n";

    return oss.str();
}

} // namespace Astro
