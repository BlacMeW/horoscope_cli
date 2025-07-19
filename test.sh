#!/bin/bash

# Test script for Horoscope CLI
# Tests various functionality with sample birth data

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

EXECUTABLE="./bin/horoscope_cli"
if [ ! -f "$EXECUTABLE" ]; then
    EXECUTABLE="./build/horoscope_cli"
fi

if [ ! -f "$EXECUTABLE" ]; then
    echo -e "${RED}Error: horoscope_cli executable not found. Build the project first.${NC}"
    exit 1
fi

echo -e "${GREEN}Testing Horoscope CLI${NC}"
echo "=================================="

# Test 1: Version check
echo -e "\n${YELLOW}Test 1: Version check${NC}"
$EXECUTABLE --version

# Test 2: Help message
echo -e "\n${YELLOW}Test 2: Help message${NC}"
$EXECUTABLE --help | head -10

# Test 3: Basic birth chart (Example: Albert Einstein)
echo -e "\n${YELLOW}Test 3: Albert Einstein's Birth Chart${NC}"
echo "Date: March 14, 1879, 11:30 AM, Ulm, Germany"
$EXECUTABLE --date "1879-03-14" --time "11:30:00" --lat 48.4011 --lon 9.9876 --timezone 1

# Test 4: Different house system (Example: John Lennon)
echo -e "\n${YELLOW}Test 4: Different House System (Koch)${NC}"
echo "Date: October 9, 1940, 6:30 PM, Liverpool, UK"
$EXECUTABLE --date "1940-10-09" --time "18:30:00" --lat 53.4084 --lon -2.9916 --timezone 0 --house-system K

# Test 5: JSON output (Example: Steve Jobs)
echo -e "\n${YELLOW}Test 5: JSON Output${NC}"
echo "Date: February 24, 1955, 7:15 PM, San Francisco, CA"
$EXECUTABLE --date "1955-02-24" --time "19:15:00" --lat 37.7749 --lon -122.4194 --timezone -8 --output json | head -20

# Test 6: Southern Hemisphere (Example: Sydney, Australia)
echo -e "\n${YELLOW}Test 6: Southern Hemisphere Location${NC}"
echo "Date: January 1, 2000, 12:00 PM, Sydney, Australia"
$EXECUTABLE --date "2000-01-01" --time "12:00:00" --lat -33.8688 --lon 151.2093 --timezone 11

# Test 7: Error handling - Invalid date
echo -e "\n${YELLOW}Test 7: Error Handling (Invalid Date)${NC}"
$EXECUTABLE --date "invalid-date" --time "12:00:00" --lat 0 --lon 0 --timezone 0 2>&1 | head -3

echo -e "\n${GREEN}Testing completed!${NC}"
