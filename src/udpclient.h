/*
    SlimeVR Code is placed under the MIT license
    Copyright (c) 2021 Eiren Rain

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

#ifndef _UDP_CLIENT_H_
#define _UDP_CLIENT_H_

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Arduino.h>
#include "quat.h"
#include "configuration.h"

#define PACKET_ROTATION 1
#define PACKET_GYRO 2
#define PACKET_HANDSHAKE 3
#define PACKET_ACCEL 4
#define PACKET_MAG 5
#define PACKET_RAW_CALIBRATION_DATA 6
#define PACKET_GYRO_CALIBRATION_DATA 7
#define PACKET_CONFIG 8
#define PACKET_RAW_MAGENTOMETER 9
#define PACKET_PING_PONG 10
#define PACKET_SERIAL 11
#define PACKET_BATTERY_LEVEL 12
#define PACKET_TAP 13
#define PACKET_RESET_REASON 14

#define PACKET_RECIEVE_HEARTBEAT 1
#define PACKET_RECIEVE_VIBRATE 2
#define PACKET_RECIEVE_HANDSHAKE 3
#define PACKET_RECIEVE_COMMAND 4

#define COMMAND_CALLIBRATE 1
#define COMMAND_SEND_CONFIG 2
#define COMMAND_BLINK 3

typedef void (* configRecievedCallback)(DeviceConfig const newConfig);
typedef void (* commandRecievedCallback)(int command, void * const commandData, int commandDataLength);

void connectClient();
void clientUpdate();
void sendQuat(float * const quaternion, int type);
void sendQuat(Quat * const quaternion, int type);
void sendVector(float * const result, int type);
void sendConfig(DeviceConfig * const config, int type);
void sendFloat(float const value, int type);
void sendByte(char const value, int type);
void sendRawCalibrationData(int * const data, int type);
void setConfigRecievedCallback(configRecievedCallback);
void setCommandRecievedCallback(commandRecievedCallback);
void sendSerial(uint8_t *const data, int length, int type);
void setUpWiFi(DeviceConfig * const config);
void wifiUpkeep();
bool isConnected();

template<typename T> T convert_chars(unsigned char* src);
template<typename T> unsigned char* convert_to_chars(T src, unsigned char* target);

#endif // _UDP_CLIENT_H_