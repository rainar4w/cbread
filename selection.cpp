#include <stdio.h>

#include "selection.h"

namespace
{
    [[maybe_unused]] void printEvent(Display *display, const XSelectionRequestEvent &event)
    {
        switch (event.type)
        {
        case SelectionClear:
            printf("type: clear\n");
            break;
        case SelectionNotify:
            printf("type: notify\n");
            break;
        case SelectionRequest:
            printf("type: request\n");
            break;
        default:
            printf("type: unknown type %i\n", event.type);
            break;
        }

        printf("owner: 0x%lX\n", event.owner);
        printf("requestor: 0x%lX\n", event.requestor);
        printf("selection: '%s'\n", CXAtom(display, event.selection).name());
        printf("target: '%s'\n", CXAtom(display, event.target).name());
        printf("property: '%s'\n", CXAtom(display, event.property).name());
        printf("\n");
    }

    void initNotifyEvent(const XSelectionRequestEvent &src, XSelectionEvent &dst)
    {
        dst.type = SelectionNotify;
        dst.requestor = src.requestor;
        dst.selection = src.selection;
        dst.target = src.target;
        dst.property = src.property;
        dst.time = src.time;
    }
} // namespace

CXSelection::CXSelection(Display *display, CCbHandler *handler, std::string&& data)
    : m_dspl(display), m_handler(handler), m_data(std::move(data))
{
    m_cb = XInternAtom(getDisplay(), s_cbName, false);
    m_targets = XInternAtom(getDisplay(), s_targetsName, false);

    int screen = DefaultScreen(getDisplay());
    Window parent = RootWindow(getDisplay(), screen);
    m_wnd = XCreateSimpleWindow(getDisplay(), parent, -1, -1, 1, 1, 0, 0, 0);

    XSetSelectionOwner(getDisplay(), m_cb, m_wnd, CurrentTime);
}

void CXSelection::run()
{
    while (1)
    {
        XEvent ev{};
        XNextEvent(m_dspl, &ev);
        switch (ev.type)
        {
        case SelectionClear:
            printf("lost selection, exiting\n");
            return;
        case SelectionRequest:
            handleSelectionRequest(ev.xselectionrequest);
            break;
        default:
            fprintf(stderr, "unsupported event type %i\n", ev.type);
            break;
        }
    }
}

void CXSelection::handleTargetsRequest(const XSelectionRequestEvent &event)
{
    const CCbHandler::ListType &types = m_handler->getSupported();
    std::vector<Atom> vec(types.begin(), types.end());

    const unsigned char *data = reinterpret_cast<const unsigned char *>(vec.data());
    const int size = static_cast<int>(types.size() * sizeof(CCbHandler::ListType::value_type));
    replyEvent(event, data, size);
}

void CXSelection::handleConversionRequest(const XSelectionRequestEvent &event)
{
    const unsigned char* data = reinterpret_cast<const unsigned char *>(m_data.data());
    const int size = static_cast<int>(m_data.size());
    replyEvent(event, data, size);
}

void CXSelection::handleSelectionRequest(const XSelectionRequestEvent &event)
{
    CXAtom selection(getDisplay(), event.selection);
    CXAtom target(getDisplay(), event.target);
    if (m_cb != event.selection)
    {
        fprintf(stderr, "unexpected selection '%s'\n", selection.name());
        return;
    }

    if (m_targets == event.target)
    {
        handleTargetsRequest(event);
    }
    else if (m_handler->isSupported(event.target))
    {
        handleConversionRequest(event);
    }
    else
    {
        fprintf(stderr, "unexpected target '%s\n", target.name());
        return;
    }
}

void CXSelection::replyEvent(const XSelectionRequestEvent &event, const unsigned char *data, const int size)
{
    XChangeProperty(getDisplay(), event.requestor, event.property, event.target, 8, PropModeReplace, data, size);

    XSelectionEvent sev{};
    initNotifyEvent(event, sev);
    sev.type = SelectionNotify;

    XSendEvent(getDisplay(), sev.requestor, True, NoEventMask, reinterpret_cast<XEvent *>(&sev));
}
