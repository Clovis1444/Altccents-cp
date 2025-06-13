#include "Altccents/Altccents.h"

int main() {
    Altccents::AltccentsApp app{};

    // app.loadAccentProfiles("samples");

    // for (const Altccents::AccentProfile& i : app.loadedAccentProfiles()) {
    //     qInfo() << i.name();
    //     i.print();
    // }

    Altccents::Settings::loadSettings();
    // qInfo() << "Active profile:"
    //         << Altccents::Settings::get(Altccents::Settings::kActiveProfile);

    // Altccents::Settings::saveSettings();

    return 0;
}
