#!/usr/bin/env python3
"""
Enhanced USER_MANUAL PDF Converter with Graphics
Creates a professional PDF with custom book cover graphics and visual elements
"""

import re
import subprocess
import sys
import os
import tempfile

def create_enhanced_book_cover():
    """Create an enhanced book cover with visual graphics"""
    return '''---
title: "HOROSCOPE CLI - Complete User Manual"
subtitle: "Professional Astronomical Calculations & Multi-Cultural Calendar Systems"
author: "HoroscopeCLI Team"
date: "July 2025"
version: "2.0.0"
documentclass: book
geometry: margin=1in
fontsize: 11pt
colorlinks: true
toc: true
numbersections: true
header-includes: |
    \\usepackage{fancyhdr}
    \\usepackage{graphicx}
    \\usepackage{xcolor}
    \\usepackage{tikz}
    \\usepackage{amsmath}
    \\usepackage{amssymb}
    \\definecolor{darkblue}{RGB}{25,25,112}
    \\definecolor{gold}{RGB}{255,215,0}
    \\definecolor{starblue}{RGB}{72,61,139}
    \\pagestyle{fancy}
    \\fancyhf{}
    \\fancyhead[LE,RO]{\\thepage}
    \\fancyhead[LO,RE]{HoroscopeCLI Manual}
    \\renewcommand{\\headrulewidth}{0.4pt}
---

\\begin{titlepage}
\\centering
\\vspace*{1cm}

{\\huge\\bfseries\\color{darkblue}
🌟 HOROSCOPE CLI 🌟\\\\[0.5cm]
}

{\\Large\\color{starblue}
COMPLETE USER MANUAL \\& GUIDE\\\\[1cm]
}

\\begin{tikzpicture}[scale=0.8]
% Zodiac wheel representation
\\draw[thick, darkblue] (0,0) circle (3);
\\draw[thick, gold] (0,0) circle (2.5);
\\draw[thick, darkblue] (0,0) circle (2);

% Zodiac signs positions (simplified)
\\foreach \\angle in {0,30,60,90,120,150,180,210,240,270,300,330}
{
    \\draw[gold] (\\angle:2) -- (\\angle:2.5);
}

% Center sun symbol
\\filldraw[gold] (0,0) circle (0.3);
\\foreach \\angle in {0,45,90,135,180,225,270,315}
{
    \\draw[gold, thick] (\\angle:0.4) -- (\\angle:0.6);
}

% Cardinal directions
\\node[darkblue, font=\\large] at (0,2.75) {♈};
\\node[darkblue, font=\\large] at (2.38,1.38) {♋};
\\node[darkblue, font=\\large] at (0,-2.75) {♎};
\\node[darkblue, font=\\large] at (-2.38,1.38) {♑};
\\end{tikzpicture}

\\vspace{1cm}

{\\Large\\color{darkblue}
🔮 THE MOST COMPREHENSIVE COMMAND-LINE 🔮\\\\
ASTROLOGY TOOLKIT EVER CREATED\\\\[1cm]
}

\\begin{center}
\\begin{tabular}{|c|}
\\hline
\\textbf{\\color{starblue}KEY FEATURES} \\\\
\\hline
🌞 Professional Astronomical Calculations \\\\
🕉️ Multi-Cultural Calendar Systems \\\\
🏛️ Historical BC Era Support (6000 BC+) \\\\
📊 Swiss Ephemeris Integration \\\\
🎯 Birth Charts \\& Vedic Astrology \\\\
📅 Hindu \\& Myanmar Calendars \\\\
🌚 Eclipse \\& Conjunction Analysis \\\\
🔢 KP System \\& Ephemeris Tables \\\\
🌟 Over 100 Practical Examples \\\\
⚡ Professional-Grade Accuracy \\\\
\\hline
\\end{tabular}
\\end{center}

\\vfill

{\\large\\color{darkblue}
VERSION 2.0.0 • JULY 2025\\\\[0.5cm]
Created with ❤️ by HoroscopeCLI Team\\\\
Powered by Swiss Ephemeris\\\\
Licensed under MIT\\\\
}

\\end{titlepage}

\\newpage
\\tableofcontents
\\newpage

'''

def create_visual_dividers():
    """Create visual section dividers"""
    return {
        'major_section': '''
\\vspace{0.5cm}
\\begin{center}
\\begin{tikzpicture}
\\draw[darkblue, thick] (0,0) -- (6,0);
\\foreach \\x in {1,2,3,4,5}
{
    \\filldraw[gold] (\\x,0) circle (0.05);
}
\\end{tikzpicture}
\\end{center}
\\vspace{0.5cm}
''',
        'minor_section': '''
\\vspace{0.3cm}
\\begin{center}
\\textcolor{darkblue}{⋆ ⋆ ⋆}
\\end{center}
\\vspace{0.3cm}
'''
    }

