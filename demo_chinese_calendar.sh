#!/bin/bash

# Chinese Calendar Demonstration Script for horoscope_cli
# This script demonstrates all Chinese calendar features
# Requirements: horoscope_cli must be built with Chinese calendar support

# Color codes for beautiful output
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
YELLOW='\033[1;33m'
BOLD='\033[1m'
NC='\033[0m' # No Color

# Default location (Beijing, China)
DEFAULT_LAT=39.9042
DEFAULT_LON=116.4074
DEFAULT_TZ=8.0

# Function to print section headers
print_header() {
    echo ""
    echo -e "${BOLD}${BLUE}========================================${NC}"
    echo -e "${BOLD}${BLUE} $1${NC}"
    echo -e "${BOLD}${BLUE}========================================${NC}"
    echo ""
}

# Function to print sub-headers
print_subheader() {
    echo ""
    echo -e "${BOLD}${CYAN}--- $1 ---${NC}"
    echo ""
}

# Function to execute command and show it
run_command() {
    echo -e "${YELLOW}Command:${NC} $1"
    echo ""
    eval "$1"
    echo ""
    echo -e "${GREEN}Press Enter to continue...${NC}"
    read -r
}

# Check if horoscope_cli exists
if [ ! -f "./horoscope_cli" ] && [ ! -f "./bin/horoscope_cli" ] && [ ! -f "./build/horoscope_cli" ]; then
    echo -e "${RED}Error: horoscope_cli not found!${NC}"
    echo "Please build the project first using:"
    echo "  make"
    echo "or"
    echo "  mkdir build && cd build && cmake .. && make"
    exit 1
fi

# Determine the correct executable path
HOROSCOPE_CLI=""
if [ -f "./horoscope_cli" ]; then
    HOROSCOPE_CLI="./horoscope_cli"
elif [ -f "./bin/horoscope_cli" ]; then
    HOROSCOPE_CLI="./bin/horoscope_cli"
elif [ -f "./build/horoscope_cli" ]; then
    HOROSCOPE_CLI="./build/horoscope_cli"
fi

echo -e "${BOLD}${PURPLE}🐉 Chinese Calendar Demonstration for horoscope_cli 🐉${NC}"
echo ""
echo "This script demonstrates the comprehensive Chinese calendar system"
echo "integrated into horoscope_cli, featuring:"
echo "• 60-year Sexagenary Cycle (Gan-Zhi 干支)"
echo "• 12-year Zodiac Animals with Five Elements"
echo "• 24 Solar Terms (節氣)"
echo "• Traditional Chinese Festivals"
echo "• Four Pillars of Destiny (BaZi 八字)"
echo "• Multi-language support (Chinese, English, Pinyin)"
echo ""
echo -e "${GREEN}Using location: Beijing, China (${DEFAULT_LAT}°N, ${DEFAULT_LON}°E, UTC+${DEFAULT_TZ})${NC}"
echo ""
echo -e "${YELLOW}Press Enter to begin the demonstration...${NC}"
read -r

# 1. Basic Chinese Calendar for Current Date
print_header "🗓️  Basic Chinese Calendar Display"
echo "Display Chinese calendar information for today's date"
echo "Shows: Gan-Zhi cycle, zodiac animal, element, lunar month"

run_command "$HOROSCOPE_CLI --date $(date +%Y-%m-%d) --time 12:00:00 --lat $DEFAULT_LAT --lon $DEFAULT_LON --timezone $DEFAULT_TZ --chinese"

# 2. Chinese Calendar with Different Output Formats
print_header "📊 Chinese Calendar Output Formats"

print_subheader "Table Format (Default)"
run_command "$HOROSCOPE_CLI --date 2024-02-10 --time 12:00:00 --lat $DEFAULT_LAT --lon $DEFAULT_LON --timezone $DEFAULT_TZ --chinese --chinese-format table"

print_subheader "JSON Format"
run_command "$HOROSCOPE_CLI --date 2024-02-10 --time 12:00:00 --lat $DEFAULT_LAT --lon $DEFAULT_LON --timezone $DEFAULT_TZ --chinese --chinese-format json"

print_subheader "CSV Format"
run_command "$HOROSCOPE_CLI --date 2024-02-10 --time 12:00:00 --lat $DEFAULT_LAT --lon $DEFAULT_LON --timezone $DEFAULT_TZ --chinese --chinese-format csv"

# 3. Date Range Analysis
print_header "📅 Chinese Calendar Date Range"
echo "Generate Chinese calendar for a date range"
echo "Shows progression of days, months, and seasonal changes"

run_command "$HOROSCOPE_CLI --chinese-range 2024-02-08 2024-02-15 --lat $DEFAULT_LAT --lon $DEFAULT_LON --timezone $DEFAULT_TZ --chinese-format table"

# 4. Solar Terms Display
print_header "🌞 24 Solar Terms (節氣)"
echo "Display the 24 traditional Chinese solar terms for the current year"
echo "Shows precise astronomical timing of seasonal markers"

run_command "$HOROSCOPE_CLI --date $(date +%Y)-06-21 --time 12:00:00 --lat $DEFAULT_LAT --lon $DEFAULT_LON --timezone $DEFAULT_TZ --solar-terms"

# 5. Traditional Chinese Festivals
print_header "🎊 Traditional Chinese Festivals"
echo "Display traditional Chinese festivals and their dates"
echo "Includes Spring Festival, Mid-Autumn Festival, Dragon Boat Festival, etc."

