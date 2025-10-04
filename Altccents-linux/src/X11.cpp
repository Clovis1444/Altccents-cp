#include "Altccents/X11.h"

// XLib and Qt have some macro collission so XLib should be included after all
// Qt headers
#include <QDebug>
// clang-format off
#include <X11/Xlib.h>
// clang-format on

bool Altccents::X11::hook(const AccentProfile& ap) {
    // const QList<Qt::Key> keys{ap.accents().keys()};
    //
    // qInfo() << static_cast<int>(Qt::Key_NumLock);
    // if (!keys.length()) {
    //     return true;
    // }
    //
    // qInfo() << "Keys[" << keys.length() << "]:" << keys;
    // return false;
    //
    Display* d{XOpenDisplay(nullptr)};

    if (!d) {
        qCritical().noquote()
            << "Altccents::X11 [ERROR]: failed to open XDisplay";
        return false;
    }

    Window w{};
    w = XDefaultRootWindow(d);

    // I couldn't get it to work with XGrabKeyboard() so use XGrabKey() instead
    // 56 - B Key
    int grab_result{
        XGrabKey(d, 56, AnyModifier, w, 1, GrabModeAsync, GrabModeSync)};
    if (!grab_result) {
        qCritical().noquote() << "Altccents::X11 [ERROR]: failed to grab key";
        return false;
    }

    XEvent e{};
    bool is_remaped{};
    while (true) {
        // TODO(clovis): XChangeKeyboardMapping() is too slow. So test if it
        // suits with final logic
        XNextEvent(d, &e);

        qInfo() << "Event: " << e.xkey.type << e.xkey.keycode;

        // TODO(clovis): Create more robust mapping to origin mechanism
        // Maybe use XSelectInput() to monitor fake events?
        if (is_remaped) {
            // TODO(clovis): remap key here to ORIGINAL value
            qInfo() << "Mapped to original!";

            KeySym original_mapping{XStringToKeysym("U0062")};
            XChangeKeyboardMapping(d, static_cast<int>(e.xkey.keycode), 1,
                                   &original_mapping, 1);
        }

        // Get all keys physical state
        char keys[32]{};
        XQueryKeymap(d, keys);

        unsigned int num_keycode{77};
        bool is_num_pressed{static_cast<bool>(keys[num_keycode / 8] &
                                              (1 << (num_keycode % 8)))};

        // If Num Lock is down
        if (is_num_pressed) {
            // TODO(clovis): remap key here to DESIRED value
            qInfo() << "Mapped to desired!";

            KeySym new_mapping{XStringToKeysym("U03C3")};
            XChangeKeyboardMapping(d, static_cast<int>(e.xkey.keycode), 1,
                                   &new_mapping, 1);
            is_remaped = true;
        }

        // Discard
        // XAllowEvents(d, AsyncKeyboard, CurrentTime);
        // Proceed
        // XAllowEvents(d, ReplayKeyboard, CurrentTime);

        XAllowEvents(d, ReplayKeyboard, CurrentTime);
    }

    // TODO(clovis): Ungrab key/keyboard here
    XUngrabKey(d, 56, AnyModifier, XDefaultRootWindow(d));
    XCloseDisplay(d);
    return true;
}
