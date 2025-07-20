#!/bin/bash

# Example: Myanmar Calendar Multi-Criteria Search Usage
# This shows how to search for "Full Moon OR Yatyaza" using the command line

echo "🌙 Myanmar Calendar Multi-Criteria Search Example"
echo "================================================="
echo ""

# Set the binary path
BINARY="./bin/horoscope_cli"

echo "🔍 Example: Search for Full Moon OR Yatyaza days in January 2024"
echo ""

# This demonstrates the search you wanted: Full moon OR yatyaza
$BINARY --myanmar-search 2024-01-01 2024-01-31 \
        --myanmar-search-moon-phase 1 \
        --myanmar-search-yatyaza \
        --myanmar-search-logic or \
        --myanmar-search-tabular

echo ""
echo "✅ This search finds dates that are EITHER:"
echo "   • Full moon days (moon phase 1), OR"
echo "   • Yatyaza (inauspicious) days"
echo ""
echo "📚 More Examples:"
echo "• For Sabbath AND Full Moon: --myanmar-search-logic and"
echo "• For Weekend days: --myanmar-search-weekday 0 --myanmar-search-weekday 1 --myanmar-search-logic or"
echo "• For multiple astrological events: --myanmar-search-sabbath --myanmar-search-thamanyo --myanmar-search-logic or"
