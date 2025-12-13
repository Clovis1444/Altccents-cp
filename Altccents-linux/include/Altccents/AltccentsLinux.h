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
    static void onCharSendRequested(Key, QChar symbol);

   private:
    static bool isProgramInstalled(const QString& p_name);
    static QString getXdotoolKeyArg(QChar symbol);
    static void showDependencyDialog(const QString& p_name,
                                     const QString& install_link);
};
}  // namespace Altccents