def clean_markdown_for_pdf(content):
    """Remove problematic elements and enhance for PDF conversion"""

    # Symbol replacements for better PDF rendering
    symbol_replacements = {
        # Astrological symbols
        '♈': '\\textcolor{darkblue}{Aries}', '♉': '\\textcolor{darkblue}{Taurus}',
        '♊': '\\textcolor{darkblue}{Gemini}', '♋': '\\textcolor{darkblue}{Cancer}',
        '♌': '\\textcolor{darkblue}{Leo}', '♍': '\\textcolor{darkblue}{Virgo}',
        '♎': '\\textcolor{darkblue}{Libra}', '♏': '\\textcolor{darkblue}{Scorpio}',
        '♐': '\\textcolor{darkblue}{Sagittarius}', '♑': '\\textcolor{darkblue}{Capricorn}',
        '♒': '\\textcolor{darkblue}{Aquarius}', '♓': '\\textcolor{darkblue}{Pisces}',
        '☉': '\\textcolor{gold}{Sun}', '☽': '\\textcolor{darkblue}{Moon}',
        '☿': 'Mercury', '♀': 'Venus', '♂': 'Mars', '♃': 'Jupiter',
        '♄': 'Saturn', '♅': 'Uranus', '♆': 'Neptune', '♇': 'Pluto',

        # Special symbols with colors
        '⭐': '\\textcolor{gold}{★}', '✨': '\\textcolor{gold}{✦}',
        '🌟': '\\textcolor{gold}{★}', '🌞': '\\textcolor{gold}{☀}',
        '🌚': '\\textcolor{darkblue}{☾}', '🌙': '\\textcolor{darkblue}{☽}',
        '🔮': '\\textcolor{starblue}{◉}', '🎯': '\\textcolor{darkblue}{⊙}',

        # Remove complex emojis but keep meaning
        '🌕': 'Full Moon', '🌑': 'New Moon', '🌗': 'Waxing Moon', '🌘': 'Waning Moon',
        '📅': 'Calendar', '📊': 'Chart', '📈': 'Analytics', '📋': 'List',
        '📖': 'Manual', '🛠️': 'Tools', '⚙️': 'Settings', '🔧': 'Configuration',
        '💾': 'Save', '⚡': 'Fast', '🔍': 'Search', '🪐': 'Planet',
        '🎨': 'Design', '🏛️': 'Classical', '🌌': 'Cosmic', '🔢': 'Numbers',
        '🧮': 'Calculator', '🚀': 'Advanced', '❤️': 'Love',

        # Technical symbols
        '±': '$\\pm$', '°': '°', '≥': '$\\geq$', '≤': '$\\leq$',
        '→': '$\\rightarrow$', '←': '$\\leftarrow$', '↑': '$\\uparrow$', '↓': '$\\downarrow$',
        '✓': '\\textcolor{darkblue}{✓}', '✅': '\\textcolor{darkblue}{✓}',
        '❌': '\\textcolor{red}{✗}', '…': '\\ldots',
        '"': '"', '"': '"', ''': "'", ''': "'", '–': '--', '—': '---',
    }

    # Apply symbol replacements
    for symbol, replacement in symbol_replacements.items():
        content = content.replace(symbol, replacement)

    # Remove remaining problematic Unicode ranges
    # Remove emojis (U+1F600-U+1F64F, U+1F300-U+1F5FF, U+1F680-U+1F6FF, U+1F1E0-U+1F1FF)
    content = re.sub(r'[\U0001F600-\U0001F64F\U0001F300-\U0001F5FF\U0001F680-\U0001F6FF\U0001F1E0-\U0001F1FF\U0001F900-\U0001F9FF\U0001FA70-\U0001FAFF]', '', content)

    # Remove other problematic Unicode ranges
    content = re.sub(r'[\U00002600-\U000026FF]', '', content)  # Miscellaneous symbols
    content = re.sub(r'[\U00002700-\U000027BF]', '', content)  # Dingbats
    content = re.sub(r'[\U0001F000-\U0001F02F]', '', content)  # Mahjong tiles
    content = re.sub(r'[\U0001F0A0-\U0001F0FF]', '', content)  # Playing cards

    # Remove HTML div tags and center alignments
    content = re.sub(r'<div[^>]*>', '', content)
    content = re.sub(r'</div>', '', content)
    content = re.sub(r'<center>', '', content)
    content = re.sub(r'</center>', '', content)

    # Remove image badges that won't work in PDF
    content = re.sub(r'!\[.*?\]\(https://img\.shields\.io/.*?\)', '', content)
    content = re.sub(r'\[!\[.*?\]\(.*?\)\]\(.*?\)', '', content)

    # Clean up extra whitespace
    content = re.sub(r'\n\s*\n\s*\n', '\n\n', content)
    content = re.sub(r'[ \t]+\n', '\n', content)

    return content

