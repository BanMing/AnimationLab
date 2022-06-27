#pragma warning(disable : 28251)
#pragma warning(disable : 28159)

#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#include "3rd/glad/glad.h"
#undef APIENTRY
#include <windows.h>
#include <iostream>
#include "Samples/Application.h"
#include "3rd/imgui/imgui_impl_win32.h"
#include "3rd/imgui/imgui_impl_opengl3.h"
#include "Samples/Chapter13Sample02.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, PSTR, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void DrawFrameInfo();

#if _DEBUG
#pragma comment( linker, "/subsystem:console" )
int main(int argc, const char** argv)
{
	return WinMain(GetModuleHandle(NULL), NULL, GetCommandLineA(), SW_SHOWDEFAULT);
}
#else
#pragma comment( linker, "/subsystem:windows" )
#endif
#pragma comment(lib, "opengl32.lib")

#define WGL_CONTEXT_MAJOR_VERSION_ARB     0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB     0x2092
#define WGL_CONTEXT_FLAGS_ARB             0x2094
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB  0x00000001
#define WGL_CONTEXT_PROFILE_MASK_ARB      0x9126
typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC, HGLRC, const int*);

typedef const char* (WINAPI* PFNWGLGETEXTENSIONSSTRINGEXTPROC) (void);
typedef BOOL(WINAPI* PFNWGLSWAPINTERVALEXTPROC) (int);
typedef int (WINAPI* PFNWGLGETSWAPINTERVALEXTPROC) (void);

// CPU Frame Timers
struct FrameTimer
{
	// High level timers
	double frameTime = 0.0;
	float  deltaTime = 0.0f;
	// CPU timers
	double frameUpdate = 0.0;
	double frameRender = 0.0;
	double win32Events = 0.0;
	double imguiLogic = 0.0;
	double imguiRender = 0.0;
	double swapBuffer = 0.0;
	// GPU timers
	double imguiGPU = 0.0;
	double appGPU = 0.0;
};

Application* gApplication = 0;
GLuint gVertexArrayObject = 0;
GLuint gGpuApplicationStart = 0;
GLuint gGpuApplicationStop = 0;
GLuint gGpuImguiStart = 0;
GLuint gGpuImguiStop = 0;
bool gSlowFrame = false;
float gScaleFactor = 1.0f;
float gInvScaleFactor = 1.0f;
FrameTimer gDisplay;

