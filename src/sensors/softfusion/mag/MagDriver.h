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
#include <vector>

#include "../../../logging/Logger.h"

namespace SlimeVR::Sensors::SoftFusion::Mag {

using WriteRegFunc = std::function<void(uint8_t reg, uint8_t value)>;

struct MagDefinition {
	enum class DataWidth {
		SixByte,
		NineByte,
	};

	const char* name;

	uint8_t deviceId;

	uint8_t whoAmIReg;
	uint8_t expectedWhoAmI;

	std::function<void(const WriteRegFunc&)> setup;

	uint8_t dataReg;
	DataWidth dataWidth;

	float resolution;
};

class MagDriver {
public:
	explicit MagDriver(const char* loggingPrefix);
	bool init();
	void getMagSample(float outData[3]);
	[[nodiscard]] bool hasNewSample() const;
	virtual void update() = 0;

	enum class State {
		NotSetup,
		Ok,
		Error,
	};

	[[nodiscard]] State getState() const;

protected:
	virtual void writeReg(uint8_t reg, uint8_t value) = 0;
	virtual uint8_t readReg(uint8_t reg) = 0;
	virtual void setId(uint8_t deviceId) = 0;

	virtual void setup() = 0;

	MagDefinition magDefinition;
	uint8_t lastSample[9]{};
	bool newSample = false;

	Logging::Logger logger{"MagDriver"};

private:
	static std::vector<MagDefinition> supportedMags;
	State state = State::NotSetup;
};

}  // namespace SlimeVR::Sensors::SoftFusion::Mag
