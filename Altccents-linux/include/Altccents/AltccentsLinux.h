#pragma once

#include <qobject.h>
#include <qtmetamacros.h>

#include "Altccents/Altccents.h"

namespace Altccents {
class AltccentsLinux : public QObject {
    Q_OBJECT

   public:
    explicit AltccentsLinux(AltccentsApp* parent);

   private slots:
    void onCharSendRequested(Key key, QChar symbol);

   private:
    static bool isProgramInstalled(const QString& p_name);
};
}  // namespace Altccents
