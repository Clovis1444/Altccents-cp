#include "Altccents/AccentProfile/AccentProfile.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

namespace {
void critical(const QString& text) {
    qCritical().noquote() << "Altccents::AccentProfile [ERROR]:" << text;
}
void warning(const QString& text) {
    qWarning().noquote() << "Altccents::AccentProfile [WARNING]:" << text;
}
};  // namespace

namespace Altccents {
//
// PUBLIC
//

AccentProfile::AccentProfile(const QByteArray& data, const QFileInfo& fileInfo)
    : fileInfo_{fileInfo} {
    QJsonParseError e{};
    QJsonDocument json{QJsonDocument::fromJson(data, &e)};

    if (e.error != QJsonParseError::NoError) {
        critical(e.errorString());
        return;
    }

    if (!json[kJsonAccentProfileKey].isArray()) {
        QString error{"Failed to find \"%1\" JsonArray key"};
        critical(error.arg(kJsonAccentProfileKey));
        return;
    }

    QJsonArray json_profile{json["AccentProfile"].toArray()};

    for (const auto& i : json_profile) {
        if (!i.isObject()) {
            continue;
        }

        QJsonObject obj{i.toObject()};

        // Key
        qint64 key{obj[kJsonKeyKey].toInteger()};
        if (key == 0) {
            QString error{"Failed to find \"%1\" key in JsonObject"};
            warning(error.arg(kJsonKeyKey));
            continue;
        }

        // Lower
        QJsonArray lower{obj[kJsonLowerKey].toArray()};
        QList<QChar> lower_list{decerializeCharArr(lower)};

        // Upper
        QJsonArray upper{obj[kJsonUpperKey].toArray()};
        QList<QChar> upper_list{decerializeCharArr(upper)};

        if (lower.isEmpty() && upper.isEmpty()) {
            QString error{R"(Both "%1" and "%2" are empty)"};
            warning(error.arg(kJsonLowerKey).arg(kJsonUpperKey));
            continue;
        }

        // Insert accents
        accents_.insert(static_cast<Qt::Key>(key), {lower_list, upper_list});
    }
};

AccentProfile AccentProfile::deserialize(const QByteArray& data,
                                         const QFileInfo& fileInfo) {
    return AccentProfile{data, fileInfo};
};
QByteArray AccentProfile::serialize(const AccentProfile& obj,
                                    QJsonDocument::JsonFormat format) {
    return obj.toQJsonDocument().toJson(format);
};

bool AccentProfile::isEmpty() const { return !accents_.isEmpty(); };

void AccentProfile::printJsonAccentProfileExample() {
    qInfo().noquote() << kJsonAccentProfileExample.trimmed();
}

void AccentProfile::print(const QJsonDocument::JsonFormat& format) const {
    qInfo().noquote() << toQJsonDocument().toJson(format).trimmed();
}

//
// PRIVATE
//

QList<QChar> AccentProfile::decerializeCharArr(const QJsonArray& json_arr) {
    if (json_arr.isEmpty()) {
        return {};
    }

    QList<QChar> list{};
    for (const auto& i : json_arr) {
        QString error{"Error was found in char array!"};

        if (!i.isString()) {
            warning(error);
            continue;
        }

        QString str{i.toString()};

        // Skip if string is not exactly 1 char length
        // if (str.length() != 1) {
        //     warning(error);
        //     continue;
        // }

        list.push_back(str[0]);
    }
    return list;
};

QJsonDocument AccentProfile::toQJsonDocument() const {
    if (accents_.isEmpty()) {
        return {};
    }

    QJsonArray json_profile{};

    for (const auto& i : accents_) {
        QJsonObject obj{};

        obj[kJsonKeyKey] = accents_.key(i);

        QJsonArray lower{};
        for (const QChar& j : i.first) {
            lower.append(QString{j});
        }
        obj[kJsonLowerKey] = lower;

        QJsonArray upper{};
        for (const QChar& j : i.second) {
            upper.append(QString{j});
        }
        obj[kJsonUpperKey] = upper;

        json_profile.append(obj);
    }

    QJsonObject json_root{};
    json_root[kJsonAccentProfileKey] = json_profile;

    QJsonDocument json{json_root};
    return json;
};

};  // namespace Altccents
