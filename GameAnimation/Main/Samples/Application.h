#ifndef _H_APPLICATION_
#define _H_APPLICATION_
#include <windows.h>
#include "../3rd/imgui/imgui.h"
#include "../3rd/imgui/imgui_impl_win32.h"
#include "../3rd/imgui/imgui_impl_opengl3.h"

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

	void CreatGUI(_In_ HWND hWnd)
	{
		const char* glsl_version = "#version 130";

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		// Setup Dear ImGui style
		ImGui::StyleColorsLight();

		// Setup Platform/Renderer backends
		ImGui_ImplWin32_Init(hWnd);
		ImGui_ImplOpenGL3_Init(glsl_version);
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

	inline virtual void OnGUI()
	{
	}

	void ShutdownGUI()
	{
		// Cleanup
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	inline virtual void Shutdown()
	{
	}
};

#endif