#include "EspNowMessages.h"

namespace SlimeVR {
namespace Network {
namespace EspNow {

PairingMessage createPairingMessage(uint8_t numOfChildren) {
	return {
		MessageTag::Pairing,
		numOfChildren
	};
}

TrackerStateMessage createTrackerStateMessage(TrackerReport report) {
	return {
		MessageTag::TrackerState,
		report
	};
}

}  // namespace EspNow
}  // namespace Network
}  // namespace SlimeVR