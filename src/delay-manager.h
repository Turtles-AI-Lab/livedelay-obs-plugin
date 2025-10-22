/*
LiveDelay - Delay Management System
Handles the core logic for adding/removing stream delay
*/

#pragma once

#include <obs.h>
#include <vector>
#include <mutex>

enum DelayPreset {
	DELAY_NONE = 0,
	DELAY_30_SEC = 30,
	DELAY_1_MIN = 60,
	DELAY_2_MIN = 120,
	DELAY_5_MIN = 300,
	DELAY_CUSTOM = -1
};

class LiveDelayManager {
public:
	LiveDelayManager();
	~LiveDelayManager();

	// Core functionality
	bool enableDelay(int seconds);
	bool disableDelay();
	bool isDelayActive();
	int getCurrentDelay();

	// Preset management
	bool setPreset(DelayPreset preset);
	bool setCustomDelay(int seconds);

	// Status
	const char *getStatusText();
	bool isTransitioning();

private:
	// Delay state
	bool m_delayActive;
	int m_delaySeconds;
	bool m_transitioning;

	// OBS output references
	obs_output_t *m_streamOutput;

	// Internal methods
	bool applyDelay(int seconds);
	void updateStreamDelay();
	obs_output_t *getActiveStreamOutput();

	// Thread safety
	std::mutex m_mutex;
};
