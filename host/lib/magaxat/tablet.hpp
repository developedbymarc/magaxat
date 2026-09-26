#pragma once

#include <cstdint>
#include <memory>

#include <magaxat/protocol.hpp>

namespace magaxat {

    class IVirtualTablet {
    public:
        IVirtualTablet() = default;
        virtual ~IVirtualTablet() = default;

        IVirtualTablet(IVirtualTablet &&) = delete;
        IVirtualTablet(const IVirtualTablet &) = delete;
        IVirtualTablet &operator=(IVirtualTablet &&) = delete;
        IVirtualTablet &operator=(const IVirtualTablet &) = delete;

        virtual void emitPosition(uint16_t x, uint16_t y) = 0;
        virtual void emitPressure(uint16_t pressure) = 0;
        virtual void emitButton(TabletButton btn, bool down) = 0;
        virtual void sync() = 0;  // commit the frame (~ EV_SYN/SYN_REPORT)
    };

    std::unique_ptr<IVirtualTablet> createVirtualTablet();

}  // namespace magaxat
