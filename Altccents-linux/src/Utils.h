#pragma once

// XLib and Qt have some macro collission so XLib should be included after all
// Qt headers
// clang-format off
#include <QDebug>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
// clang-format on
#include <fcntl.h>
#include <libevdev-1.0/libevdev/libevdev.h>
#include <unistd.h>

#include <iostream>

namespace Altccents {
namespace Utils {
inline void printKeyMapping(Display* d, KeyCode key) {
    if (!d) {
        std::cerr << "Failed to print key mapping: Wrong display ptr"
                  << std::endl;
        return;
    }

    int min_keycode{};
    int max_keycode{};
    XDisplayKeycodes(d, &min_keycode, &max_keycode);

    if (key < min_keycode || key > max_keycode) {
        std::cerr << "Failed to print key mapping: Invalid keycode "
                  << static_cast<int>(key) << std::endl;
        return;
    }

    // Get the keyboard mapping
    int keysyms_per_keycode{};
    KeySym* keysyms = XGetKeyboardMapping(d, key, 1, &keysyms_per_keycode);

    if (!keysyms) {
        std::cerr << "Failed to get keyboard mapping" << std::endl;
        return;
    }

    // Print the mappings for the keycode
    std::cout << "Keycode " << key << " mappings:" << std::endl;
    for (int i{}; i < keysyms_per_keycode; ++i) {
        KeySym keysym = keysyms[i];
        if (keysym == NoSymbol) {
            std::cout << "  NoSymbol" << std::endl;
            continue;
        }

        std::string modifier;
        switch (i) {
            case 0:
                modifier = "No modifier";
                break;
            case 1:
                modifier = "Shift";
                break;
            case 2:
                modifier = "AltGr";
                break;
            case 3:
                modifier = "Shift + AltGr";
                break;
            default:
                modifier = "Unknown modifier";
                break;
        }

        char* keysym_name = XKeysymToString(keysym);
        if (keysym_name) {
            std::cout << "  " << modifier << ": " << keysym_name << std::endl;
        } else {
            std::cout << "  " << modifier << ": Unknown keysym" << std::endl;
        }
    }
    XFree(keysyms);
}

inline void readEvent() {
    const std::string dev_path{"/dev/input/event2"};
    int fd{open(dev_path.c_str(), O_RDWR | O_NONBLOCK)};

    libevdev* dev{};

    int rc{libevdev_new_from_fd(fd, &dev)};

    if (rc < 0) {
        std::cout << "Failed to init libevdev" << std::endl;
        return;
    }

    std::cout << "Device name: " << libevdev_get_name(dev) << std::endl;
    std::cout << "Bustype: " << libevdev_get_id_bustype(dev) << std::endl;
    std::cout << "Vendor: " << libevdev_get_id_vendor(dev) << std::endl;
    std::cout << "Product: " << libevdev_get_id_product(dev) << std::endl;

    while (true) {
        input_event ev{};

        rc = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_NORMAL, &ev);

        if (rc == 0) {
            std::cout << "Type: " << libevdev_event_type_get_name(ev.type)
                      << '|';
            std::cout << "Code: "
                      << libevdev_event_code_get_name(ev.type, ev.code) << '|';
            std::cout << "Value: " << ev.value << '|';
            std::cout << "Time: [" << ev.time.tv_sec << ", " << ev.time.tv_usec
                      << ']' << std::endl;

            if (ev.code == KEY_Q) {
                libevdev_free(dev);
                close(fd);
                return;
            }
            // On key 7 down - emulate keyboard input
            if (ev.code == KEY_7 && ev.value == 1) {
                //
                input_event ie{};
                ie.type = EV_KEY;
                ie.code = KEY_S;
                ie.value = 1;
                ie.time = ev.time;
                // Send key down
                write(fd, &ie, sizeof(ie));
                ie.value = 0;
                // Send key up
                write(fd, &ie, sizeof(ie));
            }
        }
    }
}

// TODO(clovis): Does not works for all windows(Nemo, app launcher etc)
inline void xHook() {
    Display* display{XOpenDisplay(nullptr)};
    if (!display) {
        qCritical().noquote()
            << "Altccents::Utils [ERROR]: failed to open XDisplay";
        return;
    }

    Window root_w{DefaultRootWindow(display)};
    Window focus_w{};
    int revert{};

    // Get focus window
    XGetInputFocus(display, &focus_w, &revert);
    {
        char* name{};
        XFetchName(display, focus_w, &name);
        qInfo() << "Focus:" << focus_w << name;
    }
    // Event hook
    XSelectInput(display, focus_w,
                 KeyPressMask | KeyReleaseMask | FocusChangeMask);

    while (true) {
        XEvent e{};
        XNextEvent(display, &e);

        switch (e.type) {
            case FocusOut: {
                // If focus changed - stop receiving events from old focus
                if (focus_w != root_w) {
                    XSelectInput(display, focus_w, 0);
                }
                // Update focus window
                XGetInputFocus(display, &focus_w, &revert);
                {
                    char* name{};
                    XFetchName(display, focus_w, &name);
                    qInfo() << "Focus:" << focus_w << name;
                }

                // Handle "focus-follows-mouse" mode
                if (focus_w == PointerRoot) {
                    focus_w = root_w;
                }

                // Start receiving events from new focus
                XSelectInput(display, focus_w,
                             KeyPressMask | KeyReleaseMask | FocusChangeMask);

                break;
            }
            case KeyPress: {
                XkbStateRec xkb_state{};
                // Keyboard state
                XkbGetState(display, XkbUseCoreKbd, &xkb_state);

                // Keycode -> KeySym
                KeySym key_sym{XkbKeycodeToKeysym(
                    display, static_cast<KeyCode>(e.xkey.keycode),
                    xkb_state.locked_group, xkb_state.group)};

                // KeySym -> string
                char* str{XKeysymToString(key_sym)};

                qInfo() << "Keycode:" << e.xkey.keycode << key_sym << str;

                break;
            }
            default: {
                break;
            }
        }
    }

    XCloseDisplay(display);
}

}  // namespace Utils
}  // namespace Altccents
