#pragma once

#include <Windows.h>

#include <QObject>

#include "Altccents/Altccents.h"
#include "Altccents/HotkeyManager.h"

namespace Altccents {
class AltccentsWin : public QObject {
    Q_OBJECT
   public:
    explicit AltccentsWin(AltccentsApp* parent);
    ~AltccentsWin() override;

   private slots:
    void onProgramStateChanged(bool state);
    static void onCharSendRequested(Key, QChar symbol);

   private:
    // SetWindowsHookEx() accepts only static or global functions
    static LRESULT CALLBACK hook_proc(int code, WPARAM wparam, LPARAM lparam);
    // Returns true on success; Returns false and do nothing in OneShotMode
    bool setHook();
    void unsetHook();

    static bool isKeyDown(int v_key);

    static void sendKeyInput(QChar ch);

    static QString getWindowTitle(HWND window);

    inline static AltccentsApp* altccents_{};
    HHOOK hook_{nullptr};
    HotkeyManager* hotkey_manager_;
};
}  // namespace Altccents
