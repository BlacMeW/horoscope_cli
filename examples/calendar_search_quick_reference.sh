#!/bin/bash

# 📖 CALENDAR SEARCH QUICK REFERENCE & DEMO 📖
# Quick demonstration of all calendar search features
# Use this for rapid testing and parameter reference

echo "📖 CALENDAR SEARCH QUICK REFERENCE & DEMO 📖"
echo "═══════════════════════════════════════════════════════════════════"
echo

# Binary path (adjust if needed)
BINARY="../bin/horoscope_cli"
TODAY=$(date +%Y-%m-%d)
WEEK_LATER=$(date -d "+7 days" +%Y-%m-%d)

# Check if binary exists
if [ ! -f "$BINARY" ]; then
    echo "❌ Binary not found at $BINARY"
    echo "Please compile the project first: make clean && make"
    exit 1
fi

echo "🚀 Quick Demo using dates: $TODAY to $WEEK_LATER"
echo

# =============================================================================
# QUICK DEMO SECTION
# =============================================================================

echo "═══════════════════════════════════════════════════════════════════"
echo "🎯 QUICK DEMO - Basic Searches"
echo "═══════════════════════════════════════════════════════════════════"

echo
echo "🇮🇳 Hindu Calendar - Find all Sundays this week:"
$BINARY --hindu-search $TODAY $WEEK_LATER --search-weekday 0 --lat 28.61 --lon 77.21 --timezone "+05:30"

echo
echo "🇲🇲 Myanmar Calendar - Find Buddhist Sabbath days this week:"
$BINARY --myanmar-search $TODAY $WEEK_LATER --myanmar-search-sabbath --lat 16.84 --lon 96.17 --timezone "+06:30"

echo
echo "🌕 Find Full Moon days this month:"
echo "Hindu (Purnima):"
$BINARY --hindu-search $TODAY $(date -d "+30 days" +%Y-%m-%d) --search-tithi 15 --lat 28.61 --lon 77.21 --timezone "+05:30"
echo
echo "Myanmar (Full Moon):"
$BINARY --myanmar-search $TODAY $(date -d "+30 days" +%Y-%m-%d) --myanmar-search-moon-phase 1 --lat 16.84 --lon 96.17 --timezone "+06:30"

echo
read -p "Press Enter to see the complete parameter reference..."
echo

# =============================================================================
# COMPLETE PARAMETER REFERENCE
# =============================================================================

cat << 'EOF'

═══════════════════════════════════════════════════════════════════
📋 COMPLETE PARAMETER REFERENCE GUIDE
═══════════════════════════════════════════════════════════════════

🇮🇳 HINDU CALENDAR SEARCH PARAMETERS:
┌─────────────────────────────────────────────────────────────────┐
│ BASIC SEARCH:                                                   │
│   --hindu-search FROM TO          Date range (YYYY-MM-DD)      │
│                                                                 │
│ YEAR SEARCH:                                                    │
│   --search-year YEAR         Specific Hindu year         │
│   --search-year-range S E    Year range (start-end)      │
│                                                                 │
│ MONTH SEARCH:                                                   │
│   --search-month N           Specific month (0-11)       │
│   --search-month-range S E   Month range                 │
│                                                                 │
│ LUNAR DAY SEARCH:                                               │
│   --search-tithi N           Specific tithi (1-30)       │
│   --search-tithi-range S E   Tithi range                 │
│                                                                 │
│ MOON PHASE SEARCH:                                              │
│   --search-paksha N          Moon phase (0=Wax, 1=Wane)  │
│                                                                 │
│ WEEKDAY SEARCH:                                                 │
│   --search-weekday N         Weekday (0=Sun...6=Sat)     │
│                                                                 │
│ NAKSHATRA SEARCH:                                               │
│   --search-nakshatra N       Specific nakshatra (0-26)   │
│   --search-nakshatra-range   Nakshatra range             │
│                                                                 │
│ YOGA SEARCH:                                                    │
│   --search-yoga N            Specific yoga (0-26)        │
│   --search-yoga-range S E    Yoga range                  │
│                                                                 │
│ KARANA SEARCH:                                                  │
│   --search-karana N          Specific karana (0-10)      │
│   --search-karana-range S E  Karana range                │
│                                                                 │
│ MATCHING MODES:                                                 │
│   --search-exact             Exact matching (default)    │
│   --search-near N            Near matching, tolerance N  │
└─────────────────────────────────────────────────────────────────┘

