#pragma once

#include <memory>
#include <string>
#include <vector>

#include "cb_handlers.h"
#include "x11_helpers.hpp"

class CXSelection final
{
public:
    explicit CXSelection(Display *display, CCbHandler *handler, std::string&& data);

    void run();

private:
    static constexpr const char *s_cbName = "CLIPBOARD";
    static constexpr const char *s_targetsName = "TARGETS";

    Display *m_dspl;
    Window m_wnd;
    Atom m_cb;
    Atom m_targets;
    CCbHandler *m_handler;
    std::string m_data;

    Display *getDisplay() const { return m_dspl; }
    void handleTargetsRequest(const XSelectionRequestEvent &event);
    void handleConversionRequest(const XSelectionRequestEvent &event);
    void handleSelectionRequest(const XSelectionRequestEvent &event);
    void replyEvent(const XSelectionRequestEvent &event, const unsigned char *data, const int size);
};
