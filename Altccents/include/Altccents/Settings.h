#include <QDir>
#include <QSettings>
#include <QString>

namespace Altccents {
class Settings {
   public:
    inline static const QString kProgramName{"Altccents"};

#ifdef __linux__
    inline static const QString kSettingsDir{
        QDir::home().absolutePath() + "/." + kProgramName.toLower() + '/'};
// TODO(clovis)
#elifdef _WIN32
    inline static const QString kSettingsDir{""};
// TODO(clovis)
#else
    inline static const QString kSettingsDir{""};
#endif

    inline static const QString kAccentProfileDir{kSettingsDir + "profiles/"};

    inline static const QString kSettingsFileName{"settings.ini"};
    inline static const QString kSettingsFilePath{kSettingsDir +
                                                  kSettingsFileName};

    static void saveSettings() {
        QSettings settings{kSettingsFilePath, QSettings::IniFormat};

        settings.setValue("General/test_value1", "Kefteme");
        settings.setValue("Cache/path_list",
                          QList<QString>{"path1", "path2", "path2"});
    }

    static void loadSettings() {
        QSettings settings{kSettingsFilePath, QSettings::IniFormat};

        qInfo() << settings
                       .value(kSettingsEntries[0].key,
                              kSettingsEntries[0].def_value)
                       .toList();
    }

   private:
    struct SettingEntry {
        QString key;
        QVariant def_value;
    };

    struct SettingList {
        QString active_profile;
        QList<QString> loaded_profiles;
    };

    inline static SettingList settings_{};

    inline static const QList<SettingEntry> kSettingsEntries{
        {.key = "Cache/active_profile", .def_value = QString{}},
        {.key = "Cache/loaded_profiles", .def_value = QList<QString>{}}};
};
}  // namespace Altccents
