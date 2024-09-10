/*
	SlimeVR Code is placed under the MIT license
	Copyright (c) 2021 Eiren Rain & SlimeVR contributors

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

#include "sensors/bno080sensor.h"

#include "GlobalVars.h"
#include "utils.h"

#define BNO_USE_ARVR_STABILIZATION true

void BNO080Sensor::motionSetup() {
	if (!imu.begin(addr, Wire, m_IntPin)) {
		return;
	}

	imu.enableRawGyro(10);
	imu.enableRawAccelerometer(10);
	imu.enableRawMagnetometer(10);

	lastReset = 0;
	lastData = millis();
	working = true;
	configured = true;
	printf("Timestamp;Accel X;Accel Y;Accel Z;Gyro X;Gyro Y;Gyro Z;Mag X;Mag Y;Mag Z\n"
	);
}

void BNO080Sensor::motionLoop() {
	// Look for reports from the IMU
	while (imu.dataAvailable()) {
		{
			float gyroX = imu.getRawGyroX() / float(1 << 8);
			float gyroY = imu.getRawGyroY() / float(1 << 8);
			float gyroZ = imu.getRawGyroZ() / float(1 << 8);

			float accelX = imu.getRawAccelX() / float(1 << 7);
			float accelY = imu.getRawAccelY() / float(1 << 7);
			float accelZ = imu.getRawAccelZ() / float(1 << 7);

			float magX = imu.getRawMagX() / float(1 << 3);
			float magY = imu.getRawMagY() / float(1 << 3);
			float magZ = imu.getRawMagZ() / float(1 << 3);

			uint32_t timestampMicro = imu.getTimeStamp();
			float timestampSecond = timestampMicro / 1'000'000;

			Serial.print(timestampSecond, 6);
			Serial.print(";");

			Serial.print(accelX, 6);
			Serial.print(";");
			Serial.print(accelY, 6);
			Serial.print(";");
			Serial.print(accelZ, 6);
			Serial.print(";");

			Serial.print(gyroX, 6);
			Serial.print(";");
			Serial.print(gyroY, 6);
			Serial.print(";");
			Serial.print(gyroZ, 6);
			Serial.print(";");

			Serial.print(magX, 6);
			Serial.print(";");
			Serial.print(magY, 6);
			Serial.print(";");
			Serial.print(magZ, 6);
			Serial.print("\n");
		}
	}
}

SensorStatus BNO080Sensor::getSensorState() { return SensorStatus::SENSOR_ERROR; }

void BNO080Sensor::sendData() {}

void BNO080Sensor::startCalibration(int calibrationType) {}
