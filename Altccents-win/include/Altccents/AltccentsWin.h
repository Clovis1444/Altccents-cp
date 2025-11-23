#pragma once

#include <QObject>

#include "Altccents/Altccents.h"

namespace Altccents {
class AltccentsWin : public QObject {
    Q_OBJECT
   public:
    explicit AltccentsWin(AltccentsApp* parent);

   private slots:

   private:
    AltccentsApp* altccents_;
};
}  // namespace Altccents
