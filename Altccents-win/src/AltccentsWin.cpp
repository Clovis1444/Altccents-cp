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

    // Setup HotkeyManager
    hotkey_manager_ = new HotkeyManager{};
    // Setup hook
    if (altccents_->programState()) {
        setHook();
    }

    // Connects
    connect(altccents_, &AltccentsApp::programStateChanged, this,
            &AltccentsWin::onProgramStateChanged);
    QObject::connect(parent, &AltccentsApp::charSendRequested, this,
                     &AltccentsWin::onCharSendRequested);
}
AltccentsWin::~AltccentsWin() {
    unsetHook();
    delete hotkey_manager_;
}

void AltccentsWin::onProgramStateChanged(bool state) {
    if (state) {
        setHook();
    } else {
        unsetHook();
    }
}
void AltccentsWin::onCharSendRequested(Key, QChar symbol) {
    sendKeyInput(symbol);
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

void AltccentsWin::sendKeyInput(QChar ch) {
    INPUT input[2]{};
    // KEY DOWN
    input[0].type = INPUT_KEYBOARD;
    input[0].ki.dwFlags = KEYEVENTF_UNICODE;
    input[0].ki.wVk = 0;
    input[0].ki.wScan = ch.unicode();
    // KEY UP
    input[1].type = INPUT_KEYBOARD;
    input[1].ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
    input[1].ki.wVk = 0;
    input[1].ki.wScan = ch.unicode();

    // Send input to foreground window
    UINT result{SendInput(2, input, sizeof(INPUT))};

    if (result != 2) {
        qWarning().noquote() << "AltccentsWin [WARNING]: failed to SendInput()";
    }
}

QString AltccentsWin::getWindowTitle(HWND window) {
    wchar_t w_title[256];
    GetWindowTextW(window, w_title, 256);
    return QString::fromWCharArray(w_title);
}
}  // namespace Altccents
