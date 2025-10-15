#include "Altccents/HookManager.h"
// XLib and Qt have some macro collission so XLib should be included AFTER all
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
    // Do nothing if there is no keys to grab
    if (ap_.accents().keys().count() == 0) {
        return;
    }

    if (!updateHook()) {
        return;
    }

    ///////////////////////////////////////////////////////////////////////
    /////////////////////////////[HOOK LOOP]///////////////////////////////
    ///////////////////////////////////////////////////////////////////////
    qInfo().noquote() << "Altccents::HookThread [INFO]: hook was started";
    XEvent e{};
    while (!isInterruptionRequested()) {
        // If there is no events in event queue - continue
        if (!XPending(d_)) {
            msleep(10);
            continue;
        }

        XNextEvent(d_, &e);

        qInfo() << "Event: " << e.xkey.type << e.xkey.keycode;

        /// TODO(clovis)
        XAllowEvents(d_, ReplayKeyboard, CurrentTime);
        simulateKeyPress(e.xkey.keycode);
        XAllowEvents(d_, AsyncKeyboard, CurrentTime);
        continue;
        ///

        if (isControlKeyDown()) {
            // TODO(clovis): open popup here

            // emit popupShouldOpen(Key{static_cast<int>(e.xkey.keycode)});
            // continue;
        }

        // Discard
        // XAllowEvents(d_, AsyncKeyboard, CurrentTime);
        // Proceed
        // XAllowEvents(d_, ReplayKeyboard, CurrentTime);

        // Proceed key event
        XAllowEvents(d_, ReplayKeyboard, CurrentTime);
    }
    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////[HOOK LOOP END]/////////////////////////////
    ///////////////////////////////////////////////////////////////////////
}

bool HookThread::updateHook() {
    // Ungrab all grabbed keys
    XUngrabKey(d_, AnyKey, AnyModifier, XDefaultRootWindow(d_));

    const QList<Key> keys{ap_.accents().keys()};

    // Grab keys
    // NOLINTNEXTLINE
    for (const auto& i : keys) {
        int grab_result{XGrabKey(d_, i.kc(), AnyModifier,
                                 XDefaultRootWindow(d_), 1, GrabModeAsync,
                                 GrabModeSync)};
        if (!grab_result) {
            qCritical().noquote()
                << "Altccents::HookThread [ERROR]: failed to grab key";
            return false;
        }
    }

    return true;
}

bool HookThread::isControlKeyDown() const {
    int control_key{Settings::get(Settings::kControlKey).toInt()};

    // If control_key is out of bounds
    if (control_key < 0 || control_key >= 256) {
        return false;
    }

    // Each bit represent each key. 32 * 8bits(256bits) == 256 keys
    char keys[32]{};
    XQueryKeymap(d_, keys);

    bool control_key_down{
        static_cast<bool>(keys[control_key / 8] & (1 << control_key % 8))};

    return control_key_down;
}

void HookThread::sendKeyEvent(uint64_t window, uint32_t keycode,
                              bool is_press_event, uint64_t delay) {
    XEvent e{};
    e.xkey.window = window;
    e.xkey.keycode = keycode;
    // TODO(clovis): handle modifiers here
    e.xkey.state = 0;
    e.xkey.type = is_press_event ? KeyPress : KeyRelease;

    int64_t e_mask{is_press_event ? KeyPressMask : KeyReleaseMask};

    int result{XSendEvent(d_, window, 1, e_mask, &e)};

    // If XSendEvent() fails
    if (result != 1) {
        qWarning().noquote()
            << "Altccents::HookThread [ERROR]: failed to XSendEvent()";
        return;
    }

    XFlush(d_);

    if (delay > 0) {
        usleep(delay);
    }
}
// TODO(clovis): add keymap param
void HookThread::simulateKeyPress(uint32_t keycode) {
    Window w{};
    int revert_to{};
    XGetInputFocus(d_, &w, &revert_to);

    sendKeyEvent(w, keycode, true, 100);
    sendKeyEvent(w, keycode, false);
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
    QObject::connect(parent, &AltccentsApp::programStateChanged, this,
                     &HookManager::onProgramStateChanged);

    hook_->setAccentProfile(parent_->activeProfile());
    hook_->start();
}

void HookManager::onProfileChange() {
    hook_->setAccentProfile(parent_->activeProfile());
}

void HookManager::onProgramStateChanged(bool new_state) {
    if (hook_ == nullptr) {
        return;
    }

    new_state ? hook_->start() : hook_->stop();
}

void HookManager::openPopup(Key key) {
    qInfo() << "Key param:" << key.kc();

    parent_->popup();
}
}  // namespace Altccents
