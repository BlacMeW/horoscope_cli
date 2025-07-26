#!/usr/bin/env python3
"""
USER_MANUAL PDF Converter
Converts the USER_MANUAL.md to PDF format by cleaning up problematic elements
"""

import re
import subprocess
import sys
import os

def clean_markdown_for_pdf(content):
    """Remove problematic elements for PDF conversion"""
    
    # Remove all emojis and Unicode symbols with a comprehensive approach
    # First, replace specific symbols we want to keep meaning for
    symbol_replacements = {
        # Astrological symbols
        '♈': 'Aries', '♉': 'Taurus', '♊': 'Gemini', '♋': 'Cancer',
        '♌': 'Leo', '♍': 'Virgo', '♎': 'Libra', '♏': 'Scorpio',
        '♐': 'Sagittarius', '♑': 'Capricorn', '♒': 'Aquarius', '♓': 'Pisces',
        '☉': 'Sun', '☽': 'Moon', '☿': 'Mercury', '♀': 'Venus',
        '♂': 'Mars', '♃': 'Jupiter', '♄': 'Saturn', '♅': 'Uranus',
        '♆': 'Neptune', '♇': 'Pluto',
        
        # Special symbols
        '±': '+/-', '°': ' degrees', '≥': '>=', '≤': '<=',
        '→': '->', '←': '<-', '↑': '^', '↓': 'v',
        '✓': 'Y', '✅': 'YES', '❌': 'NO', '⭐': '*', '✨': '*',
        '…': '...', '"': '"', '"': '"', ''': "'", ''': "'",
        '–': '-', '—': '--',
        
        # Time and arrows
        '⏰': '(time)', '⏳': '(time)', '⌚': '(time)',
        '🕐': '1:00', '🕑': '2:00', '🕒': '3:00', '🕓': '4:00',
        '🕔': '5:00', '🕕': '6:00', '🕖': '7:00', '🕗': '8:00',
        '🕘': '9:00', '🕙': '10:00', '🕚': '11:00', '🕛': '12:00',
        
        # Common emojis to meaningful text
        '🌟': '*', '🌞': 'Sun', '🌚': 'Moon', '🌙': 'Moon',
        '🌕': 'Full Moon', '🌑': 'New Moon', '🌗': 'Waxing Moon', '🌘': 'Waning Moon',
        '🔮': '(crystal ball)', '🎯': '(target)', '🚀': '(rocket)',
        '📅': '(calendar)', '📊': '(chart)', '📈': '(trending up)',
        '📋': '(clipboard)', '📖': '(book)', '🛠️': '(tools)',
        '⚙️': '(gear)', '🔧': '(wrench)', '💾': '(save)',
        '⚡': '(lightning)', '🔍': '(search)', '🪐': '(planet)',
        '🎨': '(art)', '🏛️': '(building)', '🌌': '(galaxy)',
        '🔢': '(numbers)', '🧮': '(abacus)',
    }
    
    # Apply symbol replacements
    for symbol, replacement in symbol_replacements.items():
        content = content.replace(symbol, replacement)
    
    # Remove remaining Unicode characters that could cause issues
    # This removes most emojis and special Unicode characters
    import unicodedata
    
    # Convert content to ASCII-safe version
    lines = content.split('\n')
    clean_lines = []
    
    for line in lines:
        # Remove any remaining non-ASCII characters except basic ones
        clean_line = ''
        for char in line:
            if ord(char) < 128 or char in ['—', '–', '"', '"', ''', ''']:
                clean_line += char
            elif unicodedata.category(char).startswith('L'):  # Letter
                # Skip non-Latin letters
                continue
            else:
                # Replace other characters with space
                clean_line += ' '
        
        # Clean up extra spaces
        clean_line = ' '.join(clean_line.split())
        clean_lines.append(clean_line)
    
    content = '\n'.join(clean_lines)
    
    # Remove HTML tags
    content = re.sub(r'<div[^>]*>', '', content)
    content = re.sub(r'</div>', '', content)
    
    # Remove badges and shields.io links
    content = re.sub(r'\[!\[.*?\]\([^)]*\)\]\([^)]*\)', '', content)
    content = re.sub(r'!\[.*?\]\([^)]*\)', '', content)
    
    # Clean up table formatting that might cause issues
    content = re.sub(r'\|:--:\|', '|---|', content)
    content = re.sub(r'\|:--\|', '|---|', content)
    content = re.sub(r'\|--:\|', '|---|', content)
    
    # Clean up excessive whitespace
    content = re.sub(r'\n\s*\n\s*\n', '\n\n', content)
    content = re.sub(r'  +', ' ', content)  # Multiple spaces to single space
    
    return content

def main():
    input_file = "USER_MANUAL.md"
    output_file = "USER_MANUAL.pdf"
    temp_file = "USER_MANUAL_clean.md"
    
    print("Converting USER_MANUAL.md to PDF...")
    print("=" * 50)
    
    # Check if input file exists
    if not os.path.exists(input_file):
        print(f"Error: {input_file} not found")
        sys.exit(1)
    
    # Read and clean the markdown
    print("Processing markdown file...")
    with open(input_file, 'r', encoding='utf-8') as f:
        content = f.read()
    
    cleaned_content = clean_markdown_for_pdf(content)
    
    # Write cleaned content
    with open(temp_file, 'w', encoding='utf-8') as f:
        f.write(cleaned_content)
    
    print("Converting to PDF...")
    
    # Try conversion with different options
    commands = [
        # Try with basic options first
        ["pandoc", temp_file, "-o", output_file, "--toc", "--number-sections"],
        # Fallback to minimal conversion
        ["pandoc", temp_file, "-o", output_file],
    ]
    
    success = False
    for i, cmd in enumerate(commands):
        try:
            print(f"Attempt {i+1}: {' '.join(cmd)}")
            result = subprocess.run(cmd, capture_output=True, text=True, timeout=120)
            
            if result.returncode == 0 and os.path.exists(output_file):
                success = True
                break
            else:
                print(f"Attempt {i+1} failed:")
                if result.stderr:
                    print(result.stderr[:500])
                    
        except subprocess.TimeoutExpired:
            print(f"Attempt {i+1} timed out")
        except Exception as e:
            print(f"Attempt {i+1} error: {e}")
    
    # Clean up temp file
    if os.path.exists(temp_file):
        os.remove(temp_file)
    
    if success:
        print("SUCCESS: PDF created successfully!")
        print(f"Output file: {output_file}")
        if os.path.exists(output_file):
            size = os.path.getsize(output_file)
            print(f"File size: {size / 1024 / 1024:.1f} MB")
        print("\nPDF Features:")
        print("   • Professional typography")
        print("   • Table of contents")
        print("   • Numbered sections")
        print("   • Searchable text")
        print("   • Print-ready formatting")
    else:
        print("ERROR: PDF conversion failed")
        sys.exit(1)

if __name__ == "__main__":
    main()
