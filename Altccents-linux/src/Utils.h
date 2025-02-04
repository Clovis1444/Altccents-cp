#pragma once

#include <X11/Xlib.h>

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
}  // namespace Utils
}  // namespace Altccents
