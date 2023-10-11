#include <windows.h>
#include "Example3_1.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	// Create a new Application object
	Example3_1 app;

	// Initialize it
	if (FAILED(app.Init(hInstance, true)))
		return 0;
	// Keep track of the time
	MSG msg;
	memset(&msg, 0, sizeof(MSG));

	// Keep track of the time
	DWORD startTime = GetTickCount();

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			// If there's a message, deal with it and send it onward
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else // Otherwise update the game
		{
			// Calculate the delta time
			DWORD t = GetTickCount();
			float deltaTime = (t - startTime) * 0.001f;

			// Update the application 
			app.Update(deltaTime);

			// Render the application
			app.Render();

			startTime = t;
		}
	}

	// Release all resources
	app.Cleanup();

	return (int)msg.wParam;
}