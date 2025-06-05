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
};  // namespace

// PUBLIC
namespace Altccents {
void ReadAccentProfiles(const QString& dir_path) {
    QDir dir{dir_path};
    // Check if dir exists
    if (!dir.exists()) {
        QString e{"Directory %1 does not exist"};
        warning(e.arg(dir.absolutePath()));
        return;
    }

    dir.setFilter(QDir::Files);
    dir.setNameFilters(QList<QString>{"*.json"});

    QList<QFileInfo> jsons{dir.entryInfoList()};

    // Check if at least 1 json was found
    if (jsons.isEmpty()) {
        QString e{"No profiles in %1"};
        warning(e.arg(dir.absolutePath()));
        return;
    }

    QList<AccentProfile> profiles{};

    // Deserialize profiles
    for (const auto& i : jsons) {
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

        AccentProfile profile{AccentProfile::Deserialize(file_data)};

        // Check if profile is Valid
        if (profile.isValid()) {
            profiles.push_back(profile);
        }
    }

    // TODO(Clovis): do something with profiles
    qInfo() << "Number of valid profiles:" << profiles.count();
}
}  // namespace Altccents
