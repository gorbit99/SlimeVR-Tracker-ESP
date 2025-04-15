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

#pragma once

#include <cstdint>
#include <functional>

#include "../../../sensorinterface/RegisterInterface.h"
#include "MagDriver.h"

namespace SlimeVR::Sensors::SoftFusion::Mag {

class PollingMagDriver : public MagDriver {
public:
	explicit PollingMagDriver(std::function<RegisterInterface*(uint8_t)>&& getInterface
	);
	void update() final;

protected:
	void writeReg(uint8_t reg, uint8_t value) final;
	uint8_t readReg(uint8_t reg) final;
	void setId(uint8_t deviceId) final;

	void setup() final;

private:
	static constexpr uint32_t MagPollingRateHz = 100;
	static constexpr uint32_t MagPollingRateMicros = 1000'000'000 / MagPollingRateHz;

	uint64_t lastPollTimeMicros;
	RegisterInterface* interface {};
	std::function<RegisterInterface*(uint8_t)> getInterface;
};

}  // namespace SlimeVR::Sensors::SoftFusion::Mag
