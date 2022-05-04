#ifndef _H_IMGUI_SAMPLE_
#define _H_IMGUI_SAMPLE_

#include "Application.h"

// CPU Frame Timers
struct FrameTimer
{
	// High Level timers
	double frameTime = 0.0f;
	float deltaTime = 0.0f;

	// CPU timers
	double frameUpdate = 0.0f;
	double frameRender = 0.0f;
	double win32Events = 0.0f;
	double imguiLogic = 0.0f;
	double imguiRender = 0.0f;
	double swapBuffer = 0.0f;

	// GPU timers
	double imguiGPU = 0.0f;
	double appGPU = 0.0f;
};

class ImGUISample :public Application
{
protected:
	bool mShowDemoWindow;
	bool mShowAnotherWindow;
	ImVec4 mClearColor;
public:
	void Initialize();
	void OnGUI();
};
#endif
