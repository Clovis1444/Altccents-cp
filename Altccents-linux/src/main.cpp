#include <QApplication>

#include "Altccents/Altccents.h"
#include "Altccents/X11.h"
#include "Utils.h"

int main(int argc, char** argv) {
    QApplication a{argc, argv};

    Altccents::AltccentsApp* altccents{new Altccents::AltccentsApp{}};
    //
    // qInfo() << QString{altccents.nextAccent(Altccents::Key{69}, true)};
    // qInfo() << QString{altccents.nextAccent(Altccents::Key{69}, true)};
    // qInfo() << QString{altccents.nextAccent(Altccents::Key{69}, true)};
    // qInfo() << QString{altccents.nextAccent(Altccents::Key{65}, false)};
    // qInfo() << QString{altccents.nextAccent(Altccents::Key{69}, true)};
    // qInfo() << QString{altccents.nextAccent(Altccents::Key{69}, true)};
    // qInfo() << QString{altccents.nextAccent(Altccents::Key{69}, false)};
    // qInfo() << QString{altccents.nextAccent(Altccents::Key{69}, false)};
    // qInfo() << QString{altccents.nextAccent(Altccents::Key{69}, true)};
    //
    // altccents.popup();
    // return QApplication::exec();

    // Altccents::Utils::readEvent();

    // Altccents::Utils::xHook();

    // bool hook_result{
    //     Altccents::X11::hook(altccents.activeProfile(), altccents)};
    // if (!hook_result) {
    //     return 1;
    // }

    // Altccents::X11::init(altccents);
    altccents->popup();

    return QApplication::exec();

    return 0;
}
