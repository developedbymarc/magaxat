#include <fcntl.h>
#include <linux/input-event-codes.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <unistd.h>

#include <cstring>
#include <memory>
#include <stdexcept>

#include <magaxat/magaxat.hpp>

class LinuxVirtualTablet : public magaxat::IVirtualTablet {
public:
    LinuxVirtualTablet() {
        _fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
        if (_fd < 0) {
            throw std::runtime_error("open /dev/uinput");
        }

        setupDevice();
    }

    ~LinuxVirtualTablet() override {
        if (_fd >= 0) {
            ioctl(_fd, UI_DEV_DESTROY);
            close(_fd);
        }
    }

    LinuxVirtualTablet(LinuxVirtualTablet &&) = delete;
    LinuxVirtualTablet(const LinuxVirtualTablet &) = delete;
    LinuxVirtualTablet &operator=(LinuxVirtualTablet &&) = delete;
    LinuxVirtualTablet &operator=(const LinuxVirtualTablet &) = delete;

    void emitPosition(uint16_t x, uint16_t y) override {
        emit(EV_ABS, ABS_X, x);
        emit(EV_ABS, ABS_Y, y);
    }
    void emitPressure(uint16_t pressure) override { emit(EV_ABS, ABS_PRESSURE, pressure); }
    void emitButton(magaxat::TabletButton btn, bool down) override {
        emit(EV_KEY, toLinuxCode(btn), down);
    }
    void sync() override { emit(EV_SYN, SYN_REPORT, 1); }

private:
    int _fd = -1;

    void setupDevice() {
        // enable synchronization
        if (ioctl(_fd, UI_SET_EVBIT, EV_SYN) < 0)
            throw std::runtime_error("ioctl UI_SET_EVBIT EV_SYN");

        // enable 1 button
        if (ioctl(_fd, UI_SET_EVBIT, EV_KEY) < 0)
            throw std::runtime_error("ioctl UI_SET_EVBIT EV_KEY");
        if (ioctl(_fd, UI_SET_KEYBIT, BTN_TOUCH) < 0)
            throw std::runtime_error("ioctl UI_SET_KEYBIT");
        if (ioctl(_fd, UI_SET_KEYBIT, BTN_TOOL_PEN) < 0)
            throw std::runtime_error("ioctl UI_SET_KEYBIT");
        if (ioctl(_fd, UI_SET_KEYBIT, BTN_STYLUS) < 0)
            throw std::runtime_error("ioctl UI_SET_KEYBIT");
        if (ioctl(_fd, UI_SET_KEYBIT, BTN_STYLUS2) < 0)
            throw std::runtime_error("ioctl UI_SET_KEYBIT");

        // enable 2 main axes + pressure (absolute positioning)
        if (ioctl(_fd, UI_SET_EVBIT, EV_ABS) < 0)
            throw std::runtime_error("ioctl UI_SET_EVBIT EV_ABS");
        if (ioctl(_fd, UI_SET_ABSBIT, ABS_X) < 0)
            throw std::runtime_error("ioctl UI_SETEVBIT ABS_X");
        if (ioctl(_fd, UI_SET_ABSBIT, ABS_Y) < 0)
            throw std::runtime_error("ioctl UI_SETEVBIT ABS_Y");
        if (ioctl(_fd, UI_SET_ABSBIT, ABS_PRESSURE) < 0)
            throw std::runtime_error("ioctl UI_SETEVBIT ABS_PRESSURE");

        {
            struct uinput_abs_setup abs_setup;
            struct uinput_setup setup;

            memset(&abs_setup, 0, sizeof(abs_setup));
            abs_setup.code = ABS_X;
            abs_setup.absinfo.value = 0;
            abs_setup.absinfo.minimum = 0;
            abs_setup.absinfo.maximum = UINT16_MAX;
            abs_setup.absinfo.fuzz = 0;
            abs_setup.absinfo.flat = 0;
            abs_setup.absinfo.resolution = 400;
            if (ioctl(_fd, UI_ABS_SETUP, &abs_setup) < 0)
                throw std::runtime_error("UI_ABS_SETUP ABS_X");

            memset(&abs_setup, 0, sizeof(abs_setup));
            abs_setup.code = ABS_Y;
            abs_setup.absinfo.value = 0;
            abs_setup.absinfo.minimum = 0;
            abs_setup.absinfo.maximum = UINT16_MAX;
            abs_setup.absinfo.fuzz = 0;
            abs_setup.absinfo.flat = 0;
            abs_setup.absinfo.resolution = 400;
            if (ioctl(_fd, UI_ABS_SETUP, &abs_setup) < 0)
                throw std::runtime_error("UI_ABS_SETUP ABS_Y");

            memset(&abs_setup, 0, sizeof(abs_setup));
            abs_setup.code = ABS_PRESSURE;
            abs_setup.absinfo.value = 0;
            abs_setup.absinfo.minimum = 0;
            abs_setup.absinfo.maximum = INT16_MAX;
            abs_setup.absinfo.fuzz = 0;
            abs_setup.absinfo.flat = 0;
            abs_setup.absinfo.resolution = 0;
            if (ioctl(_fd, UI_ABS_SETUP, &abs_setup) < 0)
                throw std::runtime_error("UI_ABS_SETUP ABS_PRESSURE");

            memset(&setup, 0, sizeof(setup));
            std::memcpy(setup.name, magaxat::metadata::name, sizeof(magaxat::metadata::name));
            setup.id.bustype = BUS_VIRTUAL;
            setup.id.vendor = 0x1;
            setup.id.product = 0x1;
            setup.id.version = 2;
            setup.ff_effects_max = 0;
            if (ioctl(_fd, UI_DEV_SETUP, &setup) < 0)
                throw std::runtime_error("UI_DEV_SETUP");

            if (ioctl(_fd, UI_DEV_CREATE) < 0)
                throw std::runtime_error("ioctl");
        }
    }

    void emit(int type, int code, int value) {
        struct input_event ev{};
        ev.type = type;
        ev.code = code;
        ev.value = value;
        if (write(_fd, &ev, sizeof(ev)) < 0) {
            throw std::runtime_error("write input_event");
        }
    }

    static int toLinuxCode(magaxat::TabletButton b) {
        switch (b) {
            case magaxat::TabletButton::Hover: {
                return BTN_TOOL_PEN;
            };
            case magaxat::TabletButton::Touch: {
                return BTN_TOUCH;
            };
            case magaxat::TabletButton::Stylus1: {
                return BTN_STYLUS;
            };
            case magaxat::TabletButton::Stylus2: {
                return BTN_STYLUS2;
            };
            default: {
                return -1;
            };
        }
    }
};

std::unique_ptr<magaxat::IVirtualTablet> magaxat::createVirtualTablet() {
    return std::make_unique<LinuxVirtualTablet>();
}
