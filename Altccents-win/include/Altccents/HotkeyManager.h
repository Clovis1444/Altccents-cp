#pragma once

#include <Windows.h>

#include <QHash>
#include <QThread>

#include "Altccents/Settings.h"

namespace Altccents {

class HotkeyManager : public QObject {
    Q_OBJECT

   public:
    explicit HotkeyManager();
    ~HotkeyManager() override;

   signals:
    void popupHotkeyTriggered();
    void toggleHotkeyTriggered();

   public slots:
    void onHotkeyChanged(int hotkey_id);

   private:
    void postLoopThreadMsg(UINT msg);

    void startMsgLoop();
    void stopMsgLoop();

    // [IMPORTANT]: All methods with postfix "_msg" should be called like this:
    // QMetaObject::invokeMethod(this, &HotkeyManager::<methode_name>);
    // OR call them inside msgLoop_msg()
    void msgLoop_msg();
    // NOTE(clovis): this function will fail if hotkey already set by other app
    static void setHotkey_msg(int hotkey_id);
    // If hotkey_id equals 0 - unset ALL hotkeys
    static void unsetHotkey_msg(int hotkey_id = 0);
    //

    QThread msg_thread_{};
    DWORD msgLoopThreadId_{};
    bool is_msg_loop_running_{};

    static constexpr int kPopupHotkeyId{1444};
    static constexpr int kToggleHotkeyId{1445};
    static constexpr UINT kStopMsgLoopMsg{WM_USER + 1};
    static constexpr UINT kUpdatePopupHotkeyMsg{WM_USER + 2};
    static constexpr UINT kUpdateToggleHotkeyMsg{WM_USER + 3};

    // NOTE(clovis): Hotkey.id MUST not be 0
    struct Hotkey {
        // Hotkey id for RegisterHotKey()
        int id{};
        // Setting to get keys from
        Settings::SettingsType setting;
        // Ptr to a signal that Settings class emitting when setting is updated
        void (Settings::*setting_signal)();
        // Message that will be posted to MsgLoop when setting_signal is emited
        UINT update_msg{};
        // Signal that will be emited by HotkeyManager when hotkey is triggered
        std::function<void(HotkeyManager*)> signal;
    };
    // NOTE(clovis): to create new hotkey do this:
    // 1. Create setting and signal in Settings.h
    // 2. Create signal in HotkeyManager
    // 3. Add hotkey into kHotkeys
    // 4. Create slot in AltccentsWin and connect it to signal from
    // HotkeyManager
    static const QHash<int, Hotkey> kHotkeys;
};
}  // namespace Altccents
