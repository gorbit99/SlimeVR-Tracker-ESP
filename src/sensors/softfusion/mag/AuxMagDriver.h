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

#include "MagDriver.h"

template <typename IMU>
concept ImuWithAux = requires(IMU& imu, uint8_t reg) { imu.readAux(reg); };

namespace SlimeVR::Sensors::SoftFusion::Mag {

template <ImuWithAux IMU>
class AuxMagDriver : public MagDriver {
public:
	explicit AuxMagDriver(IMU& imu)
		: MagDriver{"AuxMagDriver"}
		, imu{imu} {}
	void update() final {}

protected:
	void writeReg(uint8_t reg, uint8_t value) final { imu.writeAux(reg, value); }

	uint8_t readReg(uint8_t reg) final { return imu.readAux(reg); }

	void setId(uint8_t deviceId) final { imu.setAuxId(deviceId); }

	void setup() final {
		const auto callback = [&](const uint8_t magData[9]) {
			memcpy(lastSample, magData, sizeof(lastSample));
			newSample = true;
		};

		imu.setupAuxPolling(
			magDefinition.dataReg,
			magDefinition.dataWidth,
			std::move(callback)
		);
	}

private:
	IMU& imu;
};

}  // namespace SlimeVR::Sensors::SoftFusion::Mag
