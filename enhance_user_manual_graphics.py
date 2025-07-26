#!/usr/bin/env python3
"""
Graphics-Enhanced USER_MANUAL PDF Converter
Enhances the existing USER_MANUAL.md with graphics and creates a professional PDF
"""

import re
import subprocess
import sys
import os

def create_graphics_enhanced_cover(original_content):
    """Replace the existing ASCII cover with LaTeX graphics-enhanced version"""

    # Extract everything after the book cover
    content_start = original_content.find('## Table of Contents')
    if content_start == -1:
        content_start = original_content.find('## 📋 Table of Contents')
    if content_start == -1:
        content_start = original_content.find('## Quick Start')

    if content_start == -1:
        # If we can't find a clear start, keep original content
        remaining_content = original_content
    else:
        remaining_content = original_content[content_start:]

    # Create LaTeX-enhanced frontmatter
    enhanced_cover = '''---
title: "HOROSCOPE CLI - Complete User Manual"
subtitle: "Professional Astronomical Calculations & Multi-Cultural Calendar Systems"
author: "HoroscopeCLI Team"
date: "July 2025"
version: "2.0.0"
documentclass: article
geometry: margin=1in
fontsize: 11pt
colorlinks: true
toc: true
numbersections: true
header-includes: |
    \\usepackage{fancyhdr}
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

{\\Huge\\bfseries\\color{darkblue}
🌟 HOROSCOPE CLI 🌟\\\\[0.8cm]
}

{\\Large\\color{starblue}
COMPLETE USER MANUAL \\& GUIDE\\\\[1.5cm]
}

% Enhanced Zodiac Wheel Graphics
\\begin{tikzpicture}[scale=1.0]
% Outer circle
\\draw[thick, darkblue, line width=2pt] (0,0) circle (3.5);
% Inner circles
\\draw[thick, gold, line width=1.5pt] (0,0) circle (3);
\\draw[thick, darkblue] (0,0) circle (2.5);
\\draw[thick, gold] (0,0) circle (2);

% Zodiac divisions
\\foreach \\angle in {0,30,60,90,120,150,180,210,240,270,300,330}
{
    \\draw[gold, line width=1pt] (\\angle:2) -- (\\angle:3.5);
}

% Enhanced center sun
\\filldraw[gold] (0,0) circle (0.4);
\\foreach \\angle in {0,30,60,90,120,150,180,210,240,270,300,330}
{
    \\draw[gold, thick, line width=2pt] (\\angle:0.5) -- (\\angle:0.8);
}

% Cardinal signs with enhanced positioning
\\node[darkblue, font=\\Huge] at (0,3.25) {♈};     % Aries
\\node[darkblue, font=\\Huge] at (2.8,1.65) {♋};   % Cancer
\\node[darkblue, font=\\Huge] at (0,-3.25) {♎};    % Libra
\\node[darkblue, font=\\Huge] at (-2.8,1.65) {♑};  % Capricorn

% Fixed signs
\\node[starblue, font=\\Large] at (2.8,-1.65) {♌};  % Leo
\\node[starblue, font=\\Large] at (2.8,1.65) {♏};   % Scorpio
\\node[starblue, font=\\Large] at (-2.8,1.65) {♒}; % Aquarius
\\node[starblue, font=\\Large] at (-2.8,-1.65) {♉}; % Taurus

% Decorative stars
\\foreach \\angle in {15,45,75,105,135,165,195,225,255,285,315,345}
{
    \\node[gold, font=\\tiny] at (\\angle:4) {★};
}
\\end{tikzpicture}

\\vspace{1cm}

{\\Large\\color{darkblue}
🔮 THE MOST COMPREHENSIVE COMMAND-LINE 🔮\\\\
ASTROLOGY TOOLKIT EVER CREATED\\\\[1cm]
}

% Feature highlights box
\\begin{center}
\\colorbox{starblue!10}{
\\begin{minipage}{0.8\\textwidth}
\\centering
\\textbf{\\color{darkblue}\\Large KEY FEATURES} \\\\[0.5cm]
\\begin{tabular}{ll}
🌞 & Professional Astronomical Calculations \\\\[0.2cm]
🕉️ & Multi-Cultural Calendar Systems \\\\[0.2cm]
🏛️ & Historical BC Era Support (6000 BC+) \\\\[0.2cm]
📊 & Swiss Ephemeris Integration \\\\[0.2cm]
🎯 & Birth Charts \\& Vedic Astrology \\\\[0.2cm]
📅 & Hindu \\& Myanmar Calendars \\\\[0.2cm]
🌚 & Eclipse \\& Conjunction Analysis \\\\[0.2cm]
🔢 & KP System \\& Ephemeris Tables \\\\[0.2cm]
🌟 & Over 100 Practical Examples \\\\[0.2cm]
⚡ & Professional-Grade Accuracy \\\\[0.2cm]
🎨 & Beautiful ASCII Chart Visualizations \\\\
\\end{tabular}
\\end{minipage}
}
\\end{center}

\\vfill

{\\large\\color{darkblue}
\\textbf{VERSION 2.0.0 • JULY 2025}\\\\[0.5cm]
Created with ❤️ by HoroscopeCLI Team\\\\
Powered by Swiss Ephemeris\\\\
Licensed under MIT\\\\
}

\\end{titlepage}

\\newpage
\\tableofcontents
\\newpage

'''

    return enhanced_cover + remaining_content

