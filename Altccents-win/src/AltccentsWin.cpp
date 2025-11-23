#include "Altccents/AltccentsWin.h"

#include <Windows.h>

#include "Altccents/Settings.h"

namespace Altccents {
AltccentsWin::AltccentsWin(AltccentsApp* parent)
    : QObject{parent}, altccents_{parent} {
    if (!altccents_) {
        qInfo().noquote()
            << "AltccentsWin [ERROR]: "
            << "Failed to initialize AltccentsWin: AltccentsApp* == nullptr!";
        std::exit(1);
    }

    qInfo() << Settings::get(Settings::kControlKey);

    qInfo() << "Hello from AltccentsWin";
}
}  // namespace Altccents
