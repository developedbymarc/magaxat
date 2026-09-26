#pragma once

#include <cstdint>

#define PROTOCOL_VERSION 1

namespace magaxat {
    enum class EventType { Motion, Button };

    enum class TabletButton {
        Hover,    // BTN_TOOL_PEN,
        Touch,    // BTN_TOUCH,
        Stylus1,  // BTN_STYLUS,
        Stylus2,  // BTN_STYLUS2
    };

#pragma pack(push, 1)

    struct EventPacket {
        char signature[9];
        uint16_t version;
        uint8_t type;             /* EVENT_TYPE_... */
        struct TouchInformation { /* required */
            uint16_t x, y;
            uint16_t pressure;
            uint16_t tilt;
        } touchInfo;

        struct ActionInformation { /* only required for EVENT_TYPE_BUTTON */
            /* button id:
             *   -1 = stylus in range,
             *    0 = tap/left click/button 0,
             *    1 = button 1,
             *    2 = button 2
             */
            int8_t button;
            int8_t down; /* 1 = button down, 0 = button up */
        } actionInfo;
    };

#pragma pack(pop)

}  // namespace magaxat