def clean_content_for_graphics_pdf(content):
    """Clean content while preserving some visual elements for graphics PDF"""

    # Enhanced symbol replacements with LaTeX colors
    symbol_replacements = {
        # Astrological symbols with colors
        '♈': '\\textcolor{darkblue}{♈}', '♉': '\\textcolor{darkblue}{♉}',
        '♊': '\\textcolor{darkblue}{♊}', '♋': '\\textcolor{darkblue}{♋}',
        '♌': '\\textcolor{darkblue}{♌}', '♍': '\\textcolor{darkblue}{♍}',
        '♎': '\\textcolor{darkblue}{♎}', '♏': '\\textcolor{darkblue}{♏}',
        '♐': '\\textcolor{darkblue}{♐}', '♑': '\\textcolor{darkblue}{♑}',
        '♒': '\\textcolor{darkblue}{♒}', '♓': '\\textcolor{darkblue}{♓}',

        # Planetary symbols with colors
        '☉': '\\textcolor{gold}{☉}', '☽': '\\textcolor{darkblue}{☽}',
        '☿': '\\textcolor{starblue}{☿}', '♀': '\\textcolor{gold}{♀}',
        '♂': '\\textcolor{red}{♂}', '♃': '\\textcolor{darkblue}{♃}',
        '♄': '\\textcolor{darkblue}{♄}', '♅': '\\textcolor{starblue}{♅}',
        '♆': '\\textcolor{darkblue}{♆}', '♇': '\\textcolor{darkblue}{♇}',

        # Visual elements with colors
        '⭐': '\\textcolor{gold}{★}', '✨': '\\textcolor{gold}{✦}',
        '🌟': '\\textcolor{gold}{★}', '🌞': '\\textcolor{gold}{☀}',
        '🌚': '\\textcolor{darkblue}{☾}', '🌙': '\\textcolor{darkblue}{☽}',
        '🔮': '\\textcolor{starblue}{◉}', '🎯': '\\textcolor{darkblue}{⊙}',

        # Technical symbols
        '°': '°', '±': '$\\pm$', '≥': '$\\geq$', '≤': '$\\leq$',
        '→': '$\\rightarrow$', '←': '$\\leftarrow$',
        '↑': '$\\uparrow$', '↓': '$\\downarrow$',
        '✓': '\\textcolor{darkblue}{✓}', '✅': '\\textcolor{green}{✓}',
        '❌': '\\textcolor{red}{✗}',

        # Clean up quotes and dashes
        '"': '"', '"': '"', ''': "'", ''': "'", '–': '--', '—': '---',
        '…': '\\ldots',
    }

    # Apply symbol replacements
    for symbol, replacement in symbol_replacements.items():
        content = content.replace(symbol, replacement)

    # Remove complex emojis that don't have good LaTeX equivalents
    emoji_to_text = {
        '🌕': 'Full Moon', '🌑': 'New Moon', '🌗': 'Waxing Moon', '🌘': 'Waning Moon',
        '📅': 'Calendar', '📊': 'Chart', '📈': 'Analytics', '📋': 'List',
        '📖': 'Manual', '🛠️': 'Tools', '⚙️': 'Settings', '🔧': 'Configuration',
        '💾': 'Save', '⚡': 'Fast', '🔍': 'Search', '🪐': 'Planet',
        '🎨': 'Design', '🏛️': 'Classical', '🌌': 'Cosmic', '🔢': 'Numbers',
        '🧮': 'Calculator', '🚀': 'Advanced', '❤️': 'Love', '🕉️': 'Om',
    }

    for emoji, text in emoji_to_text.items():
        content = content.replace(emoji, text)

    # Remove remaining problematic Unicode
    content = re.sub(r'[\U0001F600-\U0001F64F\U0001F300-\U0001F5FF\U0001F680-\U0001F6FF\U0001F1E0-\U0001F1FF\U0001F900-\U0001F9FF\U0001FA70-\U0001FAFF]', '', content)
    content = re.sub(r'[\U00002600-\U000026FF]', '', content)
    content = re.sub(r'[\U00002700-\U000027BF]', '', content)

    # Remove HTML elements
    content = re.sub(r'<div[^>]*>', '', content)
    content = re.sub(r'</div>', '', content)
    content = re.sub(r'<center>', '', content)
    content = re.sub(r'</center>', '', content)

    # Remove badge images
    content = re.sub(r'!\[.*?\]\(https://img\.shields\.io/.*?\)', '', content)
    content = re.sub(r'\[!\[.*?\]\(.*?\)\]\(.*?\)', '', content)

    # Clean up whitespace
    content = re.sub(r'\n\s*\n\s*\n', '\n\n', content)
    content = re.sub(r'[ \t]+\n', '\n', content)

    return content

