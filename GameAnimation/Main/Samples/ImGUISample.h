#ifndef _H_IMGUI_SAMPLE_
#define _H_IMGUI_SAMPLE_

#include "Application.h"

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
