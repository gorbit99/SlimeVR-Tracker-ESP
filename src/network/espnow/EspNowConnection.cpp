#include "EspNowConnection.h"

#include "globals.h"

#ifdef USE_ESPNOW_COMMUNICATION

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

#include "GlobalVars.h"
#include "messages/EspNowMessages.h"

SlimeVR::Network::EspNowConnection espnowConnection;

namespace SlimeVR {
    namespace Network {

        void receiverCallback(uint8_t *peerMac, uint8_t *incomingData, uint8_t dataLength) {
            espnowConnection.handleReceivedMessage(peerMac, incomingData, dataLength);
        }

        void EspNowConnection::init() {
            WiFi.mode(WIFI_STA);
            esp_now_init();
            esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);

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

            // TODO: set numOfChildren properly
            auto pairingMessage = EspNow::createPairingMessage(1);
            auto status = esp_now_send(
                configuration.getReceiverMacAddress().data(),
                reinterpret_cast<uint8_t *>(&pairingMessage),
                sizeof(pairingMessage)
            );
            if (status != 0) {
                m_Logger.fatal("Couldn't send pairing request!");
                return;
            }
            m_Logger.info("Broadcasted pairing request!");
        }

        void EspNowConnection::sendReport(EspNow::TrackerReport &report) {
            if (!configuration.hasReceiverMacAddress()) {
                return;
            }

            auto stateMessage = EspNow::createTrackerStateMessage(report);

            esp_now_send(
                configuration.getReceiverMacAddress().data(),
                reinterpret_cast<uint8_t *>(&stateMessage),
                sizeof(stateMessage)
            );
        }

        void EspNowConnection::addCurrentReceiverAsPeer() {
            uint8_t *currentPeer = esp_now_fetch_peer(true);

            if (currentPeer != nullptr) {
                esp_now_del_peer(currentPeer);
            }

            esp_now_add_peer(configuration.getReceiverMacAddress().data(), ESP_NOW_ROLE_CONTROLLER, 1, nullptr, 0);
        }

        void EspNowConnection::handleReceivedMessage(uint8_t *peerMac, uint8_t *incomingData, uint8_t dataLength) {
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