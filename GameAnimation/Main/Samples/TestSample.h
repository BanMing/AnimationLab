#ifndef _H_TEST_SAMPLE_
#define _H_TEST_SAMPLE_

#include "Application.h"
class TestSample : public Application
{
public:
	void Initialize();
	void Update(float deltaTime);
	void Render(float inAspectRatio);
	void OnGUI();
	void Shutdown();
};

#endif
