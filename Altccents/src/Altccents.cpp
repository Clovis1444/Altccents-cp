#include "Altccents/Altccents.h"

#include <qlist.h>

#include <QDebug>
#include <QDir>
#include <QFile>

#include "Altccents/AccentProfile/AccentProfile.h"

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
        if (!QDir().mkdir(dir.absolutePath())) {
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
        if (profile.isEmpty()) {
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

AltccentsApp::AltccentsApp() {
    // Accent Profiles MUST be loaded before config
    loadAccentProfiles();
    loadConfig();
}

bool AltccentsApp::loadAccentProfiles(const QString& dir) {
    QList<AccentProfile> profiles{readAccentProfiles(dir)};
    // Check if find any valid profile
    if (profiles.isEmpty()) {
        return false;
    }

    loadedAccentProfiles_ = profiles;

    // If found loaded profile with the same accents as active - update active
    // else - set emtpy as active profile
    bool found_active_profile{};
    for (const AccentProfile& i : loadedAccentProfiles_) {
        if (i.accents() == activeAccentProfile_.accents()) {
            activeAccentProfile_ = i;
            found_active_profile = true;
            break;
        }
    }
    if (!found_active_profile) {
        activeAccentProfile_ = {};
    }

    return true;
}

void AltccentsApp::loadConfig() {
    Settings::loadSettings();

    for (int i{}; i < static_cast<int>(Settings::kEnumLength); ++i) {
        Settings::SettingsType s{static_cast<Settings::SettingsType>(i)};

        switch (s) {
            case Settings::kActiveProfile: {
                QString s_val{Settings::get(s).toString()};
                setActiveProfile(s_val);
                break;
            }
            case Settings::kEnumLength: {
                break;
            }
        }
    }
}

QString AltccentsApp::activeProfileName() const {
    return activeAccentProfile_.name();
}

void AltccentsApp::setActiveProfile(const AccentProfile& profile) {
    if (loadedAccentProfiles_.contains(profile)) {
        activeAccentProfile_ = profile;
    }
}
// profile param may be profile name or profile file absolute path
void AltccentsApp::setActiveProfile(const QString& profile) {
    for (const AccentProfile& i : loadedAccentProfiles_) {
        if (i.name() == profile || i.filePath() == profile) {
            setActiveProfile(i);
            return;
        }
    }
}

QChar AltccentsApp::nextAccent(const Qt::Key& key, bool is_capital) {
    // If there is no key in the profile - return
    if (!activeAccentProfile_.contains(key)) {
        lastAccent_ = {};
        return {};
    }

    QList<QChar> chars{activeAccentProfile_.chars(key, is_capital)};

    // If there is no chars for this Case - return
    if (chars.isEmpty()) {
        lastAccent_ = {};
        return {};
    }

    // If AccentInput is the same as last one - just change index
    if (lastAccent_.key == key && lastAccent_.is_capital == is_capital) {
        int index{
            lastAccent_.index >= chars.count() - 1 ? 0 : lastAccent_.index + 1};

        lastAccent_.index = index;
        return chars[index];
    }
    // Otherwise - return char with index 0
    lastAccent_ = {key, is_capital, 0};
    return chars[0];
}

}  // namespace Altccents
