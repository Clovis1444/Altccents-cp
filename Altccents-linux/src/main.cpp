#include <fcntl.h>
#include <libevdev-1.0/libevdev/libevdev.h>
#include <linux/input-event-codes.h>
#include <sys/select.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <iostream>

int main() {
    libevdev* dev = nullptr;
    int fd{};
    int rc{};

    fd = open("/dev/input/event2", O_RDWR | O_NONBLOCK);

    rc = libevdev_new_from_fd(fd, &dev);

    // File open handling
    if (fd < 0) {
        std::cout << "Failed to open device: " << strerror(errno) << '\n';
        return errno;
    }

    // Libevdev creation handling
    if (rc < 0) {
        close(fd);
        std::cout << "Failed to init libevdev: " << strerror(-rc) << '\n';
        return -rc;
    }

    std::cout << "Device name is \"" << libevdev_get_name(dev) << "\"\n";

    while (true) {
        input_event ev;
        rc = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_NORMAL, &ev);

        if (rc == 0 && ev.type == EV_KEY) {
            if (ev.code == KEY_A) {
                libevdev_grab(dev, LIBEVDEV_GRAB);
                continue;
            }
            if (ev.code == KEY_S) {
                libevdev_grab(dev, LIBEVDEV_UNGRAB);
                continue;
            }
            if (ev.code == KEY_Q) {
                libevdev_grab(dev, LIBEVDEV_UNGRAB);
                break;
            }
            // Emulate keyboard input
            if (ev.code == KEY_P && ev.value == 1) {
                input_event virtual_ev{};

                gettimeofday(&virtual_ev.time, nullptr);
                virtual_ev.type = EV_KEY;
                virtual_ev.code = KEY_2;
                virtual_ev.value = 1444;  // 1 key press; 0 - key release

                // Key press
                write(fd, &virtual_ev, sizeof(virtual_ev));
                // Key release
                virtual_ev.value = 0;
                write(fd, &virtual_ev, sizeof(virtual_ev));
                // Syncronize events
                virtual_ev.type = EV_SYN;
                virtual_ev.code = SYN_REPORT;
                virtual_ev.value = 0;
                write(fd, &virtual_ev, sizeof(virtual_ev));
            }

            std::cout << libevdev_event_code_get_name(EV_KEY, ev.code)
                      << (ev.value ? ((ev.value > 1) ? " REpeated" : "pressed")
                                   : " released")
                      << std::endl;
        }
    }

    libevdev_grab(dev, LIBEVDEV_UNGRAB);
    libevdev_free(dev);
    close(fd);
    return 0;
}
