#pragma once
#include <qtmetamacros.h>

#include <QThread>

#include "Altccents/Altccents.h"

namespace Altccents {
class HookThread : public QThread {
    Q_OBJECT

   public:
    explicit HookThread(QObject* parent);
    ~HookThread() override;

    void setAccentProfile(const AccentProfile& ap);

    void stop();

   signals:
    void popupShouldOpen(Key);

   private:
    void run() override;
    // Returns false if hook was not installed, return true otherwise
    bool updateHook();

    bool isControlKeyDown() const;

    // If this function is called INSIDE AN EVENT LOOP - call it LIKE THIS:
    //// XAllowEvents(..., ReplayKeyboard, ...);
    //// sendKeyEvent(...); // Function call(s) here
    //// XAllowEvents(..., AsyncKeyboard, ...);
    //// continue;
    //
    // If you want to send KeyPress event - set is_press_event as true,
    // otherwise - set is_press_event as false
    void sendKeyEvent(uint64_t window, uint32_t keycode, bool is_press_event,
                      uint64_t delay = 0);
    void simulateKeyPress(uint32_t keycode);

    AccentProfile ap_;
    Display* d_{};
};

class HookManager : public QObject {
    Q_OBJECT

   public:
    explicit HookManager(AltccentsApp* parent);

   public slots:
    void onProfileChange();
    void onProgramStateChanged(bool new_state);
    void openPopup(Key key);

   private:
    HookThread* hook_;
    AltccentsApp* parent_;
};
}  // namespace Altccents
