#pragma once

#include <set>

#include <X11/Xlib.h>

#include "x11_helpers.hpp"

class CCbHandler
{
public:
    using ListType = std::set<Atom>;

    explicit CCbHandler(Display *display) : m_dspl(display) {}
    virtual ~CCbHandler() = default;

    virtual const ListType &getSupported() const = 0;

    bool isSupported(const Atom conversion) const;

protected:
    Display *getDisplay() const { return m_dspl; };

private:
    Display *m_dspl;
};

// handles paste request from as text (f.e. from file editors or console)
class CTextHandler : public CCbHandler
{
public:
    explicit CTextHandler(Display *display) : CCbHandler(display) {}
    ~CTextHandler() override = default;

    const ListType &getSupported() const override;
};

// handles paste request from file managers
class CFileHandler : public CCbHandler
{
public:
    explicit CFileHandler(Display *display) : CCbHandler(display) {}
    ~CFileHandler() override = default;

    const ListType &getSupported() const override;
};
