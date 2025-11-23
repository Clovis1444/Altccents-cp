#include "Altccents/AltccentsWin.h"

namespace Altccents {
AltccentsWin::AltccentsWin(AltccentsApp* parent)
    : QObject{parent}, altccents_{parent} {
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
    if (code >= 0) {
        // TODO(clovis): use cast that does not emit warnings
        KBDLLHOOKSTRUCT* kb_struct{reinterpret_cast<KBDLLHOOKSTRUCT*>(lparam)};

        if (wparam == WM_KEYDOWN) {
            qInfo() << "HOOK:" << kb_struct->vkCode;
        }

        // To discard events - return non zero instead of CallNexHookEx()
        if (kb_struct->vkCode == VK_OEM_3) {
            qInfo() << "HOOK:" << "Discarding ` key...";
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
