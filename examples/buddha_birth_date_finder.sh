#!/bin/bash

# 🏛️ BUDDHA BIRTH DATE FINDER 🏛️
# Historical research script to find possible dates for Buddha's birth
# Based on traditional accounts and astronomical calculations

echo "🏛️ BUDDHA BIRTH DATE FINDER - HISTORICAL ANALYSIS 🏛️"
echo "═══════════════════════════════════════════════════════════════════"
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
# SEARCH PARAMETERS EXPLANATION
# =============================================================================
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
# EXAMPLE 1: Modern Equivalent Search - Vaishakha Friday Purnima
# =============================================================================
echo "🔍 EXAMPLE 1: Finding Vaishakha Friday Purnima (Modern Search)"
echo "─────────────────────────────────────────────────────────────────"
echo "Searching for dates matching Buddha birth criteria in recent years..."
echo "📊 Results in detailed format with full panchanga information:"
echo
$BINARY --hindu-search 2020-01-01 2025-12-31 \
        --search-month 1 \
        --search-tithi 15 \
        --search-weekday 5 \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 2: Extended Search - Vaishakha Purnima (Any Day)
# =============================================================================
echo "🔍 EXAMPLE 2: All Vaishakha Purnima Days (Extended Search)"
echo "─────────────────────────────────────────────────────────────────"
echo "Finding all Vaishakha full moon days regardless of weekday..."
echo "📊 Results showing Buddhist festival dates (Buddha Purnima/Vesak):"
echo
$BINARY --hindu-search 2020-01-01 2025-12-31 \
        --search-month 1 \
        --search-tithi 15 \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 3: Near Match Search - Close to Ideal Criteria
# =============================================================================
echo "🔍 EXAMPLE 3: Near Match Search (Flexible Criteria)"
echo "─────────────────────────────────────────────────────────────────"
echo "Using tolerance to find close matches to ideal criteria..."
echo "📊 Results with scoring showing how close matches are to ideal pattern:"
echo
$BINARY --hindu-search 2020-01-01 2025-12-31 \
        --search-month 1 \
        --search-tithi 15 \
        --search-weekday 5 \
        --search-near 2 \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 4: Seasonal Analysis - Spring Full Moons
# =============================================================================
echo "🔍 EXAMPLE 4: Spring Season Analysis (Seasonal Pattern)"
echo "─────────────────────────────────────────────────────────────────"
echo "Analyzing spring months (Chaitra-Jyeshtha) for Friday full moons..."
echo "📊 Seasonal comparison in structured format:"
echo
$BINARY --hindu-search 2020-01-01 2025-12-31 \
        --search-month-range 0 2 \
        --search-tithi 15 \
        --search-weekday 5 \
        --hindu-calendar-format table \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 5: Nakshatra Analysis - Pushya Influence
# =============================================================================
echo "🔍 EXAMPLE 5: Nakshatra Analysis (Pushya Connection)"
echo "─────────────────────────────────────────────────────────────────"
echo "Searching for Pushya nakshatra (associated with Buddha's birth)..."
echo "📊 Nakshatra analysis with detailed table output:"
echo
$BINARY --hindu-search 2020-01-01 2025-12-31 \
        --search-month 1 \
        --search-nakshatra 7 \
        --search-tithi-range 14 15 \
        --hindu-calendar-format table \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 6: Output Format Comparison - Multiple Table Styles
# =============================================================================
echo "🔍 EXAMPLE 6: Output Format Comparison (All Available Formats)"
echo "─────────────────────────────────────────────────────────────────"
echo "Demonstrating different output formats for Buddha Purnima analysis..."
echo

echo "📊 FORMAT 1: Default Detailed Format"
echo "════════════════════════════════════════════════════════════════"
$BINARY --hindu-search 2024-05-01 2024-05-31 \
        --search-month 1 \
        --search-tithi 15 \
        --lat $LAT --lon $LON --timezone $TIMEZONE | head -30
