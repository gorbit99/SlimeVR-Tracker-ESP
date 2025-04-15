/*
	SlimeVR Code is placed under the MIT license
	Copyright (c) 2025 Gorbit99 & SlimeVR Contributors

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

#include "MagDriver.h"

namespace SlimeVR::Sensors::SoftFusion::Mag {

std::vector<MagDefinition> MagDriver::supportedMags = {
	MagDefinition{
		.name = "IST8306",

		.deviceId = 0x19,

		.whoAmIReg = 0x00,
		.expectedWhoAmI = 0x06,

		.setup =
			[](const WriteRegFunc& writeI2C) {
				writeI2C(0x32, 0x01);  // Soft reset
				delay(50);
				writeI2C(0x30, 0x20);  // Noise suppression: low
				writeI2C(0x41, 0x2d);  // Oversampling: 32X
				writeI2C(0x31, 0x02);  // Continuous measurement @ 10Hz
			},

		.dataReg = 0x11,
		.dataWidth = MagDefinition::DataWidth::SixByte,

		.resolution = 0.3,
	},
	MagDefinition{
		.name = "QMC6309",

		.deviceId = 0x7c,

		.whoAmIReg = 0x00,
		.expectedWhoAmI = 0x90,

		.setup =
			[](const WriteRegFunc& writeI2C) {
				writeI2C(0x0b, 0x80);
				writeI2C(0x0b, 0x00);  // Soft reset
				delay(10);
				writeI2C(0x0b, 0x48);  // Set/reset on, 8g full range, 200Hz
				writeI2C(0x0a, 0x21);  // LP filter 2, 8x Oversampling, normal mode
			},

		.dataReg = 0x01,
		.dataWidth = MagDefinition::DataWidth::SixByte,

		.resolution = 1 / 4000.0f  // @ 8G field range
	},
	MagDefinition{
		.name = "LIS2MDL",

		.deviceId = 0x1e,

		.whoAmIReg = 0x4f,
		.expectedWhoAmI = 0x40,

		.setup =
			[](const WriteRegFunc& writeI2C) {
				writeI2C(0x60, 0x20);  // Soft reset
				delay(10);
				writeI2C(0x60, 0x8c);  // Temp compensation enabled, 100Hz, continuous
				writeI2C(0x62, 0x01);  // Enable mag, drdy on pin
			},

		.dataReg = 0x68,
		.dataWidth = MagDefinition::DataWidth::SixByte,

		.resolution = 1.5f / 1000,
	},
};

MagDriver::MagDriver(const char* loggingPrefix)
	: logger{loggingPrefix} {}

bool MagDriver::init() {
	for (auto& mag : supportedMags) {
		setId(mag.deviceId);
		logger.info("Trying mag %s", mag.name);
		if (readReg(mag.whoAmIReg) != mag.expectedWhoAmI) {
			continue;
		}

		magDefinition = mag;
		state = State::Ok;

		logger.info("Found mag of type %s, initializing!", mag.name);
		magDefinition.setup([&](uint8_t reg, uint8_t value) { writeReg(reg, value); });
		setup();

		return true;
	}

	logger.info("No mag found!");
	state = State::Error;
	return false;
}

#pragma pack(push, 1)
struct ThreeByteValues {
	int32_t x : 24;
	int32_t y : 24;
	int32_t z : 24;
};
#pragma pack(pop)
static_assert(sizeof(ThreeByteValues) == 9);

void MagDriver::getMagSample(float outData[3]) {
	if (state != State::Ok) {
		return;
	}

	int32_t rawDataConcat[3];

	if (magDefinition.dataWidth == MagDefinition::DataWidth::SixByte) {
		rawDataConcat[0] = reinterpret_cast<const int16_t*>(lastSample)[0];
		rawDataConcat[1] = reinterpret_cast<const int16_t*>(lastSample)[1];
		rawDataConcat[2] = reinterpret_cast<const int16_t*>(lastSample)[2];
	} else {
		rawDataConcat[0] = reinterpret_cast<const ThreeByteValues*>(lastSample)->x;
		rawDataConcat[1] = reinterpret_cast<const ThreeByteValues*>(lastSample)->y;
		rawDataConcat[2] = reinterpret_cast<const ThreeByteValues*>(lastSample)->z;
	}

	outData[0] = rawDataConcat[0] * magDefinition.resolution;
	outData[1] = rawDataConcat[1] * magDefinition.resolution;
	outData[2] = rawDataConcat[2] * magDefinition.resolution;
	newSample = false;
}

bool MagDriver::hasNewSample() const { return newSample; }

MagDriver::State MagDriver::getState() const { return state; }

}  // namespace SlimeVR::Sensors::SoftFusion::Mag
