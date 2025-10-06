#include "Altccents/HookManager.h"

// XLib and Qt have some macro collission so XLib should be included after all
// clang-format off
#include <X11/Xlib.h>
// clang-format on

namespace Altccents {
void HookThread::run() {
    // If profile is empty - do not install hook
    if (ap_.isEmpty()) {
        return;
    }

    const QList<Key> keys{ap_.accents().keys()};

    d_ = XOpenDisplay(nullptr);

    if (!d_) {
        qCritical().noquote()
            << "Altccents::HookThread [ERROR]: failed to open XDisplay";
        return;
    }

    Window root_w{XDefaultRootWindow(d_)};

    for (const auto& i : keys) {
        int grab_result{XGrabKey(d_, i.kc(), AnyModifier, root_w, 1,
                                 GrabModeAsync, GrabModeSync)};
        if (!grab_result) {
            qCritical().noquote()
                << "Altccents::HookThread [ERROR]: failed to grab key";

            stopHook();

            return;
        }
    }

    ///////////////////////////////////////////////////////////////////////
    /////////////////////////////[HOOK LOOP]///////////////////////////////
    ///////////////////////////////////////////////////////////////////////
    XEvent e{};
    while (true) {
        XNextEvent(d_, &e);

        qInfo() << "Event: " << e.xkey.type << e.xkey.keycode;

        // TODO(clovis): emit "open popup" signal here
        emit popupShouldOpen(Key{static_cast<int>(e.xkey.keycode)});

        // Discard
        // XAllowEvents(d, AsyncKeyboard, CurrentTime);
        // Proceed
        // XAllowEvents(d, ReplayKeyboard, CurrentTime);

        XAllowEvents(d_, ReplayKeyboard, CurrentTime);
    }

    qInfo() << "Loop end";
}

void HookThread::stopHook() {
    XUngrabKey(d_, AnyKey, AnyModifier, XDefaultRootWindow(d_));
    XCloseDisplay(d_);
}

void HookManager::onProfileChange() {
    // if (hook_ != nullptr) {
    //     // hook_->quit();
    //     // hook_->wait();
    // }
    delete hook_;

    if (parent_ != nullptr) {
        AccentProfile ap{parent_->activeProfile()};
        if (ap.isEmpty()) {
            return;
        }

        qInfo() << ap.name();
        hook_ = new HookThread{parent_, ap};

        QObject::connect(hook_, &HookThread::popupShouldOpen, this,
                         &HookManager::openPopup);

        // Start hook loop
        hook_->start();
    }
}

void HookManager::openPopup(Key key) {
    qInfo() << "Key param:" << key.kc();

    parent_->popup();
}
}  // namespace Altccents
