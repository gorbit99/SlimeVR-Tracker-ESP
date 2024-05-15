#include "TrackerReport.h"

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

namespace SlimeVR {
namespace Network {
namespace EspNow {

TrackerReport::TrackerReport(
        uint8_t imuId,
        Quat rotation,
        Vector3 acceleration,
        float temperature,
        float batteryVoltage
): imuId(imuId), rssi(WiFi.RSSI()) {
    this->acceleration = {
        static_cast<uint16_t>((acceleration.x * (1 << 7)) + 32768),
        static_cast<uint16_t>((acceleration.y * (1 << 7)) + 32768),
        static_cast<uint16_t>((acceleration.z * (1 << 7)) + 32768),
    };
    this->rotation = {
        static_cast<uint16_t>((rotation.x * (1 << 15))),
        static_cast<uint16_t>((rotation.y * (1 << 15))),
        static_cast<uint16_t>((rotation.z * (1 << 15))),
        static_cast<uint16_t>((rotation.w * (1 << 15))),
    };
    battery = static_cast<uint8_t>(std::floor(batteryVoltage));
    batterymV = static_cast<uint16_t>((batteryVoltage - battery) * 1000);
}

}  // namespace EspNow
}  // namespace Network
}  // namespace SlimeVR
