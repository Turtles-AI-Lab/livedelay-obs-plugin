/*
LiveDelay OBS Plugin
Main plugin entry point and OBS module registration
*/

#include <obs-module.h>
#include <obs-frontend-api.h>
#include <QMainWindow>
#include <QAction>
#include <QMessageBox>
#include "delay-manager.h"
#include "ui-components.h"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("livedelay", "en-US")

LiveDelayManager *g_delayManager = nullptr;
LiveDelayDock *g_delayDock = nullptr;

bool obs_module_load(void)
{
	blog(LOG_INFO, "[LiveDelay] Plugin loaded");

	// Initialize delay manager
	g_delayManager = new LiveDelayManager();

	// Wait for OBS UI to be ready
	obs_frontend_add_event_callback(
		[](enum obs_frontend_event event, void *) {
			if (event == OBS_FRONTEND_EVENT_FINISHED_LOADING) {
				// Create and register dock widget
				QMainWindow *mainWindow =
					(QMainWindow *)obs_frontend_get_main_window();
				g_delayDock = new LiveDelayDock(mainWindow,
								g_delayManager);

				// Add to OBS docks
				obs_frontend_add_dock(g_delayDock);

				blog(LOG_INFO,
				     "[LiveDelay] UI components initialized");
			}
		},
		nullptr);

	return true;
}

void obs_module_unload(void)
{
	blog(LOG_INFO, "[LiveDelay] Plugin unloading");

	// Clean up
	if (g_delayDock) {
		delete g_delayDock;
		g_delayDock = nullptr;
	}

	if (g_delayManager) {
		delete g_delayManager;
		g_delayManager = nullptr;
	}

	blog(LOG_INFO, "[LiveDelay] Plugin unloaded");
}

const char *obs_module_name(void)
{
	return "LiveDelay";
}

const char *obs_module_description(void)
{
	return "Toggle stream delay without restarting. Built for tournament streamers.";
}
