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

#include "PollingMagDriver.h"

#include <limits>

namespace SlimeVR::Sensors::SoftFusion::Mag {

PollingMagDriver::PollingMagDriver(
	std::function<RegisterInterface*(uint8_t)>&& getInterface
)
	: MagDriver{"PollingMagDriver"}
	, getInterface{getInterface} {
	lastPollTimeMicros = micros();
};

void PollingMagDriver::writeReg(uint8_t reg, uint8_t value) {
	interface->writeReg(reg, value);
}

uint8_t PollingMagDriver::readReg(uint8_t reg) { return interface->readReg(reg); }

void PollingMagDriver::setup() {}

void PollingMagDriver::update() {
	uint64_t timeDiff;
	if (micros() < lastPollTimeMicros) {
		timeDiff = micros() + std::numeric_limits<uint64_t>::max() - lastPollTimeMicros;
	} else {
		timeDiff = micros() - lastPollTimeMicros;
	}

	if (timeDiff > MagPollingRateMicros) {
		return;
	}

	interface->readBytes(
		magDefinition.dataReg,
		magDefinition.dataWidth == MagDefinition::DataWidth::SixByte ? 6 : 9,
		lastSample
	);
	newSample = true;
	lastPollTimeMicros += MagPollingRateMicros;
}

void PollingMagDriver::setId(uint8_t deviceId) { interface = getInterface(deviceId); }

}  // namespace SlimeVR::Sensors::SoftFusion::Mag
