#!/bin/bash

# 🏛️ ENHANCED BUDDHA BIRTH DATE FINDER 🏛️
# Advanced historical research script to find possible dates for Buddha's birth
# Based on traditional accounts, astronomical calculations, and multiple format outputs
# Updated with new search format options and comprehensive analysis tools

echo "🏛️ ENHANCED BUDDHA BIRTH DATE FINDER - COMPREHENSIVE HISTORICAL ANALYSIS 🏛️"
echo "══════════════════════════════════════════════════════════════════════════════"
echo "🔬 Advanced astronomical research with multiple output formats and data analysis"
echo "📊 Updated with CSV, JSON, and structured table outputs for modern research"
echo

# Binary path (adjust if needed)
BINARY="../bin/horoscope_cli"

# Check if binary exists
if [ ! -f "$BINARY" ]; then
    echo "❌ Binary not found at $BINARY"
    echo "Please compile the project first: make clean && make"
    exit 1
fi

# Historical location: Lumbini, Nepal (Buddha's birthplace)
LAT=27.4833
LON=83.2767
TIMEZONE="+05:45"

echo "📍 Location: Lumbini, Nepal (${LAT}°N, ${LON}°E)"
echo "🏛️ Historical Context: Searching for Buddha's birth date"
echo

# =============================================================================
# TRADITIONAL ACCOUNTS ANALYSIS
# =============================================================================
echo "📚 TRADITIONAL ACCOUNTS OF BUDDHA'S BIRTH:"
echo "─────────────────────────────────────────────────────────────────"
echo "• Date: Vaishakha month, Purnima (Full Moon - Tithi 15)"
echo "• Day: Friday (Shukravaar)"
echo "• Year Range: Approximately 600-500 BCE"
echo "• Location: Lumbini Garden, Kapilavastu (modern Nepal)"
echo "• Mother: Queen Maya Devi"
echo "• Astronomical Event: Full moon in Vaishakha under Pushya Nakshatra"
echo
read -p "Press Enter to start the historical search..."
echo

# =============================================================================
# BC DATE SEARCH - USING HISTORICAL YEARS
# =============================================================================
echo "🏛️ EXAMPLE 1: TRADITIONAL BC DATE SEARCH (563 BCE - Theravada Tradition)"
echo "─────────────────────────────────────────────────────────────────"
echo "🔍 Searching for Vaishakha Purnima (Buddha Purnima) in 563 BCE..."
echo "📅 Using astronomical year notation: -0563 (563 BCE)"
echo

# Search for Buddha Purnima (Tithi 15) in the traditional 563 BCE year
echo "$BINARY --hindu-search -0563-01-01 -0563-12-31 --search-tithi 15 --hindu-search-format table --lat $LAT --lon $LON"
$BINARY --hindu-search -0563-01-01 -0563-12-31 --search-tithi 15 --hindu-search-format table --lat $LAT --lon $LON

echo
echo "💡 Analysis: This shows all Purnima dates in 563 BCE with detailed Hindu calendar information"
echo "🌸 Look for Vaishakha month (typically April-May) entries for traditional Buddha Purnima"
echo
read -p "Press Enter to continue to next example..."
echo

# =============================================================================
# CSV FORMAT FOR DATA ANALYSIS
# =============================================================================
echo "🏛️ EXAMPLE 2: BC DATE SEARCH IN CSV FORMAT FOR DATA ANALYSIS"
echo "─────────────────────────────────────────────────────────────────"
echo "📊 Getting structured data for 563 BCE Buddha Purnima candidates..."
echo

echo "$BINARY --hindu-search -0563-01-01 -0563-12-31 --search-tithi 15 --hindu-search-format csv --lat $LAT --lon $LON"
$BINARY --hindu-search -0563-01-01 -0563-12-31 --search-tithi 15 --hindu-search-format csv --lat $LAT --lon $LON

echo
echo "💡 Analysis: CSV format allows easy import into spreadsheets for detailed research"
echo "📈 Use this data to compare multiple year candidates or cross-reference with historical sources"
echo
read -p "Press Enter to continue to multi-year analysis..."
echo

