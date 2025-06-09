#pragma once

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
    explicit AccentProfile(
        const QHash<Qt::Key, QPair<QList<QChar>, QList<QChar>>>& accents)
        : accents_{accents} {}
    explicit AccentProfile(const QByteArray& data);

    static AccentProfile Deserialize(const QByteArray& data);
    static QByteArray Serialize(
        const AccentProfile& obj,
        QJsonDocument::JsonFormat format = QJsonDocument::Indented);

    bool isValid() const;
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
};
};  // namespace Altccents
