#include "Altccents/HotkeyManager.h"

#include "Altccents/AccentProfile/AccentProfile.h"
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

void HotkeyManager::setHotkey_msg() {
    // NOTE(clovis): fore more info see
    // https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-registerhotkey
    QList<Key> keys{AccentProfile::keysFromString(
        Settings::get(Settings::kHotkey).toString())};

    if (keys.isEmpty()) {
        qWarning() << "HotkeyManager [WARNING]: Failed to RegisterHotKey()";
        return;
    }

    const QList<Key> mod_alt{Key{VK_MENU}, Key{VK_LMENU}, Key{VK_RMENU}};
    const QList<Key> mod_ctrl{Key{VK_CONTROL}, Key{VK_LCONTROL},
                              Key{VK_RCONTROL}};
    const QList<Key> mod_shift{Key{VK_SHIFT}, Key{VK_LSHIFT}, Key{VK_RSHIFT}};
    const QList<Key> mod_win{Key{VK_LWIN}, Key{VK_RWIN}};
    // Check for modifiers and remove them from keys list
    UINT modifiers{MOD_NOREPEAT};
    for (auto i{keys.begin()}; i != keys.end();) {
        // MOD_ALT
        if (mod_alt.contains(*i)) {
            modifiers |= MOD_ALT;
            i = keys.erase(i);
        }
        // MOD_CONTROL
        else if (mod_ctrl.contains(*i)) {
            modifiers |= MOD_CONTROL;
            i = keys.erase(i);
        }
        // MOD_SHIFT
        else if (mod_shift.contains(*i)) {
            modifiers |= MOD_SHIFT;
            i = keys.erase(i);
        }
        // MOD_WIN
        else if (mod_win.contains(*i)) {
            modifiers |= MOD_WIN;
            i = keys.erase(i);
        } else {
            ++i;
        }
    }

    if (keys.isEmpty()) {
        qWarning() << "HotkeyManager [WARNING]: Failed to RegisterHotKey()";
        return;
    }
    // NOTE(clovis): use first remaining key
    UINT hotkey_key{static_cast<UINT>(keys.first().kc())};

    // Do hotkey registration
    // NOTE(clovis): it is possible to register hotkey without modifiers
    BOOL r = {RegisterHotKey(nullptr, kHotkeyId, modifiers, hotkey_key)};

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