# =============================================================================
# MULTI-YEAR BC DATE COMPARISON
# =============================================================================
echo "🏛️ EXAMPLE 3: MULTI-YEAR BC SEARCH (566-560 BCE RANGE)"
echo "─────────────────────────────────────────────────────────────────"
echo "🔍 Comparing Buddha Purnima dates across multiple traditional year candidates..."
echo

for year in 566 565 564 563 562 561 560; do
    echo "Year -$year ($year BCE):"
    echo "$BINARY --hindu-search -0${year}-04-01 -0${year}-06-01 --search-tithi 15 --hindu-search-format csv --lat $LAT --lon $LON | head -5"
    $BINARY --hindu-search -0${year}-04-01 -0${year}-06-01 --search-tithi 15 --hindu-search-format csv --lat $LAT --lon $LON | head -5
    echo
done

echo "💡 Analysis: Compare Vaishakha Purnima dates across different traditional year candidates"
echo "📊 This helps identify the most astronomically consistent year for Buddha's birth"
echo
read -p "Press Enter to continue to list format example..."
echo

# =============================================================================
# NEW LIST FORMAT EXAMPLE - SIMPLE DATE LISTS
# =============================================================================
echo "🏛️ EXAMPLE 4: NEW LIST FORMAT - SIMPLE DATE LISTS FOR SCRIPTING"
echo "─────────────────────────────────────────────────────────────────"
echo "📝 Using the new 'list' format for clean, script-friendly output..."
echo

echo "Simple list of all Buddha Purnima candidates in 563 BCE:"
echo "$BINARY --hindu-search -0563-01-01 -0563-12-31 --search-tithi 15 --hindu-search-format list"
$BINARY --hindu-search -0563-01-01 -0563-12-31 --search-tithi 15 --hindu-search-format list

echo
echo "💡 Analysis: The list format provides clean, parseable output perfect for:"
echo "   • Shell scripting and automation"
echo "   • Date extraction for further processing"
echo "   • Integration with other calendar systems"
echo "   • Quick reference and documentation"
echo
echo "🔍 Notice: Dates without scores are exact matches (score = 1.0)"
echo "          Dates with scores shown are partial matches with tolerance"
echo
read -p "Press Enter to continue to day-of-week analysis..."
echo
echo "🔍 SEARCH CRITERIA EXPLANATION:"
echo "─────────────────────────────────────────────────────────────────"
echo "1. MONTH: Vaishakha (Month 1 in Hindu calendar)"
echo "   • Spring month, typically April-May"
echo "   • Sacred month for Buddhist festivals"
echo "   • Modern Vesak/Buddha Purnima celebrated in this month"
echo
echo "2. TITHI: Purnima (Tithi 15)"
echo "   • Full Moon day"
echo "   • Most auspicious day in lunar calendar"
echo "   • Associated with spiritual births and enlightenment"
echo
echo "3. WEEKDAY: Friday (Shukravaar)"
echo "   • Day ruled by Venus (Shukra)"
echo "   • Considered auspicious for births"
echo "   • Traditional accounts specify Friday"
echo
echo "4. YEAR RANGE: 600-500 BCE"
echo "   • Buddhist historical consensus"
echo "   • Corresponds to Hindu calendar years ~4400-4500"
echo "   • Different traditions give slightly different dates"
echo
read -p "Press Enter to begin the search..."
echo

# =============================================================================
# CONVERSION NOTE
# =============================================================================
echo "⚠️  IMPORTANT NOTE ABOUT YEAR CONVERSION:"
echo "─────────────────────────────────────────────────────────────────"
echo "The current implementation uses modern Hindu calendar years."
echo "For historical BCE dates, we need to search in CE equivalents"
echo "that would correspond to the same seasonal/astronomical patterns."
echo
echo "We'll search modern years that have the same astronomical"
echo "characteristics as the historical period."
echo
read -p "Press Enter to continue with modern equivalent search..."
echo

