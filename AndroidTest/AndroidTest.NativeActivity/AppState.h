#pragma once

struct AppState {
	android_app *app;

	ASensorManager *sensorManager;
	const ASensor *accelerometerSensor;
	ASensorEventQueue *sensorEventQueue;

	EGLDisplay display;
	EGLSurface surface;
	EGLContext context;
	int32_t animating : 1;
	int32_t width : 15;
	int32_t height : 16;

	CircularBuffer<Vec3f> acceleration;

	// Initialize OpenGL ES and EGL
	void InitDisplay() {
		const EGLint attribs[] = {EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		                          EGL_BLUE_SIZE,    8,
		                          EGL_GREEN_SIZE,   8,
		                          EGL_RED_SIZE,     8,
		                          EGL_NONE};
		EGLint w, h, format;
		EGLint numConfigs;
		EGLConfig config;
		EGLSurface surface;
		EGLContext context;

		EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

		eglInitialize(display, 0, 0);
		eglChooseConfig(display, attribs, &config, 1, &numConfigs);
		eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

		ANativeWindow_setBuffersGeometry(app->window, 0, 0, format);

		surface = eglCreateWindowSurface(display, config, app->window, NULL);
		context = eglCreateContext(display, config, NULL, NULL);

		if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
			LOGW("Unable to eglMakeCurrent");
			return;
		}

		eglQuerySurface(display, surface, EGL_WIDTH, &w);
		eglQuerySurface(display, surface, EGL_HEIGHT, &h);

		this->display = display;
		this->context = context;
		this->surface = surface;
		this->width = w;
		this->height = h;

		// Initialize GL state.
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
		glEnable(GL_CULL_FACE);
		glShadeModel(GL_SMOOTH);
		glDisable(GL_DEPTH_TEST);

		return;
	}

	void Update();
	void Draw();

	void SaveState();
	void LoadState(void *data);

	void TerminateWindow() {
		if (display != EGL_NO_DISPLAY) {
			eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE,
			               EGL_NO_CONTEXT);
			if (context != EGL_NO_CONTEXT) {
				eglDestroyContext(display, context);
			}
			if (surface != EGL_NO_SURFACE) {
				eglDestroySurface(display, surface);
			}
			eglTerminate(display);
		}
		animating = 0;
		display = EGL_NO_DISPLAY;
		context = EGL_NO_CONTEXT;
		surface = EGL_NO_SURFACE;
	}

	void ProcessAcceleration() {
		if (accelerometerSensor == NULL)
			return;
		ASensorEvent event;
		while (ASensorEventQueue_getEvents(sensorEventQueue, &event, 1) > 0) {
			acceleration.Push(Vec3f{event.acceleration.x, event.acceleration.y,
			                        event.acceleration.z});
		}
	}

	void OnAppCmd(int32_t cmd) {
		switch (cmd) {
		case APP_CMD_SAVE_STATE:
			// The system has asked us to save our current state.  Do so.
			SaveState();
			break;
		case APP_CMD_INIT_WINDOW:
			// The window is being shown, get it ready.
			if (app->window != NULL) {
				InitDisplay();
				Draw();
			}
			break;
		case APP_CMD_TERM_WINDOW:
			// The window is being hidden or closed, clean it up.
			TerminateWindow();
			break;
		case APP_CMD_GAINED_FOCUS:
			// When our app gains focus, we start monitoring the accelerometer.
			if (accelerometerSensor != NULL) {
				ASensorEventQueue_enableSensor(sensorEventQueue,
				                               accelerometerSensor);
				// We'd like to get 60 events per second (in us).
				ASensorEventQueue_setEventRate(
				    sensorEventQueue, accelerometerSensor, 1000 * 1000 / 120);
			}
			animating = true;
			break;
		case APP_CMD_LOST_FOCUS:
			// When our app loses focus, we stop monitoring the accelerometer.
			// This is to avoid consuming battery while not being used.
			if (accelerometerSensor != NULL) {
				ASensorEventQueue_disableSensor(sensorEventQueue,
				                                accelerometerSensor);
			}
			// Also stop animating.
			animating = false;
			Draw();
			break;
		}
	}

	int32_t OnInputEvent(AInputEvent *event) { return 0; }

	static void HandleAppCmd(android_app *app, int32_t cmd) {
		auto state = (AppState *)app->userData;
		state->OnAppCmd(cmd);
	}

	static int32_t HandleInputEvent(android_app *app, AInputEvent *event) {
		auto state = (AppState *)app->userData;
		return state->OnInputEvent(event);
	}
};
