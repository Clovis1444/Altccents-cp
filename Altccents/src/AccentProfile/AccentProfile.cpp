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

    const QJsonArray& json_profile{json[kJsonAccentProfileKey].toArray()};

    for (const auto& i : json_profile) {
        if (!i.isObject()) {
            continue;
        }

        QJsonObject obj{i.toObject()};

        // Key
        // First, try to read value as int
        int keycode{obj[kJsonKeyKey].toInt(-1)};
        Key key{keycode};
        // Second, try to read value as String
        if (keycode == -1) {
            key = keyFromString(obj[kJsonKeyKey].toString());
        }

        // If keycode is out of bounds
        if (key.kc() == -1) {
            QString error{
                "Wrong key: \"%1\". Key value must be in [0..255] range"};
            warning(error.arg(kJsonKeyKey));
            continue;
        }

        // Lower and Upper
        const QJsonArray& lower{obj[kJsonLowerKey].toArray()};
        const QJsonArray& upper{obj[kJsonUpperKey].toArray()};

        // Check IF lower and upper is the same size OR one is empty and other
        // is not empty
        if (lower.count() != upper.count() &&
            !(lower.isEmpty() != upper.isEmpty())) {
            QString error{
                R"(Both "%1" and "%2" should be the same size OR one should be empty while other is not)"};
            warning(error.arg(kJsonLowerKey).arg(kJsonUpperKey));
            continue;
        }
        if (lower.isEmpty() && upper.isEmpty()) {
            QString error{R"(Both "%1" and "%2" are empty)"};
            warning(error.arg(kJsonLowerKey).arg(kJsonUpperKey));
            continue;
        }

        QList<QChar> lower_list{decerializeCharArr(lower)};
        QList<QChar> upper_list{decerializeCharArr(upper)};

        // Insert accents
        accents_.insert(key, {.lower = lower_list, .upper = upper_list});
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

bool AccentProfile::isEmpty() const { return accents_.isEmpty(); };

void AccentProfile::printJsonAccentProfileExample() {
    qInfo().noquote() << kJsonAccentProfileExample.trimmed();
}

void AccentProfile::print(const QJsonDocument::JsonFormat& format) const {
    qInfo().noquote() << toQJsonDocument().toJson(format).trimmed();
}

QList<QChar> AccentProfile::chars(const Key& key, bool is_capital) const {
    if (!accents_.contains(key)) {
        return {};
    }

    const KeySymbols& symbols{accents_[key]};

    if (symbols.lower.count() == symbols.upper.count()) {
        return is_capital ? symbols.upper : symbols.lower;
    }

    return symbols.lower.count() > symbols.upper.count() ? symbols.lower
                                                         : symbols.upper;
}
QChar AccentProfile::getChar(const Key& key, bool is_capital,
                             qsizetype index) const {
    if (!accents_.contains(key) || index < 0) {
        return {};
    }

    QList<QChar> list{chars(key, is_capital)};

    if (qMax(list.count() - 1, 0) < index) {
        return {};
    }

    return list[index];
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

        obj[kJsonKeyKey] = accents_.key(i).kc();

        QJsonArray lower{};
        for (const QChar& j : i.lower) {
            lower.append(QString{j});
        }
        obj[kJsonLowerKey] = lower;

        QJsonArray upper{};
        for (const QChar& j : i.upper) {
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

Key AccentProfile::keyFromString(const QString& key) {
    // Step 1: check if str contains int
    bool to_int_r{};
    // Supports hex -> bin -> oct -> dec
    int int_from_str{key.toInt(&to_int_r, 0)};
    if (to_int_r) {
        return Key{int_from_str};
    }

    // Step 2: try to convert str to int
    if (kVkMap.contains(key.toLower())) {
        return Key{kVkMap[key.toLower()]};
    }

    return {};
}
QList<Key> AccentProfile::keysFromString(QString keys) {
    if (keys.isEmpty()) {
        return {};
    }

    keys = keys.simplified().toLower().remove(' ');
    QList<QString> str_list{keys.split('+', Qt::SkipEmptyParts).toList()};

    QList<Key> key_list;
    for (const auto& i : str_list) {
        Key key{keyFromString(i)};
        if (key != Key{}) {
            key_list.push_back(key);
        }
    }

    return key_list;
}
// TODO(clovis): create printKeys option
void AccentProfile::printVkMap() {
    QString first_col_name{"Key name"};
    QString second_col_name{"Key value"};

    // Determine max key length to align print output
    qsizetype max_first_col_length{first_col_name.length()};
    qsizetype max_second_col_length{second_col_name.length()};
    for (auto i{kVkMap.begin()}; i != kVkMap.end(); ++i) {
        if (max_first_col_length < i.key().length()) {
            max_first_col_length = i.key().length();
        }
    }

    // Print title
    qInfo().noquote().nospace()
        << first_col_name.leftJustified(max_first_col_length) << " | "
        << second_col_name.leftJustified(max_second_col_length);
    // Print separator
    qInfo().noquote().nospace()
        << QString{}.leftJustified(max_first_col_length, '-') << "-+-"
        << QString{}.leftJustified(max_second_col_length, '-');
    // Sort keys
    QList<QString> keys{kVkMap.keys()};
    std::sort(keys.begin(), keys.end(), [](const QString& a, const QString& b) {
        return kVkMap[a] < kVkMap[b];
    });
    // Do print
    for (const auto& i : keys) {
        qInfo().noquote().nospace()
            << i.leftJustified(max_first_col_length) << " | " << kVkMap[i];
    }
}

};  // namespace Altccents
