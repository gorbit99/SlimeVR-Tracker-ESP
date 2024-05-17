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
        int16_t i;
        int16_t j;
        int16_t k;
        int16_t l;
    } rotation;
    struct {
        int16_t x;
        int16_t y;
        int16_t z;
    } acceleration;

    TrackerReport(
        uint8_t imuId,
        Quat rotation,
        Vector3 acceleration,
        float batteryVoltage
    );
};

}  // namespace EspNow
}  // namespace Network
}  // namespace SlimeVR

#endif