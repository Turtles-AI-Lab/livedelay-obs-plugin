/*
LiveDelay - Delay Management Implementation
Core delay switching logic
*/

#include "delay-manager.h"
#include <obs-frontend-api.h>

LiveDelayManager::LiveDelayManager()
	: m_delayActive(false),
	  m_delaySeconds(0),
	  m_transitioning(false),
	  m_streamOutput(nullptr)
{
	blog(LOG_INFO, "[LiveDelay] Delay manager initialized");
}

LiveDelayManager::~LiveDelayManager()
{
	disableDelay();
}

bool LiveDelayManager::enableDelay(int seconds)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	if (seconds <= 0) {
		blog(LOG_ERROR,
		     "[LiveDelay] Invalid delay duration: %d seconds", seconds);
		return false;
	}

	if (m_delayActive && m_delaySeconds == seconds) {
		blog(LOG_INFO, "[LiveDelay] Delay already enabled at %d seconds",
		     seconds);
		return true;
	}

	blog(LOG_INFO, "[LiveDelay] Enabling delay: %d seconds", seconds);

	m_transitioning = true;
	bool success = applyDelay(seconds);
	m_transitioning = false;

	if (success) {
		m_delayActive = true;
		m_delaySeconds = seconds;
		blog(LOG_INFO, "[LiveDelay] Delay enabled successfully");
	} else {
		blog(LOG_ERROR, "[LiveDelay] Failed to enable delay");
	}

	return success;
}

bool LiveDelayManager::disableDelay()
{
	std::lock_guard<std::mutex> lock(m_mutex);

	if (!m_delayActive) {
		blog(LOG_INFO, "[LiveDelay] Delay already disabled");
		return true;
	}

	blog(LOG_INFO, "[LiveDelay] Disabling delay");

	m_transitioning = true;
	bool success = applyDelay(0);
	m_transitioning = false;

	if (success) {
		m_delayActive = false;
		m_delaySeconds = 0;
		blog(LOG_INFO, "[LiveDelay] Delay disabled successfully");
	} else {
		blog(LOG_ERROR, "[LiveDelay] Failed to disable delay");
	}

	return success;
}

bool LiveDelayManager::applyDelay(int seconds)
{
	// Get active streaming output
	m_streamOutput = getActiveStreamOutput();

	if (!m_streamOutput) {
		blog(LOG_WARNING,
		     "[LiveDelay] No active stream output found. Delay will apply when stream starts.");
		// Store the delay setting for when stream starts
		return true;
	}

	// OBS delay is set in nanoseconds
	uint64_t delay_ns = (uint64_t)seconds * 1000000000ULL;

	// Apply the delay to the output
	obs_output_set_delay(m_streamOutput, delay_ns,
			     OBS_OUTPUT_DELAY_PRESERVE);

	blog(LOG_INFO, "[LiveDelay] Applied %d second delay to stream output",
	     seconds);

	return true;
}

obs_output_t *LiveDelayManager::getActiveStreamOutput()
{
	// Get the main streaming output
	obs_output_t *output = obs_frontend_get_streaming_output();

	if (!output) {
		blog(LOG_WARNING,
		     "[LiveDelay] Could not get streaming output");
	}

	return output;
}

bool LiveDelayManager::setPreset(DelayPreset preset)
{
	switch (preset) {
	case DELAY_NONE:
		return disableDelay();
	case DELAY_30_SEC:
		return enableDelay(30);
	case DELAY_1_MIN:
		return enableDelay(60);
	case DELAY_2_MIN:
		return enableDelay(120);
	case DELAY_5_MIN:
		return enableDelay(300);
	default:
		blog(LOG_ERROR, "[LiveDelay] Invalid preset");
		return false;
	}
}

bool LiveDelayManager::setCustomDelay(int seconds)
{
	if (seconds < 0 || seconds > 600) {
		blog(LOG_ERROR,
		     "[LiveDelay] Custom delay out of range (0-600 seconds): %d",
		     seconds);
		return false;
	}

	if (seconds == 0) {
		return disableDelay();
	}

	return enableDelay(seconds);
}

bool LiveDelayManager::isDelayActive()
{
	return m_delayActive;
}

int LiveDelayManager::getCurrentDelay()
{
	return m_delaySeconds;
}

const char *LiveDelayManager::getStatusText()
{
	if (m_transitioning) {
		return "Adjusting delay...";
	}

	if (!m_delayActive) {
		return "No delay (Live)";
	}

	static char buffer[64];
	if (m_delaySeconds >= 60) {
		int minutes = m_delaySeconds / 60;
		int seconds = m_delaySeconds % 60;
		if (seconds == 0) {
			snprintf(buffer, sizeof(buffer), "Delay: %d min",
				 minutes);
		} else {
			snprintf(buffer, sizeof(buffer),
				 "Delay: %d min %d sec", minutes, seconds);
		}
	} else {
		snprintf(buffer, sizeof(buffer), "Delay: %d seconds",
			 m_delaySeconds);
	}

	return buffer;
}

bool LiveDelayManager::isTransitioning()
{
	return m_transitioning;
}