# =============================================================================
# EXAMPLE 1: Modern Equivalent Search - Vaishakha Friday Purnima (Enhanced)
# =============================================================================
echo "🔍 EXAMPLE 1: Finding Vaishakha Friday Purnima (Enhanced Multi-Format Analysis)"
echo "═════════════════════════════════════════════════════════════════════════════"
echo "Searching for dates matching Buddha birth criteria with detailed analysis..."
echo "📊 Results in enhanced table format with comprehensive panchanga information:"
echo
$BINARY --hindu-search 2020-01-01 2025-12-31 \
        --search-month 1 \
        --search-tithi 15 \
        --search-weekday 5 \
        --hindu-search-format table \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
echo "📊 Same results in CSV format for data analysis:"
echo "─────────────────────────────────────────────"
$BINARY --hindu-search 2020-01-01 2025-12-31 \
        --search-month 1 \
        --search-tithi 15 \
        --search-weekday 5 \
        --hindu-search-format csv \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 2: Extended Search - Vaishakha Purnima (Comprehensive Analysis)
# =============================================================================
echo "🔍 EXAMPLE 2: All Vaishakha Purnima Days - Multi-Format Comprehensive Analysis"
echo "═════════════════════════════════════════════════════════════════════════════"
echo "Finding all Vaishakha full moon days with statistical analysis..."
echo "📊 Results showing Buddha Purnima/Vesak dates with detailed information:"
echo
$BINARY --hindu-search 2020-01-01 2025-12-31 \
        --search-month 1 \
        --search-tithi 15 \
        --hindu-search-format table \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
echo "📊 JSON format for API integration and data processing:"
echo "────────────────────────────────────────────────────────"
$BINARY --hindu-search 2024-04-01 2024-06-30 \
        --search-month 1 \
        --search-tithi 15 \
        --hindu-search-format json \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 3: Near Match Search - Advanced Tolerance Analysis
# =============================================================================
echo "🔍 EXAMPLE 3: Near Match Search with Advanced Scoring Analysis"
echo "═════════════════════════════════════════════════════════════════════════════"
echo "Using tolerance to find close matches with detailed scoring metrics..."
echo "📊 Results with comprehensive scoring showing match quality and confidence:"
echo
$BINARY --hindu-search 2020-01-01 2025-12-31 \
        --search-month 1 \
        --search-tithi 15 \
        --search-weekday 5 \
        --search-near 2 \
        --hindu-search-format table \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
echo "📊 CSV export for statistical analysis and pattern recognition:"
echo "─────────────────────────────────────────────────────────────"
$BINARY --hindu-search 2020-01-01 2025-12-31 \
        --search-month 1 \
        --search-tithi 15 \
        --search-weekday 5 \
        --search-near 3 \
        --hindu-search-format csv \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 4: Cross-Calendar Analysis - Hindu + Myanmar Integration
# =============================================================================
echo "🔍 EXAMPLE 4: Cross-Calendar Analysis (Hindu + Myanmar Buddhist Traditions)"
echo "═════════════════════════════════════════════════════════════════════════════"
echo "Integrating Hindu and Myanmar Buddhist calendar systems for comprehensive analysis..."
echo "📊 Hindu Calendar Analysis - Traditional Buddha Birth Pattern:"
echo
$BINARY --hindu-search 2023-01-01 2025-12-31 \
        --search-month 1 \
        --search-tithi 15 \
        --hindu-search-format table \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
echo "📊 Myanmar Calendar Analysis - Buddhist Sabbath and Full Moon Days:"
echo "───────────────────────────────────────────────────────────────────"
$BINARY --myanmar-search 2023-01-01 2025-12-31 \
        --myanmar-search-moon-phase 1 \
        --myanmar-search-sabbath \
        --myanmar-search-format table \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
echo "📊 Myanmar Calendar CSV for statistical correlation analysis:"
echo "──────────────────────────────────────────────────────────"
$BINARY --myanmar-search 2024-01-01 2024-12-31 \
        --myanmar-search-moon-phase 1 \
        --myanmar-search-format csv \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 5: Seasonal Analysis - Enhanced Spring Full Moons
