#include "Altccents/ArgManager/ArgManager.h"

#include <QApplication>

namespace Altccents {
ArgManager::ArgManager() {
    QApplication::setApplicationName(Settings::kProgramName);
    QApplication::setApplicationVersion(Settings::kProgramVersion);

    setApplicationDescription(Settings::kProgramDesc);
    addHelpOption();
    addVersionOption();

    QCommandLineOption sus_opt{QList<QString>{"s", "sus"}, "My sus option"};
    sus_opt.setValueName("float");
    addOption(sus_opt);

    // Setup args from Settings.h
    for (int i{}; i < Settings::SettingsType::kEnumLength; ++i) {
        Settings::SettingsType s{static_cast<Settings::SettingsType>(i)};

        QString key{Settings::getKey(s)};

        // TODO(clovis): decide whether cache options should be ignored
        if (key.startsWith("Cache/")) {
            continue;
        }

        qsizetype name_pos{key.lastIndexOf('/')};
        QString name{key.sliced(name_pos).remove('/')};

        QCommandLineOption opt{name, Settings::getDesc(s),
                               Settings::getTypeName(s)};
        addOption(opt);

        arg_opts_.insert(s, opt);
    }

    // TODO(clovis): add print_paths arg
}

void ArgManager::handleArgs() {
    for (auto i{arg_opts_.begin()}; i != arg_opts_.end(); ++i) {
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
}  // namespace Altccents
