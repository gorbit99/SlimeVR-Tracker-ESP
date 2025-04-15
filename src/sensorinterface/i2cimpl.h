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

#include <i2cscan.h>

#include <cstdint>

#include "I2Cdev.h"
#include "RegisterInterface.h"
#include "SensorInterface.h"

namespace SlimeVR::Sensors {

struct I2CImpl : public RegisterInterface {
	I2CImpl(uint8_t devAddr, SensorInterface* interface)
		: m_devAddr(devAddr)
		, m_interface(interface) {}

	uint8_t readReg(uint8_t regAddr) const override {
		swapIn();
		Wire.beginTransmission(m_devAddr);
		Wire.write(regAddr);
		Wire.endTransmission();
		Wire.beginTransmission(m_devAddr);
		Wire.requestFrom(m_devAddr, 1u);
		uint8_t buffer = Wire.read();
		Wire.endTransmission();
		return buffer;
	}

	uint16_t readReg16(uint8_t regAddr) const override {
		swapIn();
		uint16_t buffer = 0;
		Wire.beginTransmission(m_devAddr);
		Wire.write(regAddr);
		Wire.endTransmission();
		Wire.beginTransmission(m_devAddr);
		Wire.requestFrom(m_devAddr, 2u);
		Wire.readBytes(reinterpret_cast<uint8_t*>(&buffer), sizeof(buffer));
		Wire.endTransmission();
		return buffer;
	}

	void writeReg(uint8_t regAddr, uint8_t value) const override {
		swapIn();
		Wire.beginTransmission(m_devAddr);
		Wire.write(regAddr);
		Wire.write(value);
		Wire.endTransmission();
	}

	void writeReg16(uint8_t regAddr, uint16_t value) const override {
		swapIn();
		uint8_t* bytes = reinterpret_cast<uint8_t*>(&value);
		Wire.beginTransmission(m_devAddr);
		Wire.write(regAddr);
		Wire.write(bytes[0]);
		Wire.write(bytes[1]);
		Wire.endTransmission();
	}

	void readBytes(uint8_t regAddr, uint8_t size, uint8_t* buffer) const override {
		if (size == 0) {
			return;
		}
		swapIn();
		Wire.beginTransmission(m_devAddr);
		Wire.write(regAddr);
		Wire.endTransmission();
		Wire.beginTransmission(m_devAddr);
		Wire.requestFrom(m_devAddr, size);
		Wire.readBytes(buffer, size);
		Wire.endTransmission();
	}

	void writeBytes(uint8_t regAddr, uint8_t size, uint8_t* buffer) const override {
		if (size == 0) {
			return;
		}
		swapIn();
		Wire.beginTransmission(m_devAddr);
		Wire.write(regAddr);
		for (size_t i = 0; i < size; i++) {
			Wire.write(buffer[i]);
		}
		Wire.endTransmission();
	}

	bool hasSensorOnBus() {
		swapIn();
		return I2CSCAN::hasDevOnBus(m_devAddr);
	}

	uint8_t getAddress() const override { return m_devAddr; }

	std::string toString() const {
		return std::string("I2C(") + std::to_string(m_devAddr) + std::string(")");
	}

	void swapIn() const {
		if (m_interface) {
			m_interface->swapIn();
		}
	}

private:
	uint8_t m_devAddr;
	SensorInterface* m_interface;
};

}  // namespace SlimeVR::Sensors
