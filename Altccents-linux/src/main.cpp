#include "Altccents/Altccents.h"

int main() {
    Altccents::AltccentsApp app{};

    app.loadAccentProfiles("samples");

    for (const Altccents::AccentProfile& i : app.loadedAccentProfiles()) {
        qInfo() << i.name();
        i.print();
    }
    return 0;
}
