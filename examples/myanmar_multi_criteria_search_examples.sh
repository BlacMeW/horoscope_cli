#!/bin/bash

# 🇲🇲 MYANMAR CALENDAR MULTI-CRITERIA SEARCH EXAMPLES 🇲🇲
# Advanced examples demonstrating AND/OR logic in Myanmar calendar search
# These examples show how to combine multiple search criteria using logical operators

echo "🌙 MYANMAR CALENDAR MULTI-CRITERIA SEARCH EXAMPLES 🌙"
echo "======================================================="
echo ""

# Check if the binary exists
BINARY="../bin/horoscope_cli"
if [[ ! -f "$BINARY" ]]; then
    BINARY="./horoscope_cli"
fi

if [[ ! -f "$BINARY" ]]; then
    echo "❌ Error: horoscope_cli binary not found!"
    echo "Please compile the project first:"
    echo "   make clean && make"
    exit 1
fi

echo "Using binary: $BINARY"
echo ""

# Helper function to add separators
print_separator() {
    echo ""
    echo "═══════════════════════════════════════════════════════════"
    echo ""
}

print_example_header() {
    echo "🔍 $1"
    echo "-----------------------------------------------------------"
}

# EXAMPLE 1: Full Moon OR Yatyaza (OR Logic)
print_example_header "EXAMPLE 1: Search for Full Moon OR Yatyaza Days (OR Logic)"
echo "This finds dates that are EITHER full moon days OR yatyaza (inauspicious) days"
echo ""

$BINARY --myanmar-search 2024-01-01 2024-01-31 \
        --myanmar-search-moon-phase 1 \
        --myanmar-search-yatyaza \
        --myanmar-search-logic or \
        --myanmar-search-tabular

print_separator

# EXAMPLE 2: Sabbath AND Full Moon (AND Logic)
print_example_header "EXAMPLE 2: Search for Sabbath AND Full Moon Days (AND Logic)"
echo "This finds dates that are BOTH sabbath days AND full moon days"
echo ""

$BINARY --myanmar-search 2024-01-01 2024-03-31 \
        --myanmar-search-sabbath \
        --myanmar-search-moon-phase 1 \
        --myanmar-search-logic and \
        --myanmar-search-tabular

print_separator

# EXAMPLE 3: Saturday OR Sunday (Weekdays with OR Logic)
print_example_header "EXAMPLE 3: Search for Saturday OR Sunday (OR Logic)"
echo "This finds dates that are EITHER Saturday OR Sunday"
echo ""

$BINARY --myanmar-search 2024-02-01 2024-02-29 \
        --myanmar-search-weekday 0 \
        --myanmar-search-weekday 1 \
        --myanmar-search-logic or \
        --myanmar-search-tabular

print_separator

# EXAMPLE 4: Thamanyo AND Waxing Moon (AND Logic)
print_example_header "EXAMPLE 4: Search for Thamanyo AND Waxing Moon (AND Logic)"
echo "This finds dates that are BOTH thamanyo (auspicious) AND during waxing moon"
echo ""

$BINARY --myanmar-search 2024-01-01 2024-02-29 \
        --myanmar-search-thamanyo \
        --myanmar-search-moon-phase 0 \
        --myanmar-search-logic and \
        --myanmar-search-tabular

print_separator

# EXAMPLE 5: Complex OR - Multiple Astrological Events
print_example_header "EXAMPLE 5: Complex OR - Sabbath OR Yatyaza OR Pyathada"
echo "This finds dates that match ANY of these: sabbath OR yatyaza OR pyathada"
echo ""

$BINARY --myanmar-search 2024-01-01 2024-01-15 \
        --myanmar-search-sabbath \
        --myanmar-search-yatyaza \
        --myanmar-search-pyathada \
        --myanmar-search-logic or \
        --myanmar-search-tabular

print_separator

# EXAMPLE 6: Year Range AND Month (AND Logic)
print_example_header "EXAMPLE 6: Search for Year Range AND Specific Month (AND Logic)"
echo "This finds dates in Myanmar years 1385-1386 AND in Tagu month (month 1)"
echo ""

$BINARY --myanmar-search 2023-12-01 2024-06-01 \
        --myanmar-search-year-range 1385 1386 \
        --myanmar-search-month 1 \
        --myanmar-search-logic and \
        --myanmar-search-tabular

print_separator

# EXAMPLE 7: New Moon OR Full Moon (OR Logic)
print_example_header "EXAMPLE 7: Search for New Moon OR Full Moon (OR Logic)"
echo "This finds dates that are EITHER new moon OR full moon"
echo ""

$BINARY --myanmar-search 2024-01-01 2024-01-31 \
        --myanmar-search-moon-phase 3 \
        --myanmar-search-moon-phase 1 \
        --myanmar-search-logic or \
        --myanmar-search-tabular

print_separator

# EXAMPLE 8: Fortnight Day Range AND Weekday (AND Logic)
print_example_header "EXAMPLE 8: Search for Fortnight Days 14-15 AND Friday (AND Logic)"
echo "This finds dates that are BOTH near full moon (days 14-15) AND on Friday"
echo ""

$BINARY --myanmar-search 2024-01-01 2024-02-29 \
        --myanmar-search-fortnight-day-range 14 15 \
        --myanmar-search-weekday 6 \
        --myanmar-search-logic and \
        --myanmar-search-tabular

print_separator

echo "✅ All Myanmar calendar multi-criteria search examples completed!"
echo ""
echo "📚 USAGE SUMMARY:"
echo "• Use --myanmar-search-logic and for ALL criteria to match (default)"
echo "• Use --myanmar-search-logic or for ANY criteria to match"
echo "• Combine multiple criteria like moon phases, weekdays, astrological events"
echo "• Perfect for finding auspicious/inauspicious day combinations"
echo ""
echo "🌟 Common Use Cases:"
echo "• Full moon OR yatyaza (avoid both types of days)"
echo "• Sabbath AND full moon (highly auspicious combinations)"
echo "• Weekend days (Saturday OR Sunday)"
echo "• Multiple astrological events (sabbath OR thamanyo OR pyathada)"
echo ""
