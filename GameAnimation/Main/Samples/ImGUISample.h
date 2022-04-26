#ifndef _H_IMGUI_SAMPLE_
#define _H_IMGUI_SAMPLE_

#include "Application.h"

class ImGUISample :public Application
{
public:
	void Initialize();
	void Update(float inDeltaTime);
	void Render(float inAspectRatio);
	void Shutdown();
};
#endif
