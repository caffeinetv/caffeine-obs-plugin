/*
Plugin Name
Copyright (C) 2022 Caffeine.tv

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program. If not, see <https://www.gnu.org/licenses/>
*/

#include <obs-module.h>
#include "caffeine-dock-window.hpp"

#include <obs-frontend-api.h>

#include <util/dstr.h>

#include "plugin-macros.generated.h"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE(PLUGIN_NAME, "en-US")

bool obs_module_load(void)
{
	// Create config dir
	char *configDir = obs_module_config_path("");
	if (configDir) {
		os_mkdirs(configDir);
		bfree(configDir);
	}

	QMainWindow *mainWindow = (QMainWindow *)obs_frontend_get_main_window();

	if (mainWindow) {
		obs_frontend_push_ui_translation(obs_module_get_string);
		// Create Caffeine Dialog
		CaffeineDockWindow *caffeineDockWindow =
			new CaffeineDockWindow(mainWindow);
		obs_frontend_add_dock(caffeineDockWindow);
		obs_frontend_pop_ui_translation();

		obs_frontend_add_event_callback(
			[](enum obs_frontend_event event, void *private_data) {
				UNUSED_PARAMETER(private_data);
				if (event ==
				    OBS_FRONTEND_EVENT_STREAMING_STARTING) {
					// Started streaming
					blog(LOG_INFO,
					     "Obs started frontend streaming");
				} else if (event ==
					   OBS_FRONTEND_EVENT_STREAMING_STOPPED) {
					// Stop Caffeine Stream
					CaffeineDockWindow *caffeineDockWindow =
						static_cast<CaffeineDockWindow *>(
							private_data);
					caffeineDockWindow->stopStreaming();
					blog(LOG_INFO,
					     "Obs ended frontend streaming");
				}
			},
			nullptr);
		blog(LOG_INFO, "plugin loaded successfully (version %s)",
		     PLUGIN_VERSION);
	}

	return true;
}

const char *obs_module_name(void)
{
	return obs_module_text("Caffeine.Dock");
}

void obs_module_unload()
{
	blog(LOG_INFO, "plugin unloaded");
}
