#include "Altccents/Altccents.h"

int main() {
    Altccents::AltccentsApp app{};

    qInfo() << "Active profile:" << app.activeProfileName();
    return 0;
}
