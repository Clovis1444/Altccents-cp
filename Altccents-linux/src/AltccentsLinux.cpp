#include "Altccents/AltccentsLinux.h"

#include <qtenvironmentvariables.h>

#include <QDesktopServices>
#include <QMessageBox>
#include <QProcess>
#include <QPushButton>

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

            showDependencyDialog("xdotool",
                                 "https://github.com/jordansissel/"
                                 "xdotool?tab=readme-ov-file#installation");

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

void AltccentsLinux::showDependencyDialog(const QString& p_name,
                                          const QString& install_link) {
    QMessageBox msg_box{};
    msg_box.setIcon(QMessageBox::Warning);
    msg_box.setWindowTitle(
        QString{"%1: Missing Dependency"}.arg(Settings::kProgramName));
    msg_box.setText(QString{"%1 is not installed."}.arg(p_name));
    msg_box.setInformativeText(
        QString{"%1 requires %2 to be installed and accessible via PATH."}
            .arg(Settings::kProgramName)
            .arg(p_name));

    msg_box.setStandardButtons(QMessageBox::Close);
    QPushButton* install_button{
        msg_box.addButton("See installation page", QMessageBox::YesRole)};
    install_button->setToolTip(QString{"Visit %1"}.arg(install_link));

    msg_box.setDefaultButton(install_button);

    msg_box.exec();

    if (msg_box.clickedButton() == install_button) {
        QDesktopServices::openUrl(install_link);
    }
}
}  // namespace Altccents
