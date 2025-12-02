#include "Altccents/ArgManager/ArgManager.h"

#include <QApplication>

#include "Altccents/Settings.h"

namespace Altccents {
ArgManager::ArgManager() {
    setApplicationDescription(Settings::kProgramDesc);
    addHelpOption();
    addVersionOption();

    // Add opts from print_exit_opts_ here
    for (const auto& i : print_exit_opts_.keys()) {
        addOption(QCommandLineOption{i.first, i.second});
    }

    // Setup args from Settings.h
    for (int i{}; i < Settings::SettingsType::kEnumLength; ++i) {
        Settings::SettingsType s{static_cast<Settings::SettingsType>(i)};

        QString key{Settings::getKey(s)};

        // NOTE(clovis): ignore cache args
        if (key.startsWith("Cache/")) {
            continue;
        }

        qsizetype name_pos{key.lastIndexOf('/')};
        QString name{key.sliced(name_pos).remove('/')};
        QString type_name{Settings::getTypeName(s)};
        // Custom type name
        switch (s) {
            case Settings::kControlKey: {
                type_name += "(KEY)";
                break;
            }
            case Settings::kHotkey: {
                type_name += "(MODIFIER_KEYS+KEY)";
                break;
            }
            default: {
                break;
            }
        }

        QCommandLineOption opt{name, Settings::getDesc(s), type_name};
        addOption(opt);

        settings_opts_.insert(s, opt);
    }
}

void ArgManager::handleSettingsArgs() {
    for (auto i{settings_opts_.begin()}; i != settings_opts_.end(); ++i) {
        if (isSet(*i)) {
            QVariant val{value(*i)};
            QMetaType m_type{Settings::getMetaType(i.key())};
            // If arg value cannot be converted - do nothing
            if (!val.convert(m_type)) {
                qWarning().noquote() << Settings::kProgramName
                           << QString{"[WARNING]: failed to convert '%1' argument value \"%2\" to %3"}
                                  .arg(i->names().first()).arg(value(*i))
                                  .arg(m_type.name());
                continue;
            }

            Settings::set(i.key(), val);
        }
    }
}
void ArgManager::handlePrintExitArgs() {
    for (auto i{print_exit_opts_.constBegin()}; i != print_exit_opts_.cend();
         ++i) {
        if (isSet(i.key().first)) {
            // Call corresponding function
            i.value()();

            // QApplication::quit() does not work here
            std::exit(0);
        }
    }
}

void ArgManager::printPaths() {
    qInfo().noquote() << "Program dir: "
                      << QApplication::applicationDirPath() + '/';
    qInfo().noquote() << "Resources dir: " << Settings::resourcesDir();
    qInfo().noquote() << "Settings dir: " << Settings::kSettingsDir;
    qInfo().noquote() << "Settings file: " << Settings::kSettingsFilePath;
    qInfo().noquote() << "Profiles dir: " << Settings::kAccentProfileDir;
}
void ArgManager::printSettingsFile() {
    QFile file{Settings::kSettingsFilePath};

    if (file.open(QFile::ReadOnly | QFile::Text)) {
        qInfo().noquote().nospace() << Settings::kSettingsFilePath << ':';
        qInfo().noquote() << file.readAll();
    } else {
        qInfo().noquote() << "Failed to open " << Settings::kSettingsFilePath;
    }

    file.close();
}
}  // namespace Altccents
