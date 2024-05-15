#ifndef SLIMEVR_RESETCOUNTER_H
#define SLIMEVR_RESETCOUNTER_H

#include <stdint.h>

#include "../logging/Logger.h"

namespace SlimeVR {
namespace Peripherals {

class ResetCounter {
public:
    void init();
    void update();

    uint8_t getResetCount();

private:
    uint32_t timeSinceReset = 0;
    uint8_t resetCount = 0;

    Logging::Logger m_Logger = Logging::Logger("ResetCounter");
};

}  // namespace Peripherals
}  // namespace SlimeVR

#endif