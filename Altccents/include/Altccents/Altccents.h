#pragma once
#include <QList>
#include <QString>

#include "Altccents/AccentProfile/AccentProfile.h"
#include "Altccents/Settings.h"

namespace Altccents {
QList<AccentProfile> readAccentProfiles(const QString& dir);

class AltccentsApp {
   public:
    AltccentsApp();

    bool loadAccentProfiles(const QString& dir = Settings::kAccentProfileDir);

    QList<AccentProfile> loadedAccentProfiles() const {
        return loadedAccentProfiles_;
    }

    void loadConfig();

    QString activeProfileName() const;

    void setActiveProfile(const AccentProfile& profile);
    void setActiveProfile(const QString& profile);

    QChar nextAccent(const Qt::Key& key, bool is_capital);

   private:
    AccentProfile activeAccentProfile_;
    QList<AccentProfile> loadedAccentProfiles_;

    struct AccentInput {
        Qt::Key key;
        bool is_capital;
        int index;
    };

    AccentInput lastAccent_{};
};
}  // namespace Altccents
