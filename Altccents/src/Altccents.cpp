#include "Altccents/Altccents.h"

#include <QActionGroup>
#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QList>
#include <QMenu>
#include <QSharedMemory>
#include <QSystemTrayIcon>

#include "Altccents/AccentProfile/AccentProfile.h"
#include "Altccents/Settings.h"

// PRIVATE
namespace {
void warning(const QString& text) {
    qWarning().noquote() << "Altccents [WARNING]:" << text;
}

void info(const QString& text) {
    qInfo().noquote() << "Altccents [INFO]:" << text;
}
};  // namespace

// PUBLIC
namespace Altccents {
QList<AccentProfile> readAccentProfiles(const QString& dir_path) {
    QDir dir{dir_path};
    // If dir does not exists - create it
    if (!dir.exists()) {
        // If failed to create dir
        if (!QDir().mkpath(dir.absolutePath())) {
            QString e{"Failed to create %1 dir"};
            warning(e.arg(dir.absolutePath()));
            return {};
        }
    }

    dir.setFilter(QDir::Files);
    dir.setNameFilters(QList<QString>{"*.json"});

    QList<QFileInfo> jsons{dir.entryInfoList()};

    // Check if at least 1 json was found
    if (jsons.isEmpty()) {
        QString e{"No profiles in %1"};
        warning(e.arg(dir.absolutePath()));
        return {};
    }

    QList<AccentProfile> profiles{};

    // Deserialize profiles
    for (const QFileInfo& i : jsons) {
        QFile f{i.absoluteFilePath()};

        // Skip if failed to open file
        if (!f.open(QFile::ReadOnly)) {
            QString e{"Failed to open file %1"};
            warning(e.arg(i.absoluteFilePath()));

            continue;
        };

        // Read file data
        QByteArray file_data{f.readAll()};
        f.close();

        info(QString{"Deserializing \"%1\""}.arg(i.absoluteFilePath()));
        AccentProfile profile{AccentProfile::deserialize(file_data, i)};

        // Check if profile is Valid
        if (!profile.isEmpty()) {
            profiles.push_back(profile);
            info(QString{"\"%1\" was successfully deserialized"}.arg(
                i.absoluteFilePath()));
        } else {
            warning(QString{"Failed to deserialize \"%1\""}.arg(
                i.absoluteFilePath()));
        }
    }

    return profiles;
}

bool isAlreadyRunning() {
    QString shared_memory_key{Settings::kProgramName};

    // Shared memory may not be clean up correctly on crash?
    static QSharedMemory shared_memory{shared_memory_key};

    // create() returns false if the memory is already created
    return !shared_memory.create(1, QSharedMemory::ReadOnly);
}

AltccentsApp::AltccentsApp(const QApplication& qapp) {
    // Preprocess args before doing anything
    argManager_.process(qapp);

    // Without this line the program will close after closing message box
    QApplication::setQuitOnLastWindowClosed(false);
    // Accent Profiles MUST be loaded before config
    loadAccentProfiles();
    loadConfig();

    // Do arg handling to override config options
    argManager_.handleArgs();

    popup_ = new Popup{};

    // Popup signals
    QObject::connect(popup_, &Popup::hidden, this,
                     &AltccentsApp::onPopupHidden);
    QObject::connect(popup_, &Popup::accentChosen, this,
                     &AltccentsApp::onPopupAccentChosen);
    QObject::connect(popup_, &Popup::accentCopyChosen, this,
                     &AltccentsApp::onPopupAccentCopyChosen);
    QObject::connect(popup_, &Popup::nextAccent, this,
                     &AltccentsApp::onPopupNextAccent);
    QObject::connect(popup_, &Popup::nextTab, this,
                     &AltccentsApp::onPopupNextTab);
    QObject::connect(popup_, &Popup::capitalChanged, this,
                     &AltccentsApp::onPopupCapitalChanged);

    // If running in one shot mode - do not create tray icon
    if (Settings::get(Settings::kOneShotMode).toBool()) {
        return;
    }

    tray_ = new QSystemTrayIcon{};
    trayMenu_ = new QMenu{};
    //
    // Tray
    //
    updateTrayIcon();

    updateTrayMenu();
    // On click
    QObject::connect(tray_, &QSystemTrayIcon::activated,
                     [&](QSystemTrayIcon::ActivationReason r) {
                         // On single left click
                         if (r == QSystemTrayIcon::Trigger) {
                             toggleProgramState();
                             updateTray();
                         }
                     });

    // Tooltip
    updateTrayToolTip();

    tray_->show();
}

AltccentsApp::~AltccentsApp() {
    delete tray_;
    delete trayMenu_;
    delete popup_;

    // Save Cache if not in OneShot mode
    if (!Settings::get(Settings::kOneShotMode).toBool()) {
        writeCacheToFile();
    }

    QObject::~QObject();
};

bool AltccentsApp::loadAccentProfiles(const QString& dir) {
    QList<AccentProfile> profiles{readAccentProfiles(dir)};
    // Check if find any valid profile
    if (profiles.isEmpty()) {
        updateTray();
        setActiveProfile();
        return false;
    }

    loadedAccentProfiles_ = profiles;

    // If found loaded profile with the same accents as active - update active
    // else - set emtpy as active profile
    bool found_active_profile{};
    for (const AccentProfile& i : loadedAccentProfiles_) {
        if (i.accents() == activeAccentProfile_.accents()) {
            setActiveProfile(i);
            found_active_profile = true;
            break;
        }
    }
    if (!found_active_profile) {
        setActiveProfile();
    }

    updateTray();
    return true;
}

void AltccentsApp::loadConfig() {
    Settings::loadSettings();

    for (int i{}; i < static_cast<int>(Settings::kEnumLength); ++i) {
        Settings::SettingsType s{static_cast<Settings::SettingsType>(i)};

        // Handle all settings needed for AltccentsApp here
        switch (s) {
            case Settings::kActiveProfile: {
                QString s_val{Settings::get(s).toString()};
                setActiveProfile(s_val);
                break;
            }
            case Settings::kProgramState: {
                bool s_val{Settings::get(s).toBool()};
                setProgramState(s_val);
                break;
            }
            case Settings::kSaveCache: {
                bool s_val{Settings::get(s).toBool()};
                setSaveCache(s_val);
                break;
            }
            default:
                break;
        }
    }
}

QString AltccentsApp::activeProfileName() const {
    return activeAccentProfile_.name();
}
AccentProfile AltccentsApp::activeProfile() const {
    return activeAccentProfile_;
}

void AltccentsApp::setActiveProfile(const AccentProfile& profile) {
    if (loadedAccentProfiles_.contains(profile)) {
        activeAccentProfile_ = profile;
        emit activeProfileChanged();
    }

    updateTray();
}
// profile param may be profile name or profile file absolute path
void AltccentsApp::setActiveProfile(const QString& profile) {
    for (const AccentProfile& i : loadedAccentProfiles_) {
        if (i.name() == profile || i.filePath() == profile) {
            setActiveProfile(i);
            emit activeProfileChanged();
            return;
        }
    }
}
// Set active profile to None
void AltccentsApp::setActiveProfile() {
    activeAccentProfile_ = {};

    updateTray();

    emit activeProfileChanged();
}

void AltccentsApp::inputAccentNext(bool forward) {
    if (accentInput_.key == Key{} ||
        !activeAccentProfile_.contains(accentInput_.key)) {
        return;
    }

    KeySymbols ks{activeAccentProfile_.accents()[accentInput_.key]};
    qsizetype symbols_count{qMax(ks.lower.count(), ks.upper.count())};

    if (forward) {
        ++accentInput_.index;
        if (accentInput_.index >= symbols_count) accentInput_.index = 0;
    } else {
        --accentInput_.index;
        if (accentInput_.index < 0)
            accentInput_.index = qMax(symbols_count - 1, 0);
    }
}
void AltccentsApp::inputTabNext(bool forward) {
    if (accentInput_.key == Key{} ||
        !activeAccentProfile_.contains(accentInput_.key)) {
        return;
    }

    QList<Key> keys{activeAccentProfile_.accents().keys()};
    if (keys.isEmpty()) {
        return;
    }

    qsizetype keys_count{keys.count()};
    qsizetype key_index{keys.indexOf(accentInput_.key)};

    if (forward) {
        ++key_index;
        if (key_index >= keys_count) key_index = 0;
    } else {
        --key_index;
        if (key_index < 0) key_index = qMax(keys_count - 1, 0);
    }

    Key new_key{keys[key_index]};
    if (new_key == accentInput_.key) {
        return;
    }

    accentInput_.key = new_key;
    accentInput_.index = 0;
}

void AltccentsApp::updateTrayIcon() {
    if (!tray_) {
        return;
    }

    tray_->setIcon(QIcon{programState_ ? Settings::kLogoOnFilePath
                                       : Settings::kLogoFilePath});
}

void AltccentsApp::updateTrayMenu() {
    if (!tray_ || !trayMenu_) {
        return;
    }

    static QAction* insert_profiles_at;
    static QActionGroup* toggle_program_action_group{
        new QActionGroup{trayMenu_}};
    static QActionGroup* save_cache_action_group{new QActionGroup{trayMenu_}};

    // Create all static action HERE
    if (trayMenu_->isEmpty()) {
        //
        QAction* update_from_config{
            new QAction{"Update from config", trayMenu_}};
        QObject::connect(update_from_config, &QAction::triggered,
                         [this]() { this->loadConfig(); });
        trayMenu_->addAction(update_from_config);

        //
        QAction* update_profiles_action{
            new QAction{"Update profiles", trayMenu_}};
        QObject::connect(update_profiles_action, &QAction::triggered,
                         [this]() { this->loadAccentProfiles(); });
        trayMenu_->addAction(update_profiles_action);
        trayMenu_->addSeparator();

        //
        QAction* open_config_dir_action{
            new QAction{"Open config dir", trayMenu_}};
        QObject::connect(open_config_dir_action, &QAction::triggered, []() {
            QDesktopServices::openUrl(Settings::kSettingsDir);
        });
        trayMenu_->addAction(open_config_dir_action);
        trayMenu_->addSeparator();

        //
        QAction* toggle_program_action{
            new QAction{programState() ? "Turn off" : "Turn on", trayMenu_}};
        QObject::connect(toggle_program_action, &QAction::triggered,
                         [this]() { this->toggleProgramState(); });
        toggle_program_action->setActionGroup(toggle_program_action_group);
        trayMenu_->addAction(toggle_program_action);
        trayMenu_->addSeparator();

        //
        QAction* profiles_action{new QAction{"Profiles", trayMenu_}};
        profiles_action->setDisabled(true);
        trayMenu_->addAction(profiles_action);

        // Profile actions will be here

        insert_profiles_at = trayMenu_->addSeparator();

        //
        QAction* save_cache_action{
            new QAction{"Save cache on exit", trayMenu_}};
        save_cache_action->setCheckable(true);
        save_cache_action->setChecked(saveCache_);
        QObject::connect(
            save_cache_action, &QAction::toggled, [save_cache_action, this]() {
                this->saveCache_ = save_cache_action->isChecked();
                Settings::set(Settings::kSaveCache, this->saveCahche());
                Settings::saveSetting(Settings::kSaveCache);
            });
        save_cache_action->setActionGroup(save_cache_action_group);
        save_cache_action_group->setExclusive(false);
        trayMenu_->addAction(save_cache_action);

        //
        trayMenu_->addAction("Exit", []() { QApplication::exit(); });

        tray_->setContextMenu(trayMenu_);
    }  // End of tray menu creation

    // Uodate profile actions
    static QActionGroup* profile_actions_group{new QActionGroup{trayMenu_}};
    profile_actions_group->setExclusionPolicy(
        QActionGroup::ExclusionPolicy::ExclusiveOptional);

    // Update all dynamic actions HERE
    for (auto* i : trayMenu_->actions()) {
        if (i->actionGroup() == profile_actions_group) {
            trayMenu_->removeAction(i);
        }

        if (i->actionGroup() == toggle_program_action_group) {
            i->setText(programState() ? "Turn off" : "Turn on");
        }

        if (i->actionGroup() == save_cache_action_group) {
            i->setChecked(saveCahche());
        }
    }

    // Create all profile actions
    QList<QAction*> profile_action_list{};
    for (const auto& i : loadedAccentProfiles_) {
        QAction* profile_action{new QAction{i.name(), trayMenu_}};
        profile_action->setCheckable(true);
        profile_action->setActionGroup(profile_actions_group);

        if (activeProfileName() == i.name()) {
            profile_action->setChecked(true);
        }

        QObject::connect(profile_action, &QAction::triggered, [&]() {
            if (activeProfile() == i) {
                setActiveProfile();
            } else {
                this->setActiveProfile(i);
            }
        });

        profile_action_list.push_back(profile_action);
    }

    trayMenu_->insertActions(insert_profiles_at, profile_action_list);
}

void AltccentsApp::updateTrayToolTip() {
    if (!tray_) {
        return;
    }

    tray_->setToolTip(Settings::kProgramName +
                      (activeProfile().isEmpty()
                           ? ""
                           : QString{"[%1]"}.arg(activeProfileName())));
}

void AltccentsApp::updateTray(AltccentsApp::updateTrayFlag flags) {
    if (flags & updateTrayFlag::kTrayIcon) updateTrayIcon();
    if (flags & updateTrayFlag::kTrayMenu) updateTrayMenu();
    if (flags & updateTrayFlag::kTrayToolTip) updateTrayToolTip();
}

void AltccentsApp::setProgramState(bool state) {
    if (programState_ == state) {
        return;
    }

    programState_ = state;
    updateTray();

    emit programStateChanged(state);
}
bool AltccentsApp::toggleProgramState() {
    setProgramState(!programState());
    return programState_;
}

// Do nothing if saveCache_ == false
// NOLINTNEXTLINE
void AltccentsApp::writeCacheToFile() {
    if (!saveCahche()) {
        return;
    }

    // Set all cache settings here
    Settings::set(Settings::kActiveProfile, activeProfileName());
    Settings::set(Settings::kProgramState, programState());
    Settings::set(Settings::kSaveCache, saveCahche());

    // Write settings
    Settings::saveCache();
}

void AltccentsApp::setSaveCache(bool val) {
    saveCache_ = val;
    updateTray();
}

void AltccentsApp::popup() {
    if (popup_->isHidden() || accentInput_.isEmpty()) {
        QList<Key> keys{activeAccentProfile_.accents().keys()};

        if (keys.isEmpty()) {
            if (Settings::get(Settings::kOneShotMode).toBool()) {
                // Use QApplication::exit() does not work for some reason
                std::exit(1);
                // QApplication::exit(1);
            }
            return;
        }

        accentInput_ = AccentInput{
            .key = keys[0],
            .is_capital =
                QApplication::keyboardModifiers().testFlag(Qt::ShiftModifier),
            .index = 0};
    }

    const AccentProfile profile{activeProfile()};

    QList<QChar> chars{
        profile.chars(accentInput_.key, accentInput_.is_capital)};

    auto tab_list{tabsFromAccentInput()};

    popup_->show(chars, static_cast<unsigned int>(accentInput_.index),
                 tab_list.first, tab_list.second);
}

QPair<QList<QChar>, unsigned int> AltccentsApp::tabsFromAccentInput() const {
    const auto accents{activeAccentProfile_.accents()};

    QPair<QList<QChar>, unsigned int> tabs{};

    unsigned int i{};
    for (const auto& key : accents.keys()) {
        QChar capital{accents[key].upper.isEmpty() ? '\0'
                                                   : accents[key].upper[0]};
        QChar not_capital{accents[key].lower.isEmpty() ? '\0'
                                                       : accents[key].lower[0]};
        QChar tab{};
        if (accentInput_.is_capital) {
            tab = capital.isNull() ? not_capital : capital;
        } else {
            tab = not_capital.isNull() ? capital : not_capital;
        }
        tabs.first.push_back(tab);

        if (key == accentInput_.key) {
            tabs.second = i;
        }
        ++i;
    }

    return tabs;
}

void AltccentsApp::onPopupHidden() {
    accentInput_ = {};

    if (Settings::get(Settings::kOneShotMode).toBool()) {
        QApplication::quit();
    }
}
void AltccentsApp::onPopupAccentChosen() {
    QChar accent_to_send{activeAccentProfile_.getChar(
        accentInput_.key, accentInput_.is_capital, accentInput_.index)};

    if (accent_to_send.isNull()) {
        popup_->hide();
        return;
    }

    popup_->hide();
    emit charSendRequested(accentInput_.key, accent_to_send);
}
void AltccentsApp::onPopupAccentCopyChosen() {
    QChar accent_to_copy{activeAccentProfile_.getChar(
        accentInput_.key, accentInput_.is_capital, accentInput_.index)};

    if (accent_to_copy.isNull()) {
        popup_->hide();
        return;
    }

    // TODO(clovis): sometimes getting an QXcbClipboard error on LMDE
    QApplication::clipboard()->setText(accent_to_copy);
    popup_->hide();
}
void AltccentsApp::onPopupNextAccent(bool forward) {
    inputAccentNext(forward);
    popup();
}
void AltccentsApp::onPopupNextTab(bool forward) {
    inputTabNext(forward);
    popup();
}
void AltccentsApp::onPopupCapitalChanged(bool is_capital) {
    accentInput_.is_capital = is_capital;
    popup();
}
}  // namespace Altccents