🇲🇲 MYANMAR CALENDAR SEARCH PARAMETERS:
┌─────────────────────────────────────────────────────────────────┐
│ BASIC SEARCH:                                                   │
│   --myanmar-search FROM TO         Date range (YYYY-MM-DD)     │
│                                                                 │
│ YEAR SEARCH:                                                    │
│   --myanmar-search-year YEAR       Myanmar Era year            │
│   --myanmar-search-year-range S E  Year range                  │
│                                                                 │
│ MONTH SEARCH:                                                   │
│   --myanmar-search-month N         Specific month (0-14)       │
│   --myanmar-search-month-range S E Month range                 │
│                                                                 │
│ MOON PHASE SEARCH:                                              │
│   --myanmar-search-moon-phase N    Phase (0=Wax,1=Full,2=Wan,3=New) │
│   --myanmar-search-moon-phase-range Moon phase range           │
│                                                                 │
│ WEEKDAY SEARCH:                                                 │
│   --myanmar-search-weekday N       Weekday (0=Sat...6=Fri)     │
│                                                                 │
│ FORTNIGHT DAY SEARCH:                                           │
│   --myanmar-search-fortnight-day N Day in fortnight (1-15)     │
│   --myanmar-search-fortnight-day-range                         │
│                                                                 │
│ ASTROLOGICAL DAY SEARCH:                                        │
│   --myanmar-search-sabbath         Buddhist sabbath days       │
│   --myanmar-search-sabbath-eve     Sabbath eve days            │
│   --myanmar-search-yatyaza         Inauspicious yatyaza        │
│   --myanmar-search-pyathada        Inauspicious pyathada       │
│   --myanmar-search-thamanyo        Auspicious thamanyo         │
│                                                                 │
│ MATCHING MODES:                                                 │
│   --myanmar-search-exact           Exact matching (default)    │
│   --myanmar-search-near N          Near matching, tolerance N  │
└─────────────────────────────────────────────────────────────────┘

📅 VALUE REFERENCE TABLES:
┌─────────────────────────────────────────────────────────────────┐
│ HINDU MONTHS (--search-month):                           │
│   0=Chaitra    1=Vaisakha   2=Jyeshtha    3=Ashadha            │
│   4=Shravana   5=Bhadrapada 6=Ashwin      7=Kartik             │
│   8=Margashirsha 9=Pausha  10=Magha      11=Phalguna           │
│                                                                 │
│ HINDU TITHIS (--search-tithi):                           │
│   1-15: Krishna/Shukla Paksha days                             │
│   30: Amavasya (New Moon)                                      │
│   15: Purnima (Full Moon)                                      │
│   11: Ekadashi (Religious fasting day)                         │
│                                                                 │
│ NAKSHATRAS (--search-nakshatra):                         │
│   0=Ashwini    1=Bharani     2=Krittika    3=Rohini            │
│   4=Mrigashira 5=Ardra       6=Punarvasu   7=Pushya            │
│   ...and so on to 26=Revati                                    │
└─────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────┐
│ MYANMAR MONTHS (--myanmar-search-month):                       │
│   0=First Waso  1=Tagu      2=Kason       3=Nayon              │
│   4=Waso        5=Wagaung   6=Tawthalin   7=Thadingyut         │
│   8=Tazaungmone 9=Nadaw     10=Pyatho     11=Tabodwe           │
│   12=Tabaung    13=Late Tagu 14=Late Kason                     │
│                                                                 │
│ MOON PHASES (--myanmar-search-moon-phase):                     │
│   0=Waxing      1=Full      2=Waning      3=New                │
│                                                                 │
│ WEEKDAYS:                                                       │
│   Hindu:  0=Sun 1=Mon 2=Tue 3=Wed 4=Thu 5=Fri 6=Sat          │
│   Myanmar: 0=Sat 1=Sun 2=Mon 3=Tue 4=Wed 5=Thu 6=Fri         │
│                                                                 │
│ FORTNIGHT DAYS (--myanmar-search-fortnight-day):               │
│   1-15: Day within lunar fortnight                             │
│   15: Full moon day                                            │
│   1: New moon + 1 day                                          │
└─────────────────────────────────────────────────────────────────┘

