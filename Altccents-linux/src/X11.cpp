#include "Altccents/X11.h"

namespace Altccents {
void X11::init(AltccentsApp* parent) {
    if (hm_ != nullptr) {
        return;
    }

    hm_ = new HookManager{parent};
}
}  // namespace Altccents
