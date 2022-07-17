/******************************************************************
** @File         : Chapter14Sample01.h
** @Author       : BanMing
** @Date         : 2022/07/17
** @Description  :
*******************************************************************/
#ifndef _H_CHAPTER14_SAMPLE01_
#define _H_CHAPTER14_SAMPLE01_

#include "Application.h"
#include "../OpenGL/Texture.h"
#include "../OpenGL/Shader.h"
#include "../Animation/Mesh.h"
#include "../Animation/Skeleton.h"
#include "../Animation/TClip.h"

#include <vector>

class Chapter14Sample01 : public Application
{
protected:
	std::vector<Mesh> mMeshes;
	Skeleton mSkeleton;
	Pose mCurPose;
	FastClip mCurClip;

	Texture* mDiffuseTexture;
	Shader* mLBSShader;
	Shader* mDQShader;

	std::vector<DualQuaternion> mDqPosePalette;
	std::vector<DualQuaternion> mDqInvBindPalette;
	std::vector<mat4> mLbPosePalette;
	std::vector<mat4> mLbInvBindPalette;

	float mPlayTime;
	int mBlendMode;
public:
	void Initialize();
	void Update(float deltaTime);
	void Render(float inAspectRatio);
	void OnGUI();
	void Shutdown();
};

#endif
