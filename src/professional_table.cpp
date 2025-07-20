#include "professional_table.h"
#include <algorithm>
#include <iostream>
#include <cmath>
#include <locale>
#include <codecvt>

namespace Astro {

// Define border characters for different styles (using strings for Unicode support)
const std::map<BorderStyle, ProfessionalTable::BorderChars> ProfessionalTable::borderChars = {
    {BorderStyle::ASCII, {'-', '|', '+', '+', '+', '+', '+', '+', '+', '+', '+'}},
    {BorderStyle::UNICODE_LIGHT, {'-', '|', '+', '+', '+', '+', '+', '+', '+', '+', '+'}}, // Simplified for now
    {BorderStyle::UNICODE_HEAVY, {'-', '|', '+', '+', '+', '+', '+', '+', '+', '+', '+'}}, // Simplified for now
    {BorderStyle::UNICODE_DOUBLE, {'=', '|', '+', '+', '+', '+', '+', '+', '+', '+', '+'}}, // Simplified for now
    {BorderStyle::MINIMAL, {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}}
};

// TableCell implementation
std::vector<std::string> TableCell::getWrappedLines(int maxWidth) const {
    if (!format.wordWrap || maxWidth <= 0) {
        return {content};
    }

    std::vector<std::string> lines;
    std::string remaining = content;

    while (!remaining.empty()) {
        if (static_cast<int>(remaining.length()) <= maxWidth) {
            lines.push_back(remaining);
            break;
        }

        // Find best break point
        size_t breakPoint = maxWidth;
        while (breakPoint > 0 && remaining[breakPoint] != ' ' && remaining[breakPoint] != '\n') {
            breakPoint--;
        }

        if (breakPoint == 0) {
            breakPoint = maxWidth; // Force break if no space found
        }

        lines.push_back(remaining.substr(0, breakPoint));
        remaining = remaining.substr(breakPoint + 1);
    }

    return lines;
}

int TableCell::getDisplayWidth() const {
    if (format.multiByteSupport) {
        // Simple approximation for multi-byte character width
        int width = 0;
        for (char c : content) {
            if ((c & 0x80) == 0) width += 1;      // ASCII
            else if ((c & 0xE0) == 0xC0) width += 1; // 2-byte UTF-8 start
            else if ((c & 0xF0) == 0xE0) width += 2; // 3-byte UTF-8 start (CJK chars)
            else if ((c & 0xF8) == 0xF0) width += 2; // 4-byte UTF-8 start
        }
        return width;
    }
    return static_cast<int>(content.length());
}

// TableRow implementation
TableRow::TableRow(const std::vector<std::string>& data) {
    for (const auto& str : data) {
        cells.emplace_back(str);
    }
}

TableCell& TableRow::operator[](size_t index) {
    if (index >= cells.size()) {
        cells.resize(index + 1);
    }
    return cells[index];
}

const TableCell& TableRow::operator[](size_t index) const {
    static const TableCell empty;
    return index < cells.size() ? cells[index] : empty;
}

void TableRow::addCell(const TableCell& cell) {
    cells.push_back(cell);
}

void TableRow::addCell(const std::string& content, const CellFormat& format) {
    cells.emplace_back(content, format);
}

void TableRow::applyFormatToAll(const CellFormat& format) {
    for (auto& cell : cells) {
        cell.setFormat(format);
    }
}

int TableRow::getMaxHeight(const std::vector<int>& columnWidths) const {
    int maxHeight = 1;
    for (size_t i = 0; i < cells.size() && i < columnWidths.size(); ++i) {
        auto lines = cells[i].getWrappedLines(columnWidths[i] - cells[i].getFormat().paddingLeft - cells[i].getFormat().paddingRight);
        maxHeight = std::max(maxHeight, static_cast<int>(lines.size()));
    }
    return maxHeight;
}

// ProfessionalTable implementation
ProfessionalTable::ProfessionalTable() {
    // Set default global formatting
    globalFormat.alignment = FontAlign::LEFT;
    globalFormat.paddingLeft = 1;
    globalFormat.paddingRight = 1;
}

void ProfessionalTable::addRow(const std::vector<std::string>& data) {
    rows.emplace_back(data);
}

void ProfessionalTable::addRow(const std::vector<TableCell>& cells) {
    rows.emplace_back(cells);
}

void ProfessionalTable::addRow(const TableRow& row) {
    rows.push_back(row);
}

TableRow& ProfessionalTable::operator[](size_t index) {
    if (index >= rows.size()) {
        rows.resize(index + 1);
    }
    return rows[index];
}

const TableRow& ProfessionalTable::operator[](size_t index) const {
    static const TableRow empty;
    return index < rows.size() ? rows[index] : empty;
}

void ProfessionalTable::setColumnFormat(size_t columnIndex, const CellFormat& format) {
    if (columnIndex >= columnFormats.size()) {
        columnFormats.resize(columnIndex + 1);
    }
    columnFormats[columnIndex] = format;
}

CellFormat ProfessionalTable::getColumnFormat(size_t columnIndex) const {
    return columnIndex < columnFormats.size() ? columnFormats[columnIndex] : globalFormat;
}

ProfessionalTable& ProfessionalTable::setBorderStyle(BorderStyle style) {
    borderStyle = style;
    return *this;
}

ProfessionalTable& ProfessionalTable::setGlobalFormat(const CellFormat& format) {
    globalFormat = format;
    return *this;
}

ProfessionalTable& ProfessionalTable::setTitle(const std::string& titleText) {
    title = titleText;
    return *this;
}

ProfessionalTable& ProfessionalTable::setSubtitle(const std::string& subtitleText) {
    subtitle = subtitleText;
    return *this;
}

ProfessionalTable& ProfessionalTable::setShowHeaders(bool show) {
    showHeadersFlag = show;
    return *this;
}

ProfessionalTable& ProfessionalTable::setShowBorders(bool show) {
    showBordersFlag = show;
    return *this;
}

ProfessionalTable& ProfessionalTable::enableZebra(Color color1, Color color2) {
    zebra = true;
    zebraColor1 = color1;
    zebraColor2 = color2;
    return *this;
}

// Professional styling presets
ProfessionalTable& ProfessionalTable::applyHinduCalendarStyle() {
    setBorderStyle(BorderStyle::UNICODE_LIGHT);
    setTitle("🕉️  HINDU CALENDAR SEARCH RESULTS - PROFESSIONAL VIEW  🕉️");

    // Set specific column widths and alignments
    if (columnFormats.size() < 12) columnFormats.resize(12);

    columnFormats[0].setWidth(13).align(FontAlign::CENTER); // Date
    columnFormats[1].setWidth(10).align(FontAlign::LEFT);   // Weekday
    columnFormats[2].setWidth(12).align(FontAlign::LEFT);   // Tithi
    columnFormats[3].setWidth(12).align(FontAlign::LEFT);   // Nakshatra
    columnFormats[4].setWidth(10).align(FontAlign::LEFT);   // Yoga
    columnFormats[5].setWidth(10).align(FontAlign::LEFT);   // Karana
    columnFormats[6].setWidth(12).align(FontAlign::LEFT);   // H.Month
    columnFormats[7].setWidth(8).align(FontAlign::LEFT);    // Paksha
    columnFormats[8].setWidth(6).align(FontAlign::CENTER);  // Year
    columnFormats[9].setWidth(5).align(FontAlign::CENTER);  // Muh
    columnFormats[10].setWidth(18).align(FontAlign::LEFT);  // Events
    columnFormats[11].setWidth(6).align(FontAlign::CENTER); // Score

    // Header formatting
    if (!rows.empty()) {
        rows[0].applyFormatToAll(CellFormat().style({FontStyle::BOLD}).align(FontAlign::CENTER).color(Color::YELLOW));
    }

    enableZebra(Color::NONE, Color::BLUE);
    return *this;
}

ProfessionalTable& ProfessionalTable::applyMyanmarCalendarStyle() {
    setBorderStyle(BorderStyle::UNICODE_LIGHT);
    setTitle("🇲🇲  MYANMAR CALENDAR SEARCH RESULTS - PROFESSIONAL VIEW  🇲🇲");

    // Set specific column widths and alignments
    if (columnFormats.size() < 12) columnFormats.resize(12);

    columnFormats[0].setWidth(13).align(FontAlign::CENTER); // Date
    columnFormats[1].setWidth(12).align(FontAlign::LEFT);   // Weekday
    columnFormats[2].setWidth(6).align(FontAlign::CENTER);  // MY.YR
    columnFormats[3].setWidth(13).align(FontAlign::LEFT);   // Month
    columnFormats[4].setWidth(5).align(FontAlign::CENTER);  // Day
    columnFormats[5].setWidth(11).align(FontAlign::LEFT);   // Moon Phase
    columnFormats[6].setWidth(12).align(FontAlign::LEFT);   // Mahabote
    columnFormats[7].setWidth(9).align(FontAlign::LEFT);    // Nakhat
    columnFormats[8].setWidth(8).align(FontAlign::CENTER);  // Nagahle
    columnFormats[9].setWidth(10).align(FontAlign::LEFT);   // Religious
    columnFormats[10].setWidth(16).align(FontAlign::LEFT);  // Astro Days
    columnFormats[11].setWidth(6).align(FontAlign::CENTER); // Score

    // Header formatting
    if (!rows.empty()) {
        rows[0].applyFormatToAll(CellFormat().style({FontStyle::BOLD}).align(FontAlign::CENTER).color(Color::YELLOW));
    }

    enableZebra(Color::NONE, Color::BLUE);
    return *this;
}

ProfessionalTable& ProfessionalTable::applyEphemerisStyle() {
    setBorderStyle(BorderStyle::ASCII);
    setTitle("EPHEMERIS TABLE");

    // Compact style for ephemeris data
    globalFormat.paddingLeft = 0;
    globalFormat.paddingRight = 1;

    return *this;
}

ProfessionalTable& ProfessionalTable::applyMinimalStyle() {
    setBorderStyle(BorderStyle::MINIMAL);
    setShowBorders(false);
    globalFormat.paddingLeft = 2;
    globalFormat.paddingRight = 2;
    return *this;
}

ProfessionalTable& ProfessionalTable::applyClassicStyle() {
    setBorderStyle(BorderStyle::ASCII);
    globalFormat.paddingLeft = 1;
    globalFormat.paddingRight = 1;
    return *this;
}

// Main output generation
std::string ProfessionalTable::toString() const {
    if (rows.empty()) {
        return "";
    }

    std::ostringstream output;
    auto widths = calculateColumnWidths();

    // Add title section if present
    if (!title.empty() || !subtitle.empty()) {
        output << formatHeaderSection();
    }

    // Add table border and headers
    if (showBordersFlag) {
        output << formatRowSeparator(widths, true);
    }

    // Add header row if enabled
    if (showHeadersFlag && !rows.empty()) {
        output << formatDataRow(rows[0], widths, 0);
        if (showBordersFlag) {
            output << formatRowSeparator(widths);
        }
    }

    // Add data rows
    size_t startRow = (showHeadersFlag && !rows.empty()) ? 1 : 0;
    for (size_t i = startRow; i < rows.size(); ++i) {
        output << formatDataRow(rows[i], widths, i);

        // Add separator every 5 rows for better readability
        if (showBordersFlag && (i - startRow + 1) % 5 == 0 && i != rows.size() - 1) {
            output << formatRowSeparator(widths);
        }
    }

    // Add final border
    if (showBordersFlag) {
        output << formatRowSeparator(widths, false, true);
    }

    return output.str();
}

std::string ProfessionalTable::toMarkdown() const {
    if (rows.empty()) return "";

    std::ostringstream output;
    auto widths = calculateColumnWidths();

    // Add header row
    if (showHeadersFlag && !rows.empty()) {
        output << "|";
        for (size_t i = 0; i < rows[0].size(); ++i) {
            output << " " << rows[0][i].getText() << " |";
        }
        output << "\n|";

        // Add separator row with alignment indicators
        for (size_t i = 0; i < rows[0].size(); ++i) {
            auto format = i < columnFormats.size() ? columnFormats[i] : globalFormat;
            switch (format.alignment) {
                case FontAlign::RIGHT:
                    output << "---:|";
                    break;
                case FontAlign::CENTER:
                    output << ":-:|";
                    break;
                default:
                    output << "----|";
                    break;
            }
        }
        output << "\n";
    }

    // Add data rows
    size_t startRow = (showHeadersFlag && !rows.empty()) ? 1 : 0;
    for (size_t i = startRow; i < rows.size(); ++i) {
        output << "|";
        for (size_t j = 0; j < rows[i].size(); ++j) {
            output << " " << rows[i][j].getText() << " |";
        }
        output << "\n";
    }

    return output.str();
}

std::string ProfessionalTable::toCSV() const {
    std::ostringstream output;

    for (const auto& row : rows) {
        for (size_t i = 0; i < row.size(); ++i) {
            if (i > 0) output << ",";

            std::string content = row[i].getText();
            // Escape quotes and wrap in quotes if necessary
            if (content.find(',') != std::string::npos ||
                content.find('"') != std::string::npos ||
                content.find('\n') != std::string::npos) {
                // Replace quotes with double quotes
                size_t pos = 0;
                while ((pos = content.find('"', pos)) != std::string::npos) {
                    content.insert(pos, "\"");
                    pos += 2;
                }
                content = "\"" + content + "\"";
            }
            output << content;
        }
        output << "\n";
    }

    return output.str();
}

std::string ProfessionalTable::toJSON() const {
    std::ostringstream output;
    output << "{\n  \"table\": {\n";

    if (!title.empty()) {
        output << "    \"title\": \"" << title << "\",\n";
    }

    output << "    \"rows\": [\n";

    for (size_t i = 0; i < rows.size(); ++i) {
        output << "      [";
        for (size_t j = 0; j < rows[i].size(); ++j) {
            if (j > 0) output << ", ";
            output << "\"" << rows[i][j].getText() << "\"";
        }
        output << "]";
        if (i < rows.size() - 1) output << ",";
        output << "\n";
    }

    output << "    ]\n  }\n}";
    return output.str();
}

std::string ProfessionalTable::toHTML() const {
    std::ostringstream output;
    output << "<table class=\"professional-table\">\n";

    if (!title.empty()) {
        output << "  <caption>" << title << "</caption>\n";
    }

    // Header
    if (showHeadersFlag && !rows.empty()) {
        output << "  <thead>\n    <tr>\n";
        for (const auto& cell : rows[0]) {
            output << "      <th>" << cell.getText() << "</th>\n";
        }
        output << "    </tr>\n  </thead>\n";
    }

    // Body
    output << "  <tbody>\n";
    size_t startRow = (showHeadersFlag && !rows.empty()) ? 1 : 0;
    for (size_t i = startRow; i < rows.size(); ++i) {
        output << "    <tr>\n";
        for (const auto& cell : rows[i]) {
            output << "      <td>" << cell.getText() << "</td>\n";
        }
        output << "    </tr>\n";
    }
    output << "  </tbody>\n</table>\n";

    return output.str();
}

// Private helper methods
std::vector<int> ProfessionalTable::calculateColumnWidths() const {
    std::vector<int> widths;

    // Find maximum number of columns
    size_t maxCols = 0;
    for (const auto& row : rows) {
        maxCols = std::max(maxCols, row.size());
    }

    widths.resize(maxCols, 0);

    // Calculate based on content and formats
    for (size_t colIdx = 0; colIdx < maxCols; ++colIdx) {
        int maxWidth = 0;

        // Check format width first
        if (colIdx < columnFormats.size() && columnFormats[colIdx].width > 0) {
            maxWidth = columnFormats[colIdx].width;
        } else {
            // Calculate from content
            for (const auto& row : rows) {
                if (colIdx < row.size()) {
                    const auto& cell = row[colIdx];
                    int contentWidth = cell.getDisplayWidth() +
                                     cell.getFormat().paddingLeft +
                                     cell.getFormat().paddingRight;
                    maxWidth = std::max(maxWidth, contentWidth);
                }
            }

            // Apply minimum width
            maxWidth = std::max(maxWidth, 8); // Minimum column width
        }

        widths[colIdx] = maxWidth;
    }

    return widths;
}

std::string ProfessionalTable::formatHeaderSection() const {
    if (title.empty() && subtitle.empty()) return "";

    std::ostringstream output;
    auto widths = calculateColumnWidths();

    // Calculate total width
    int totalWidth = 0;
    for (int w : widths) totalWidth += w;
    totalWidth += static_cast<int>(widths.size()) + 1; // Account for separators

    const auto& chars = getBorderChars();

    // Top border
    output << chars.corner_tl << std::string(totalWidth - 2, chars.horizontal) << chars.corner_tr << "\n";

    // Title
    if (!title.empty()) {
        std::string paddedTitle = alignText(title, totalWidth - 4, FontAlign::CENTER);
        output << chars.vertical << " " << paddedTitle << " " << chars.vertical << "\n";
    }

    // Subtitle
    if (!subtitle.empty()) {
        std::string paddedSubtitle = alignText(subtitle, totalWidth - 4, FontAlign::CENTER);
        output << chars.vertical << " " << paddedSubtitle << " " << chars.vertical << "\n";
    }

    // Bottom border of header
    output << chars.corner_bl << std::string(totalWidth - 2, chars.horizontal) << chars.corner_br << "\n";

    return output.str();
}

std::string ProfessionalTable::formatRowSeparator(const std::vector<int>& widths, bool isTop, bool isBottom) const {
    if (!showBordersFlag) return "";

    std::ostringstream output;
    const auto& chars = getBorderChars();

    // Choose appropriate corner/junction characters
    char leftChar, rightChar, junctionChar;
    if (isTop) {
        leftChar = chars.corner_tl;
        rightChar = chars.corner_tr;
        junctionChar = chars.t_top;
    } else if (isBottom) {
        leftChar = chars.corner_bl;
        rightChar = chars.corner_br;
        junctionChar = chars.t_bottom;
    } else {
        leftChar = chars.t_left;
        rightChar = chars.t_right;
        junctionChar = chars.cross;
    }

    output << leftChar;
    for (size_t i = 0; i < widths.size(); ++i) {
        output << std::string(widths[i], chars.horizontal);
        if (i < widths.size() - 1) {
            output << junctionChar;
        }
    }
    output << rightChar << "\n";

    return output.str();
}

std::string ProfessionalTable::formatDataRow(const TableRow& row, const std::vector<int>& widths, size_t rowIndex) const {
    std::ostringstream output;
    const auto& chars = getBorderChars();

    // Handle multi-line cells
    int rowHeight = row.getMaxHeight(widths);

    for (int lineIdx = 0; lineIdx < rowHeight; ++lineIdx) {
        if (showBordersFlag) {
            output << chars.vertical;
        }

        for (size_t colIdx = 0; colIdx < widths.size(); ++colIdx) {
            std::string cellContent;

            if (colIdx < row.size()) {
                const auto& cell = row[colIdx];
                auto lines = cell.getWrappedLines(widths[colIdx] - cell.getFormat().paddingLeft - cell.getFormat().paddingRight);

                if (lineIdx < static_cast<int>(lines.size())) {
                    cellContent = lines[lineIdx];
                }
            }

            // Apply column formatting if available
            CellFormat cellFormat = (colIdx < row.size()) ? row[colIdx].getFormat() : globalFormat;
            if (colIdx < columnFormats.size()) {
                // Merge column format with cell format
                if (cellFormat.alignment == FontAlign::LEFT && columnFormats[colIdx].alignment != FontAlign::LEFT) {
                    cellFormat.alignment = columnFormats[colIdx].alignment;
                }
            }

            // Apply zebra coloring
            if (zebra && rowIndex > 0) {
                if (rowIndex % 2 == 0 && zebraColor1 != Color::NONE) {
                    cellFormat.backgroundColor = zebraColor1;
                } else if (rowIndex % 2 == 1 && zebraColor2 != Color::NONE) {
                    cellFormat.backgroundColor = zebraColor2;
                }
            }

            output << formatCell(TableCell(cellContent, cellFormat), widths[colIdx], colIdx == widths.size() - 1);

            if (showBordersFlag && colIdx < widths.size() - 1) {
                output << chars.vertical;
            }
        }

        if (showBordersFlag) {
            output << chars.vertical;
        }
        output << "\n";
    }

    return output.str();
}

std::string ProfessionalTable::formatCell(const TableCell& cell, int width, bool isLast) const {
    const auto& format = cell.getFormat();
    std::string content = cell.getText();

    // Apply padding
    int contentWidth = width - format.paddingLeft - format.paddingRight;

    // Align text
    std::string aligned = alignText(content, contentWidth, format.alignment);

    // Add padding
    std::string padded = std::string(format.paddingLeft, ' ') + aligned + std::string(format.paddingRight, ' ');

    // Apply color and styling
    return applyColorAndStyle(padded, format);
}

std::string ProfessionalTable::alignText(const std::string& text, int width, FontAlign alignment) const {
    if (static_cast<int>(text.length()) >= width) {
        return text.substr(0, width);
    }

    int padding = width - static_cast<int>(text.length());

    switch (alignment) {
        case FontAlign::RIGHT:
            return std::string(padding, ' ') + text;
        case FontAlign::CENTER: {
            int leftPad = padding / 2;
            int rightPad = padding - leftPad;
            return std::string(leftPad, ' ') + text + std::string(rightPad, ' ');
        }
        default: // LEFT
            return text + std::string(padding, ' ');
    }
}

std::string ProfessionalTable::applyColorAndStyle(const std::string& text, const CellFormat& format) const {
    std::string result = text;

    // Apply font styles
    for (auto style : format.fontStyles) {
        result = getStyleCode(style) + result + getStyleCode(style, true);
    }

    // Apply colors
    if (format.fontColor != Color::NONE) {
        result = getColorCode(format.fontColor) + result + "\033[39m"; // Reset foreground
    }

    if (format.backgroundColor != Color::NONE) {
        result = getColorCode(format.backgroundColor, true) + result + "\033[49m"; // Reset background
    }

    return result;
}

std::string ProfessionalTable::getColorCode(Color color, bool background) const {
    int base = background ? 40 : 30;
    int bright = background ? 100 : 90;

    switch (color) {
        case Color::RED: return "\033[" + std::to_string(base + 1) + "m";
        case Color::GREEN: return "\033[" + std::to_string(base + 2) + "m";
        case Color::YELLOW: return "\033[" + std::to_string(base + 3) + "m";
        case Color::BLUE: return "\033[" + std::to_string(base + 4) + "m";
        case Color::MAGENTA: return "\033[" + std::to_string(base + 5) + "m";
        case Color::CYAN: return "\033[" + std::to_string(base + 6) + "m";
        case Color::WHITE: return "\033[" + std::to_string(base + 7) + "m";
        case Color::BRIGHT_RED: return "\033[" + std::to_string(bright + 1) + "m";
        case Color::BRIGHT_GREEN: return "\033[" + std::to_string(bright + 2) + "m";
        case Color::BRIGHT_YELLOW: return "\033[" + std::to_string(bright + 3) + "m";
        case Color::BRIGHT_BLUE: return "\033[" + std::to_string(bright + 4) + "m";
        case Color::BRIGHT_MAGENTA: return "\033[" + std::to_string(bright + 5) + "m";
        case Color::BRIGHT_CYAN: return "\033[" + std::to_string(bright + 6) + "m";
        case Color::BRIGHT_WHITE: return "\033[" + std::to_string(bright + 7) + "m";
        default: return "";
    }
}

std::string ProfessionalTable::getStyleCode(FontStyle style, bool end) const {
    switch (style) {
        case FontStyle::BOLD: return end ? "\033[22m" : "\033[1m";
        case FontStyle::ITALIC: return end ? "\033[23m" : "\033[3m";
        case FontStyle::UNDERLINE: return end ? "\033[24m" : "\033[4m";
        case FontStyle::DIM: return end ? "\033[22m" : "\033[2m";
        default: return "";
    }
}

const ProfessionalTable::BorderChars& ProfessionalTable::getBorderChars() const {
    return borderChars.at(borderStyle);
}

// Stream operator
std::ostream& operator<<(std::ostream& os, const ProfessionalTable& table) {
    os << table.toString();
    return os;
}

// Factory functions
ProfessionalTable createHinduCalendarTable() {
    ProfessionalTable table;
    table.applyHinduCalendarStyle();

    // Add header row
    table.addRow({"DATE", "WEEKDAY", "TITHI", "NAKSHATRA", "YOGA", "KARANA",
                  "H.MONTH", "PAKSHA", "YEAR", "MUH", "SPECIAL EVENTS", "SCORE"});

    return table;
}

ProfessionalTable createMyanmarCalendarTable() {
    ProfessionalTable table;
    table.applyMyanmarCalendarStyle();

    // Add header row
    table.addRow({"DATE", "WEEKDAY", "MY.YR", "MONTH", "DAY", "MOON.PHASE",
                  "MAHABOTE", "NAKHAT", "NAGAHLE", "RELIGIOUS", "ASTRO.DAYS", "SCORE"});

    return table;
}

ProfessionalTable createEphemerisTable() {
    ProfessionalTable table;
    table.applyEphemerisStyle();
    return table;
}

ProfessionalTable createSearchResultsTable() {
    ProfessionalTable table;
    table.applyClassicStyle();
    return table;
}

// Utility functions
void addHinduCalendarRow(ProfessionalTable& table,
                        const std::string& date,
                        const std::string& weekday,
                        const std::string& tithi,
                        const std::string& nakshatra,
                        const std::string& yoga,
                        const std::string& karana,
                        const std::string& month,
                        const std::string& paksha,
                        const std::string& year,
                        const std::string& muhurta,
                        const std::string& events,
                        const std::string& score) {
    std::vector<std::string> rowData = {date, weekday, tithi, nakshatra, yoga, karana,
                                       month, paksha, year, muhurta, events, score};
    table.addRow(rowData);
}

void addMyanmarCalendarRow(ProfessionalTable& table,
                          const std::string& date,
                          const std::string& weekday,
                          const std::string& myYear,
                          const std::string& month,
                          const std::string& day,
                          const std::string& moonPhase,
                          const std::string& mahabote,
                          const std::string& nakhat,
                          const std::string& nagahle,
                          const std::string& religious,
                          const std::string& astroDays,
                          const std::string& score) {
    std::vector<std::string> rowData = {date, weekday, myYear, month, day, moonPhase,
                                       mahabote, nakhat, nagahle, religious, astroDays, score};
    table.addRow(rowData);
}

// Eclipse Events Table Functions
void addEclipseEventRow(ProfessionalTable& table,
                       const std::string& date,
                       const std::string& type,
                       const std::string& time,
                       const std::string& magnitude,
                       const std::string& duration,
                       const std::string& visibility,
                       const std::string& saros,
                       const std::string& pathWidth,
                       const std::string& centralLine) {
    std::vector<std::string> rowData = {date, type, time, magnitude, duration,
                                       visibility, saros, pathWidth, centralLine};
    table.addRow(rowData);
}

// Conjunction Events Table Functions
void addConjunctionEventRow(ProfessionalTable& table,
                           const std::string& date,
                           const std::string& time,
                           const std::string& planet1,
                           const std::string& planet2,
                           const std::string& separation,
                           const std::string& orb,
                           const std::string& sign,
                           const std::string& strength,
                           const std::string& significance) {
    std::vector<std::string> rowData = {date, time, planet1, planet2, separation,
                                       orb, sign, strength, significance};
    table.addRow(rowData);
}

// KP System Table Functions
void addKPSystemRow(ProfessionalTable& table,
                   const std::string& planet,
                   const std::string& longitude,
                   const std::string& sign,
                   const std::string& nakshatra,
                   const std::string& subLord,
                   const std::string& subSub,
                   const std::string& subSubSub,
                   const std::string& kpNotation,
                   const std::string& signification) {
    std::vector<std::string> rowData = {planet, longitude, sign, nakshatra, subLord,
                                       subSub, subSubSub, kpNotation, signification};
    table.addRow(rowData);
}

void addGrahaYuddhaRow(ProfessionalTable& table,
                      const std::string& date,
                      const std::string& time,
                      const std::string& combatants,
                      const std::string& separation,
                      const std::string& winner,
                      const std::string& loser,
                      const std::string& victoryMargin,
                      const std::string& effects,
                      const std::string& significance) {
    std::vector<std::string> rowData = {date, time, combatants, separation, winner,
                                       loser, victoryMargin, effects, significance};
    table.addRow(rowData);
}

// Factory functions for new table types
ProfessionalTable createEclipseTable() {
    ProfessionalTable table;
    table.setBorderStyle(BorderStyle::ASCII)
         .setGlobalFormat(CellFormat().align(FontAlign::LEFT))
         .setTitle("Eclipse Events Analysis");

    // Add headers
    std::vector<std::string> headers = {
        "DATE", "TYPE", "TIME (UTC)", "MAGNITUDE", "DURATION",
        "VISIBILITY", "SAROS", "PATH WIDTH", "CENTRAL LINE"
    };
    table.addRow(headers);

    return table;
}

ProfessionalTable createConjunctionTable() {
    ProfessionalTable table;
    table.setBorderStyle(BorderStyle::ASCII)
         .setGlobalFormat(CellFormat().align(FontAlign::LEFT))
         .setTitle("Planetary Conjunctions Analysis");

    // Add headers
    std::vector<std::string> headers = {
        "DATE", "TIME (UTC)", "PLANET 1", "PLANET 2", "SEPARATION",
        "ORB", "SIGN", "STRENGTH", "SIGNIFICANCE"
    };
    table.addRow(headers);

    return table;
}

ProfessionalTable createKPSystemTable() {
    ProfessionalTable table;
    table.setBorderStyle(BorderStyle::ASCII)
         .setGlobalFormat(CellFormat().align(FontAlign::LEFT))
         .setTitle("KP System - Sub Lord Analysis");

    // Add headers
    std::vector<std::string> headers = {
        "PLANET", "LONGITUDE", "SIGN", "NAKSHATRA", "SUB LORD",
        "SUB-SUB", "SUB³", "KP NOTATION", "SIGNIFICATION"
    };
    table.addRow(headers);

    return table;
}

ProfessionalTable createGrahaYuddhaTable() {
    ProfessionalTable table;
    table.setBorderStyle(BorderStyle::ASCII)
         .setGlobalFormat(CellFormat().align(FontAlign::LEFT))
         .setTitle("Graha Yuddha - Planetary Wars Analysis");

    // Add headers
    std::vector<std::string> headers = {
        "DATE", "TIME (UTC)", "COMBATANTS", "SEPARATION", "WINNER",
        "LOSER", "VICTORY MARGIN", "EFFECTS", "SIGNIFICANCE"
    };
    table.addRow(headers);

    return table;
}

} // namespace Astro
