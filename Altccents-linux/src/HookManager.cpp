#include "Altccents/HookManager.h"

// XLib and Qt have some macro collission so XLib should be included after all
// clang-format off
#include <X11/Xlib.h>
// clang-format on

namespace Altccents {
HookThread::HookThread(QObject* parent) : QThread{parent} {
    d_ = XOpenDisplay(nullptr);

    if (!d_) {
        qCritical().noquote()
            << "Altccents::HookThread [ERROR]: failed to open XDisplay";
        return;
    }
}
HookThread::~HookThread() {
    stop();

    XCloseDisplay(d_);

    QThread::~QThread();
}

void HookThread::setAccentProfile(const AccentProfile& ap) {
    ap_ = ap;
    updateHook();
}

void HookThread::stop() {
    XUngrabKey(d_, AnyKey, AnyModifier, XDefaultRootWindow(d_));
    // Flush is important here. It prevents deadlock
    XFlush(d_);

    requestInterruption();
    wait();

    qInfo().noquote() << "Altccents::HookThread [INFO]: hook was stopped";
}

void HookThread::run() {
    updateHook();

    ///////////////////////////////////////////////////////////////////////
    /////////////////////////////[HOOK LOOP]///////////////////////////////
    ///////////////////////////////////////////////////////////////////////
    XEvent e{};
    while (!isInterruptionRequested()) {
        // If there is no events in event queue - continue
        if (!XPending(d_)) {
            msleep(10);
            continue;
        }

        XNextEvent(d_, &e);

        qInfo() << "Event: " << e.xkey.type << e.xkey.keycode;

        // TODO(clovis): implement logic when to open popup here
        emit popupShouldOpen(Key{static_cast<int>(e.xkey.keycode)});

        // Discard
        // XAllowEvents(d, AsyncKeyboard, CurrentTime);
        // Proceed
        // XAllowEvents(d, ReplayKeyboard, CurrentTime);

        XAllowEvents(d_, ReplayKeyboard, CurrentTime);
    }
    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////[HOOK LOOP END]/////////////////////////////
    ///////////////////////////////////////////////////////////////////////
}

void HookThread::updateHook() {
    // Ungrab all grabbed keys
    XUngrabKey(d_, AnyKey, AnyModifier, XDefaultRootWindow(d_));

    const QList<Key> keys{ap_.accents().keys()};

    // Grab keys
    for (const auto& i : keys) {
        int grab_result{XGrabKey(d_, i.kc(), AnyModifier,
                                 XDefaultRootWindow(d_), 1, GrabModeAsync,
                                 GrabModeSync)};
        if (!grab_result) {
            qCritical().noquote()
                << "Altccents::HookThread [ERROR]: failed to grab key";
        }
    }
}

HookManager::HookManager(AltccentsApp* parent)
    : QObject{parent}, hook_{new HookThread{this}}, parent_{parent} {
    if (parent == nullptr) {
        qCritical().noquote()
            << "Altccents::HookManager [ERROR]: failed to init "
               "HookManager. AltccentsApp == nullptr";
        return;
    }

    QObject::connect(parent, &AltccentsApp::activeProfileChanged, this,
                     &HookManager::onProfileChange);
    QObject::connect(hook_, &HookThread::popupShouldOpen, this,
                     &HookManager::openPopup);
    hook_->setAccentProfile(parent_->activeProfile());
    hook_->start();
}

void HookManager::onProfileChange() {
    hook_->setAccentProfile(parent_->activeProfile());
}

void HookManager::openPopup(Key key) {
    qInfo() << "Key param:" << key.kc();

    parent_->popup();
}
}  // namespace Altccents
