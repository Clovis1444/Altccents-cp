#include "Altccents/Altccents.h"

int main() {
    Altccents::AltccentsApp app{};

    qInfo() << QString{app.nextAccent(static_cast<Qt::Key>(69), true)};
    qInfo() << QString{app.nextAccent(static_cast<Qt::Key>(69), true)};
    qInfo() << QString{app.nextAccent(static_cast<Qt::Key>(69), true)};
    qInfo() << QString{app.nextAccent(static_cast<Qt::Key>(65), false)};
    qInfo() << QString{app.nextAccent(static_cast<Qt::Key>(69), true)};
    qInfo() << QString{app.nextAccent(static_cast<Qt::Key>(69), true)};
    qInfo() << QString{app.nextAccent(static_cast<Qt::Key>(69), false)};
    qInfo() << QString{app.nextAccent(static_cast<Qt::Key>(69), false)};
    qInfo() << QString{app.nextAccent(static_cast<Qt::Key>(69), true)};
    return 0;
}