# =============================================================================
echo "� EXAMPLE 5: Enhanced Seasonal Analysis (Advanced Spring Pattern Detection)"
echo "═════════════════════════════════════════════════════════════════════════════"
echo "Advanced analysis of spring months with comprehensive format comparison..."
echo "📊 Detailed table format for human-readable analysis:"
echo
$BINARY --hindu-search 2020-01-01 2025-12-31 \
        --search-month-range 0 2 \
        --search-tithi 15 \
        --search-weekday 5 \
        --hindu-search-format table \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
echo "📊 JSON format for algorithmic pattern analysis:"
echo "───────────────────────────────────────────────"
$BINARY --hindu-search 2024-03-01 2024-06-30 \
        --search-month-range 0 2 \
        --search-tithi 15 \
        --hindu-search-format json \
        --lat $LAT --lon $LON --timezone $TIMEZONE | head -50
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 6: Advanced Nakshatra Analysis with Multiple Outputs
# =============================================================================
echo "🔍 EXAMPLE 6: Advanced Nakshatra Analysis (Pushya and Related Stellar Patterns)"
echo "═════════════════════════════════════════════════════════════════════════════"
echo "Comprehensive nakshatra analysis with multiple output formats for research..."
echo "📊 Traditional Pushya nakshatra analysis (associated with Buddha's birth):"
echo
$BINARY --hindu-search 2020-01-01 2025-12-31 \
        --search-month 1 \
        --search-tithi-range 14 15 \
        --hindu-search-format table \
        --lat $LAT --lon $LON --timezone $TIMEZONE | head -50
echo
echo "📊 CSV data export for nakshatra pattern statistical analysis:"
echo "─────────────────────────────────────────────────────────────"
$BINARY --hindu-search 2023-01-01 2024-12-31 \
        --search-month 1 \
        --search-tithi-range 13 16 \
        --hindu-search-format csv \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 7: Comprehensive Output Format Showcase - Advanced Data Analysis
# =============================================================================
echo "🔍 EXAMPLE 7: Complete Format Showcase (All Output Types & Use Cases)"
echo "═════════════════════════════════════════════════════════════════════════════"
echo "Demonstrating all available output formats with practical applications..."
echo

# Get a specific date range for consistent comparison
DEMO_START="2024-05-01"
DEMO_END="2024-05-31"

echo "📊 FORMAT 1: Enhanced Table Format (Human-Readable Research)"
echo "════════════════════════════════════════════════════════════════════════════"
echo "Perfect for: Academic papers, detailed analysis, traditional research"
$BINARY --hindu-search $DEMO_START $DEMO_END \
        --search-month 1 \
        --search-tithi 15 \
        --hindu-search-format table \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
echo "📊 FORMAT 2: CSV Format (Statistical Analysis & Spreadsheets)"
echo "════════════════════════════════════════════════════════════════════════════"
echo "Perfect for: Excel analysis, statistical software, data correlation studies"
echo "Date,Score,HinduYear,Month,Tithi,Vara,Nakshatra,Yoga,Karana"
$BINARY --hindu-search $DEMO_START $DEMO_END \
        --search-month 1 \
        --search-tithi 15 \
        --hindu-search-format csv \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
echo "📊 FORMAT 3: JSON Format (API Integration & Programming)"
echo "════════════════════════════════════════════════════════════════════════════"
echo "Perfect for: Web APIs, mobile apps, automated processing, machine learning"
$BINARY --hindu-search $DEMO_START $DEMO_END \
        --search-month 1 \
        --search-tithi 15 \
        --hindu-search-format json \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
echo "📊 MYANMAR CALENDAR FORMAT COMPARISON:"
echo "════════════════════════════════════════════════════════════════════════════"
echo "Myanmar Table Format (Traditional Display):"
echo "──────────────────────────────────────────"
$BINARY --myanmar-search 2024-05-01 2024-05-31 \
        --myanmar-search-moon-phase 1 \
        --myanmar-search-format table \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
echo "Myanmar CSV Format (Data Export):"
echo "─────────────────────────────────"
$BINARY --myanmar-search 2024-05-01 2024-05-31 \
        --myanmar-search-moon-phase 1 \
        --myanmar-search-format csv \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 8: Statistical Analysis and Data Mining
