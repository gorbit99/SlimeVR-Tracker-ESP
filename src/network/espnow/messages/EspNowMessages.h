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

#pragma pack(push, 1)

struct PairingMessage {
	MessageTag tag;
	uint8_t numOfChildren;
};

PairingMessage createPairingMessage(uint8_t numOfChildren);

struct PairingAckMessage {
	MessageTag tag = MessageTag::PairingAck;
	uint8_t trackerId;
};

struct TrackerStateMessage {
	MessageTag tag = MessageTag::TrackerState;
	TrackerReport trackerReport;
};

#pragma pack(pop)

TrackerStateMessage createTrackerStateMessage(TrackerReport report);

}  // namespace EspNow
}  // namespace Network
}  // namespace SlimeVR

#endif