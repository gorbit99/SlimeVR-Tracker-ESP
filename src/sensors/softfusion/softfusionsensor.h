/*
	SlimeVR Code is placed under the MIT license
	Copyright (c) 2024 Tailsy13 & SlimeVR Contributors

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

#pragma once

#include <array>
#include <cstring>

#include "../../consts.h"
#include "../sensor.h"
#include "GlobalVars.h"

char printBuf[2048];
char* printBufHead = printBuf;

namespace SlimeVR::Sensors {

template <template <typename I2CImpl> typename T, typename I2CImpl>
class SoftFusionSensor : public Sensor {
	using imu = T<I2CImpl>;
	using RawVectorT = std::array<int16_t, 3>;

	static constexpr double GScale
		= ((32768. / imu::GyroSensitivity) / 32768.) * (PI / 180.0);
	static constexpr double AScale = CONST_EARTH_GRAVITY / imu::AccelSensitivity;

	bool detected() const {
		const auto value = m_sensor.i2c.readReg(imu::Regs::WhoAmI::reg);
		if (imu::Regs::WhoAmI::value != value) {
			return false;
		}

		return true;
	}

public:
	static constexpr auto TypeID = imu::Type;
	static constexpr uint8_t Address = imu::Address;

	SoftFusionSensor(
		uint8_t id,
		uint8_t addrSuppl,
		float rotation,
		uint8_t sclPin,
		uint8_t sdaPin,
		uint8_t
	)
		: Sensor(
			imu::Name,
			imu::Type,
			id,
			imu::Address + addrSuppl,
			rotation,
			sclPin,
			sdaPin
		)
		, m_sensor(I2CImpl(imu::Address + addrSuppl)) {}
	~SoftFusionSensor() {}

	void motionLoop() override final {
		// printBufHead = printBuf;
		// printBuf[0] = '\0';
		m_sensor.bulkRead(
			[](const int16_t xyz[3], const sensor_real_t timeDelta) {
				Serial.print("1;");
				Serial.print(xyz[0] * AScale, 6);
				Serial.print(";");
				Serial.print(xyz[1] * AScale, 6);
				Serial.print(";");
				Serial.print(xyz[2] * AScale, 6);
				Serial.print("\n");
			},
			[](const int16_t xyz[3], const sensor_real_t timeDelta) {
				Serial.print("0;");
				Serial.print(xyz[0] * GScale, 6);
				Serial.print(";");
				Serial.print(xyz[1] * GScale, 6);
				Serial.print(";");
				Serial.print(xyz[2] * GScale, 6);
				Serial.print("\n");
			}
		);

		// printf(printBuf);
	}

	static constexpr bool HasMotionlessCalib
		= requires(imu& i) { typename imu::MotionlessCalibrationData; };
	static constexpr size_t MotionlessCalibDataSize() {
		if constexpr (HasMotionlessCalib) {
			return sizeof(typename imu::MotionlessCalibrationData);
		} else {
			return 0;
		}
	}

	void motionSetup() override final {
		if (!detected()) {
			m_status = SensorStatus::SENSOR_ERROR;
			return;
		}

		if constexpr (HasMotionlessCalib) {
			typename imu::MotionlessCalibrationData calibData;
			m_sensor.initialize(calibData);
		} else {
			m_sensor.initialize();
		}

		m_status = SensorStatus::SENSOR_OK;
		working = true;

		printf("Sensor;X;Y;Z\n");
	}

	void startCalibration(int calibrationType) override final {}

	void saveCalibration() {}

	void calibrateGyroOffset() {}

	void calibrateAccel() {}

	void calibrateSampleRate() {}

	SensorStatus getSensorState() override final { return m_status; }

	T<I2CImpl> m_sensor;
	SensorStatus m_status = SensorStatus::SENSOR_OFFLINE;
};

}  // namespace SlimeVR::Sensors
