#ifndef SLIMEVR_TRACKERREPORT_H
#define SLIMEVR_TRACKERREPORT_H

#include <cstdint>

#include <quat.h>

namespace SlimeVR {
namespace Network {
namespace EspNow {

struct TrackerReport
{
    uint8_t type;
    uint8_t imuId;
    uint8_t rssi;
    uint8_t battery;
    uint16_t batterymV;
    struct {
        uint16_t i;
        uint16_t j;
        uint16_t k;
        uint16_t l;
    } rotation;
    struct {
        uint16_t x;
        uint16_t y;
        uint16_t z;
    } acceleration;

    TrackerReport(
        uint8_t imuId,
        Quat rotation,
        Vector3 acceleration,
        float temperature,
        float batteryVoltage
    );
};

}  // namespace EspNow
}  // namespace Network
}  // namespace SlimeVR

#endif