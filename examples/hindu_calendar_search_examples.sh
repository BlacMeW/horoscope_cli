#!/bin/bash

# 🇮🇳 HINDU CALENDAR SEARCH EXAMPLES 🇮🇳
# Comprehensive examples of Hindu Panchanga calendar search functionality
# These examples demonstrate all available search criteria and combinations

echo "🕉️  HINDU CALENDAR SEARCH EXAMPLES 🕉️"
echo "═══════════════════════════════════════════════════════════════════"
echo

# Set common parameters for Delhi, India
LAT=28.6139
LON=77.2090
TIMEZONE="+05:30"

# Binary path (adjust if needed)
BINARY="../bin/horoscope_cli"

# Check if binary exists
if [ ! -f "$BINARY" ]; then
    echo "❌ Binary not found at $BINARY"
    echo "Please compile the project first: make clean && make"
    exit 1
fi

echo "📍 Location: Delhi, India (${LAT}°N, ${LON}°E)"
echo "🗓️  Date Range: 2024-01-01 to 2024-02-29"
echo

# =============================================================================
# EXAMPLE 1: Basic Year Search
# =============================================================================
echo "🔍 EXAMPLE 1: Search for Hindu Year 5125 (Shaka Era Year)"
echo "─────────────────────────────────────────────────────────────────"
$BINARY --hindu-search 2024-01-01 2024-01-15 \
        --search-year 5125 \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 2: Year Range Search
# =============================================================================
echo "🔍 EXAMPLE 2: Search for Hindu Year Range 5124-5126"
echo "─────────────────────────────────────────────────────────────────"
$BINARY --hindu-search 2023-12-01 2024-03-01 \
        --search-year-range 5124 5126 \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 3: Month Search (Chaitra = 0, Vaisakha = 1, etc.)
# =============================================================================
echo "🔍 EXAMPLE 3: Search for Chaitra Month (Month 0)"
echo "─────────────────────────────────────────────────────────────────"
$BINARY --hindu-search 2024-03-01 2024-05-01 \
        --search-month 0 \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 4: Month Range Search
# =============================================================================
echo "🔍 EXAMPLE 4: Search for Spring Months (Chaitra to Jyeshtha: 0-2)"
echo "─────────────────────────────────────────────────────────────────"
$BINARY --hindu-search 2024-03-01 2024-07-01 \
        --search-month-range 0 2 \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 5: Tithi Search (Lunar Day)
# =============================================================================
echo "🔍 EXAMPLE 5: Search for Purnima (Full Moon - Tithi 15)"
echo "─────────────────────────────────────────────────────────────────"
$BINARY --hindu-search 2024-01-01 2024-03-31 \
        --search-tithi 15 \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 6: Tithi Range Search
# =============================================================================
echo "🔍 EXAMPLE 6: Search for Krishna Paksha (Waning Moon - Tithis 1-15)"
echo "─────────────────────────────────────────────────────────────────"
$BINARY --hindu-search 2024-02-01 2024-02-29 \
        --search-tithi-range 1 15 \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 7: Paksha Search (Moon Phase)
# =============================================================================
echo "🔍 EXAMPLE 7: Search for Shukla Paksha (Waxing Moon Phase)"
echo "─────────────────────────────────────────────────────────────────"
$BINARY --hindu-search 2024-01-01 2024-01-31 \
        --search-paksha 0 \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 8: Weekday Search
# =============================================================================
echo "🔍 EXAMPLE 8: Search for Sundays (Weekday 0)"
echo "─────────────────────────────────────────────────────────────────"
$BINARY --hindu-search 2024-01-01 2024-01-31 \
        --search-weekday 0 \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 9: Nakshatra Search (Lunar Mansion)
# =============================================================================
echo "🔍 EXAMPLE 9: Search for Ashwini Nakshatra (Nakshatra 0)"
echo "─────────────────────────────────────────────────────────────────"
$BINARY --hindu-search 2024-01-01 2024-02-29 \
        --search-nakshatra 0 \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 10: Nakshatra Range Search
# =============================================================================
echo "🔍 EXAMPLE 10: Search for First Quarter Nakshatras (0-6)"
echo "─────────────────────────────────────────────────────────────────"
$BINARY --hindu-search 2024-01-01 2024-01-31 \
        --search-nakshatra-range 0 6 \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 11: Yoga Search
# =============================================================================
echo "🔍 EXAMPLE 11: Search for Vishkambha Yoga (Yoga 0)"
echo "─────────────────────────────────────────────────────────────────"
$BINARY --hindu-search 2024-01-01 2024-02-29 \
        --search-yoga 0 \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 12: Karana Search
