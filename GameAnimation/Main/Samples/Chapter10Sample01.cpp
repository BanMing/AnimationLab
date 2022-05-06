#include "Chapter10Sample01.h"
#include "../GLTF/GLTFLoader.h"

void Chapter10Sample01::Initialize()
{
	cgltf_data* data = LoadGLTFFile("Assets/Woman.gltf");
	mSkeleton = LoadSkeleton(data);

	mRestPoseDraw = new DebugDraw();
	mRestPoseDraw->FromPose(mSkeleton.GetRestPose());
	mRestPoseDraw->UpdateOpenGLBuffers();

	mBindPoseDraw = new DebugDraw();
	mBindPoseDraw->FromPose(mSkeleton.GetBindPose());
	mBindPoseDraw->UpdateOpenGLBuffers();

	mCurrentPose = mSkeleton.GetRestPose();
	mCurPoseDraw = new DebugDraw();
	mCurPoseDraw->FromPose(mSkeleton.GetRestPose());
	mCurPoseDraw->UpdateOpenGLBuffers();

	mClips = LoadAnimationClips(data);
	mCurrentClip = 0;
	mPlaybackTime = 0;
	for (unsigned int i = 0; i < mClips.size(); i++)
	{
		if (mClips[i].GetName() == "Walking")
		{
			mCurrentClip = i;
			break;
		}
	}
	FreeGLTFFile(data);
}
void Chapter10Sample01::Update(float inDeltaTime)
{
	mPlaybackTime=mClips[mCurrentClip].Sample(mCurrentPose, mPlaybackTime + inDeltaTime);
	
	mCurPoseDraw->FromPose(mCurrentPose);
	mCurPoseDraw->UpdateOpenGLBuffers();
}

void Chapter10Sample01::Render(float inAspectRatio)
{
	mat4 projection = perspective(60.0f, inAspectRatio, 0.01f, 1000.0f);
	mat4 view = lookAt(vec3(0, 4, 7), vec3(0, 4, 0), vec3(0, 1, 0));
	mat4 mvp = projection * view; // No model

	mRestPoseDraw->Draw(DebugDrawMode::Lines, vec3(1, 0, 0), mvp);
	mBindPoseDraw->Draw(DebugDrawMode::Lines, vec3(0, 1, 0), mvp);
	mCurPoseDraw->Draw(DebugDrawMode::Lines, vec3(0, 0, 1), mvp);
}

void Chapter10Sample01::Shutdown()
{
	delete mRestPoseDraw;
	delete mBindPoseDraw;
	delete mCurPoseDraw;
	
	mPlaybackTime = 0;
	mClips.clear();
	mClips.shrink_to_fit();
}