#pragma once
#include <qtmetamacros.h>

#include <QThread>

#include "Altccents/Altccents.h"

namespace Altccents {
class HookThread : public QThread {
    Q_OBJECT

   public:
    explicit HookThread(QObject* parent);
    ~HookThread() override;

    void setAccentProfile(const AccentProfile& ap);

    void stop();

   signals:
    void popupShouldOpen(Key);

   private:
    void run() override;
    void updateHook();

    AccentProfile ap_;
    Display* d_{};
};

class HookManager : public QObject {
    Q_OBJECT

   public:
    explicit HookManager(AltccentsApp* parent);

   public slots:
    void onProfileChange();
    void openPopup(Key key);

   private:
    HookThread* hook_;
    AltccentsApp* parent_;
};
}  // namespace Altccents
