#!/bin/bash

# 🌏 COMBINED CALENDAR SEARCH EXAMPLES 🌏
# Demonstrates both Hindu and Myanmar calendar search capabilities
# Shows comparative searches and combined usage patterns

echo "🌏 COMBINED HINDU & MYANMAR CALENDAR SEARCH EXAMPLES 🌏"
echo "═══════════════════════════════════════════════════════════════════"
echo

# Set common parameters
BINARY="../bin/horoscope_cli"
DATE_START="2024-01-01"
DATE_END="2024-01-31"

# Locations
DELHI_LAT=28.6139
DELHI_LON=77.2090
DELHI_TZ="+05:30"

YANGON_LAT=16.8409
YANGON_LON=96.1735
YANGON_TZ="+06:30"

# Check if binary exists
if [ ! -f "$BINARY" ]; then
    echo "❌ Binary not found at $BINARY"
    echo "Please compile the project first: make clean && make"
    exit 1
fi

echo "📍 Locations: Delhi, India & Yangon, Myanmar"
echo "🗓️  Date Range: ${DATE_START} to ${DATE_END}"
echo

# =============================================================================
# EXAMPLE 1: Comparative Full Moon Search
# =============================================================================
echo "🌕 EXAMPLE 1: Full Moon Comparison - Hindu Purnima vs Myanmar Full Moon"
echo "─────────────────────────────────────────────────────────────────"
echo "🇮🇳 Hindu Calendar - Delhi (Purnima - Tithi 15):"
$BINARY --hindu-search $DATE_START $DATE_END \
        --search-tithi 15 \
        --lat $DELHI_LAT --lon $DELHI_LON --timezone $DELHI_TZ

echo
echo "🇲🇲 Myanmar Calendar - Yangon (Full Moon - Phase 1):"
$BINARY --myanmar-search $DATE_START $DATE_END \
        --myanmar-search-moon-phase 1 \
        --lat $YANGON_LAT --lon $YANGON_LON --timezone $YANGON_TZ
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 2: New Moon Comparison
# =============================================================================
echo "🌑 EXAMPLE 2: New Moon Comparison - Hindu Amavasya vs Myanmar New Moon"
echo "─────────────────────────────────────────────────────────────────"
echo "🇮🇳 Hindu Calendar - Delhi (Amavasya - Tithi 30):"
$BINARY --hindu-search $DATE_START $DATE_END \
        --search-tithi 30 \
        --lat $DELHI_LAT --lon $DELHI_LON --timezone $DELHI_TZ

echo
echo "🇲🇲 Myanmar Calendar - Yangon (New Moon - Phase 3):"
$BINARY --myanmar-search $DATE_START $DATE_END \
        --myanmar-search-moon-phase 3 \
        --lat $YANGON_LAT --lon $YANGON_LON --timezone $YANGON_TZ
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 3: Sunday Comparison
# =============================================================================
echo "☀️ EXAMPLE 3: Sunday Search Comparison"
echo "─────────────────────────────────────────────────────────────────"
echo "🇮🇳 Hindu Calendar - Delhi (Sunday - Weekday 0):"
$BINARY --hindu-search $DATE_START $DATE_END \
        --search-weekday 0 \
        --lat $DELHI_LAT --lon $DELHI_LON --timezone $DELHI_TZ

echo
echo "🇲🇲 Myanmar Calendar - Yangon (Sunday - Weekday 1):"
$BINARY --myanmar-search $DATE_START $DATE_END \
        --myanmar-search-weekday 1 \
        --lat $YANGON_LAT --lon $YANGON_LON --timezone $YANGON_TZ
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 4: Religious Days Comparison
# =============================================================================
echo "🙏 EXAMPLE 4: Religious Observance Days"
echo "─────────────────────────────────────────────────────────────────"
echo "🇮🇳 Hindu Calendar - Delhi (Ekadashi - Tithi 11):"
$BINARY --hindu-search $DATE_START $DATE_END \
        --search-tithi 11 \
        --lat $DELHI_LAT --lon $DELHI_LON --timezone $DELHI_TZ

echo
echo "🇲🇲 Myanmar Calendar - Yangon (Buddhist Sabbath):"
$BINARY --myanmar-search $DATE_START $DATE_END \
        --myanmar-search-sabbath \
        --lat $YANGON_LAT --lon $YANGON_LON --timezone $YANGON_TZ
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 5: Year Transition Comparison
# =============================================================================
echo "📅 EXAMPLE 5: Year Transition Analysis"
echo "─────────────────────────────────────────────────────────────────"
echo "🇮🇳 Hindu Calendar - Delhi (Hindu Year 5125):"
$BINARY --hindu-search 2024-01-01 2024-02-15 \
        --search-year 5125 \
        --lat $DELHI_LAT --lon $DELHI_LON --timezone $DELHI_TZ

echo
echo "🇲🇲 Myanmar Calendar - Yangon (Myanmar Era 1385):"
$BINARY --myanmar-search 2024-01-01 2024-02-15 \
        --myanmar-search-year 1385 \
        --lat $YANGON_LAT --lon $YANGON_LON --timezone $YANGON_TZ
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 6: Spring Season Analysis
# =============================================================================
echo "🌸 EXAMPLE 6: Spring Season Calendar Analysis"
echo "─────────────────────────────────────────────────────────────────"
echo "🇮🇳 Hindu Calendar - Delhi (Chaitra Month - Month 0):"
$BINARY --hindu-search 2024-03-01 2024-04-30 \
        --search-month 0 \
        --lat $DELHI_LAT --lon $DELHI_LON --timezone $DELHI_TZ

