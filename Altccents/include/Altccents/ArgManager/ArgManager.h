#pragma once

#include <QCommandLineParser>
#include <QHash>
#include <functional>

#include "Altccents/Settings.h"

namespace Altccents {
class ArgManager : public QCommandLineParser {
   public:
    ArgManager();

    void handleSettingsArgs();
    void handlePrintExitArgs();

    static void printPaths();
    static void printSettingsFile();

   private:
    QHash<Settings::SettingsType, QCommandLineOption> settings_opts_;
    // QHash<<Arg_name, Arg_desc>, Arg_handle_func>
    QHash<QPair<QString, QString>, std::function<void()>> print_exit_opts_{
        {{"print-paths", "Prints some program related paths."},
         []() { printPaths(); }},
        {{"print-settings", QString{"Prints content of %1 file."}.arg(
                                Settings::kSettingsFileName)},
         []() { printSettingsFile(); }}
        // Insert new args here
    };
};
}  // namespace Altccents
