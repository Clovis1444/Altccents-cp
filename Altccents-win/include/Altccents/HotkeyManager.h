#pragma once

#include <Windows.h>

#include <QThread>

namespace Altccents {

class HotkeyManager : public QObject {
    Q_OBJECT

   public:
    explicit HotkeyManager();
    ~HotkeyManager() override;

   signals:
    void hotkeyTriggered();

   public slots:
    void updateHotkey();

   private:
    void startMsgLoop();
    void stopMsgLoop();

    // [IMPORTANT]: All methods with postfix "_msg" should be called like this:
    // QMetaObject::invokeMethod(this, &HotkeyManager::<methode_name>);
    void msgLoop_msg();
    static void setHotkey_msg();
    static void unsetHotkey_msg();
    //

    QThread msg_thread_{};
    bool is_msg_loop_running_{};

    static constexpr int kHotkeyId{1444};
    static constexpr UINT kStopMsgLoopMsg{WM_USER + 1};
};
}  // namespace Altccents
