#!/bin/bash

# 🇲🇲 MYANMAR CALENDAR SEARCH EXAMPLES 🇲🇲
# Comprehensive examples of Myanmar calendar search functionality
# These examples demonstrate all available search criteria and combinations

echo "🏛️  MYANMAR CALENDAR SEARCH EXAMPLES 🏛️"
echo "═══════════════════════════════════════════════════════════════════"
echo

# Set common parameters for Yangon, Myanmar
LAT=16.8409
LON=96.1735
TIMEZONE="+06:30"

# Binary path (adjust if needed)
BINARY="../bin/horoscope_cli"

# Check if binary exists
if [ ! -f "$BINARY" ]; then
    echo "❌ Binary not found at $BINARY"
    echo "Please compile the project first: make clean && make"
    exit 1
fi

echo "📍 Location: Yangon, Myanmar (${LAT}°N, ${LON}°E)"
echo "🗓️  Date Range: 2024-01-01 to 2024-02-29"
echo

# =============================================================================
# EXAMPLE 1: Basic Myanmar Year Search
# =============================================================================
echo "🔍 EXAMPLE 1: Search for Myanmar Era Year 1385"
echo "─────────────────────────────────────────────────────────────────"
$BINARY --myanmar-search 2024-01-01 2024-01-15 \
        --myanmar-search-year 1385 \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 2: Myanmar Year Range Search
# =============================================================================
echo "🔍 EXAMPLE 2: Search for Myanmar Year Range 1384-1386"
echo "─────────────────────────────────────────────────────────────────"
$BINARY --myanmar-search 2023-12-01 2024-03-01 \
        --myanmar-search-year-range 1384 1386 \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 3: Month Search (Myanmar Calendar Months)
# =============================================================================
echo "🔍 EXAMPLE 3: Search for Tagu Month (Month 1)"
echo "─────────────────────────────────────────────────────────────────"
$BINARY --myanmar-search 2024-03-15 2024-05-15 \
        --myanmar-search-month 1 \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 4: Month Range Search
# =============================================================================
echo "🔍 EXAMPLE 4: Search for Hot Season Months (Tagu to Nayon: 1-3)"
echo "─────────────────────────────────────────────────────────────────"
$BINARY --myanmar-search 2024-03-01 2024-07-01 \
        --myanmar-search-month-range 1 3 \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 5: Moon Phase Search
# =============================================================================
echo "🔍 EXAMPLE 5: Search for Full Moon Days (Moon Phase 1)"
echo "─────────────────────────────────────────────────────────────────"
$BINARY --myanmar-search 2024-01-01 2024-03-31 \
        --myanmar-search-moon-phase 1 \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 6: Moon Phase Range Search
# =============================================================================
echo "🔍 EXAMPLE 6: Search for Waxing to Full Moon (Phases 0-1)"
echo "─────────────────────────────────────────────────────────────────"
$BINARY --myanmar-search 2024-02-01 2024-02-29 \
        --myanmar-search-moon-phase-range 0 1 \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 7: Weekday Search (Myanmar System)
# =============================================================================
echo "🔍 EXAMPLE 7: Search for Saturdays (Weekday 0 in Myanmar System)"
echo "─────────────────────────────────────────────────────────────────"
$BINARY --myanmar-search 2024-01-01 2024-01-31 \
        --myanmar-search-weekday 0 \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 8: Fortnight Day Search
# =============================================================================
echo "🔍 EXAMPLE 8: Search for 15th Fortnight Day (Full Moon Day)"
echo "─────────────────────────────────────────────────────────────────"
$BINARY --myanmar-search 2024-01-01 2024-03-31 \
        --myanmar-search-fortnight-day 15 \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 9: Fortnight Day Range Search
