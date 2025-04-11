#include <array>

#include <stdio.h>
#include "x11_helpers.hpp"

namespace {
    constexpr static std::array<const char*, 3> selections = {
        "PRIMARY", "SECONDARY", "CLIPBOARD",
    };

    void printCbOwner(const CXDisplay &display)
    {
        for (size_t i = 0; i < selections.size(); ++i)
        {
            Atom clipboard = XInternAtom(static_cast<Display*>(display), "CLIPBOARD", false);
            Window owner = XGetSelectionOwner(static_cast<Display*>(display), clipboard);

            printf("'%s' owner 0x%lX\n", selections[i], owner);
        }
    }

} // namespace

int main()
{
    CXDisplay display(XOpenDisplay(nullptr));
    if (!display)
    {
        fprintf(stderr, "failed to open display\n");
        return -1;
    }

    printCbOwner(display);
}
