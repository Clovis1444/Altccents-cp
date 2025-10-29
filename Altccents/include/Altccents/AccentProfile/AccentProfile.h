#pragma once

#include <qfileinfo.h>
#include <qhash.h>
#include <qhashfunctions.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qlist.h>
#include <qnamespace.h>
#include <qstringview.h>

namespace Altccents {
class Key {
   public:
    constexpr Key() : keycode_{-1} {}
    constexpr explicit Key(const int& keycode) {
        if (keycode < 0 || keycode > 255) {
            keycode_ = -1;
            return;
        }

        keycode_ = keycode;
    }

    bool operator==(const Key& other) const {
        return keycode_ == other.keycode_;
    }

    // Returns -1 if an object is invalid
    int kc() const {
        if (keycode_ < 0 || keycode_ > 255) {
            return -1;
        }
        return keycode_;
    }

   private:
    int keycode_{};
};
constexpr size_t qHash(const Key& key, size_t = 0) {
    return static_cast<size_t>(key.kc());
}
constexpr QDebug& operator<<(QDebug& q_debug, const Key& key) {
    return q_debug.noquote() << QString{"Key{%1}"}.arg(key.kc());
}

struct KeySymbols {
    QList<QChar> lower;
    QList<QChar> upper;

    constexpr bool operator==(const KeySymbols& other) const {
        return lower == other.lower && upper == other.upper;
    }
};

class AccentProfile {
   public:
    AccentProfile(const QByteArray& data, const QFileInfo& fileInfo);
    AccentProfile() = default;

    bool operator==(const AccentProfile& other) const {
        return accents_ == other.accents_ && fileInfo_ == other.fileInfo_;
    }

    static AccentProfile deserialize(const QByteArray& data,
                                     const QFileInfo& fileInfo);
    static QByteArray serialize(
        const AccentProfile& obj,
        QJsonDocument::JsonFormat format = QJsonDocument::Indented);

    QFileInfo fileInfo() const { return fileInfo_; }
    QHash<Key, KeySymbols> accents() const { return accents_; };
    QString filePath() const { return fileInfo_.absoluteFilePath(); }
    QString name() const { return fileInfo_.baseName(); }

    bool isEmpty() const;

    static void printJsonAccentProfileExample();
    void print(const QJsonDocument::JsonFormat& format =
                   QJsonDocument::Indented) const;

    bool contains(Key key) const { return accents_.contains(key); };
    QList<QChar> chars(Key key, bool is_capital) const {
        return is_capital ? accents_[key].upper : accents_[key].lower;
    };

   private:
    static QList<QChar> decerializeCharArr(const QJsonArray& json_arr);

    QJsonDocument toQJsonDocument() const;

    inline static const QString kJsonAccentProfileKey{"AccentProfile"};
    inline static const QString kJsonKeyKey{"Key"};
    inline static const QString kJsonLowerKey{"Lower"};
    inline static const QString kJsonUpperKey{"Upper"};
    inline static const QString kJsonAccentProfileExample{
        QString{R"(
    "%1": [
         {
             "%2": 123,
             "%3": ["a", "b"],
             "%4": ["A", "B"],
         },
         ...
    ]
    )"}
            .arg(kJsonAccentProfileKey)
            .arg(kJsonKeyKey)
            .arg(kJsonLowerKey)
            .arg(kJsonUpperKey)};

    QHash<Key, KeySymbols> accents_;
    QFileInfo fileInfo_;
};
};  // namespace Altccents
