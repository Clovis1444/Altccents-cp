#pragma once

#include <qstandardpaths.h>

#include <QApplication>
#include <QDir>
#include <QHash>
#include <QIcon>
#include <QSettings>
#include <QStandardPaths>
#include <QString>
#include <QVariant>

namespace Altccents {
class Settings {
   public:
    inline static const QString kProgramName{"Altccents"};
    inline static const QString kProgramVersion{PROJECT_VERSION};
    inline static const QString kProgramOrgName{"Clovis1444"};
    inline static const QString kProgramSite{
        "https://github.com/Clovis1444/Altccents-cp"};
    inline static const QString kProgramDesc{
        kProgramName +
        " - is a simple utility for typing accented characters.\nFor more info "
        "see " +
        kProgramSite + '.'};

#ifdef __linux__
    inline static const QString kSettingsDir{
        QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + '/' +
        kProgramName + '/'};
#elifdef _WIN32
    inline static const QString kSettingsDir{
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) +
        '/' + kProgramName + '/'};
// TODO(clovis): define for other platforms
#else
    // inline static const QString kSettingsDir{""};
    // inline static const QString kResourcesDir{""};
#endif

    static QString resourcesDir() {
        return QApplication::applicationDirPath() + "/resources/";
    }

    inline static const QString kAccentProfileDir{kSettingsDir + "profiles/"};

    inline static const QString kSettingsFileName{"settings.ini"};
    inline static const QString kSettingsFilePath{kSettingsDir +
                                                  kSettingsFileName};

    static QString iconFilePath() { return resourcesDir() + "icon.png"; }
    static QString logoFilePath() { return resourcesDir() + "logo.png"; }
    static QString logoOffFilePath() { return resourcesDir() + "logo_off.png"; }

    static QIcon iconPlaceholder() {
        return QIcon::fromTheme(QIcon::ThemeIcon::InsertImage);
    }
    static QIcon logoPlaceholder() {
        return QIcon::fromTheme(QIcon::ThemeIcon::InsertImage);
    }

    inline static const QList<Qt::Key> kNextAccentKeys{Qt::Key_K,
                                                       Qt::Key_Right};
    inline static const QList<Qt::Key> kPrevAccentKeys{Qt::Key_J, Qt::Key_Left};
    inline static const QList<Qt::Key> kNextTabKeys{Qt::Key_L, Qt::Key_Up};
    inline static const QList<Qt::Key> kPrevTabKeys{Qt::Key_H, Qt::Key_Down};

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
        kOneShotMode,
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
        settingAssert(s);

        // Always launch app on one shot mod on linux
#ifdef __linux__
        if (s == kOneShotMode) {
            return true;
        }
#endif

        bool val_is_null{settings_[s].val.isNull()};
        bool val_has_wrong_type{settings_[s].val.metaType() !=
                                settings_[s].def_val.metaType()};

