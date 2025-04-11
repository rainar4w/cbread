#pragma once

#include <optional>

#include <X11/Xlib.h>

class CXDisplay
{
public:
    explicit CXDisplay(Display *display) : m_dspl(display) {}
    ~CXDisplay()
    {
        if (nullptr != m_dspl)
            XCloseDisplay(m_dspl);
    }

    CXDisplay(const CXDisplay &) = delete;
    CXDisplay(CXDisplay &&) = delete;
    const CXDisplay &operator=(const CXDisplay &) = delete;
    const CXDisplay &operator=(CXDisplay &&) = delete;

    explicit operator Display *() const { return m_dspl; }
    explicit operator bool() const { return (nullptr != m_dspl); }

private:
    Display *m_dspl;
};

class CXMemory
{
public:
    explicit CXMemory(char *memory) : m_mem(static_cast<void *>(memory)) {}
    virtual ~CXMemory()
    {
        XFree(m_mem);
    }

    CXMemory(const CXMemory &) = delete;
    CXMemory(CXMemory &&) = delete;
    const CXMemory &operator=(const CXMemory &) = delete;
    const CXMemory &operator=(CXMemory &&) = delete;

    explicit operator char *() const { return static_cast<char *>(m_mem); }

private:
    void *m_mem;
};

class CXAtom
{
public:
    explicit CXAtom(Display *display, const Atom &atom)
        : m_dspl(display), m_atom(atom) {}

    explicit operator Atom() const { return m_atom; }

    const char *name()
    {
        // not thread safe
        if (!m_name.has_value())
            m_name.emplace(XGetAtomName(m_dspl, m_atom));

        return static_cast<const char *>(m_name.value());
    }

private:
    Display *m_dspl;
    Atom m_atom;
    std::optional<CXMemory> m_name;
};
