#include <memory>

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "cb_handlers.h"
#include "selection.h"
#include "x11_helpers.hpp"

namespace
{
    Display *getDisplay()
    {
        static CXDisplay dspl(XOpenDisplay(nullptr));
        return static_cast<Display *>(dspl);
    }

    bool isFile(const char *path)
    {
        struct stat s{};
        if (1 == stat(path, &s))
        {
            if (errno != ENOENT)
                fprintf(stderr, "stat failed, errno %i\n", errno);
            return false;
        }

        return S_ISREG(s.st_mode);
    }

    std::string makeFileUri(const char* filename)
    {
        char path[PATH_MAX] = {};
        if (nullptr == realpath(filename, path))
        {
            fprintf(stderr, "realpath failed, errno %i\n", errno);
            return {};
        }

        return std::string{"file://"} + filename;
    }

    void printUsage()
    {
        printf("usage:\tcbread <input>\n");
        printf("\tcbread <filename>\n");
        printf("\t\tallows to paste file in file managers\n");
        printf("\tcbread <text>\n");
        printf("\t\tallows to paste text in text editors, console, etc\n");
    }

} // namespace

int main(int argc, const char *argv[])
{
    if (argc != 2)
    {
        printUsage();
        return -1;
    }

    const char *input = argv[1];
    std::string data;

    std::unique_ptr<CCbHandler> hndl;
    if (isFile(input))
    {
        // need this to make proper file URI on
        // on my Ubuntu 24.04 with Gnome paste to file manager fails otherwise
        // not sure about other systems/desktop managers
        data = makeFileUri(input);
        hndl = std::make_unique<CFileHandler>(getDisplay());
    }
    else
    {
        data = input;
        hndl = std::make_unique<CTextHandler>(getDisplay());
    }

    CXSelection sel(getDisplay(), hndl.get(), std::move(data));
    sel.run();

    return 0;
}
