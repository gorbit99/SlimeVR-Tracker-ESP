#include "EspNowConnection.h"

#include "globals.h"

#ifdef USE_ESPNOW_COMMUNICATION

#include <Arduino.h>
#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <espnow.h>
#endif
#ifdef ESP32
#include <WiFi.h>
#include <esp_now.h>
#endif

#include "GlobalVars.h"
#include "messages/EspNowMessages.h"

SlimeVR::Network::EspNowConnection espnowConnection;

namespace SlimeVR {
    namespace Network {

        #ifdef ESP8266
        void receiverCallback(uint8_t *peerMac, uint8_t *incomingData, uint8_t dataLength) {
            espnowConnection.handleReceivedMessage(peerMac, incomingData, dataLength);
        }
        #endif

        #ifdef ESP32
        void receiverCallback(const esp_now_recv_info_t *espnowInfo, const uint8_t *incomingData, int dataLength) {
            espnowConnection.handleReceivedMessage(espnowInfo->src_addr, incomingData, dataLength);
        }
        #endif

        void EspNowConnection::init() {
            WiFi.mode(WIFI_STA);
            esp_now_init();
            #ifdef ESP8266
            esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
            #endif

            if (!configuration.hasReceiverMacAddress()) {
                m_Logger.info("No receiver paired! Pairing is required!");
            }

            // The default value of the mac address is broadcast
            addCurrentReceiverAsPeer();

            esp_now_register_recv_cb(receiverCallback);
        }

        void EspNowConnection::sendPairingRequest() {
            configuration.forgetReceiverMacAddress();
            addCurrentReceiverAsPeer();

            auto pairingMessage = EspNow::createPairingMessage(MAX_IMU_COUNT);
            auto status = esp_now_send(
                configuration.getReceiverMacAddress().data(),
                reinterpret_cast<uint8_t *>(&pairingMessage),
                sizeof(pairingMessage)
            );
            if (status != 0) {
                m_Logger.fatal("Couldn't send pairing request!");
                m_Logger.info("%d\n", status);
                return;
            }
            m_Logger.info("Broadcasted pairing request!");
        }

        void EspNowConnection::sendPacket(uint8_t imuId, Quat rotation, Vector3 acceleration) {
            if (!configuration.hasReceiverMacAddress()) {
                return;
            }

            EspNow::TrackerReport report(
                imuId,
                rotation,
                acceleration,
                currentBatteryVoltage
            );
            auto stateMessage = EspNow::createTrackerStateMessage(report);

            esp_now_send(
                configuration.getReceiverMacAddress().data(),
                reinterpret_cast<uint8_t *>(&stateMessage),
                sizeof(stateMessage)
            );
        }

        #ifdef ESP32
        void EspNowConnection::addCurrentReceiverAsPeer() {
            esp_now_peer_info_t peer;
            esp_err_t result = esp_now_fetch_peer(true, &peer);

            if (result == ESP_OK) {
                esp_now_del_peer(peer.peer_addr);
            }

            memset(&peer, 0, sizeof(peer));
            peer.channel = 0;
            peer.encrypt = false;
            memcpy(
                peer.peer_addr,
                configuration.getReceiverMacAddress().data(),
                sizeof(peer.peer_addr)
            );

            m_Logger.debug("Mac is %02x:%02x:%02x:%02x:%02x:%02x!",
                peer.peer_addr[0],
                peer.peer_addr[1],
                peer.peer_addr[2],
                peer.peer_addr[3],
                peer.peer_addr[4],
                peer.peer_addr[5]
            );
            esp_now_add_peer(&peer);
            // esp_now_add_peer(configuration.getReceiverMacAddress().data(), ESP_NOW_ROLE_CONTROLLER, 1, nullptr, 0);
        }
        #endif

        void EspNowConnection::setCurrentBatteryVoltage(float batteryVoltage) {
            currentBatteryVoltage = batteryVoltage;
        }

        void EspNowConnection::handleReceivedMessage(uint8_t *peerMac, const uint8_t *incomingData, uint8_t dataLength) {
            if (dataLength == 0) {
                return;
            }

            if (static_cast<EspNow::MessageTag>(incomingData[0]) != EspNow::MessageTag::PairingAck
                || dataLength != sizeof(EspNow::PairingAckMessage)
            ) {
                return;
            }

            m_Logger.debug("Received pairing ack message!");
            EspNow::PairingAckMessage ackMessage;
            memcpy(&ackMessage, incomingData, sizeof(ackMessage));

            m_Logger.debug("Received tracker id: %d!", ackMessage.trackerId);
            m_Logger.debug("Received from %02x:%02x:%02x:%02x:%02x:%02x!",
                peerMac[0],
                peerMac[1],
                peerMac[2],
                peerMac[3],
                peerMac[4],
                peerMac[5]
            );

            configuration.setStartingTrackerIndex(ackMessage.trackerId);
            std::array<uint8_t, 6> peerMacArray;
            memcpy(
                peerMacArray.data(),
                peerMac,
                sizeof(uint8_t) * 6
            );
            configuration.setReceiverMacAddress(peerMacArray);
        }

	}
}

#endif