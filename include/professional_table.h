#pragma once

#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include <map>
#include <functional>

namespace Astro {

// Forward declarations
enum class FontAlign { LEFT, CENTER, RIGHT };
enum class FontStyle { NORMAL, BOLD, ITALIC, UNDERLINE, DIM };
enum class BorderStyle { ASCII, UNICODE_LIGHT, UNICODE_HEAVY, UNICODE_DOUBLE, MINIMAL };

// Color support for terminal output
enum class Color {
    NONE,
    RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE,
    BRIGHT_RED, BRIGHT_GREEN, BRIGHT_YELLOW, BRIGHT_BLUE,
    BRIGHT_MAGENTA, BRIGHT_CYAN, BRIGHT_WHITE
};

// Cell formatting configuration
struct CellFormat {
    FontAlign alignment = FontAlign::LEFT;
    std::vector<FontStyle> fontStyles;
    Color fontColor = Color::NONE;
    Color backgroundColor = Color::NONE;
    int width = 0;  // 0 = auto-width
    int paddingLeft = 1;
    int paddingRight = 1;
    bool wordWrap = true;
    bool multiByteSupport = false;

    CellFormat() = default;
    CellFormat& align(FontAlign align) { alignment = align; return *this; }
    CellFormat& style(const std::vector<FontStyle>& styles) { fontStyles = styles; return *this; }
    CellFormat& color(Color c) { fontColor = c; return *this; }
    CellFormat& bgColor(Color c) { backgroundColor = c; return *this; }
    CellFormat& setWidth(int w) { width = w; return *this; }
    CellFormat& padding(int left, int right) { paddingLeft = left; paddingRight = right; return *this; }
    CellFormat& enableWordWrap(bool enable = true) { wordWrap = enable; return *this; }
    CellFormat& enableMultiByte(bool enable = true) { multiByteSupport = enable; return *this; }
};

// Table cell
class TableCell {
private:
    std::string content;
    CellFormat format;

public:
    TableCell() = default;
    TableCell(const std::string& text) : content(text) {}
    TableCell(const std::string& text, const CellFormat& fmt) : content(text), format(fmt) {}

    // Content access
    const std::string& getText() const { return content; }
    void setText(const std::string& text) { content = text; }

    // Format access
    const CellFormat& getFormat() const { return format; }
    CellFormat& getFormat() { return format; }
    void setFormat(const CellFormat& fmt) { format = fmt; }

    // Convenience formatting methods (fluent interface)
    TableCell& align(FontAlign alignment) { format.align(alignment); return *this; }
    TableCell& style(const std::vector<FontStyle>& styles) { format.style(styles); return *this; }
    TableCell& color(Color c) { format.color(c); return *this; }
    TableCell& bgColor(Color c) { format.bgColor(c); return *this; }
    TableCell& setWidth(int w) { format.setWidth(w); return *this; }
    TableCell& padding(int left, int right) { format.padding(left, right); return *this; }

    // Utility methods
    std::vector<std::string> getWrappedLines(int maxWidth) const;
    int getDisplayWidth() const;
};

// Table row
class TableRow {
private:
    std::vector<TableCell> cells;
    CellFormat defaultFormat;

public:
    TableRow() = default;
    TableRow(const std::vector<std::string>& data);
    TableRow(const std::vector<TableCell>& cellData) : cells(cellData) {}

    // Cell access
    TableCell& operator[](size_t index);
    const TableCell& operator[](size_t index) const;
    void addCell(const TableCell& cell);
    void addCell(const std::string& content, const CellFormat& format = CellFormat());
    size_t size() const { return cells.size(); }

    // Row formatting (applies to all cells in row)
    CellFormat& getDefaultFormat() { return defaultFormat; }
    void applyFormatToAll(const CellFormat& format);

    // Iterator support
    std::vector<TableCell>::iterator begin() { return cells.begin(); }
    std::vector<TableCell>::iterator end() { return cells.end(); }
    std::vector<TableCell>::const_iterator begin() const { return cells.begin(); }
    std::vector<TableCell>::const_iterator end() const { return cells.end(); }

