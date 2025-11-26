#include "Altccents/AltccentsWin.h"

namespace Altccents {
AltccentsWin::AltccentsWin(AltccentsApp* parent) : QObject{parent} {
    altccents_ = parent;
    if (!altccents_) {
        qCritical().noquote()
            << "AltccentsWin [ERROR]: "
            << "Failed to initialize AltccentsWin: AltccentsApp* == nullptr!";
        std::exit(1);
    }

    if (altccents_->programState()) {
        setHook();
    }

    // Connects
    connect(altccents_, &AltccentsApp::programStateChanged, this,
            &AltccentsWin::onProgramStateChanged);
}
AltccentsWin::~AltccentsWin() { unsetHook(); }

void AltccentsWin::onProgramStateChanged(bool state) {
    if (state) {
        setHook();
    } else {
        unsetHook();
    }
}

// TODO(clovis): add MODES: one-shot, hook, hotkey
LRESULT CALLBACK AltccentsWin::hook_proc(int code, WPARAM wparam,
                                         LPARAM lparam) {
    if (code >= 0 && (wparam == WM_KEYDOWN || wparam == WM_SYSKEYDOWN) &&
        !altccents_->isPopupOpen()) {
        // NOLINTNEXTLINE
        KBDLLHOOKSTRUCT* kb_struct{reinterpret_cast<KBDLLHOOKSTRUCT*>(lparam)};

        bool is_accent_key{
            altccents_->activeProfile().accents().keys().contains(
                kb_struct->vkCode)};
        bool is_control_key_down{
            isKeyDown(Settings::get(Settings::kControlKey).toInt())};
        if (is_accent_key && is_control_key_down) {
            altccents_->inputSetKey(Key{kb_struct->vkCode});
            altccents_->popup();

            // Discard
            return 1;
        }
    }
    // Call next hook from other apps
    return CallNextHookEx(nullptr, code, wparam, lparam);
}
bool AltccentsWin::setHook() {
    // Do nothing in OneShotMode
    if (Settings::get(Settings::kOneShotMode).toBool()) {
        return false;
    }

    hook_ =
        SetWindowsHookEx(WH_KEYBOARD_LL, &AltccentsWin::hook_proc, nullptr, 0);

    if (hook_ == nullptr) {
        qWarning().noquote()
            << "AltccentsWin [WARNING]: failed to SetWindowsHookEx()";
    }

    return hook_ != nullptr;
}
void AltccentsWin::unsetHook() {
    // Do nothing in OneShotMode
    if (Settings::get(Settings::kOneShotMode).toBool() && hook_ == nullptr) {
        return;
    }

    BOOL r{UnhookWindowsHookEx(hook_)};
    if (r == 0) {
        qWarning().noquote()
            << "AltccentsWin [WARNING]: failed to UnhookWindowsHookEx()";
    }

    hook_ = nullptr;
}

bool AltccentsWin::isKeyDown(int v_key) {
    return (GetAsyncKeyState(v_key) & 0x8000) != 0;
}
}  // namespace Altccents