# =============================================================================
echo "🔍 EXAMPLE 9: Search for Near Full Moon (Fortnight Days 14-15)"
echo "─────────────────────────────────────────────────────────────────"
$BINARY --myanmar-search 2024-01-01 2024-02-29 \
        --myanmar-search-fortnight-day-range 14 15 \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 10: Buddhist Sabbath Search
# =============================================================================
echo "🔍 EXAMPLE 10: Search for Buddhist Sabbath Days"
echo "─────────────────────────────────────────────────────────────────"
$BINARY --myanmar-search 2024-01-01 2024-02-29 \
        --myanmar-search-sabbath \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 11: Sabbath Eve Search
# =============================================================================
echo "🔍 EXAMPLE 11: Search for Sabbath Eve Days"
echo "─────────────────────────────────────────────────────────────────"
$BINARY --myanmar-search 2024-01-01 2024-01-31 \
        --myanmar-search-sabbath-eve \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 12: Yatyaza (Inauspicious) Days Search
# =============================================================================
echo "🔍 EXAMPLE 12: Search for Yatyaza (Inauspicious) Days"
echo "─────────────────────────────────────────────────────────────────"
$BINARY --myanmar-search 2024-01-01 2024-01-31 \
        --myanmar-search-yatyaza \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 13: Pyathada (Inauspicious) Days Search
# =============================================================================
echo "🔍 EXAMPLE 13: Search for Pyathada (Inauspicious) Days"
echo "─────────────────────────────────────────────────────────────────"
$BINARY --myanmar-search 2024-01-01 2024-01-31 \
        --myanmar-search-pyathada \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 14: Thamanyo (Auspicious) Days Search
# =============================================================================
echo "🔍 EXAMPLE 14: Search for Thamanyo (Auspicious) Days"
echo "─────────────────────────────────────────────────────────────────"
$BINARY --myanmar-search 2024-01-01 2024-01-31 \
        --myanmar-search-thamanyo \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 15: Multi-Criteria Search (Complex)
# =============================================================================
echo "🔍 EXAMPLE 15: Multi-Criteria - Kason Month + Full Moon + Saturday"
echo "─────────────────────────────────────────────────────────────────"
$BINARY --myanmar-search 2024-04-01 2024-06-01 \
        --myanmar-search-month 2 \
        --myanmar-search-moon-phase 1 \
        --myanmar-search-weekday 0 \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 16: Near Matching with Tolerance
# =============================================================================
echo "🔍 EXAMPLE 16: Near Matching - Find dates near Full Moon with tolerance 2"
echo "─────────────────────────────────────────────────────────────────"
$BINARY --myanmar-search 2024-01-01 2024-02-29 \
        --myanmar-search-moon-phase 1 \
        --myanmar-search-near 2 \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 17: Water Festival Season Search (Thingyan)
# =============================================================================
echo "🔍 EXAMPLE 17: Water Festival Season - Tagu Month + Specific Days"
echo "─────────────────────────────────────────────────────────────────"
$BINARY --myanmar-search 2024-04-10 2024-04-20 \
        --myanmar-search-month 1 \
        --myanmar-search-fortnight-day-range 1 5 \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 18: Monsoon Season Analysis
# =============================================================================
echo "🔍 EXAMPLE 18: Monsoon Season - Waso Month Analysis"
echo "─────────────────────────────────────────────────────────────────"
$BINARY --myanmar-search 2024-07-01 2024-08-31 \
        --myanmar-search-month 4 \
        --myanmar-search-sabbath \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 19: New Year Period Search
# =============================================================================
echo "🔍 EXAMPLE 19: Myanmar New Year Period - Tagu to Kason Transition"
echo "─────────────────────────────────────────────────────────────────"
$BINARY --myanmar-search 2024-04-01 2024-05-15 \
        --myanmar-search-month-range 1 2 \
        --myanmar-search-thamanyo \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 20: Extended Religious Calendar Analysis
