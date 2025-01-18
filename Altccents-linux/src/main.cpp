#include <fcntl.h>
#include <libevdev-1.0/libevdev/libevdev.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <iostream>

int main() {
    libevdev* dev = nullptr;
    int fd{};
    int rc{};

    fd = open("/dev/input/event2", O_RDONLY | O_NONBLOCK);

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
            std::cout << libevdev_event_code_get_name(EV_KEY, ev.code)
                      << (ev.value ? ((ev.value > 1) ? " REpeated" : " pressed")
                                   : " released")
                      << std::endl;
        }
    }

    libevdev_free(dev);
    close(fd);
    return 0;
}
