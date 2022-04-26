#ifndef _H_APPLICATION_
#define _H_APPLICATION_
#include <windows.h>

class Application
{
private:
	Application(const Application&);
	Application& operator=(const Application&);
public:
	inline Application()
	{
	}

	inline virtual ~Application()
	{
	}

	inline virtual void CreatGUI(_In_ HWND hWnd)
	{
	}

	inline virtual void Initialize()
	{
	}

	inline virtual void Update(float inDeltaTime)
	{
	}

	inline virtual void Render(float inAspectRatio)
	{
	}

	inline virtual void OnGUI() {}

	inline virtual void Shutdown()
	{
	}
};

#endif