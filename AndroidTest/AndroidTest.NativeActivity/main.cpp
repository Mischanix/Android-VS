/*
* Copyright (C) 2010 The Android Open Source Project
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/

#include "all.h"

/**
* This is the main entry point of a native application that is using
* android_native_app_glue.  It runs in its own thread, with its own
* event loop for receiving input events and doing other things.
*/
void android_main(android_app *app) {
	AppState state{};

	memset(&state, 0, sizeof(state));
	app->userData = &state;
	app->onAppCmd = AppState::HandleAppCmd;
	app->onInputEvent = AppState::HandleInputEvent;
	state.app = app;

	// Prepare to monitor accelerometer
	state.sensorManager = ASensorManager_getInstance();
	state.accelerometerSensor = ASensorManager_getDefaultSensor(
	    state.sensorManager, ASENSOR_TYPE_ACCELEROMETER);
	state.sensorEventQueue = ASensorManager_createEventQueue(
	    state.sensorManager, app->looper, LOOPER_ID_USER, NULL, NULL);

	if (app->savedState != NULL) {
		// We are starting with a previous saved state; restore from it.
		state.LoadState(app->savedState);
	}

	state.animating = 1;

	// loop waiting for stuff to do.

	while (1) {
		// Read all pending events.
		int ident;
		int events;
		android_poll_source *source;

		// If not animating, we will block forever waiting for events.
		// If animating, we loop until all events are read, then continue
		// to draw the next frame of animation.
		while ((ident = ALooper_pollAll(state.animating ? 0 : -1, NULL, &events,
		                                (void **)&source)) >= 0) {

			// Process this event.
			if (source != NULL) {
				source->process(app, source);
			}

			// If a sensor has data, process it now.
			if (ident == LOOPER_ID_USER) {
				state.ProcessAcceleration();
			}

			// Check if we are exiting.
			if (app->destroyRequested != 0) {
				state.TerminateWindow();
				return;
			}
		}

		if (state.animating) {
			// Done with events; draw next animation frame.
			state.Update();

			// Drawing is throttled to the screen update rate, so there
			// is no need to do timing here.
			state.Draw();
		}
	}
}
