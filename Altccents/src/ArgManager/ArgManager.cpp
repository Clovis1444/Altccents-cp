#include "Altccents/ArgManager/ArgManager.h"

#include <QApplication>

#include "Altccents/Settings.h"

namespace Altccents {
ArgManager::ArgManager() {
    QApplication::setApplicationName(Settings::kProgramName);
    QApplication::setApplicationVersion(Settings::kProgramVersion);

    setApplicationDescription(Settings::kProgramDesc);
    addVersionOption();
    addHelpOption();

    QCommandLineOption sus_opt{QList<QString>{"s", "sus"}, "My sus option"};
    addOption(sus_opt);
}
}  // namespace Altccents