# =============================================================================
echo "🔍 EXAMPLE 20: Extended Religious Analysis - Full Year Sabbath Days"
echo "─────────────────────────────────────────────────────────────────"
$BINARY --myanmar-search 2024-01-01 2024-12-31 \
        --myanmar-search-sabbath \
        --myanmar-search-year 1385 \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 21: Advanced Astrological Day Combination
# =============================================================================
echo "🔍 EXAMPLE 21: Advanced Search - Avoid Inauspicious Days"
echo "─────────────────────────────────────────────────────────────────"
echo "Searching for good days (excluding yatyaza and pyathada)..."
$BINARY --myanmar-search 2024-01-01 2024-01-31 \
        --myanmar-search-thamanyo \
        --myanmar-search-moon-phase-range 0 1 \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 22: Seasonal Festival Planning
# =============================================================================
echo "🔍 EXAMPLE 22: Festival Planning - Waso Robe Offering Season"
echo "─────────────────────────────────────────────────────────────────"
$BINARY --myanmar-search 2024-07-01 2024-10-31 \
        --myanmar-search-month-range 4 7 \
        --myanmar-search-sabbath \
        --myanmar-search-moon-phase 1 \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 23: Wedding Planning Example
# =============================================================================
echo "🔍 EXAMPLE 23: Wedding Planning - Auspicious Days in Cool Season"
echo "─────────────────────────────────────────────────────────────────"
$BINARY --myanmar-search 2024-11-01 2024-02-28 \
        --myanmar-search-month-range 8 11 \
        --myanmar-search-thamanyo \
        --myanmar-search-weekday-range 1 5 \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 24: High Precision Complex Search
# =============================================================================
echo "🔍 EXAMPLE 24: High Precision - Multiple Criteria with Near Matching"
echo "─────────────────────────────────────────────────────────────────"
$BINARY --myanmar-search 2024-03-01 2024-03-31 \
        --myanmar-search-year 1385 \
        --myanmar-search-month-range 1 2 \
        --myanmar-search-fortnight-day-range 10 15 \
        --myanmar-search-near 3 \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo

echo "🎉 Myanmar Calendar Search Examples Complete!"
echo "═══════════════════════════════════════════════════════════════════"
echo
echo "📝 SEARCH PARAMETER REFERENCE:"
echo "   Years: Myanmar Era years (e.g., 1385 for 2024 CE)"
echo "   Months: 0=First Waso, 1=Tagu, 2=Kason, 3=Nayon, 4=Waso,"
echo "           5=Wagaung, 6=Tawthalin, 7=Thadingyut, 8=Tazaungmone,"
echo "           9=Nadaw, 10=Pyatho, 11=Tabodwe, 12=Tabaung,"
echo "           13=Late Tagu, 14=Late Kason"
echo "   Moon Phases: 0=Waxing, 1=Full, 2=Waning, 3=New"
echo "   Weekdays: 0=Saturday, 1=Sunday, 2=Monday, ..., 6=Friday"
echo "   Fortnight Days: 1-15 (day within moon cycle)"
echo
echo "🏛️ ASTROLOGICAL DAYS:"
echo "   --myanmar-search-sabbath: Buddhist sabbath days (8th, 15th, 23rd, new moon)"
echo "   --myanmar-search-sabbath-eve: Day before sabbath"
echo "   --myanmar-search-yatyaza: Inauspicious days (avoid business/travel)"
echo "   --myanmar-search-pyathada: Inauspicious days (avoid ceremonies)"
echo "   --myanmar-search-thamanyo: Auspicious days (good for ceremonies)"
echo
echo "🔧 MATCHING MODES:"
echo "   --myanmar-search-exact: Exact matching (default)"
echo "   --myanmar-search-near N: Near matching with tolerance N"
echo
echo "💡 USAGE TIPS:"
echo "   • Combine multiple criteria for complex searches"
echo "   • Use near matching for flexible date planning"
echo "   • Consider astrological days for traditional events"
echo "   • Check both auspicious and inauspicious days for planning"
