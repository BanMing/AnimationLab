#ifndef _H_IMGUI_SAMPLE_
#define _H_IMGUI_SAMPLE_

#include "Application.h"

class ImGUISample :public Application
{
public:
	void CreatGUI(_In_ HWND hWnd);
	void Initialize();
	void Update(float inDeltaTime);
	void Render(float inAspectRatio);
	void Shutdown();
};
#endif
