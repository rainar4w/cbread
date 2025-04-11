#include <algorithm>

#include <stdio.h>
#include <string.h>

#include "cb_handlers.h"

bool CCbHandler::isSupported(const Atom conversion) const
{
    const auto &types = getSupported();
    return std::find_if(types.begin(), types.end(), [conversion](const auto &t)
                        { return t == conversion; }) != types.end();
}

const CCbHandler::ListType &CTextHandler::getSupported() const
{
    static const ListType types = {
        XInternAtom(getDisplay(), "TEXT", false),
        XInternAtom(getDisplay(), "STRING", false),
        XInternAtom(getDisplay(), "UTF8_STRING", false),
        XInternAtom(getDisplay(), "text/plain", false),
        XInternAtom(getDisplay(), "text/plain;charset=utf-8", false),
    };

    return types;
}

const CCbHandler::ListType &CFileHandler::getSupported() const
{
    static const ListType types = {
        XInternAtom(getDisplay(), "text/uri-list", false),
    };

    return types;
}