        return (val_is_null || val_has_wrong_type) ? settings_[s].def_val
                                                   : settings_[s].val;
    }

    static void set(SettingsType s, const QVariant& val = {}) {
        settingAssert(s);

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

    static QString getKey(SettingsType s) {
        settingAssert(s);

        return settings_[s].key;
    }
    static QString getDesc(SettingsType s) {
        settingAssert(s);

        return settings_[s].desc;
    }
    static QString getTypeName(SettingsType s) {
        settingAssert(s);

        return settings_[s].def_val.typeName();
    }
    static QMetaType getMetaType(SettingsType s) {
        settingAssert(s);

        return settings_[s].def_val.metaType();
    }

   private:
    static void settingAssert(SettingsType s) {
        // Runtime asserts
        if (s == SettingsType::kEnumLength) {
            qFatal() << "Altccents::Settings::get() [FATAL]: wrong "
                        "SettingsType enum value!";
        };
        if (SettingsType::kEnumLength != settings_.count()) {
            qFatal() << "Altccents::Settings::get() [FATAL]: setting_.count() "
                        "and SettingsType enum length does not match!";
        };
    }

    struct SettingEntry {
        QString key;
        QString desc;
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
        {kPopupX,
         {.key{"Popup/popup_x"},
          .desc{"Popup relative x pos [0..1]."},
          .def_val{0.5},
          .val{}}},
        // [0..1]: 0 - center up; 1 - center bottom
        {kPopupY,
         {.key{"Popup/popup_y"},
          .desc{"Popup relative y pos [0..1]."},
          .def_val{0.9},
          .val{}}},
        // [0..1]: 0 - transparent; 1 - opaque
        {kPopupOpacity,
         {.key{"Popup/popup_opacity"},
          .desc{"Popup opacity [0..1]."},
          .def_val{0.9},
          .val{}}},
        {kPopupMargins,
         {.key{"Popup/popup_margins"},
          .desc{"Margins between char boxes."},
          .def_val{10},
          .val{}}},
        {kPopupTabMargins,
         {.key{"Popup/popup_tab_margins"},
          .desc{"Margins between tabs."},
          .def_val{1},
          .val{}}},
        {kCharBoxSize,
         {.key{"Popup/char_box_size"},
          .desc{"Char box size."},
          .def_val{100},
          .val{}}},
        {kPopupTabSize,
         {.key{"Popup/popup_tab_size"},
          .desc{"Tab size."},
          .def_val{50},
          .val{}}},
        {kPopupBackgorundColor,
         {.key{"Popup/popup_background_color"},
          .desc{"Popup background color."},
          .def_val{QColor{"#181825"}},
          .val{}}},
        {kCharBoxColor,
         {.key{"Popup/char_box_color"},
          .desc{"Char box color."},
          .def_val{QColor{"#313244"}},
          .val{}}},
        {kCharBoxActiveColor,
         {.key{"Popup/char_box_active_color"},
          .desc{"Selected char box color."},
          .def_val{QColor{"#585b70"}},
          .val{}}},
        // [0..100]: 0 - no rounding; 100 - circle rounding
        {kPopupRounding,
         {.key{"Popup/popup_rounding"},
          .desc{"Percentage rounding for popup elements [0..100]."},
          .def_val{0.35},
          .val{}}},
        {kPopupBorderWidth,
         {.key{"Popup/popup_border_width"},
          .desc{"Popup border width."},
          .def_val{3},
          .val{}}},
        {kPopupBorderColor,
         {.key{"Popup/popup_border_color"},
          .desc{"Popup border color."},
          .def_val{QColor{"#cba6f7"}},
          .val{}}},
        {kCharBoxBorderColor,
         {.key{"Popup/char_box_border_color"},
          .desc{"Char box border color."},
          .def_val{QColor{"#89b4fa"}},
          .val{}}},
        {kCharBoxActiveBorderColor,
         {.key{"Popup/char_box_border_active_color"},
          .desc{"Selected char box border color."},
          .def_val{QColor{"#b4befe"}},
          .val{}}},
        {kPopupFontColor,
         {.key{"Popup/popup_font_color"},
          .desc{"Popup font color."},
          .def_val{QColor{"#eba0ac"}},
          .val{}}},
        {kPopupActiveFontColor,
         {.key{"Popup/popup_active_font_color"},
          .desc{"Popup selected font color."},
          .def_val{QColor{"#a6e3a1"}},
          .val{}}},
        {kPopupFontFamily,
         {.key{"Popup/popup_font_family"},
          .desc{"Popup font family."},
          .def_val{QString{"Comic Sans MS"}},
          .val{}}},
        {kPopupFontPointSize,
         {.key{"Popup/popup_font_point_size"},
          .desc{"Popup font size."},
          .def_val{50},
          .val{}}},
        {kPopupTabFontPointSize,
         {.key{"Popup/popup_tab_font_point_size"},
          .desc{"Popup tab font size."},
          .def_val{25},
          .val{}}},
        {kPopupFontWeight,
         {.key{"Popup/popup_font_weight"},
          .desc{"Popup font weight."},
          .def_val{-1},
          .val{}}},
        {kPopupFontItalic,
         {.key{"Popup/popup_font_italic"},
          .desc{"Use italic font for for popup."},
          .def_val{false},
          .val{}}},
        {kActiveProfile,
         {.key{"Cache/active_profile"},
          .desc{"Set active profile."},
          .def_val{QString{}},
          .val{}}},
        {kProgramState,
         {.key{"Cache/program_state"},
          .desc{"Set program state."},
          .def_val{true},
          .val{}}},
        {kSaveCache,
         {.key{"Cache/save_cache"},
          .desc{
              "Set whether the cache will be saved after closing the program."},
          .def_val{true},
          .val{}}},
        {kControlKey,
         {.key{"General/control_key"},
          .desc{"Define 'control' key."},
          .def_val{77},
          .val{}}},
        {kOneShotMode,
         {.key{"General/one_shot_mode"},
          .desc{"Start program in OneShot mode."},
          .def_val{false},
          .val{}}},
        //
    };
};
}  // namespace Altccents
