#ifndef SLIMEVR_ESPNOWMESSAGES_H
#define SLIMEVR_ESPNOWMESSAGES_H

#include <stdint.h>

#include "TrackerReport.h"

namespace SlimeVR {
namespace Network {
namespace EspNow {

enum class MessageTag : uint8_t {
	Pairing = 0x00,
	PairingAck = 0x01,
	TrackerState = 0x02,
};

struct BaseMessage {
	MessageTag tag;
};

struct PairingMessage : BaseMessage {
	PairingMessage(uint8_t numOfChildren);
	uint8_t numOfChildren;
};

struct PairingAckMessage : BaseMessage {
	PairingAckMessage(uint8_t trackerId = 0);
	uint8_t trackerId;
};

struct TrackerStateMessage : BaseMessage {
	TrackerStateMessage(TrackerReport trackerReport);
	TrackerReport trackerReport;
};

}  // namespace EspNow
}  // namespace Network
}  // namespace SlimeVR

#endif