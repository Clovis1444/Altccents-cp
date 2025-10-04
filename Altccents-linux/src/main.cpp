#include <QApplication>

#include "Altccents/Altccents.h"
#include "Altccents/X11.h"
#include "Utils.h"

int main(int argc, char** argv) {
    QApplication a{argc, argv};

    Altccents::AltccentsApp altccents{};
    //
    // qInfo() << QString{altccents.nextAccent(static_cast<Qt::Key>(69), true)};
    // qInfo() << QString{altccents.nextAccent(static_cast<Qt::Key>(69), true)};
    // qInfo() << QString{altccents.nextAccent(static_cast<Qt::Key>(69), true)};
    // qInfo() << QString{altccents.nextAccent(static_cast<Qt::Key>(65),
    // false)}; qInfo() <<
    // QString{altccents.nextAccent(static_cast<Qt::Key>(69), true)}; qInfo() <<
    // QString{altccents.nextAccent(static_cast<Qt::Key>(69), true)}; qInfo() <<
    // QString{altccents.nextAccent(static_cast<Qt::Key>(69), false)}; qInfo()
    // << QString{altccents.nextAccent(static_cast<Qt::Key>(69), false)};
    // qInfo() << QString{altccents.nextAccent(static_cast<Qt::Key>(69), true)};
    //
    // altccents.popup();
    // return QApplication::exec();

    // Altccents::Utils::readEvent();

    // Altccents::Utils::xHook();

    bool hook_result{Altccents::X11::hook(altccents.activeProfile())};
    if (!hook_result) {
        return 1;
    }

    return QApplication::exec();

    return 0;
}
