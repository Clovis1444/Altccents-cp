#include <QApplication>

#include "Altccents/Altccents.h"
#include "Altccents/AltccentsWin.h"

int main(int argc, char** argv) {
    if (Altccents::isAlreadyRunning()) {
        qInfo().noquote()
            << Altccents::Settings::kProgramName
            << ": Another instance of the application is already running.";
        return 0;
    }

    QApplication app{argc, argv};

    Altccents::AltccentsApp altccents{app};
    Altccents::AltccentsWin altccents_win{&altccents};

    return QApplication::exec();
}
