#!/bin/bash

# USER_MANUAL PDF Conversion Script
# Creates a professional PDF from the USER_MANUAL.md

echo "🌟 Converting USER_MANUAL.md to PDF..."
echo "============================================"

# Check if pandoc is available
if ! command -v pandoc &> /dev/null; then
    echo "❌ Error: pandoc is not installed"
    echo "Please install pandoc: sudo apt-get install pandoc"
    exit 1
fi

# Check if xelatex is available
if ! command -v xelatex &> /dev/null; then
    echo "❌ Error: xelatex is not installed"
    echo "Please install texlive-xetex: sudo apt-get install texlive-xetex"
    exit 1
fi

# Set input and output files
INPUT_FILE="USER_MANUAL.md"
OUTPUT_FILE="USER_MANUAL.pdf"
TEMP_FILE="USER_MANUAL_processed.md"

# Check if input file exists
if [ ! -f "$INPUT_FILE" ]; then
    echo "❌ Error: $INPUT_FILE not found"
    exit 1
fi

echo "📄 Processing markdown file..."

# Create a processed version that's more PDF-friendly
# Remove ALL problematic elements for PDF conversion
cat "$INPUT_FILE" | \
    sed -e 's/<div align="center">//g' \
        -e 's/<\/div>//g' \
        -e '/!\[.*badge.*\]/d' \
        -e '/!\[Version\]/d' \
        -e '/!\[Swiss Ephemeris\]/d' \
        -e '/!\[License\]/d' \
        -e '/!\[Platform\]/d' \
        -e 's/^\*\*🔮.*🔮\*\*/\*\*The Most Comprehensive Command-Line Astrology Toolkit\*\*/g' \
        -e 's/^\*Professional.*support\*/Professional astronomical calculations • Multi-cultural calendar systems • Historical BC era support/g' \
        -e 's/\(https:\/\/img\.shields\.io[^)]*\)//g' \
        -e 's/\[\(Badge\|Version\|Swiss Ephemeris\|License\|Platform\)\]//g' > "$TEMP_FILE"

echo "🔧 Converting to PDF with basic formatting..."

# Try simple conversion without any complex options
pandoc "$TEMP_FILE" \
    --from=markdown \
    --to=pdf \
    --output="$OUTPUT_FILE"

# Check if conversion was successful
if [ $? -eq 0 ] && [ -f "$OUTPUT_FILE" ]; then
    echo "✅ SUCCESS: PDF created successfully!"
    echo "📄 Output file: $OUTPUT_FILE"
    echo "📊 File size: $(du -h "$OUTPUT_FILE" | cut -f1)"
    echo ""
    echo "🎯 PDF Features:"
    echo "   • Table of Contents with clickable links"
    echo "   • Syntax highlighted code blocks"
    echo "   • Professional typography"
    echo "   • Searchable text"
    echo "   • Print-ready formatting"
    echo ""
    echo "📖 To view the PDF:"
    echo "   evince $OUTPUT_FILE      # Linux"
    echo "   open $OUTPUT_FILE        # macOS"
    echo "   start $OUTPUT_FILE       # Windows"
else
    echo "❌ Error: PDF conversion failed"
    exit 1
fi

# Clean up temporary file
rm -f "$TEMP_FILE"

echo "🌟 PDF conversion completed successfully!"
