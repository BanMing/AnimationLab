/******************************************************************
** @File         : Chapter14Sample03.h
** @Author       : BanMing
** @Date         : 2022/06/28
** @Description  :
*******************************************************************/

#ifndef _H_CHAPTER13_SAMPLE03_
#define _H_CHAPTER13_SAMPLE03_
#include "Application.h"
#include "../OpenGL/DebugDraw.h"
#include "../OpenGL/Texture.h"
#include "../Animation/Mesh.h"

class Chapter13Sample03 :public Application
{
protected:
	Texture* mCourseTexture;
	std::vector<Mesh> mIKGround;

	//DebugDraw* mCurPoseDraw;

	Shader* mStaticShader;

	// UI control
	bool mIsShowGround;
public:
	void Initialize();
	void Update(float inDeltaTime);
	void Render(float inAspectRatio);
	void OnGUI() override;
	void Shutdown();
};

#endif // _H_CHAPTER13_SAMPLE03_