💡 USAGE EXAMPLES:

Simple searches:
  $BINARY --hindu-search 2024-01-01 2024-01-31 --search-weekday 0
  $BINARY --myanmar-search 2024-01-01 2024-01-31 --myanmar-search-sabbath

Complex searches:
  $BINARY --hindu-search 2024-01-01 2024-12-31 \\
          --search-month-range 0 2 \\
          --search-tithi 15 \\
          --search-weekday 0 \\
          --lat 28.61 --lon 77.21 --timezone "+05:30"

  $BINARY --myanmar-search 2024-01-01 2024-12-31 \\
          --myanmar-search-year 1385 \\
          --myanmar-search-moon-phase-range 0 1 \\
          --myanmar-search-thamanyo \\
          --lat 16.84 --lon 96.17 --timezone "+06:30"

Near matching:
  $BINARY --hindu-search 2024-01-01 2024-01-31 \\
          --search-tithi 11 \\
          --search-near 2

  $BINARY --myanmar-search 2024-01-01 2024-01-31 \\
          --myanmar-search-moon-phase 1 \\
          --myanmar-search-near 2

📍 LOCATION PARAMETERS (Required for accurate calculations):
  --lat LATITUDE     Latitude in decimal degrees
  --lon LONGITUDE    Longitude in decimal degrees
  --timezone OFFSET  Timezone offset (e.g., "+05:30", "-08:00")

Common locations:
  Delhi:   --lat 28.61 --lon 77.21 --timezone "+05:30"
  Mumbai:  --lat 19.08 --lon 72.88 --timezone "+05:30"
  Yangon:  --lat 16.84 --lon 96.17 --timezone "+06:30"
  Bangkok: --lat 13.76 --lon 100.50 --timezone "+07:00"

═══════════════════════════════════════════════════════════════════
🎯 PRACTICAL TIPS:

1. FESTIVAL PLANNING:
   • Hindu: Search for specific tithis (e.g., 11 for Ekadashi)
   • Myanmar: Use --myanmar-search-sabbath for religious days

2. WEDDING PLANNING:
   • Hindu: Avoid Krishna Paksha, prefer Shukla Paksha Sundays
   • Myanmar: Use --myanmar-search-thamanyo for auspicious days

3. BUSINESS PLANNING:
   • Hindu: Avoid Amavasya (tithi 30), prefer Purnima (tithi 15)
   • Myanmar: Avoid yatyaza/pyathada, prefer thamanyo days

4. FLEXIBLE DATES:
   • Use --search-near or --myanmar-search-near with tolerance
   • Combine multiple criteria with ranges

5. COMPARATIVE ANALYSIS:
   • Run both Hindu and Myanmar searches for cross-cultural events
   • Compare lunar calculations between systems

═══════════════════════════════════════════════════════════════════
EOF

echo
echo "✅ Quick reference complete! Use the example files for detailed demonstrations:"
echo "   📄 hindu_calendar_search_examples.sh - 20 Hindu calendar examples"
echo "   📄 myanmar_calendar_search_examples.sh - 24 Myanmar calendar examples"
echo "   📄 combined_calendar_search_examples.sh - 10 comparative examples"
echo
echo "🚀 Ready to explore calendar search capabilities!"
