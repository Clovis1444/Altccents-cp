#include <QDir>
#include <QString>

namespace Altccents {
// TODO(clovis): consider implementing this as class instead?
// TODO(clovis): implement settings loading from file
// TODO(clovis): consider using QSettings !!!!!!!!!!
// See https://doc.qt.io/qt-6/qsettings.html#details
namespace Settings {
const QString kProgramName{"Altccents"};

#ifdef __linux__
const QString kSettingsDir{QDir::home().absolutePath() + '/' + kProgramName +
                           '/'};
// TODO(clovis)
#elifdef _WIN32
const QString kSettingsDir{""};
// TODO(clovis)
#else
const QString kSettingsDir{""};
#endif

const QString kAccentProfileDir{kSettingsDir + "profiles/"};

const QString kSettingsFileName{"settings.json"};
const QString kSettingsFilePath{kSettingsDir + kSettingsFileName};
};  // namespace Settings
}  // namespace Altccents
