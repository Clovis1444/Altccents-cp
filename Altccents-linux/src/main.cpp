#include <QApplication>

#include "Altccents/Altccents.h"

int main(int argc, char** argv) {
    QApplication a{argc, argv};

    Altccents::AltccentsApp altccents{};

    qInfo() << QString{altccents.nextAccent(static_cast<Qt::Key>(69), true)};
    qInfo() << QString{altccents.nextAccent(static_cast<Qt::Key>(69), true)};
    qInfo() << QString{altccents.nextAccent(static_cast<Qt::Key>(69), true)};
    qInfo() << QString{altccents.nextAccent(static_cast<Qt::Key>(65), false)};
    qInfo() << QString{altccents.nextAccent(static_cast<Qt::Key>(69), true)};
    qInfo() << QString{altccents.nextAccent(static_cast<Qt::Key>(69), true)};
    qInfo() << QString{altccents.nextAccent(static_cast<Qt::Key>(69), false)};
    qInfo() << QString{altccents.nextAccent(static_cast<Qt::Key>(69), false)};
    qInfo() << QString{altccents.nextAccent(static_cast<Qt::Key>(69), true)};

    altccents.popup();
    return QApplication::exec();
}
