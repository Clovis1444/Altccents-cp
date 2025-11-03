#include <QApplication>

#include "Altccents/Altccents.h"
#include "Altccents/AltccentsLinux.h"

int main(int argc, char** argv) {
    QApplication app{argc, argv};

    Altccents::AltccentsApp* altccents{new Altccents::AltccentsApp{app}};
    Altccents::AltccentsLinux altccents_linux{altccents};

    altccents->popup();

    return QApplication::exec();

    return 0;
}
