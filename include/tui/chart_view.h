#pragma once

#define Uses_TWindow
#define Uses_TScroller
#define Uses_TScrollBar
#define Uses_TRect
#define Uses_TDrawBuffer
#define Uses_TEvent
#define Uses_TColorAttr
#define Uses_TPalette
#include <tvision/tv.h>

#include <string>
#include <vector>

namespace AstroTui {

class TTextScroller : public TScroller {
public:
    std::vector<std::string> lines;

    TTextScroller(const TRect& bounds,
                  TScrollBar* aHScrollBar,
                  TScrollBar* aVScrollBar,
                  const std::string& text);

    virtual void draw() override;
    void setText(const std::string& text);
};

class TTextWindow : public TWindow {
public:
    TTextScroller* scroller;

    TTextWindow(const TRect& bounds, const char* aTitle, const std::string& text);
    void setText(const std::string& text);
};

} // namespace AstroTui