const ImVec4 kRed = ImVec4(1, 0, 0, 1);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	gApplication = new Chapter13Sample02();

	#pragma region Windows Class


	WNDCLASSEX wndclass;
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wndclass.lpszMenuName = 0;
	wndclass.lpszClassName = L"Win32 Game Window";
	RegisterClassEx(&wndclass);

	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	int clientWidth = 800;
	int clientHeight = 600;
	RECT windowRect;
	SetRect(&windowRect, (screenWidth / 2) - (clientWidth / 2), (screenHeight / 2) - (clientHeight / 2), (screenWidth / 2) + (clientWidth / 2), (screenHeight / 2) + (clientHeight / 2));

	DWORD style = (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX); // WS_THICKFRAME to resize
	AdjustWindowRectEx(&windowRect, style, FALSE, 0);
	HWND hwnd = CreateWindowEx(0, wndclass.lpszClassName, L"Game Window", style, windowRect.left, windowRect.top, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, NULL, NULL, hInstance, szCmdLine);
	HDC hdc = GetDC(hwnd);

	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 32;
	pfd.cStencilBits = 8;
	pfd.iLayerType = PFD_MAIN_PLANE;
	int pixelFormat = ChoosePixelFormat(hdc, &pfd);
	SetPixelFormat(hdc, pixelFormat, &pfd);
	#pragma endregion

	#pragma region OpenGL Create
	HGLRC tempRC = wglCreateContext(hdc);
	wglMakeCurrent(hdc, tempRC);
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;
	wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

	const int attribList[] = {
	WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
	WGL_CONTEXT_MINOR_VERSION_ARB, 3,
	WGL_CONTEXT_FLAGS_ARB, 0,
	WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
	0, };

	HGLRC hglrc = wglCreateContextAttribsARB(hdc, 0, attribList);

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(tempRC);
	wglMakeCurrent(hdc, hglrc);

	if (!gladLoadGL())
	{
		std::cout << "Could not initialize GLAD\n";
	}
	else
	{
		std::cout << "OpenGL Version " << GLVersion.major << "." << GLVersion.minor << " loaded\n";
	}

	PFNWGLGETEXTENSIONSSTRINGEXTPROC _wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC)wglGetProcAddress("wglGetExtensionsStringEXT");
	bool swapControlSupported = strstr(_wglGetExtensionsStringEXT(), "WGL_EXT_swap_control") != 0;

	int vsynch = 0;
	if (swapControlSupported)
	{
		PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
		PFNWGLGETSWAPINTERVALEXTPROC wglGetSwapIntervalEXT = (PFNWGLGETSWAPINTERVALEXTPROC)wglGetProcAddress("wglGetSwapIntervalEXT");

		if (wglSwapIntervalEXT(1))
		{
			std::cout << "Enabled vsynch\n";
			vsynch = wglGetSwapIntervalEXT();
		}
		else
		{
			std::cout << "Could not enable vsynch\n";
		}
	}
	else
	{ // !swapControlSupported
		std::cout << "WGL_EXT_swap_control not supported\n";
	}

	glGenVertexArrays(1, &gVertexArrayObject);
	glBindVertexArray(gVertexArrayObject);
	#pragma endregion

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);
	gApplication->Initialize();
	gApplication->CreatGUI(hwnd);

	#pragma region Profiling Attributes
	// CPU timings
	LARGE_INTEGER timerFrequency;
	LARGE_INTEGER timerStart;
	LARGE_INTEGER timerStop;
	LARGE_INTEGER frameStart;
	LARGE_INTEGER frameStop;
	LONGLONG timerDiff;

	// GPU Timers
	bool firstRenderSample = true;
	GLint timerResultAvailable = 0;
	GLuint64 gpuStartTime = 0;
	GLuint64 gpuStopTime = 0;

	glGenQueries(1, &gGpuApplicationStart);
	glGenQueries(1, &gGpuApplicationStop);
	glGenQueries(1, &gGpuImguiStart);
	glGenQueries(1, &gGpuImguiStop);

	FrameTimer accumulator;
	memset(&gDisplay, 0, sizeof(FrameTimer));
	memset(&accumulator, 0, sizeof(FrameTimer));
	int frameCounter = 0;

	bool enableFrameTiming = true;
	if (!QueryPerformanceFrequency(&timerFrequency))
	{
		std::cout << "WinMain: QueryPerformanceFrequency failed\n";
		enableFrameTiming = false;
	}

	// Get Display Frequency
	HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY);
	MONITORINFOEX monitorInfo;
	monitorInfo.cbSize = sizeof(MONITORINFOEX);
	GetMonitorInfo(hMonitor, &monitorInfo);
	DEVMODE devMode;
	devMode.dmSize = sizeof(DEVMODE);
	devMode.dmDriverExtra = 0;
	EnumDisplaySettings(monitorInfo.szDevice, ENUM_CURRENT_SETTINGS, &devMode);
	int displayFrequency = (int)devMode.dmDisplayFrequency;
	double frameBudget = (1000.0 / (double)displayFrequency);

	#pragma endregion

	DWORD lastTick = GetTickCount();
	MSG msg;
	while (true)
	{
		#pragma region Win32 events
		QueryPerformanceCounter(&timerStart);
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		QueryPerformanceCounter(&timerStop);
		timerDiff = timerStop.QuadPart - timerStart.QuadPart;
		accumulator.win32Events += (double)timerDiff * 1000.0 / (double)timerFrequency.QuadPart;
		#pragma endregion

		QueryPerformanceCounter(&frameStart);

		#pragma region Update
		QueryPerformanceCounter(&timerStart);
		DWORD thisTick = GetTickCount();
		float deltaTime = float(thisTick - lastTick) * 0.001f;
		lastTick = thisTick;
		accumulator.deltaTime += deltaTime;
		if (gApplication != 0)
		{
			gApplication->Update(deltaTime);
		}
		QueryPerformanceCounter(&timerStop);
		timerDiff = timerStop.QuadPart - timerStart.QuadPart;
		accumulator.frameUpdate += (double)timerDiff * 1000.0 / (double)timerFrequency.QuadPart;
		#pragma endregion

		#pragma region Render
		QueryPerformanceCounter(&timerStart);
		if (gApplication != 0)
		{
			RECT clientRect;
			GetClientRect(hwnd, &clientRect);
			clientWidth = clientRect.right - clientRect.left;
			clientHeight = clientRect.bottom - clientRect.top;
			glViewport(0, 0, clientWidth, clientHeight);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			glPointSize(5.0f * gScaleFactor);
			glLineWidth(1.5f * gScaleFactor);
			glBindVertexArray(gVertexArrayObject);

			glClearColor(0.5f, 0.6f, 0.7f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glBindVertexArray(gVertexArrayObject);

			if (!firstRenderSample)
			{
				// Application GPU Timer
				glGetQueryObjectiv(gGpuApplicationStop, GL_QUERY_RESULT, &timerResultAvailable);
				while (!timerResultAvailable)
				{
					std::cout << "Waiting on app GPU timer!\n";
					glGetQueryObjectiv(gGpuApplicationStop, GL_QUERY_RESULT, &timerResultAvailable);
				}
				glGetQueryObjectui64v(gGpuApplicationStart, GL_QUERY_RESULT, &gpuStartTime);
				glGetQueryObjectui64v(gGpuApplicationStop, GL_QUERY_RESULT, &gpuStopTime);
				accumulator.appGPU += (double)(gpuStopTime - gpuStartTime) / 1000000.0;
			}

			glQueryCounter(gGpuApplicationStart, GL_TIMESTAMP);
			float aspect = (float)clientWidth / (float)clientHeight;
			gApplication->Render(aspect);
			glQueryCounter(gGpuApplicationStop, GL_TIMESTAMP);
		}
		QueryPerformanceCounter(&timerStop);
		timerDiff = timerStop.QuadPart - timerStart.QuadPart;
		accumulator.frameRender += (double)timerDiff * 1000.0 / (double)timerFrequency.QuadPart;
		#pragma endregion

		#pragma region IMGUI Update
		QueryPerformanceCounter(&timerStart);
		if (gApplication != 0)
		{
			DrawFrameInfo();
			gApplication->OnGUI();
		}
		QueryPerformanceCounter(&timerStop);
		timerDiff = timerStop.QuadPart - timerStart.QuadPart;
		accumulator.imguiLogic += (double)timerDiff * 1000.0 / (double)timerFrequency.QuadPart;
		#pragma endregion

		#pragma region IMGUI Render
		QueryPerformanceCounter(&timerStart);
		if (gApplication != 0)
		{
			if (!firstRenderSample)
			{
				// Imgui GPU Timer
				glGetQueryObjectiv(gGpuImguiStop, GL_QUERY_RESULT, &timerResultAvailable);
				while (!timerResultAvailable)
				{
					std::cout << "Waiting on imgui GPU timer!\n";
					glGetQueryObjectiv(gGpuImguiStop, GL_QUERY_RESULT, &timerResultAvailable);
				}
				glGetQueryObjectui64v(gGpuImguiStart, GL_QUERY_RESULT, &gpuStartTime);
				glGetQueryObjectui64v(gGpuImguiStop, GL_QUERY_RESULT, &gpuStopTime);
				accumulator.imguiGPU += (double)(gpuStopTime - gpuStartTime) / 1000000.0;
			}

			glQueryCounter(gGpuImguiStart, GL_TIMESTAMP);
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			glQueryCounter(gGpuImguiStop, GL_TIMESTAMP);
		}
		QueryPerformanceCounter(&timerStop);
		timerDiff = timerStop.QuadPart - timerStart.QuadPart;
		accumulator.imguiRender += (double)timerDiff * 1000.0 / (double)timerFrequency.QuadPart;
		#pragma endregion

		#pragma region Wait for GPU
		QueryPerformanceCounter(&timerStart);
		if (gVertexArrayObject != 0)
		{
			SwapBuffers(hdc);
			if (vsynch != 0)
			{
				glFinish();
			}
		}
		QueryPerformanceCounter(&timerStop);
		timerDiff = timerStop.QuadPart - timerStart.QuadPart;
		accumulator.swapBuffer += (double)timerDiff * 1000.0 / (double)timerFrequency.QuadPart;
		#pragma endregion

		QueryPerformanceCounter(&frameStop);
		timerDiff = frameStop.QuadPart - frameStart.QuadPart;
		double frameTime = (double)timerDiff * 1000.0 / (double)timerFrequency.QuadPart;
		accumulator.frameTime += frameTime;

		#pragma region Profiling house keeping
		firstRenderSample = false;
		if (++frameCounter >= 60)
		{
			frameCounter = 0;

			gDisplay.win32Events = accumulator.win32Events / 60.0;
			gDisplay.frameUpdate = accumulator.frameUpdate / 60.0;
			gDisplay.frameRender = accumulator.frameRender / 60.0;
			gDisplay.imguiLogic = accumulator.imguiLogic / 60.0;
			gDisplay.imguiRender = accumulator.imguiRender / 60.0;
			gDisplay.swapBuffer = accumulator.swapBuffer / 60.0;
			gDisplay.frameTime = accumulator.frameTime / 60.0;
			gDisplay.deltaTime = accumulator.deltaTime / 60.0f;
			gDisplay.appGPU = accumulator.appGPU / 60.0;
			gDisplay.imguiGPU = accumulator.imguiGPU / 60.0;

			memset(&accumulator, 0, sizeof(FrameTimer));
			gSlowFrame = gDisplay.frameTime >= frameBudget;
		}
		#pragma endregion 
	} // End of game loop

	if (gApplication != 0)
	{
		std::cout << "Expected application to be null on exit\n";
		delete gApplication;
	}

	return (int)msg.wParam;
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, iMsg, wParam, lParam))
		return true;

	switch (iMsg)
	{
	case WM_CLOSE:
		if (gApplication != 0)
		{
			gApplication->ShutdownGUI();
			gApplication->Shutdown();
			gApplication = 0;
			DestroyWindow(hwnd);
		}
		else
		{
			std::cout << "Already shut down!\n";
		}
		break;
	case WM_DESTROY:
		if (gVertexArrayObject != 0)
		{
			HDC hdc = GetDC(hwnd);
			HGLRC hglrc = wglGetCurrentContext();

			glBindVertexArray(0);
			glDeleteVertexArrays(1, &gVertexArrayObject);
			gVertexArrayObject = 0;

			wglMakeCurrent(NULL, NULL);
			wglDeleteContext(hglrc);
			ReleaseDC(hwnd, hdc);

			PostQuitMessage(0);
		}
		else
		{
			std::cout << "Got multiple destroy messages\n";
		}
		break;
	case WM_PAINT:
	case WM_ERASEBKGND:
		return 0;
	}

	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

