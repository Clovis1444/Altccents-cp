#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>

#include <chrono>
#include <iostream>
#include <thread>

// Returns 0 if failed to find unused keycode
KeyCode findUnusedKeycode(Display* display) {
    char keys[32];
    XQueryKeymap(display, keys);

    int min_keycode{};
    int max_keycode{};
    XDisplayKeycodes(display, &min_keycode, &max_keycode);

    for (int keycode{min_keycode}; keycode <= max_keycode; ++keycode) {
        int byte_index{keycode / 8};
        int bit_index{keycode % 8};
        if (!(keys[byte_index] & (1 << (bit_index)))) {
            return static_cast<KeyCode>(keycode);  // This keycode is unused
        }
    }

    return 0;
}

// Returns old KeySym of the passed keycode
KeySym mapKey(Display* display, KeyCode keycode, KeySym new_keysym) {
    // TODO(clovis): refactor this using XKB
    KeySym old_keysym{XKeycodeToKeysym(display, keycode, 0)};

    XChangeKeyboardMapping(display, keycode, 1, &new_keysym, 1);
    XFlush(display);

    return old_keysym;
}

// void printKeycodes() {
//     Display* display = XOpenDisplay(nullptr);
//
//     char keys[32];
//     XQueryKeymap(display, keys);
//
//     int min_keycode{};
//     int max_keycode{};
//     XDisplayKeycodes(display, &min_keycode, &max_keycode);
//
//     std::cout << "Min: " << min_keycode << "\nMax: " << max_keycode
//               << std::endl;
//
//     int pressed_count{};
//     for (int keycode{min_keycode}; keycode <= max_keycode; ++keycode) {
//         int byte{keycode / 8};
//         int bit{keycode % 8};
//         bool key_value{static_cast<bool>(keys[byte] & (1 << bit))};
//
//         if (key_value) {
//             char* key{XKeysymToString(XKeycodeToKeysym(display, keycode,
//             0))}; std::cout << key << '[' << keycode << "] : " << key_value
//                       << std::endl;
//             ++pressed_count;
//         }
//     }
//
//     std::cout << "Total keys pressed: " << pressed_count << std::endl;
//
//     XCloseDisplay(display);
// }

int main() {
    Display* display = XOpenDisplay(nullptr);
    if (!display) {
        std::cerr << "Unable to open X display" << std::endl;
        return 1;
    }

    // Find an unused keycode
    KeyCode temp_keycode = findUnusedKeycode(display);
    if (temp_keycode == 0) {
        std::cerr << "No unused keycode found" << std::endl;
        XCloseDisplay(display);
        return 1;
    }

    // Desired keysym(unicode character)
    KeySym new_keysym{XStringToKeysym("U0414")};
    // Map to desired keysym
    KeySym old_keysym{mapKey(display, temp_keycode, new_keysym)};

    // Emulate key presses
    for (int i = 0; i < 5; ++i) {
        XTestFakeKeyEvent(display, temp_keycode, True, CurrentTime);
        XTestFakeKeyEvent(display, temp_keycode, False, CurrentTime);
        XFlush(display);

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // Map back to default keysym
    mapKey(display, temp_keycode, old_keysym);

    XCloseDisplay(display);

    return 0;
}