    // Get maximum height (for multi-line cells)
    int getMaxHeight(const std::vector<int>& columnWidths) const;
};

// Professional table class inspired by p-ranav/tabulate
class ProfessionalTable {
private:
    std::vector<TableRow> rows;
    BorderStyle borderStyle = BorderStyle::ASCII;
    CellFormat globalFormat;
    std::vector<CellFormat> columnFormats;
    std::string title;
    std::string subtitle;
    bool showHeadersFlag = true;
    bool showBordersFlag = true;
    bool zebra = false;  // Alternating row colors
    Color zebraColor1 = Color::NONE;
    Color zebraColor2 = Color::NONE;

    // Border characters for different styles
    struct BorderChars {
        char horizontal, vertical, corner_tl, corner_tr, corner_bl, corner_br;
        char cross, t_top, t_bottom, t_left, t_right;
    };

    static const std::map<BorderStyle, BorderChars> borderChars;

public:
    ProfessionalTable();
    ~ProfessionalTable() = default;

    // Row management
    void addRow(const std::vector<std::string>& data);
    void addRow(const std::vector<TableCell>& cells);
    void addRow(const TableRow& row);
    TableRow& operator[](size_t index);
    const TableRow& operator[](size_t index) const;
    size_t rowCount() const { return rows.size(); }

    // Column access (returns a pseudo-column for formatting)
    void setColumnFormat(size_t columnIndex, const CellFormat& format);
    CellFormat getColumnFormat(size_t columnIndex) const;

    // Global table formatting
    ProfessionalTable& setBorderStyle(BorderStyle style);
    ProfessionalTable& setGlobalFormat(const CellFormat& format);
    ProfessionalTable& setTitle(const std::string& title);
    ProfessionalTable& setSubtitle(const std::string& subtitle);
    ProfessionalTable& setShowHeaders(bool show = true);
    ProfessionalTable& setShowBorders(bool show = true);
    ProfessionalTable& enableZebra(Color color1 = Color::NONE, Color color2 = Color::WHITE);

    // Professional styling presets
    ProfessionalTable& applyHinduCalendarStyle();
    ProfessionalTable& applyMyanmarCalendarStyle();
    ProfessionalTable& applyEphemerisStyle();
    ProfessionalTable& applyMinimalStyle();
    ProfessionalTable& applyClassicStyle();

    // Output generation
    std::string toString() const;
    std::string toMarkdown() const;
    std::string toCSV() const;
    std::string toJSON() const;
    std::string toHTML() const;

    // Utility methods
    void clear() { rows.clear(); }
    bool empty() const { return rows.empty(); }

private:
    // Internal rendering methods
    std::vector<int> calculateColumnWidths() const;
    std::string formatHeaderSection() const;
    std::string formatRowSeparator(const std::vector<int>& widths, bool isTop = false, bool isBottom = false) const;
    std::string formatDataRow(const TableRow& row, const std::vector<int>& widths, size_t rowIndex) const;
    std::string formatCell(const TableCell& cell, int width, bool isLast = false) const;

    // Color and style formatting
    std::string applyColorAndStyle(const std::string& text, const CellFormat& format) const;
    std::string getColorCode(Color color, bool background = false) const;
    std::string getStyleCode(FontStyle style, bool end = false) const;

    // Text processing utilities
    std::string alignText(const std::string& text, int width, FontAlign alignment) const;
    std::string padText(const std::string& text, int width, char padChar = ' ') const;
    int getTextDisplayWidth(const std::string& text, bool multiByteSupport = false) const;
    std::vector<std::string> wrapText(const std::string& text, int width) const;

    // Border character access
    const BorderChars& getBorderChars() const;
};

// Stream operator support
std::ostream& operator<<(std::ostream& os, const ProfessionalTable& table);

// Factory functions for common table types
ProfessionalTable createHinduCalendarTable();
ProfessionalTable createMyanmarCalendarTable();
ProfessionalTable createEphemerisTable();
ProfessionalTable createSearchResultsTable();

// Utility functions for Hindu calendar
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
                        const std::string& score);

// Utility functions for Myanmar calendar
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
                          const std::string& score);

} // namespace Astro
