#include <QtWidgets/qapplication.h>

#include <QDir>
#include <QHash>
#include <QSettings>
#include <QString>
#include <QVariant>

namespace Altccents {
class Settings {
   public:
    inline static const QString kProgramName{"Altccents"};

#ifdef __linux__
    inline static const QString kSettingsDir{QDir::home().absolutePath() +
                                             "/.config/" +
                                             kProgramName.toLower() + '/'};
// TODO(clovis)
#elifdef _WIN32
    inline static const QString kSettingsDir{""};
// TODO(clovis)
#else
    inline static const QString kSettingsDir{""};
#endif

    inline static const QString kAccentProfileDir{kSettingsDir + "profiles/"};
    // TODO(clovis)
    // TODO(clovis): when QApplication::applicationDirPath() called -
    // QApplication is not yet created
    inline static const QString kResourcesDir{
        QApplication::applicationDirPath() + "resources/"};

    inline static const QString kSettingsFileName{"settings.ini"};
    inline static const QString kSettingsFilePath{kSettingsDir +
                                                  kSettingsFileName};

    inline static const QString kLogoFilePath{kResourcesDir + "logo128.png"};
    inline static const QString kLogoOnFilePath{kResourcesDir +
                                                "logo128_on.png"};

    // NOLINTNEXTLINE
    enum SettingsType {
        kPopupX,
        kPopupY,
        kPopupOpacity,
        kPopupMargins,
        kPopupTabMargins,
        kCharBoxSize,
        kPopupTabSize,
        kPopupBackgorundColor,
        kCharBoxColor,
        kCharBoxActiveColor,
        kPopupRounding,
        kPopupBorderWidth,
        kPopupBorderColor,
        kCharBoxBorderColor,
        kCharBoxActiveBorderColor,
        kPopupFontColor,
        kPopupActiveFontColor,
        kPopupFontFamily,
        kPopupFontPointSize,
        kPopupTabFontPointSize,
        kPopupFontWeight,
        kPopupFontItalic,
        kActiveProfile,
        kProgramState,
        kSaveCache,
        kControlKey,
        // Insert new members here
        kEnumLength
    };

    // TODO(clovis): implement saveSettings() and saveCache() using this func
    static void saveSetting(SettingsType s) {
        // Runtime asserts
        if (s == SettingsType::kEnumLength) {
            qFatal() << "Altccents::Settings::get() [FATAL]: wrong "
                        "SettingsType enum value!";
        };
        if (SettingsType::kEnumLength != settings_.count()) {
            qFatal() << "Altccents::Settings::get() [FATAL]: setting_.count() "
                        "and SettingsType enum length does not match!";
        };

        QSettings settings{kSettingsFilePath, QSettings::IniFormat};

        const SettingEntry& entry{settings_[s]};

        // Check if current value is correct
        if (entry.val.isNull() ||
            entry.val.metaType() != entry.def_val.metaType()) {
            settings.setValue(entry.key, entry.def_val);
        } else {
            settings.setValue(entry.key, entry.val);
        }
    }

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