# =============================================================================
echo "🔍 EXAMPLE 8: Advanced Statistical Analysis and Pattern Mining"
echo "═════════════════════════════════════════════════════════════════════════════"
echo "Comprehensive statistical analysis of Buddha birth patterns with data mining..."
echo

echo "📊 10-Year Statistical Analysis (2020-2030) - Buddha Birth Pattern Frequency:"
echo "════════════════════════════════════════════════════════════════════════════"
echo "Analyzing the rarity of exact Buddha birth criteria (Vaishakha Friday Purnima):"
$BINARY --hindu-search 2020-01-01 2030-12-31 \
        --search-month 1 \
        --search-tithi 15 \
        --search-weekday 5 \
        --hindu-search-format csv \
        --lat $LAT --lon $LON --timezone $TIMEZONE | tee buddha_exact_pattern.csv
echo
echo "📊 Comparative Analysis - All Vaishakha Purnima (Any Weekday) vs Friday Only:"
echo "═══════════════════════════════════════════════════════════════════════════"
echo "All Vaishakha Full Moons (Baseline for comparison):"
$BINARY --hindu-search 2020-01-01 2030-12-31 \
        --search-month 1 \
        --search-tithi 15 \
        --hindu-search-format csv \
        --lat $LAT --lon $LON --timezone $TIMEZONE | wc -l | xargs echo "Total Vaishakha Purnima days found:"
echo
echo "📊 Pattern Probability Analysis:"
echo "───────────────────────────────"
echo "Calculating statistical probability of Buddha birth criteria alignment..."
ALL_PURNIMA=$(mktemp)
FRIDAY_PURNIMA=$(mktemp)

$BINARY --hindu-search 2020-01-01 2030-12-31 \
        --search-month 1 \
        --search-tithi 15 \
        --hindu-search-format csv \
        --lat $LAT --lon $LON --timezone $TIMEZONE > "$ALL_PURNIMA"

$BINARY --hindu-search 2020-01-01 2030-12-31 \
        --search-month 1 \
        --search-tithi 15 \
        --search-weekday 5 \
        --hindu-search-format csv \
        --lat $LAT --lon $LON --timezone $TIMEZONE > "$FRIDAY_PURNIMA"

TOTAL_COUNT=$(tail -n +2 "$ALL_PURNIMA" | wc -l)
FRIDAY_COUNT=$(tail -n +2 "$FRIDAY_PURNIMA" | wc -l)

if [ "$TOTAL_COUNT" -gt 0 ]; then
    PERCENTAGE=$(echo "scale=2; $FRIDAY_COUNT * 100 / $TOTAL_COUNT" | bc -l 2>/dev/null || echo "calculating...")
    echo "• Total Vaishakha Purnima days (2020-2030): $TOTAL_COUNT"
    echo "• Vaishakha Friday Purnima days: $FRIDAY_COUNT"
    echo "• Probability of Friday alignment: $PERCENTAGE% (approximately 1 in 7 expected)"
    echo "• This demonstrates the rarity of the traditional Buddha birth pattern"
else
    echo "• Statistical analysis data being generated..."
fi

rm -f "$ALL_PURNIMA" "$FRIDAY_PURNIMA"
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 9: Modern Applications and Practical Use Cases
# =============================================================================
echo "🔍 EXAMPLE 9: Modern Applications - Vesak Date Calculation & Festival Planning"
echo "═════════════════════════════════════════════════════════════════════════════"
echo "Practical applications for modern Buddhist communities and scholars..."
echo

echo "📊 Annual Vesak/Buddha Purnima Date Calculator (Next 5 Years):"
echo "════════════════════════════════════════════════════════════════"
echo "For Buddhist communities planning annual celebrations:"
$BINARY --hindu-search 2025-01-01 2030-12-31 \
        --search-month 1 \
        --search-tithi 15 \
        --hindu-search-format table \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
echo "📊 Export for Digital Calendars (CSV Format for Import):"
echo "═══════════════════════════════════════════════════════"
echo "Ready for import into Google Calendar, Outlook, or festival planning software:"
echo "Date,Score,HinduYear,Month,Tithi,Vara,Nakshatra,Yoga,Karana"
$BINARY --hindu-search 2025-01-01 2030-12-31 \
        --search-month 1 \
        --search-tithi 15 \
        --hindu-search-format csv \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