def enhance_markdown_structure(content):
    """Add visual enhancements to the markdown structure"""
    dividers = create_visual_dividers()

    # Add visual dividers before major sections
    major_sections = ['## Quick Start', '## Installation', '## Features Overview',
                     '## Chart Generation', '## Calendar Systems', '## Advanced Features']

    for section in major_sections:
        content = content.replace(section, dividers['major_section'] + section)

    # Simple replacement for subsections to avoid regex issues
    lines = content.split('\n')
    enhanced_lines = []
    for line in lines:
        if line.startswith('### '):
            enhanced_lines.append('')
            enhanced_lines.append('\\vspace{0.3cm}')
            enhanced_lines.append('\\begin{center}')
            enhanced_lines.append('\\textcolor{darkblue}{⋆ ⋆ ⋆}')
            enhanced_lines.append('\\end{center}')
            enhanced_lines.append('\\vspace{0.3cm}')
            enhanced_lines.append('')
        enhanced_lines.append(line)

    return '\n'.join(enhanced_lines)

def main():
    input_file = "USER_MANUAL.md"
    output_file = "USER_MANUAL_enhanced.pdf"
    temp_file = "USER_MANUAL_enhanced.md"

    print("Creating Enhanced USER_MANUAL PDF with Graphics...")
    print("=" * 60)

    # Check if input file exists
    if not os.path.exists(input_file):
        print(f"Error: {input_file} not found")
        sys.exit(1)

    # Read and process the markdown
    print("Processing markdown file...")
    with open(input_file, 'r', encoding='utf-8') as f:
        content = f.read()

    # Remove the existing title section and replace with enhanced cover
    content = re.sub(r'^# .*?(?=\n## )', '', content, flags=re.DOTALL)

    # Create enhanced content
    enhanced_content = create_enhanced_book_cover()
    cleaned_content = clean_markdown_for_pdf(content)
    enhanced_content += enhance_markdown_structure(cleaned_content)

    # Write enhanced content
    with open(temp_file, 'w', encoding='utf-8') as f:
        f.write(enhanced_content)

    print("Converting to PDF with graphics...")

    # Enhanced pandoc command with LaTeX engine for graphics support
    commands = [
        # Try with XeLaTeX for best Unicode and graphics support
        ["pandoc", temp_file, "-o", output_file,
         "--pdf-engine=xelatex", "--toc", "--number-sections",
         "--template=default", "--variable", "geometry:margin=1in"],

        # Fallback to pdflatex
        ["pandoc", temp_file, "-o", output_file,
         "--pdf-engine=pdflatex", "--toc", "--number-sections"],

        # Simple fallback
        ["pandoc", temp_file, "-o", output_file, "--toc"]
    ]

    success = False
    for i, cmd in enumerate(commands):
        try:
            print(f"Attempt {i+1}: {' '.join(cmd[:6])}...")
            result = subprocess.run(cmd, capture_output=True, text=True, timeout=180)

            if result.returncode == 0 and os.path.exists(output_file):
                success = True
                break
            else:
                print(f"Attempt {i+1} failed:")
                if result.stderr:
                    print(result.stderr[:300])

        except subprocess.TimeoutExpired:
            print(f"Attempt {i+1} timed out")
        except Exception as e:
            print(f"Attempt {i+1} error: {e}")

    # Clean up temp file
    if os.path.exists(temp_file):
        os.remove(temp_file)

    if success:
        print("SUCCESS: Enhanced PDF with graphics created!")
        print(f"Output file: {output_file}")
        if os.path.exists(output_file):
            size = os.path.getsize(output_file)
            print(f"File size: {size / 1024 / 1024:.1f} MB")
        print("\nEnhanced PDF Features:")
        print("   • Professional book cover with zodiac graphics")
        print("   • Color-coded astrological symbols")
        print("   • Visual section dividers")
        print("   • Professional typography with LaTeX")
        print("   • Enhanced table of contents")
        print("   • Print-ready formatting")
        print("   • Searchable text with graphics")
    else:
        print("ERROR: Enhanced PDF conversion failed")
        print("Falling back to standard conversion...")

        # Fallback to original converter
        try:
            subprocess.run(["python3", "convert_to_pdf.py"], check=True)
            print("Standard PDF created as fallback")
        except:
            print("Both enhanced and standard conversion failed")
            sys.exit(1)

if __name__ == "__main__":
    main()
