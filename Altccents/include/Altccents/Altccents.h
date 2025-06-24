#pragma once
#include <QtWidgets/qsystemtrayicon.h>

#include <QList>
#include <QString>

#include "Altccents/AccentProfile/AccentProfile.h"
#include "Altccents/Settings.h"

namespace Altccents {
QList<AccentProfile> readAccentProfiles(const QString& dir);

class AltccentsApp {
   public:
    AltccentsApp();
    ~AltccentsApp();

    bool loadAccentProfiles(const QString& dir = Settings::kAccentProfileDir);

    QList<AccentProfile> loadedAccentProfiles() const {
        return loadedAccentProfiles_;
    }

    void loadConfig();

    QString activeProfileName() const;

    void setActiveProfile(const AccentProfile& profile);
    void setActiveProfile(const QString& profile);

    QChar nextAccent(const Qt::Key& key, bool is_capital);

    void createTrayIcon();

    int start(int argc, char** argv);

    void updateTrayIcon();

   private:
    AccentProfile activeAccentProfile_;
    QList<AccentProfile> loadedAccentProfiles_;

    QSystemTrayIcon* trayIcon_{};

    // TODO(clovis): add config setting for this
    bool isAppOn_{};

    struct AccentInput {
        Qt::Key key;
        bool is_capital;
        int index;
    };

    AccentInput lastAccent_{};
};
}  // namespace Altccents
