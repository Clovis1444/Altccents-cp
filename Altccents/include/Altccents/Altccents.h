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
    AccentProfile activeProfile() const;

    void setActiveProfile(const AccentProfile& profile);
    void setActiveProfile(const QString& profile);
    void setActiveProfile();

    QChar nextAccent(const Qt::Key& key, bool is_capital);

    void createTray();

    int start(int argc, char** argv);

    void updateTrayIcon();
    void updateTrayMenu();
    void updateTrayToolTip();
    // NOLINTNEXTLINE
    enum class updateTrayFlag : uint32_t {
        kNone = 0,
        kTrayIcon = (1 << 0),
        kTrayMenu = (1 << 1),
        kTrayToolTip = (1 << 2),
        kAll = kTrayIcon | kTrayMenu | kTrayToolTip

    };
    void updateTray(updateTrayFlag flags = updateTrayFlag::kAll);

    bool programState() const { return programState_; }
    void setProgramState(bool state);
    bool toggleProgramState();

    void writeCacheToFile();

    bool saveCahche() const { return saveCache_; }
    void setSaveCache(bool val);

    void updatePopup();

   private:
    AccentProfile activeAccentProfile_;
    QList<AccentProfile> loadedAccentProfiles_;

    QSystemTrayIcon* tray_{};
    QMenu* trayMenu_{};

    QWidget* popup_{};

    bool programState_{};

    bool saveCache_{true};

    struct AccentInput {
        Qt::Key key;
        bool is_capital;
        int index;
    };

    AccentInput lastAccent_{};
};  // AltccentsApp

constexpr AltccentsApp::updateTrayFlag operator|(
    AltccentsApp::updateTrayFlag a, AltccentsApp::updateTrayFlag b) {
    return static_cast<AltccentsApp::updateTrayFlag>(static_cast<uint32_t>(a) |
                                                     static_cast<uint32_t>(b));
}
constexpr bool operator&(AltccentsApp::updateTrayFlag a,
                         AltccentsApp::updateTrayFlag b) {
    return static_cast<bool>(static_cast<uint32_t>(a) &
                             static_cast<uint32_t>(b));
}

}  // namespace Altccents
