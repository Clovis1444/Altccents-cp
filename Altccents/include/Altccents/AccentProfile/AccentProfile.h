#pragma once

#include <qhash.h>
#include <qhashfunctions.h>
#include <qlist.h>
#include <qnamespace.h>
#include <qstringview.h>

namespace Altccents {
class AccentProfile {
   public:
    explicit AccentProfile(const QHash<Qt::Key, QList<QChar>>& accents)
        : accents_{accents} {}
    explicit AccentProfile(const QByteArray& data);

    static AccentProfile Deserialize(const QByteArray& data);
    static QByteArray Serialize(const AccentProfile& obj);

    bool isValid();

   private:
    QHash<Qt::Key, QList<QChar>> accents_;
};
};  // namespace Altccents
