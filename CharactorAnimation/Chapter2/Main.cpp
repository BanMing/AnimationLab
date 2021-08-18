#include "FirstApp.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//User specified events
	switch (msg)
	{
		case WM_CREATE:
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;
	}

	//Default events
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmd, int showCmd)
{
	// Create a new Application object
	FirstApp app;

	// Initialize it
	if (!app.Initialize(hInstance, true, WndProc))
	{
		return FALSE;
	}

	// Start the windows message loop
	MSG msg;
	memset(&msg, 0, sizeof(MSG));

	// Keep track of the time
	DWORD startTime = GetTickCount();

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			// If there is a message, deal with it and send it onward
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

	// ... and Quit
	return (int)msg.wParam;
}