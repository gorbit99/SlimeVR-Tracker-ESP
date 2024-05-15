#include "ResetCounter.h"

#include <LittleFS.h>

#include "../FSHelper.h"

#define RESET_CLEAR_TIME_MS 1000
#define RESET_COUNT_PATH "/reset_count.bin"

namespace SlimeVR {
namespace Peripherals {

void ResetCounter::init() {
	if (!LittleFS.exists(RESET_COUNT_PATH)) {
		resetCount = 1;
		File file = LittleFS.open(RESET_COUNT_PATH, "w");
		file.write(resetCount);
		file.close();

		m_Logger.info("Current reset count: 1");

		return;
	}

	File file = LittleFS.open(RESET_COUNT_PATH, "r+");
	file.read(&resetCount, sizeof(resetCount));
	resetCount++;
	file.seek(0, fs::SeekSet);
	file.write(resetCount);
	file.close();

	m_Logger.info("Current reset count: %d", resetCount);
}

void ResetCounter::update() {
	if (timeSinceReset >= RESET_CLEAR_TIME_MS * 1000) {
		return;
	}

	timeSinceReset += micros();
	if (timeSinceReset < RESET_CLEAR_TIME_MS * 1000) {
		return;
	}

	File file = LittleFS.open(RESET_COUNT_PATH, "w");
	file.write(static_cast<uint8_t>(0));
	file.close();

	m_Logger.info("Reset clear time elapsed!");
}

uint8_t ResetCounter::getResetCount() {
    return resetCount;
}

}  // namespace Peripherals

}  // namespace SlimeVR