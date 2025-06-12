#include <QDir>
#include <QHash>
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

    // NOLINTNEXTLINE
    enum SettingsType {
        // QString
        kActiveProfile,
        // QList<QString>
        kLoadedProfiles,
        // Insert new members here
        kEnumLength
    };

    // TODO(clovis): fix invalid values save and loading
    static void saveSettings() {
        QSettings settings{kSettingsFilePath, QSettings::IniFormat};

        for (const SettingEntry& i : settings_) {
            // Check if current value is correct
            if (i.val.isNull() || i.val.metaType() != i.def_val.metaType()) {
                settings.setValue(i.key, i.def_val);
            } else {
                settings.setValue(i.key, i.val);
            }
        }
    }

    // TODO(clovis): is settings.value() always returns QVariant<QString> type?
    // If so - bad. Find a way to cast type def_val metaType
    static void loadSettings() {
        QSettings settings{kSettingsFilePath, QSettings::IniFormat};

        for (SettingEntry& i : settings_) {
            QVariant new_val{settings.value(i.key, i.def_val)};

            if (new_val.metaType() == i.def_val.metaType()) {
                i.val = new_val;
            }
        }
    }

    static QVariant get(SettingsType s) {
        // Runtime asserts
        if (s == SettingsType::kEnumLength) {
            qFatal() << "Altccents::Settings::get() [FATAL]: wrong "
                        "SettingsType enum value!";
        };
        if (SettingsType::kEnumLength != settings_.count()) {
            qFatal() << "Altccents::Settings::get() [FATAL]: setting_.count() "
                        "and SettingsType enum length does not match!";
        };

        bool val_is_null{settings_[s].val.isNull()};
        bool val_has_wrong_type{settings_[s].val.metaType() !=
                                settings_[s].def_val.metaType()};

        return (val_is_null || val_has_wrong_type) ? settings_[s].def_val
                                                   : settings_[s].val;
    }

    static void set(SettingsType s, const QVariant& val = {}) {
        // Runtime asserts
        if (s == SettingsType::kEnumLength) {
            qFatal() << "Altccents::Settings::set() [FATAL]: wrong "
                        "SettingsType enum value!";
        };
        if (SettingsType::kEnumLength != settings_.count()) {
            qFatal() << "Altccents::Settings::set() [FATAL]: setting_.count() "
                        "and SettingsType enum length does not match!";
        };

        // Type check
        if (val.metaType() != settings_[s].def_val.metaType()) {
            qWarning() << "Altccents::Settings::set() [WARNING]: wrong "
                          "type of setting was passed!";
            return;
        }

        settings_[s].val = val;
    }

   private:
    // TODO(clovis): find a way to make key and def_val const
    struct SettingEntry {
        QString key;
        QVariant def_val;
        QVariant val;
    };

    // IMPORTANT:
    // - settings_.count() == SettingsType::kEnumLength MUST be true
    // - def_val type MUST be defined in settings_
    // - key and def_val MUST not be changed
    inline static QHash<SettingsType, SettingEntry> settings_{
        {kActiveProfile,
         {.key{"Cache/active_profile"}, .def_val{QString{}}, .val{}}},
        {kLoadedProfiles,
         {.key{"Cache/loaded_profiles"}, .def_val{QList<QString>{}}, .val{}}}};
};
}  // namespace Altccents
