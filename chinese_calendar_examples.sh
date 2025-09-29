#!/bin/bash

# Simple Chinese Calendar Usage Examples
# Quick reference for common Chinese calendar commands

echo "🐉 Chinese Calendar Usage Examples for horoscope_cli"
echo "=================================================="
echo ""

# Check if horoscope_cli exists
HOROSCOPE_CLI=""
if [ -f "./horoscope_cli" ]; then
    HOROSCOPE_CLI="./horoscope_cli"
elif [ -f "./bin/horoscope_cli" ]; then
    HOROSCOPE_CLI="./bin/horoscope_cli"
elif [ -f "./build/horoscope_cli" ]; then
    HOROSCOPE_CLI="./build/horoscope_cli"
else
    echo "Error: horoscope_cli not found! Please build first."
    exit 1
fi

echo "Using executable: $HOROSCOPE_CLI"
echo ""

# Location: Beijing coordinates
LAT=39.9042
LON=116.4074
TZ=8.0

echo "📅 Basic Commands:"
echo ""

echo "1. Show Chinese calendar for today:"
echo "   $HOROSCOPE_CLI --date $(date +%Y-%m-%d) --time 12:00:00 --lat $LAT --lon $LON --timezone $TZ --chinese"
echo ""

echo "2. Show Chinese calendar for a specific date:"
echo "   $HOROSCOPE_CLI --date 2024-02-10 --time 12:00:00 --lat $LAT --lon $LON --timezone $TZ --chinese"
echo ""

echo "3. Show Chinese calendar range (Spring Festival 2024):"
echo "   $HOROSCOPE_CLI --chinese-range 2024-02-08 2024-02-15 --lat $LAT --lon $LON --timezone $TZ"
echo ""

echo "4. Show 24 Solar Terms for current year:"
echo "   $HOROSCOPE_CLI --date $(date +%Y)-06-21 --time 12:00:00 --lat $LAT --lon $LON --timezone $TZ --solar-terms"
echo ""

echo "5. Show traditional Chinese festivals:"
echo "   $HOROSCOPE_CLI --date $(date +%Y)-01-01 --time 12:00:00 --lat $LAT --lon $LON --timezone $TZ --chinese-festivals"
echo ""

echo "6. Show Four Pillars of Destiny (BaZi) analysis:"
echo "   $HOROSCOPE_CLI --date 1990-05-15 --time 14:30:00 --lat $LAT --lon $LON --timezone $TZ --bazi"
echo ""

echo "7. Complete analysis (all features):"
echo "   $HOROSCOPE_CLI --date 2024-04-04 --time 15:30:00 --lat $LAT --lon $LON --timezone $TZ --chinese --solar-terms --chinese-festivals --bazi"
echo ""

echo "📊 Output Formats:"
echo ""
echo "• Table format (default): --chinese-format table"
echo "• JSON format:           --chinese-format json"
echo "• CSV format:            --chinese-format csv"
echo ""

echo "🌍 Location Notes:"
echo ""
echo "• Beijing, China:   --lat 39.9042 --lon 116.4074 --timezone 8.0"
echo "• Shanghai, China:  --lat 31.2304 --lon 121.4737 --timezone 8.0"
echo "• Hong Kong:        --lat 22.3193 --lon 114.1694 --timezone 8.0"
echo "• Taipei, Taiwan:   --lat 25.0330 --lon 121.5654 --timezone 8.0"
echo "• Singapore:        --lat 1.3521  --lon 103.8198 --timezone 8.0"
echo ""

echo "🔍 Quick Tests:"
echo ""
echo "Test Dragon Year 2024:"
echo "$HOROSCOPE_CLI --date 2024-02-10 --time 12:00:00 --lat $LAT --lon $LON --timezone $TZ --chinese"
echo ""

echo "Test current date:"
echo "$HOROSCOPE_CLI --date $(date +%Y-%m-%d) --time $(date +%H:%M:%S) --lat $LAT --lon $LON --timezone $TZ --chinese"
echo ""

echo "Run the demonstration script for a complete walkthrough:"
echo "./demo_chinese_calendar.sh"
echo ""

echo "For full documentation, see USER_MANUAL.md and README.md"