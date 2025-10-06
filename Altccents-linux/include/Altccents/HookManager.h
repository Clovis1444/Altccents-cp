#pragma once
#include <qtmetamacros.h>

#include <QThread>

#include "Altccents/Altccents.h"

namespace Altccents {
class HookThread : public QThread {
    Q_OBJECT

   public:
    HookThread(AltccentsApp* parent, AccentProfile ap)
        : QThread{parent}, ap_{std::move(ap)} {}
    ~HookThread() override {
        stopHook();
        QThread::~QThread();
    }

   signals:
    void popupShouldOpen(Key);

   private:
    void run() override;
    void stopHook();

    const AccentProfile ap_;
    Display* d_{};
};

class HookManager : public QObject {
    Q_OBJECT

   public:
    explicit HookManager(AltccentsApp* parent)
        : QObject{parent}, parent_{parent} {
        if (parent == nullptr) {
            qCritical().noquote()
                << "Altccents::HookManager [ERROR]: failed to init "
                   "HookManager. AltccentsApp == nullptr";
            return;
        }

        QObject::connect(parent, &AltccentsApp::activeProfileChanged, this,
                         &HookManager::onProfileChange);

        // First hook setup
        onProfileChange();
    }

   public slots:
    void onProfileChange();
    void openPopup(Key key);

   private:
    HookThread* hook_;
    AltccentsApp* parent_;
};
}  // namespace Altccents
