#include <QApplication>

#include "Altccents/Altccents.h"
#include "Altccents/AltccentsLinux.h"
#include "Altccents/Settings.h"

int main(int argc, char** argv) {
    if (Altccents::isAlreadyRunning()) {
        qInfo().noquote()
            << Altccents::Settings::kProgramName
            << ": Another instance of the application is already running.";
        return 0;
    }

    QApplication app{argc, argv};

    Altccents::AltccentsApp* altccents{new Altccents::AltccentsApp{app}};
    Altccents::AltccentsLinux altccents_linux{altccents};

    altccents->popup();

    return QApplication::exec();

    return 0;
}