echo
echo "🇲🇲 Myanmar Calendar - Yangon (Tagu Month - Month 1):"
$BINARY --myanmar-search 2024-03-01 2024-04-30 \
        --myanmar-search-month 1 \
        --lat $YANGON_LAT --lon $YANGON_LON --timezone $YANGON_TZ
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 7: Auspicious Day Planning
# =============================================================================
echo "✨ EXAMPLE 7: Auspicious Day Planning"
echo "─────────────────────────────────────────────────────────────────"
echo "🇮🇳 Hindu Calendar - Delhi (Shukla Paksha Sundays):"
$BINARY --hindu-search $DATE_START $DATE_END \
        --search-paksha 0 \
        --search-weekday 0 \
        --lat $DELHI_LAT --lon $DELHI_LON --timezone $DELHI_TZ

echo
echo "🇲🇲 Myanmar Calendar - Yangon (Thamanyo Sundays):"
$BINARY --myanmar-search $DATE_START $DATE_END \
        --myanmar-search-thamanyo \
        --myanmar-search-weekday 1 \
        --lat $YANGON_LAT --lon $YANGON_LON --timezone $YANGON_TZ
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 8: Complex Multi-Criteria Search
# =============================================================================
echo "🔍 EXAMPLE 8: Complex Multi-Criteria Analysis"
echo "─────────────────────────────────────────────────────────────────"
echo "🇮🇳 Hindu Calendar - Delhi (Multiple Criteria):"
$BINARY --hindu-search $DATE_START $DATE_END \
        --search-tithi-range 10 15 \
        --search-weekday-range 0 1 \
        --search-nakshatra-range 0 5 \
        --lat $DELHI_LAT --lon $DELHI_LON --timezone $DELHI_TZ

echo
echo "🇲🇲 Myanmar Calendar - Yangon (Multiple Criteria):"
$BINARY --myanmar-search $DATE_START $DATE_END \
        --myanmar-search-moon-phase-range 0 1 \
        --myanmar-search-weekday-range 0 1 \
        --myanmar-search-fortnight-day-range 10 15 \
        --lat $YANGON_LAT --lon $YANGON_LON --timezone $YANGON_TZ
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 9: Festival Season Planning
# =============================================================================
echo "🎉 EXAMPLE 9: Festival Season Planning"
echo "─────────────────────────────────────────────────────────────────"
echo "🇮🇳 Hindu Calendar - Delhi (Diwali Season - Kartik Amavasya):"
$BINARY --hindu-search 2024-10-01 2024-11-30 \
        --search-month 7 \
        --search-tithi 30 \
        --lat $DELHI_LAT --lon $DELHI_LON --timezone $DELHI_TZ

echo
echo "🇲🇲 Myanmar Calendar - Yangon (Thadingyut Season - Light Festival):"
$BINARY --myanmar-search 2024-10-01 2024-11-30 \
        --myanmar-search-month 7 \
        --myanmar-search-moon-phase 1 \
        --lat $YANGON_LAT --lon $YANGON_LON --timezone $YANGON_TZ
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 10: Near Matching Comparison
# =============================================================================
echo "🎯 EXAMPLE 10: Near Matching Comparison"
echo "─────────────────────────────────────────────────────────────────"
echo "🇮🇳 Hindu Calendar - Delhi (Near Purnima - Tolerance 2):"
$BINARY --hindu-search $DATE_START $DATE_END \
        --search-tithi 15 \
        --search-near 2 \
        --lat $DELHI_LAT --lon $DELHI_LON --timezone $DELHI_TZ

echo
echo "🇲🇲 Myanmar Calendar - Yangon (Near Full Moon - Tolerance 2):"
$BINARY --myanmar-search $DATE_START $DATE_END \
        --myanmar-search-moon-phase 1 \
        --myanmar-search-near 2 \
        --lat $YANGON_LAT --lon $YANGON_LON --timezone $YANGON_TZ
echo

echo "🎊 Combined Calendar Search Examples Complete!"
echo "═══════════════════════════════════════════════════════════════════"
echo
echo "📊 COMPARATIVE ANALYSIS SUMMARY:"
echo "   • Both systems track lunar cycles with different naming conventions"
echo "   • Hindu calendar uses Tithi (1-30), Myanmar uses Moon Phases (0-3)"
echo "   • Weekday numbering differs: Hindu starts Sunday=0, Myanmar starts Saturday=0"
echo "   • Both have religious observance days: Ekadashi vs Sabbath"
echo "   • Year systems: Hindu (Shaka/Vikram Era) vs Myanmar Era"
echo "   • Month systems: 12 Hindu months vs 12-14 Myanmar months"
echo
echo "🔍 SEARCH STRATEGY RECOMMENDATIONS:"
echo "   1. Use exact matching for precise ceremonial dates"
echo "   2. Use near matching for flexible event planning"
echo "   3. Combine multiple criteria for complex requirements"
echo "   4. Compare both systems for cross-cultural events"
echo "   5. Consider location-specific calculations for accuracy"
