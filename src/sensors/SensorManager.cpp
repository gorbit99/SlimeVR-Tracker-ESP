/*
	SlimeVR Code is placed under the MIT license
	Copyright (c) 2022 TheDevMinerTV

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.
*/

#include "SensorManager.h"

#include "sensoraddresses.h"
#include "softfusion/drivers/bmi270.h"
#include "softfusion/drivers/icm42688.h"
#include "softfusion/drivers/lsm6ds3trc.h"
#include "softfusion/drivers/lsm6dso.h"
#include "softfusion/drivers/lsm6dsr.h"
#include "softfusion/drivers/lsm6dsv.h"
#include "softfusion/i2cimpl.h"
#include "softfusion/softfusionsensor.h"

#if ESP32
#include "driver/i2c.h"
#endif

namespace SlimeVR {
namespace Sensors {
using SoftFusionLSM6DS3TRC
	= SoftFusionSensor<SoftFusion::Drivers::LSM6DS3TRC, SoftFusion::I2CImpl>;
using SoftFusionBMI270
	= SoftFusionSensor<SoftFusion::Drivers::BMI270, SoftFusion::I2CImpl>;
using SoftFusionLSM6DSV
	= SoftFusionSensor<SoftFusion::Drivers::LSM6DSV, SoftFusion::I2CImpl>;
using SoftFusionLSM6DSO
	= SoftFusionSensor<SoftFusion::Drivers::LSM6DSO, SoftFusion::I2CImpl>;
using SoftFusionLSM6DSR
	= SoftFusionSensor<SoftFusion::Drivers::LSM6DSR, SoftFusion::I2CImpl>;

// TODO Make it more generic in the future and move another place (abstract sensor
// interface)
void SensorManager::swapI2C(uint8_t sclPin, uint8_t sdaPin) {
	if (sclPin != activeSCL || sdaPin != activeSDA || !running) {
		Wire.flush();
#if ESP32
		if (running) {
		} else {
			// Reset HWI2C to avoid being affected by I2CBUS reset
			Wire.end();
		}
		// Disconnect pins from HWI2C
		gpio_set_direction((gpio_num_t)activeSCL, GPIO_MODE_INPUT);
		gpio_set_direction((gpio_num_t)activeSDA, GPIO_MODE_INPUT);

		if (running) {
			i2c_set_pin(I2C_NUM_0, sdaPin, sclPin, false, false, I2C_MODE_MASTER);
		} else {
			Wire.begin(static_cast<int>(sdaPin), static_cast<int>(sclPin), I2C_SPEED);
			Wire.setTimeOut(150);
		}
#else
		Wire.begin(static_cast<int>(sdaPin), static_cast<int>(sclPin));
#endif

		activeSCL = sclPin;
		activeSDA = sdaPin;
	}
}

void SensorManager::setup() {
	running = false;
	activeSCL = PIN_IMU_SCL;
	activeSDA = PIN_IMU_SDA;

	uint8_t sensorID = 0;
	uint8_t activeSensorCount = 0;
#define IMU_DESC_ENTRY(ImuType, ...)                                     \
	{                                                                    \
		sensor = std::move(buildSensor<ImuType>(sensorID, __VA_ARGS__)); \
		if (sensor->isWorking()) {                                       \
			activeSensorCount++;                                         \
		}                                                                \
		sensorID++;                                                      \
	}
	// Apply descriptor list and expand to entrys
	IMU_DESC_LIST;

#undef IMU_DESC_ENTRY
}

void SensorManager::postSetup() { sensor->postSetup(); }

void SensorManager::update() { sensor->motionLoop(); }

std::unique_ptr<Sensor> SensorManager::sensor;

}  // namespace Sensors
}  // namespace SlimeVR