run_command "$HOROSCOPE_CLI --date $(date +%Y)-01-01 --time 12:00:00 --lat $DEFAULT_LAT --lon $DEFAULT_LON --timezone $DEFAULT_TZ --chinese-festivals"

# 6. Four Pillars of Destiny (BaZi)
print_header "🔮 Four Pillars of Destiny (BaZi 八字)"
echo "Complete BaZi astrological analysis"
echo "Shows Year, Month, Day, and Hour pillars with element interactions"

run_command "$HOROSCOPE_CLI --date 1990-05-15 --time 14:30:00 --lat $DEFAULT_LAT --lon $DEFAULT_LON --timezone $DEFAULT_TZ --bazi"

# 7. Historical Dates Analysis
print_header "📜 Historical Dates Analysis"
echo "Analyze significant historical dates in Chinese calendar"

print_subheader "Chinese New Year 2024 (Dragon Year)"
run_command "$HOROSCOPE_CLI --date 2024-02-10 --time 00:00:00 --lat $DEFAULT_LAT --lon $DEFAULT_LON --timezone $DEFAULT_TZ --chinese --bazi"

print_subheader "Mid-Autumn Festival 2024"
run_command "$HOROSCOPE_CLI --date 2024-09-17 --time 20:00:00 --lat $DEFAULT_LAT --lon $DEFAULT_LON --timezone $DEFAULT_TZ --chinese --chinese-festivals"

print_subheader "Winter Solstice 2024"
run_command "$HOROSCOPE_CLI --date 2024-12-21 --time 12:00:00 --lat $DEFAULT_LAT --lon $DEFAULT_LON --timezone $DEFAULT_TZ --chinese --solar-terms"

# 8. Zodiac Year Transitions
print_header "🐲 Zodiac Year Transitions"
echo "Examine the transition between zodiac years"

print_subheader "End of Rabbit Year 2023"
run_command "$HOROSCOPE_CLI --date 2024-02-09 --time 23:59:59 --lat $DEFAULT_LAT --lon $DEFAULT_LON --timezone $DEFAULT_TZ --chinese"

print_subheader "Beginning of Dragon Year 2024"
run_command "$HOROSCOPE_CLI --date 2024-02-10 --time 00:00:01 --lat $DEFAULT_LAT --lon $DEFAULT_LON --timezone $DEFAULT_TZ --chinese"

# 9. Different Locations
print_header "🌍 Chinese Calendar Across Different Locations"
echo "Chinese calendar calculations for different global locations"

print_subheader "Shanghai, China"
run_command "$HOROSCOPE_CLI --date 2024-09-15 --time 12:00:00 --lat 31.2304 --lon 121.4737 --timezone 8.0 --chinese"

print_subheader "Hong Kong"
run_command "$HOROSCOPE_CLI --date 2024-09-15 --time 12:00:00 --lat 22.3193 --lon 114.1694 --timezone 8.0 --chinese"

print_subheader "San Francisco (Chinese diaspora)"
run_command "$HOROSCOPE_CLI --date 2024-09-15 --time 12:00:00 --lat 37.7749 --lon -122.4194 --timezone -8.0 --chinese"

# 10. Complete Analysis Example
print_header "🎯 Complete Chinese Calendar Analysis"
echo "Comprehensive analysis combining all Chinese calendar features"

run_command "$HOROSCOPE_CLI --date 2024-04-04 --time 15:30:00 --lat $DEFAULT_LAT --lon $DEFAULT_LON --timezone $DEFAULT_TZ --chinese --solar-terms --chinese-festivals --bazi --chinese-format table"

# 11. JSON Output for Integration
print_header "🔗 JSON Output for Application Integration"
echo "Complete Chinese calendar data in JSON format for software integration"

run_command "$HOROSCOPE_CLI --chinese-range 2024-12-01 2024-12-07 --lat $DEFAULT_LAT --lon $DEFAULT_LON --timezone $DEFAULT_TZ --chinese-format json"

# Final message
print_header "✨ Demonstration Complete"
echo -e "${BOLD}${GREEN}Chinese Calendar demonstration completed successfully!${NC}"
echo ""
echo "Key features demonstrated:"
echo "✓ Basic Chinese calendar display with Gan-Zhi cycles"
echo "✓ Multiple output formats (table, JSON, CSV)"
echo "✓ Date range analysis capabilities"
echo "✓ 24 Solar Terms with astronomical precision"
echo "✓ Traditional Chinese festivals calendar"
echo "✓ Four Pillars of Destiny (BaZi) astrological analysis"
echo "✓ Historical date analysis and zodiac transitions"
echo "✓ Global location support for diaspora communities"
echo "✓ Multi-language support with Chinese characters"
echo "✓ Complete integration with existing horoscope_cli"
echo ""
echo -e "${BOLD}${PURPLE}The Chinese calendar system is now fully integrated and ready for use!${NC}"
echo ""
echo "For more information, see:"
echo "• USER_MANUAL.md - Complete usage documentation"
echo "• docs/CHINESE_CALENDAR_IMPLEMENTATION_PLAN.md - Technical details"
echo "• README.md - Quick start guide"
echo ""
echo -e "${YELLOW}Thank you for exploring the Chinese calendar system! 🐉${NC}"