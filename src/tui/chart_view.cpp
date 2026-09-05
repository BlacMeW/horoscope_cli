#include "tui/chart_view.h"
#include <sstream>
#include <algorithm>

namespace AstroTui {

TTextScroller::TTextScroller(const TRect& bounds,
                             TScrollBar* aHScrollBar,
                             TScrollBar* aVScrollBar,
                             const std::string& text) :
    TScroller(bounds, aHScrollBar, aVScrollBar)
{
    growMode = gfGrowHiX | gfGrowHiY;
    options |= ofSelectable;
    setText(text);
}

static std::string stripAnsi(const std::string& str) {
    std::string clean;
    clean.reserve(str.size());
    bool inEscape = false;
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '\033' && i + 1 < str.size() && str[i + 1] == '[') {
            inEscape = true;
            ++i;
        } else if (inEscape) {
            if ((str[i] >= 'A' && str[i] <= 'Z') || (str[i] >= 'a' && str[i] <= 'z')) {
                inEscape = false;
            }
        } else {
            clean += str[i];
        }
    }
    return clean;
}

void TTextScroller::setText(const std::string& text) {
    lines.clear();
    std::stringstream ss(text);
    std::string line;
    int maxLen = 0;
    while (std::getline(ss, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        line = stripAnsi(line);
        maxLen = std::max(maxLen, static_cast<int>(line.length()));
        lines.push_back(line);
    }
    setLimit(std::max(80, maxLen + 2), std::max(static_cast<int>(lines.size()), 1));
    drawView();
}

void TTextScroller::draw() {
    TColorAttr c = getColor(1);
    for (short i = 0; i < size.y; ++i) {
        TDrawBuffer b;
        b.moveChar(0, ' ', c, size.x);
        int lineIdx = delta.y + i;
        if (lineIdx >= 0 && lineIdx < static_cast<int>(lines.size())) {
            const std::string& line = lines[lineIdx];
            b.moveStr(0, line, c, size.x, delta.x);
        }
        writeBuf(0, i, size.x, 1, b);
    }
}

TTextWindow::TTextWindow(const TRect& bounds, const char* aTitle, const std::string& text) :
    TWindow(bounds, aTitle, wnNoNumber),
    TWindowInit(&TTextWindow::initFrame),
    scroller(nullptr)
{
    options |= ofTileable;
    TRect r = getClipRect();
    r.grow(-1, -1);
    TScrollBar* vScroll = standardScrollBar(sbVertical | sbHandleKeyboard);
    TScrollBar* hScroll = standardScrollBar(sbHorizontal | sbHandleKeyboard);
    scroller = new TTextScroller(r, hScroll, vScroll, text);
    insert(scroller);
}

void TTextWindow::setText(const std::string& text) {
    if (scroller) {
        scroller->setText(text);
    }
}

} // namespace AstroTui
