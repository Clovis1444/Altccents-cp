#include "Altccents/HotkeyManager.h"

#include "Altccents/AccentProfile/AccentProfile.h"

namespace Altccents {
HotkeyManager::HotkeyManager() : QObject{nullptr} {
    moveToThread(&msg_thread_);

    msg_thread_.start();

    startMsgLoop();

  // Hotkey signals connect
    for (const Hotkey& i : kHotkeys) {
        connect(
            &Settings::instance(), i.setting_signal, this,
            [this, i]() { onHotkeyChanged(i.id); }, Qt::DirectConnection);
    }
}
HotkeyManager::~HotkeyManager() {
    stopMsgLoop();

    msg_thread_.quit();
    msg_thread_.wait();
}

void HotkeyManager::onHotkeyChanged(int hotkey_id) {
    if (kHotkeys.contains(hotkey_id)) {
        postLoopThreadMsg(kHotkeys[hotkey_id].update_msg);
    }
}

void HotkeyManager::setHotkey_msg(int hotkey_id) {
    if (!kHotkeys.contains(hotkey_id)) {
        qWarning() << QString{
            "HotkeyManager [WARNING]: there is no hotkey with id %1 in "
            "kHotkeys"};
        return;
    }
    const Hotkey& hk{kHotkeys[hotkey_id]};

    // NOTE(clovis): fore more info see
    // https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-registerhotkey
    QList<Key> keys{
        AccentProfile::keysFromString(Settings::get(hk.setting).toString())};

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
    BOOL r = {RegisterHotKey(nullptr, hk.id, modifiers, hotkey_key)};

    if (r == 0) {
        qWarning() << "HotkeyManager [WARNING]: Failed to RegisterHotKey()";
    }
}
void HotkeyManager::unsetHotkey_msg(int hotkey_id) {
    if (hotkey_id == 0) {
        for (const Hotkey& i : kHotkeys) {
            UnregisterHotKey(nullptr, i.id);
        }
        return;
    }

    UnregisterHotKey(nullptr, hotkey_id);
}

void HotkeyManager::msgLoop_msg() {
    // Get msgLoop thread id
    msgLoopThreadId_ = static_cast<DWORD>(
        reinterpret_cast<DWORD_PTR>(msg_thread_.currentThreadId()));
    // Initial hotkey setup
    unsetHotkey_msg();
    for (const Hotkey& i : kHotkeys) {
        setHotkey_msg(i.id);
    }
    //

    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0) != 0) {
        // First check for hotkey update msgs
        for (const Hotkey& i : kHotkeys) {
            if (msg.message == i.update_msg) {
                unsetHotkey_msg(i.id);
                setHotkey_msg(i.id);
                continue;
            }
        }
        // Second check for other msgs
        switch (msg.message) {
            case WM_HOTKEY: {
                int hk_id{static_cast<int>(msg.wParam)};
                // Emit hotkey signal
                if (kHotkeys.contains(hk_id)) {
                    kHotkeys[hk_id].signal(this);
                }
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

const QHash<int, HotkeyManager::Hotkey> HotkeyManager::kHotkeys{
    {kPopupHotkeyId, Hotkey{.id = kPopupHotkeyId,
                            .setting = Settings::kPopupHotkey,
                            .setting_signal = &Settings::popupHotkeyChanged,
                            .update_msg = kUpdatePopupHotkeyMsg,
                            .signal =
                                [](HotkeyManager* hm) {
                                    if (hm) emit hm->popupHotkeyTriggered();
                                }}},
    {kToggleHotkeyId, Hotkey{.id = kToggleHotkeyId,
                             .setting = Settings::kToggleHotkey,
                             .setting_signal = &Settings::toggleHotkeyChanged,
                             .update_msg = kUpdateToggleHotkeyMsg,
                             .signal =
                                 [](HotkeyManager* hm) {
                                     if (hm) emit hm->toggleHotkeyTriggered();
                                 }}},
    // add new hotkeys here
};
}  // namespace Altccents