# =============================================================================
echo "🔍 EXAMPLE 12: Search for Bava Karana (Karana 0)"
echo "─────────────────────────────────────────────────────────────────"
$BINARY --hindu-search 2024-01-01 2024-01-31 \
        --search-karana 0 \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 13: Multi-Criteria Search (Complex)
# =============================================================================
echo "🔍 EXAMPLE 13: Multi-Criteria Search - Chaitra Month + Sunday + Shukla Paksha"
echo "─────────────────────────────────────────────────────────────────"
$BINARY --hindu-search 2024-03-01 2024-05-01 \
        --search-month 0 \
        --search-weekday 0 \
        --search-paksha 0 \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 14: Festival/Auspicious Day Search
# =============================================================================
echo "🔍 EXAMPLE 14: Search for Amavasya (New Moon - Tithi 30)"
echo "─────────────────────────────────────────────────────────────────"
$BINARY --hindu-search 2024-01-01 2024-03-31 \
        --search-tithi 30 \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 15: Near Matching with Tolerance
# =============================================================================
echo "🔍 EXAMPLE 15: Near Matching - Find dates near Ekadashi (Tithi 11) with tolerance 2"
echo "─────────────────────────────────────────────────────────────────"
$BINARY --hindu-search 2024-01-01 2024-02-29 \
        --search-tithi 11 \
        --search-near 2 \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 16: Specific Calendar System Search
# =============================================================================
echo "🔍 EXAMPLE 16: Search with Specific Calendar System Parameters"
echo "─────────────────────────────────────────────────────────────────"
$BINARY --hindu-search 2024-04-01 2024-04-30 \
        --search-month 1 \
        --search-tithi-range 10 15 \
        --search-weekday 2 \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 17: Extended Date Range Search
# =============================================================================
echo "🔍 EXAMPLE 17: Extended Search - Full Year Hindu Calendar Analysis"
echo "─────────────────────────────────────────────────────────────────"
$BINARY --hindu-search 2024-01-01 2024-12-31 \
        --search-tithi 15 \
        --search-weekday 0 \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 18: High Precision Near Matching
# =============================================================================
echo "🔍 EXAMPLE 18: High Precision Near Matching - Multiple Criteria with Tolerance"
echo "─────────────────────────────────────────────────────────────────"
$BINARY --hindu-search 2024-01-01 2024-01-31 \
        --search-month-range 0 1 \
        --search-nakshatra-range 5 10 \
        --search-near 3 \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 19: Seasonal Search
# =============================================================================
echo "🔍 EXAMPLE 19: Seasonal Search - Monsoon Months (Ashadha to Bhadrapada)"
echo "─────────────────────────────────────────────────────────────────"
$BINARY --hindu-search 2024-06-01 2024-09-30 \
        --search-month-range 3 5 \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 20: Complete Panchanga Analysis
# =============================================================================
echo "🔍 EXAMPLE 20: Complete Panchanga Analysis - All Elements Combined"
echo "─────────────────────────────────────────────────────────────────"
$BINARY --hindu-search 2024-02-01 2024-02-15 \
        --search-year 5125 \
        --search-month 10 \
        --search-tithi-range 1 15 \
        --search-nakshatra-range 10 15 \
        --search-yoga-range 5 10 \
        --search-exact \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo

echo "🎉 Hindu Calendar Search Examples Complete!"
echo "═══════════════════════════════════════════════════════════════════"
echo
echo "📝 SEARCH PARAMETER REFERENCE:"
echo "   Years: Hindu calendar years (e.g., 5125 for 2024 CE)"
echo "   Months: 0=Chaitra, 1=Vaisakha, 2=Jyeshtha, 3=Ashadha, 4=Shravana,"
echo "           5=Bhadrapada, 6=Ashwin, 7=Kartik, 8=Margashirsha,"
echo "           9=Pausha, 10=Magha, 11=Phalguna"
echo "   Tithis: 1-15 (Krishna/Shukla Paksha), 30=Amavasya"
echo "   Paksha: 0=Shukla (Waxing), 1=Krishna (Waning)"
echo "   Weekdays: 0=Sunday, 1=Monday, ..., 6=Saturday"
echo "   Nakshatras: 0=Ashwini, 1=Bharani, ..., 26=Revati"
echo "   Yogas: 0=Vishkambha, 1=Priti, ..., 26=Vaidhriti"
echo "   Karanas: 0=Bava, 1=Balava, ..., 10=Naga"
echo
echo "🔧 MATCHING MODES:"
echo "   --search-exact: Exact matching (default)"
echo "   --search-near N: Near matching with tolerance N"
echo