echo "📊 API Integration Example (JSON for Mobile Apps):"
echo "═════════════════════════════════════════════════"
echo "JSON format for Buddhist apps, websites, and automated systems:"
$BINARY --hindu-search 2025-04-01 2025-06-30 \
        --search-month 1 \
        --search-tithi 15 \
        --hindu-search-format json \
        --lat $LAT --lon $LON --timezone $TIMEZONE | head -30
echo "... (truncated for display)"
echo
read -p "Press Enter to continue to final analysis..."
echo

# =============================================================================
# HISTORICAL CONTEXT AND ANALYSIS
# =============================================================================

cat << 'EOF'

═══════════════════════════════════════════════════════════════════
📖 HISTORICAL ANALYSIS AND FINDINGS
═══════════════════════════════════════════════════════════════════

🏛️ TRADITIONAL SOURCES:
─────────────────────────────────────────────────────────────────
• Theravada Tradition: 623 BCE (Sri Lankan chronicles)
• Mahayana Tradition: 566 BCE (Chinese sources)
• Western Scholarship: 563-483 BCE (consensus range)
• Archaeological Evidence: 6th-4th centuries BCE

🌟 ASTRONOMICAL SIGNIFICANCE:
─────────────────────────────────────────────────────────────────
• VAISHAKHA MONTH: Spring season, renewal, birth
• PURNIMA: Maximum lunar influence, spiritual power
• FRIDAY: Venus day, beauty, harmony, auspiciousness
• LUMBINI: Sacred grove, natural sanctuary

🔍 SEARCH METHODOLOGY:
─────────────────────────────────────────────────────────────────
1. Modern Equivalent Search: Find contemporary dates with same criteria
2. Pattern Analysis: Identify recurring astronomical patterns
3. Seasonal Correlation: Match seasonal characteristics
4. Nakshatra Alignment: Include stellar positions

📊 FINDINGS INTERPRETATION:
─────────────────────────────────────────────────────────────────
• The search results show how rare this specific combination is
• Vaishakha Friday Purnima occurs infrequently
• Modern dates help understand historical astronomical patterns
• Each match represents a potential "Buddha birth pattern"

🎯 PRACTICAL APPLICATIONS:
─────────────────────────────────────────────────────────────────
• Vesak/Buddha Purnima festival date calculation
• Historical astronomy research
• Understanding lunar calendar patterns
• Cross-referencing traditional accounts

⚠️  LIMITATIONS:
─────────────────────────────────────────────────────────────────
• Calendar systems have evolved over millennia
• Precession affects stellar positions
• Different traditions use different starting epochs
• Historical records may contain approximations

🌸 SPIRITUAL SIGNIFICANCE:
─────────────────────────────────────────────────────────────────
"Just as the full moon illuminates the darkest night,
the birth of Buddha brought light to the world of suffering.
The specific astronomical alignment reflects the cosmic
significance of this moment in human spiritual history."

📚 FURTHER RESEARCH:
─────────────────────────────────────────────────────────────────
• Study precession effects on ancient dates
• Compare multiple calendar systems (Greek, Roman, Chinese)
• Analyze archaeological evidence correlation
• Investigate regional calendar variations

═══════════════════════════════════════════════════════════════════
🕉️  CONCLUSION
═══════════════════════════════════════════════════════════════════

This search demonstrates the rarity and significance of the
traditional Buddha birth criteria. While we cannot definitively
determine the exact historical date, the astronomical patterns
help us understand why these specific elements (Vaishakha,
Purnima, Friday) became associated with Buddha's birth in
Buddhist tradition.

The combination of spring season, full moon, and Venus day
creates an astrologically and spiritually significant moment
that ancient astronomers and spiritual teachers recognized
as particularly auspicious for the birth of an enlightened being.

Modern Buddha Purnima/Vesak celebrations continue this tradition
by observing the Vaishakha full moon, maintaining the connection
to these ancient astronomical observations.

