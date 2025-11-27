#include "Altccents/HotkeyManager.h"

#include <QDebug>

namespace Altccents {
HotkeyManager::HotkeyManager() : QObject{nullptr} {
    moveToThread(&msg_thread_);

    msg_thread_.start();

    updateHotkey();
}
HotkeyManager::~HotkeyManager() {
    stopMsgLoop();

    QMetaObject::invokeMethod(this, &HotkeyManager::unsetHotkey_msg,
                              Qt::BlockingQueuedConnection);

    msg_thread_.quit();
    msg_thread_.wait();
}

// TODO(clovis): implement runtime update
// There might be issues when calling this function second time. Need to check
void HotkeyManager::updateHotkey() {
    stopMsgLoop();

    QMetaObject::invokeMethod(this, &HotkeyManager::unsetHotkey_msg);
    QMetaObject::invokeMethod(this, &HotkeyManager::setHotkey_msg);

    startMsgLoop();
}

// TODO(clovis): implement settings option
void HotkeyManager::setHotkey_msg() {
    qInfo() << "set_hotkey: " << QThread::currentThreadId();
    // Ctrl + Alt + tilde
    BOOL r = {
        RegisterHotKey(nullptr, kHotkeyId, MOD_CONTROL | MOD_ALT, VK_OEM_3)};

    if (r == 0) {
        qWarning() << "HotkeyManager [WARNING]: Failed to RegisterHotKey()";
    }
}
void HotkeyManager::unsetHotkey_msg() {
    qInfo() << "unset_hotkey: " << QThread::currentThreadId();
    UnregisterHotKey(nullptr, kHotkeyId);
}

void HotkeyManager::msgLoop_msg() {
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0) != 0) {
        switch (msg.message) {
            case WM_HOTKEY: {
                qInfo() << "HOTKEY: " << QThread::currentThreadId();
                emit hotkeyTriggered();
                break;
            }
            case kStopMsgLoopMsg: {
                qInfo() << "HOTKEY QUIT: " << QThread::currentThreadId();
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

    auto thread_id = reinterpret_cast<DWORD_PTR>(msg_thread_.currentThreadId());
    PostThreadMessageW(static_cast<DWORD>(thread_id), kStopMsgLoopMsg, 0, 0);
    is_msg_loop_running_ = false;
}
}  // namespace Altccents
