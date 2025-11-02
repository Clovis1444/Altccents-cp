#include "Altccents/AltccentsLinux.h"

#include <qtenvironmentvariables.h>

#include <QProcess>

namespace Altccents {
AltccentsLinux::AltccentsLinux(AltccentsApp* parent) : QObject{parent} {
    if (!parent) {
        return;
    }

    QObject::connect(parent, &AltccentsApp::charSendRequested, this,
                     &AltccentsLinux::onCharSendRequested);
}

void AltccentsLinux::onCharSendRequested(Key, QChar symbol) {
    if (symbol.isNull()) {
        qCritical().noquote()
            << __PRETTY_FUNCTION__ << "[ERROR]: invalid symbol to type";

        QApplication::exit(1);
        return;
    }

    // Wayland session
    if (!qgetenv("WAYLAND_DISPLAY").isEmpty()) {
        // TODO(clovis): add Wayland support here
        QApplication::quit();
        return;
    }
    // X11 session
    if (!qgetenv("DISPLAY").isEmpty()) {
        // TODO(clovis): add xdotool alias setting option?
        const QString p_name{"xdotool"};
        if (!isProgramInstalled(p_name)) {
            qCritical().noquote()
                << __PRETTY_FUNCTION__ << "[ERROR]: " << p_name
                << "is not installed on your system";

            QApplication::exit(1);
            return;
        }

        QProcess p{};
        p.setProgram(p_name);
        // TODO(clovis): xdotool sometimes silently fails()
        p.setArguments({"key", "--clearmodifiers", "--delay", "50",
                        getXdotoolKeyArg(symbol)});
        p.start();
        qInfo().noquote() << __PRETTY_FUNCTION__ << "[INFO]: running "
                          << p.program() << p.arguments().join(" ");
        p.waitForFinished();

        if (p.exitCode()) {
            qCritical().noquote() << __PRETTY_FUNCTION__
                                  << "[ERROR]: " << p.readAllStandardError();

            QApplication::exit(1);
            return;
        }

        QApplication::quit();
        return;
    }

    qCritical().noquote() << __PRETTY_FUNCTION__
                          << "[ERROR]: failed to find X11 or Wayland session";
    QApplication::exit(1);
}

bool AltccentsLinux::isProgramInstalled(const QString& p_name) {
    QProcess p{};
    p.start("which", {p_name});
    p.waitForFinished();

    return p.exitCode() == 0;
}

QString AltccentsLinux::getXdotoolKeyArg(QChar symbol) {
    QString arg{symbol.isUpper() ? "Shift+U%1" : "U%1"};
    arg = arg.arg(symbol.unicode(), 4, 16, QChar{'0'}).toUpper();

    return arg;
}
}  // namespace Altccents
