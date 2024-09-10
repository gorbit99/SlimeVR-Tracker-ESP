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

#include "serialcommands.h"

#include <CmdCallback.hpp>

#include "GlobalVars.h"
#include "base64.hpp"
#include "utils.h"

#if ESP32
#include "nvs_flash.h"
#endif

namespace SerialCommands {
CmdCallback<6> cmdCallbacks;
CmdParser cmdParser;
CmdBuffer<256> cmdBuffer;

bool lengthCheck(
	const char* const text,
	unsigned int length,
	const char* const cmd,
	const char* const name
) {
	size_t l = text != nullptr ? strlen(text) : 0;
	if ((l > length)) {
		return false;
	}
	return true;
}

unsigned int
decode_base64_length_null(const char* const b64char, unsigned int* b64ssidlength) {
	if (b64char == NULL) {
		return 0;
	}
	*b64ssidlength = (unsigned int)strlen(b64char);
	return decode_base64_length((unsigned char*)b64char, *b64ssidlength);
}

void cmdSet(CmdParser* parser) {}

void printState() {}

void cmdGet(CmdParser* parser) {
	if (parser->getParamCount() < 2) {
		return;
	}

	if (parser->equalCmdParam(1, "INFO")) {
		printState();

		// We don't want to print this on every timed state output
	}

	if (parser->equalCmdParam(1, "CONFIG")) {
		String str
			= "BOARD=%d\n"
			  "IMU=%d\n"
			  "SECOND_IMU=%d\n"
			  "IMU_ROTATION=%f\n"
			  "SECOND_IMU_ROTATION=%f\n"
			  "BATTERY_MONITOR=%d\n"
			  "BATTERY_SHIELD_RESISTANCE=%d\n"
			  "BATTERY_SHIELD_R1=%d\n"
			  "BATTERY_SHIELD_R2=%d\n"
			  "PIN_IMU_SDA=%d\n"
			  "PIN_IMU_SCL=%d\n"
			  "PIN_IMU_INT=%d\n"
			  "PIN_IMU_INT_2=%d\n"
			  "PIN_BATTERY_LEVEL=%d\n"
			  "LED_PIN=%d\n"
			  "LED_INVERTED=%d\n";

		Serial.printf(
			str.c_str(),
			BOARD,
			static_cast<int>(sensorManager.getSensorType(0)),
			static_cast<int>(sensorManager.getSensorType(1)),
			IMU_ROTATION,
			SECOND_IMU_ROTATION,
			BATTERY_MONITOR,
			BATTERY_SHIELD_RESISTANCE,
			BATTERY_SHIELD_R1,
			BATTERY_SHIELD_R2,
			PIN_IMU_SDA,
			PIN_IMU_SCL,
			PIN_IMU_INT,
			PIN_IMU_INT_2,
			PIN_BATTERY_LEVEL,
			LED_PIN,
			LED_INVERTED
		);
	}
}

void cmdReboot(CmdParser* parser) { ESP.restart(); }

void cmdFactoryReset(CmdParser* parser) {
#if ESP8266
#elif ESP32
	nvs_flash_erase();
#else
#warning SERIAL COMMAND FACTORY RESET NOT SUPPORTED
	return;
#endif

#if defined(WIFI_CREDS_SSID) && defined(WIFI_CREDS_PASSWD)
#warning FACTORY RESET does not clear your hardcoded WiFi credentials!
#endif

	delay(3000);
	ESP.restart();
}

void cmdTemperatureCalibration(CmdParser* parser) {}

void setUp() {
	cmdCallbacks.addCmd("SET", &cmdSet);
	cmdCallbacks.addCmd("GET", &cmdGet);
	cmdCallbacks.addCmd("FRST", &cmdFactoryReset);
	cmdCallbacks.addCmd("REBOOT", &cmdReboot);
	cmdCallbacks.addCmd("TCAL", &cmdTemperatureCalibration);
}

void update() { cmdCallbacks.updateCmdProcessing(&cmdParser, &cmdBuffer, &Serial); }
}  // namespace SerialCommands