echo
echo "📊 FORMAT 2: Table Format (Structured Layout)"
echo "════════════════════════════════════════════════════════════════"
$BINARY --hindu-search 2024-05-01 2024-05-31 \
        --search-month 1 \
        --search-tithi 15 \
        --hindu-calendar-format table \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
echo "📊 FORMAT 3: CSV Format (Data Export Ready)"
echo "════════════════════════════════════════════════════════════════"
$BINARY --hindu-search 2024-05-01 2024-05-31 \
        --search-month 1 \
        --search-tithi 15 \
        --hindu-calendar-format csv \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
echo "📊 FORMAT 4: JSON Format (API Integration Ready)"
echo "════════════════════════════════════════════════════════════════"
$BINARY --hindu-search 2024-05-01 2024-05-31 \
        --search-month 1 \
        --search-tithi 15 \
        --hindu-calendar-format json \
        --lat $LAT --lon $LON --timezone $TIMEZONE
echo
read -p "Press Enter to continue..."
echo

# =============================================================================
# EXAMPLE 7: Comparative Analysis Table - Historical Patterns  
# =============================================================================
echo "🔍 EXAMPLE 7: Comparative Historical Analysis"
echo "─────────────────────────────────────────────────────────────────"
echo "Comparing Buddha birth pattern frequencies with statistical tables..."
echo

echo "📊 Buddha Birth Pattern (Vaishakha Friday Purnima) - 2020-2030:"
echo "════════════════════════════════════════════════════════════════"
$BINARY --hindu-search 2020-01-01 2030-12-31 \
        --search-month 1 \
        --search-tithi 15 \
        --search-weekday 5 \
        --hindu-calendar-format csv \
        --lat $LAT --lon $LON --timezone $TIMEZONE | head -10
echo
echo "📊 All Vaishakha Purnima Days (Any Weekday) - Statistical Comparison:"
echo "════════════════════════════════════════════════════════════════"
$BINARY --hindu-search 2020-01-01 2030-12-31 \
        --search-month 1 \
        --search-tithi 15 \
        --hindu-calendar-format table \
        --lat $LAT --lon $LON --timezone $TIMEZONE
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
echo "🎉 Buddha Birth Date Finder Analysis Complete!"
echo "═══════════════════════════════════════════════════════════════════"
echo
echo "📝 SUMMARY OF SEARCH CRITERIA:"
echo "   • Month: Vaishakha (1) - Spring month"
echo "   • Tithi: Purnima (15) - Full Moon day"  
echo "   • Weekday: Friday (5) - Venus day"
echo "   • Location: Lumbini, Nepal"
echo "   • Historical Period: ~600-500 BCE"
echo
echo "💡 USAGE FOR MODERN APPLICATIONS:"
echo "   • Calculate annual Vesak/Buddha Purnima dates"
echo "   • Research historical astronomy patterns"
echo "   • Study lunar calendar correlations"
echo "   • Understand traditional date calculations"
echo
echo "� OUTPUT FORMAT OPTIONS:"
echo "   • Default: Detailed panchanga information"
echo "   • Table: Structured tabular layout"
echo "   • CSV: Comma-separated for spreadsheet analysis"
echo "   • JSON: Machine-readable for API integration"
echo
echo "�🔬 FOR RESEARCHERS:"
echo "   • Modify date ranges for different historical periods"
echo "   • Adjust location coordinates for other Buddhist sites"
echo "   • Experiment with different nakshatra combinations"
echo "   • Use near matching for flexible historical dating"
echo "   • Export data in CSV/JSON for statistical analysis"
echo
echo "📈 DATA ANALYSIS EXAMPLES:"
echo "   ./buddha_birth_date_finder.sh > buddha_analysis.txt"
echo "   # Use --hindu-calendar-format csv > buddha_data.csv for spreadsheets"
echo "   # Use --hindu-calendar-format json > buddha_data.json for programming"
echo
echo "🚀 Ready to explore more historical astronomical events!"
