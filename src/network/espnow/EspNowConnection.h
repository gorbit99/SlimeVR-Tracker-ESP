/*
	SlimeVR Code is placed under the MIT license
	Copyright (c) 2024 Gorbit99 & SlimeVR Contributors

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
#ifndef SLIMEVR_ESPNOWCONNECTION_H
#define SLIMEVR_ESPNOWCONNECTION_H

#include "globals.h"
#include "logging/Logger.h"
#include "messages/TrackerReport.h"

#ifdef ESP32
#include <esp_now.h>
#endif

#ifdef USE_ESPNOW_COMMUNICATION

namespace SlimeVR {
namespace Network {

class EspNowConnection {
public:
    void init();
    void sendPairingRequest();
	void sendPacket(uint8_t imuId, Quat rotation, Vector3 acceleration);
	void setCurrentBatteryVoltage(float batteryVoltage);

private:
    void addCurrentReceiverAsPeer();
	void handleReceivedMessage(uint8_t *peerMac, const uint8_t *incomingData, uint8_t dataLength);

    Logging::Logger m_Logger = Logging::Logger("EspNowConnection");
	float currentBatteryVoltage = 0;

	#ifdef ESP8266
	friend void receiverCallback(uint8_t *peerMac, uint8_t *incomingData, uint8_t dataLength);
	#endif
	
	#ifdef ESP32
	friend void receiverCallback(const esp_now_recv_info_t *espnowInfo, const uint8_t *incomingData, int dataLength);
	#endif
};

}  // namespace Network
}  // namespace SlimeVR

#endif

#endif