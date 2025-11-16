#include <QApplication>

#include "Altccents/Altccents.h"

int main(int argc, char** argv) {
    if (Altccents::isAlreadyRunning()) {
        qInfo().noquote()
            << Altccents::Settings::kProgramName
            << ": Another instance of the application is already running.";
        return 0;
    }

    QApplication app{argc, argv};
    Altccents::AltccentsApp* altccents{new Altccents::AltccentsApp{app}};

    return QApplication::exec();
}