def main():
    input_file = "USER_MANUAL.md"
    output_file = "USER_MANUAL.pdf"
    temp_file = "USER_MANUAL_graphics.md"

    print("Creating Graphics-Enhanced USER_MANUAL.pdf...")
    print("=" * 50)

    # Check if input file exists
    if not os.path.exists(input_file):
        print(f"Error: {input_file} not found")
        sys.exit(1)

    # Read original content
    print("Processing markdown file...")
    with open(input_file, 'r', encoding='utf-8') as f:
        content = f.read()

    # Create graphics-enhanced version
    enhanced_content = create_graphics_enhanced_cover(content)
    cleaned_content = clean_content_for_graphics_pdf(enhanced_content)

    # Write enhanced content
    with open(temp_file, 'w', encoding='utf-8') as f:
        f.write(cleaned_content)

    print("Converting to PDF with graphics...")

    # PDF conversion commands
    commands = [
        # Primary: XeLaTeX with graphics support
        ["pandoc", temp_file, "-o", output_file,
         "--pdf-engine=xelatex", "--toc", "--number-sections"],

        # Fallback: pdflatex
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
        print("SUCCESS: Graphics-enhanced PDF created!")
        print(f"Output file: {output_file}")
        if os.path.exists(output_file):
            size = os.path.getsize(output_file)
            print(f"File size: {size / 1024 / 1024:.1f} MB")
        print("\nGraphics-Enhanced Features:")
        print("   • Professional zodiac wheel graphics")
        print("   • Color-coded astrological symbols")
        print("   • Enhanced typography with LaTeX")
        print("   • Visual section formatting")
        print("   • Professional book cover design")
        print("   • Print-ready quality")
    else:
        print("ERROR: Graphics-enhanced conversion failed")
        print("Falling back to standard conversion...")
        try:
            subprocess.run(["python3", "convert_to_pdf.py"], check=True)
            print("Standard PDF created as fallback")
        except:
            print("Both enhanced and standard conversion failed")
            sys.exit(1)

if __name__ == "__main__":
    main()
