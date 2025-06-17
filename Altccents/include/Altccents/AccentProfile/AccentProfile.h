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
    QHash<Qt::Key, QPair<QList<QChar>, QList<QChar>>> accents() const {
        return accents_;
    };
    QString filePath() const { return fileInfo_.absoluteFilePath(); }
    QString name() const { return fileInfo_.baseName(); }

    bool isEmpty() const;

    static void printJsonAccentProfileExample();
    void print(const QJsonDocument::JsonFormat& format =
                   QJsonDocument::Indented) const;

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

    // QHas<Key QPair<lower, upper>>
    QHash<Qt::Key, QPair<QList<QChar>, QList<QChar>>> accents_;
    QFileInfo fileInfo_;
};
};  // namespace Altccents
