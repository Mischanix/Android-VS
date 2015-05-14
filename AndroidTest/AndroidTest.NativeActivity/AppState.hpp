#pragma once

#include "all.h"

void AppState::Update() {}

void AppState::Draw() {
	if (display == NULL)
		return;

	// Just fill the screen with a color.
	float red = 0;
	float green = 0;
	float blue = 0;
	for (auto v : acceleration) {
		red += v.x;
		green += v.y;
		blue += v.z;
	}
	float bias = red > green ? green : red;
	bias = bias > blue ? blue : bias;
	red -= bias; green -= bias; blue -= bias;
	float norm = red > green ? red : green;
	norm = norm > blue ? norm : blue;
	red /= norm; blue /= norm; green /= norm;
	glClearColor(red, green, blue, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	eglSwapBuffers(display, surface);
}

void AppState::SaveState() {}

void AppState::LoadState(void *data) {}
