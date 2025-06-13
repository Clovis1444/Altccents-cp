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
    // Check if dir exists
    if (!dir.exists()) {
        QString e{"Directory %1 does not exist"};
        warning(e.arg(dir.absolutePath()));
        return {};
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
}  // namespace Altccents