🙏 May this research contribute to understanding the intersection
   of astronomy, history, and spiritual tradition.

EOF

echo
echo "🎉 Enhanced Buddha Birth Date Finder Analysis Complete!"
echo "═══════════════════════════════════════════════════════════════════"
echo
echo "📝 SUMMARY OF ENHANCED SEARCH CAPABILITIES:"
echo "   • Month: Vaishakha (1) - Spring month"
echo "   • Tithi: Purnima (15) - Full Moon day"
echo "   • Weekday: Friday (5) - Venus day"
echo "   • Location: Lumbini, Nepal"
echo "   • Historical Period: ~600-500 BCE"
echo "   • NEW: Multi-format outputs (Table, CSV, JSON)"
echo "   • NEW: Cross-calendar analysis (Hindu + Myanmar)"
echo "   • NEW: Statistical analysis and pattern mining"
echo
echo "💡 ENHANCED MODERN APPLICATIONS:"
echo "   • Calculate annual Vesak/Buddha Purnima dates with multiple formats"
echo "   • Research historical astronomy patterns with data export"
echo "   • Study lunar calendar correlations across traditions"
echo "   • Understand traditional date calculations with scientific precision"
echo "   • Generate digital calendar imports for Buddhist communities"
echo "   • Create API integrations for Buddhist mobile apps"
echo "   • Perform statistical analysis of astronomical patterns"
echo
echo "🆕 ADVANCED OUTPUT FORMAT OPTIONS:"
echo "   • Table: --hindu-search-format table (Enhanced structured display)"
echo "   • CSV: --hindu-search-format csv (Excel/spreadsheet analysis)"
echo "   • JSON: --hindu-search-format json (API/programming integration)"
echo "   • Myanmar: --myanmar-search-format [table|csv|json] (Buddhist calendar)"
echo
echo "🔬 ENHANCED RESEARCH CAPABILITIES:"
echo "   • Multi-calendar system integration (Hindu + Myanmar Buddhist)"
echo "   • Statistical probability analysis of rare astronomical patterns"
echo "   • Cross-format data comparison and validation"
echo "   • Advanced pattern matching with scoring algorithms"
echo "   • Export capabilities for scientific analysis software"
echo "   • Integration with modern Buddhist community planning tools"
echo
echo "� ADVANCED DATA ANALYSIS EXAMPLES:"
echo "   # Complete analysis with all formats:"
echo "   ./buddha_birth_date_finder.sh > buddha_complete_analysis.txt"
echo "   "
echo "   # CSV export for statistical analysis:"
echo "   $BINARY --hindu-search 2025-01-01 2030-12-31 \\"
echo "           --search-month 1 --search-tithi 15 \\"
echo "           --hindu-search-format csv > vesak_dates.csv"
echo "   "
echo "   # JSON for web applications:"
echo "   $BINARY --hindu-search 2025-01-01 2030-12-31 \\"
echo "           --search-month 1 --search-tithi 15 \\"
echo "           --hindu-search-format json > vesak_api.json"
echo "   "
echo "   # Cross-calendar Buddhist analysis:"
echo "   $BINARY --myanmar-search 2025-01-01 2030-12-31 \\"
echo "           --myanmar-search-moon-phase 1 --myanmar-search-sabbath \\"
echo "           --myanmar-search-format csv > myanmar_sabbath_full_moons.csv"
echo
echo "   # Use --hindu-calendar-format csv > buddha_data.csv for spreadsheets"
echo "   # Use --hindu-calendar-format json > buddha_data.json for programming"
echo
echo "🌟 NEW FEATURES IN THIS ENHANCED VERSION:"
echo "   ✅ Multi-format output support (Table/CSV/JSON)"
echo "   ✅ Cross-calendar system analysis (Hindu + Myanmar)"
echo "   ✅ Statistical probability calculations"
echo "   ✅ Digital calendar export capability"
echo "   ✅ API integration ready outputs"
echo "   ✅ Advanced pattern matching with scoring"
echo "   ✅ Modern Buddhist community applications"
echo "   ✅ Scientific research data export tools"
echo
echo "🚀 Ready to explore more historical astronomical events with enhanced capabilities!"
