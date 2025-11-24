#pragma once

#include <Windows.h>

#include <QObject>

#include "Altccents/Altccents.h"

namespace Altccents {
class AltccentsWin : public QObject {
    Q_OBJECT
   public:
    explicit AltccentsWin(AltccentsApp* parent);
    ~AltccentsWin() override;

   private slots:

   private:
    // SetWindowsHookEx() accepts only static or global functions
    static LRESULT CALLBACK hook_proc(int code, WPARAM wparam, LPARAM lparam);
    // Returns true on success
    bool setHook();
    void unsetHook();

    inline static AltccentsApp* altccents_{};
    HHOOK hook_{};
};
}  // namespace Altccents
