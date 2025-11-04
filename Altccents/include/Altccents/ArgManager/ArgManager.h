#pragma once

#include <qcommandlineparser.h>
#include <qhash.h>

#include "Altccents/Settings.h"

namespace Altccents {
class ArgManager : public QCommandLineParser {
   public:
    ArgManager();

    void handleArgs();

   private:
    QHash<Settings::SettingsType, QCommandLineOption> arg_opts_;
};
}  // namespace Altccents