    static void loadSettings() {
        // Clear current settings vals
        for (auto i : settings_) {
            i.val = {};
        }

        QSettings settings{kSettingsFilePath, QSettings::IniFormat};

        for (SettingEntry& i : settings_) {
            QVariant new_val{settings.value(i.key)};

            // If key value is an empty string - set val as def_val
            if (new_val.toString().isEmpty()) {
                i.val = i.def_val;
                continue;
            }

            // Cast value obtained from file to the def_val type
            bool cast_result{new_val.convert(i.def_val.metaType())};

            // Set val
            i.val = cast_result ? new_val : i.def_val;
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

    static void saveCache() {
        QSettings settings{kSettingsFilePath, QSettings::IniFormat};

        for (const SettingEntry& i : settings_) {
            // Ignore all non cache settings
            if (!i.key.startsWith("Cache/")) {
                continue;
            }

            // Check if current value is correct
            if (i.val.isNull() || i.val.metaType() != i.def_val.metaType()) {
                settings.setValue(i.key, i.def_val);
            } else {
                settings.setValue(i.key, i.val);
            }
        }
    }

   private:
    struct SettingEntry {
        QString key;
        QVariant def_val;
        QVariant val;
    };

    // IMPORTANT:
    // - SettingEntry variable data type MUST be defined in def_val
    // - settings_.count() == SettingsType::kEnumLength MUST be true
    // - def_val type MUST be defined in settings_
    // - key and def_val MUST not be changed
    inline static QHash<SettingsType, SettingEntry> settings_{
        // [0..1]: 0 - center left; 1 - center right
        {kPopupX, {.key{"Popup/popup_x"}, .def_val{0.5}, .val{}}},
        // [0..1]: 0 - center up; 1 - center bottom
        {kPopupY, {.key{"Popup/popup_y"}, .def_val{0.9}, .val{}}},
        // [0..1]: 0 - transparent; 1 - opaque
        {kPopupOpacity, {.key{"Popup/popup_opacity"}, .def_val{0.9}, .val{}}},
        {kPopupMargins, {.key{"Popup/popup_margins"}, .def_val{10}, .val{}}},
        {kPopupTabMargins,
         {.key{"Popup/popup_tab_margins"}, .def_val{1}, .val{}}},
        {kCharBoxSize, {.key{"Popup/char_box_size"}, .def_val{100}, .val{}}},
        {kPopupTabSize, {.key{"Popup/popup_tab_size"}, .def_val{50}, .val{}}},
        {kPopupBackgorundColor,
         {.key{"Popup/popup_background_color"},
          .def_val{QColor{"#181825"}},
          .val{}}},
        {kCharBoxColor,
         {.key{"Popup/char_box_color"}, .def_val{QColor{"#313244"}}, .val{}}},
        {kCharBoxActiveColor,
         {.key{"Popup/char_box_active_color"},
          .def_val{QColor{"#585b70"}},
          .val{}}},
        // [0..100]: 0 - no rounding; 100 - circle rounding
        {kPopupRounding,
         {.key{"Popup/popup_rounding"}, .def_val{0.35}, .val{}}},
        {kPopupBorderWidth,
         {.key{"Popup/popup_border_width"}, .def_val{3}, .val{}}},
        {kPopupBorderColor,
         {.key{"Popup/popup_border_color"},
          .def_val{QColor{"#cba6f7"}},
          .val{}}},
        {kCharBoxBorderColor,
         {.key{"Popup/char_box_border_color"},
          .def_val{QColor{"#89b4fa"}},
          .val{}}},
        {kCharBoxActiveBorderColor,
         {.key{"Popup/char_box_border_active_color"},
          .def_val{QColor{"#b4befe"}},
          .val{}}},
        {kPopupFontColor,
         {.key{"Popup/popup_font_color"}, .def_val{QColor{"#eba0ac"}}, .val{}}},
        {kPopupActiveFontColor,
         {.key{"Popup/popup_active_font_color"},
          .def_val{QColor{"#a6e3a1"}},
          .val{}}},
        {kPopupFontFamily,
         {.key{"Popup/popup_font_family"}, .def_val{QString{}}, .val{}}},
        {kPopupFontPointSize,
         {.key{"Popup/popup_font_point_size"}, .def_val{50}, .val{}}},
        {kPopupTabFontPointSize,
         {.key{"Popup/popup_tab_font_point_size"}, .def_val{25}, .val{}}},
        {kPopupFontWeight,
         {.key{"Popup/popup_font_weight"}, .def_val{-1}, .val{}}},
        {kPopupFontItalic,
         {.key{"Popup/popup_font_italic"}, .def_val{false}, .val{}}},
        {kActiveProfile,
         {.key{"Cache/active_profile"}, .def_val{QString{}}, .val{}}},
        {kProgramState, {.key{"Cache/program_state"}, .def_val{true}, .val{}}},
        {kSaveCache, {.key{"Cache/save_cache"}, .def_val{true}, .val{}}},
        {kControlKey, {.key{"General/control_key"}, .def_val{77}, .val{}}},
        //
    };
};
}  // namespace Altccents
