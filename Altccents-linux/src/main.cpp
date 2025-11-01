#include <QApplication>

#include "Altccents/Altccents.h"
#include "Altccents/AltccentsLinux.h"

int main(int argc, char** argv) {
    QApplication a{argc, argv};

    Altccents::AltccentsApp* altccents{new Altccents::AltccentsApp{}};
    Altccents::AltccentsLinux altccents_linux{altccents};
    altccents->popup();

    return QApplication::exec();

    return 0;
}
