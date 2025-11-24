#include "Altccents/AltccentsWin.h"

#include <QElapsedTimer>

namespace Altccents {
AltccentsWin::AltccentsWin(AltccentsApp* parent) : QObject{parent} {
    altccents_ = parent;
    if (!altccents_) {
        qCritical().noquote()
            << "AltccentsWin [ERROR]: "
            << "Failed to initialize AltccentsWin: AltccentsApp* == nullptr!";
        std::exit(1);
    }

    setHook();

    qInfo() << "Hello from AltccentsWin";
}
AltccentsWin::~AltccentsWin() {
    unsetHook();
    QObject::~QObject();
}

LRESULT CALLBACK AltccentsWin::hook_proc(int code, WPARAM wparam,
                                         LPARAM lparam) {
    if (code >= 0 && wparam == WM_KEYDOWN) {
        // NOLINTNEXTLINE
        KBDLLHOOKSTRUCT* kb_struct{reinterpret_cast<KBDLLHOOKSTRUCT*>(lparam)};

        QElapsedTimer timer{};
        timer.start();
        bool is_accent_key{
            altccents_->activeProfile().accents().keys().contains(
                kb_struct->vkCode)};
        bool is_control_key_down{
            (GetAsyncKeyState(Settings::get(Settings::kControlKey).toInt()) &
             0x8000) != 0};
        // qInfo() << "Control key: " << is_control_key_down;
        // qInfo() << "Accent key: " << is_accent_key;
        // TODO(clovis): implement this using double tap? And change ControlKey
        // to Hotkey
        if (is_accent_key && is_control_key_down) {
            auto time{timer.nsecsElapsed()};
            qInfo() << "Elapsed time:" << time;
            qInfo() << "Trigger popup here!";

            // Discard
            return 1;
        }
    }
    // Call next hook from other apps
    return CallNextHookEx(nullptr, code, wparam, lparam);
}
bool AltccentsWin::setHook() {
    hook_ =
        SetWindowsHookEx(WH_KEYBOARD_LL, &AltccentsWin::hook_proc, nullptr, 0);

    if (hook_ == nullptr) {
        qWarning().noquote()
            << "AltccentsWin [WARNING]: failed to SetWindowsHookEx()";
    }

    return hook_ != nullptr;
}
void AltccentsWin::unsetHook() { UnhookWindowsHookEx(hook_); }
}  // namespace Altccents