void DrawFrameInfo()
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Profiling Infomation");

	ImGui::Text("Display Stats");
	ImGui::Spacing();
	ImGui::Text("Application average: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
	ImGui::Text("Frame: %.1f FPS", ImGui::GetIO().Framerate);
	ImGui::Separator();

	ImGui::Text("High Level Timers");
	ImGui::Spacing();
	if (gSlowFrame)
	{
		ImGui::TextColored(kRed, "Frame Time: %0.5f ms\0", gDisplay.frameTime);
		ImGui::TextColored(kRed, "Delta Time: %0.5f ms\0", gDisplay.deltaTime);
	}
	else
	{
		ImGui::Text("Frame Time: %0.5f ms\0", gDisplay.frameTime);
		ImGui::Text("Delta Time: %0.5f ms\0", gDisplay.deltaTime);
	}
	ImGui::Separator();

	ImGui::Text("GPU Timers");
	ImGui::Spacing();
	ImGui::Text("Game GPU: %0.5f ms\0", gDisplay.appGPU);
	ImGui::Text("IMGUI GPU: %0.5f ms\0", gDisplay.imguiGPU);
	ImGui::Separator();

	ImGui::Text("CPU Timers");
	ImGui::Spacing();
	ImGui::Text("Win32 Events: %0.5f ms\0", gDisplay.win32Events);
	ImGui::Text("Game Update: %0.5f ms\0", gDisplay.frameUpdate);
	ImGui::Text("Game Render: %0.5f ms\0", gDisplay.frameRender);
	ImGui::Text("IMGUI logic: %0.5f ms\0", gDisplay.imguiLogic);
	ImGui::Text("IMGUI render: %0.5f ms\0", gDisplay.imguiRender);
	ImGui::Text("Swap Buffers: %0.5f ms\0", gDisplay.swapBuffer);

	ImGui::End();
}
