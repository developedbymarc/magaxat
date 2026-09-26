#pragma once

#include <sys/types.h>

#include <cstddef>
#include <cstdint>
#include <memory>

namespace magaxat {
    class IUdpListener {
    public:
        virtual ~IUdpListener() = default;
        virtual ssize_t receive(uint8_t *buffer, size_t maxLen) = 0;
    };

    std::unique_ptr<IUdpListener> createUdpListener();
}  // namespace magaxat
