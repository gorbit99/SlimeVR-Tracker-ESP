#include "TrackerReport.h"

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>
#include <limits>

namespace SlimeVR {
namespace Network {
namespace EspNow {

template <uint32_t BITCOUNT>
inline constexpr int16_t toFixed(float value) {
    return static_cast<int16_t>(
        std::clamp(
            static_cast<int32_t>(value * (1 << BITCOUNT)),
            static_cast<int32_t>(std::numeric_limits<int16_t>::min()),
            static_cast<int32_t>(std::numeric_limits<int16_t>::max())
        )
    );
}

TrackerReport::TrackerReport(
        uint8_t imuId,
        Quat rotation,
        Vector3 acceleration,
        float batteryVoltage
): imuId(imuId), rssi(WiFi.RSSI()) {
    this->acceleration = {
        toFixed<7>(acceleration.x),
        toFixed<7>(acceleration.y),
        toFixed<7>(acceleration.z)
    };
    this->rotation = {
        toFixed<15>(rotation.x),
        toFixed<15>(rotation.y),
        toFixed<15>(rotation.z),
        toFixed<15>(rotation.w)
    };
    battery = static_cast<uint8_t>(std::floor(batteryVoltage));
    batterymV = static_cast<uint16_t>((batteryVoltage - battery) * 1000);
}

}  // namespace EspNow
}  // namespace Network
}  // namespace SlimeVR
