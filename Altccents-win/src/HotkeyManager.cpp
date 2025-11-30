#include "Altccents/HotkeyManager.h"

#include "Altccents/Settings.h"

namespace Altccents {
HotkeyManager::HotkeyManager() : QObject{nullptr} {
    moveToThread(&msg_thread_);

    msg_thread_.start();

    startMsgLoop();

    connect(&Settings::instance(), &Settings::hotkeyChanged, this,
            &HotkeyManager::onSettingsHotkeyChanged, Qt::DirectConnection);
}
HotkeyManager::~HotkeyManager() {
    stopMsgLoop();

    msg_thread_.quit();
    msg_thread_.wait();
}

void HotkeyManager::onSettingsHotkeyChanged() {
    postLoopThreadMsg(kUpdateHotkeyMsg);
}

// TODO(clovis): implement settings option
void HotkeyManager::setHotkey_msg() {
    // Ctrl + Alt + tilde
    BOOL r = {
        RegisterHotKey(nullptr, kHotkeyId, MOD_CONTROL | MOD_ALT, VK_OEM_3)};

    if (r == 0) {
        qWarning() << "HotkeyManager [WARNING]: Failed to RegisterHotKey()";
    }
}
void HotkeyManager::unsetHotkey_msg() { UnregisterHotKey(nullptr, kHotkeyId); }

void HotkeyManager::msgLoop_msg() {
    // Get msgLoop thread id
    msgLoopThreadId_ = static_cast<DWORD>(
        reinterpret_cast<DWORD_PTR>(msg_thread_.currentThreadId()));
    // Initial hotkey setup
    unsetHotkey_msg();
    setHotkey_msg();
    //

    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0) != 0) {
        switch (msg.message) {
            case WM_HOTKEY: {
                emit hotkeyTriggered();
                break;
            }
            case kUpdateHotkeyMsg: {
                unsetHotkey_msg();
                setHotkey_msg();
                break;
            }
            case kStopMsgLoopMsg: {
                unsetHotkey_msg();
                return;
            }
            default: {
                break;
            }
        }
    }
}
void HotkeyManager::startMsgLoop() {
    if (is_msg_loop_running_) {
        return;
    }

    QMetaObject::invokeMethod(this, &HotkeyManager::msgLoop_msg);
    is_msg_loop_running_ = true;
}
void HotkeyManager::stopMsgLoop() {
    if (!is_msg_loop_running_) {
        return;
    }

    postLoopThreadMsg(kStopMsgLoopMsg);

    is_msg_loop_running_ = false;
}

// NOLINTNEXTLINE
void HotkeyManager::postLoopThreadMsg(UINT msg) {
    PostThreadMessageW(msgLoopThreadId_, msg, 0, 0);
}
}  // namespace Altccents
