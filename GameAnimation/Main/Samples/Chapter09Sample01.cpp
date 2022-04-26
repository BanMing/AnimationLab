#include "Chapter09Sample01.h"
#include "../GLTF/GLTFLoader.h"

void Chapter09Sample01::Initialize()
{
	cgltf_data* data = LoadGLTFFile("Assets/Woman.gltf");

	Pose restPose = LoadRestPose(data);
	mRestPoseDraw = new DebugDraw();
	mRestPoseDraw->FromPose(restPose);
	mRestPoseDraw->UpdateOpenGLBuffers();

	mClips = LoadAnimationClips(data);
	mCurrentPose = restPose;
	mCurrentClip = 0;
	mCurPoseDraw = new DebugDraw();
	mCurPoseDraw->FromPose(mCurrentPose);
	mCurPoseDraw->UpdateOpenGLBuffers();

	// For the UI
	unsigned int numUIClips = (unsigned int)mClips.size();
	for (unsigned int i = 0; i < numUIClips; ++i)
	{
		if (mClips[i].GetName() == "Running")
		{
			mCurrentClip = i;
			break;
		}
	}
}

void Chapter09Sample01::Update(float inDeltaTime)
{
	mPlaybackTime = mClips[mCurrentClip].Sample(mCurrentPose,
												mPlaybackTime + inDeltaTime);
	mCurPoseDraw->FromPose(mCurrentPose);
	mCurPoseDraw->UpdateOpenGLBuffers();
}

void Chapter09Sample01::Render(float inAspectRatio)
{
	mat4 projection = perspective(60.0f, inAspectRatio, 0.01f, 1000.0f);
	mat4 view = lookAt(vec3(0, 4, 7), vec3(0, 4, 0), vec3(0, 1, 0));
	mat4 mvp = projection * view; // No model

	mRestPoseDraw->Draw(DebugDrawMode::Lines, vec3(1, 0, 0), mvp);
	mCurPoseDraw->Draw(DebugDrawMode::Lines, vec3(0, 0, 1), mvp);
}


void Chapter09Sample01::Shutdown()
{
	delete mRestPoseDraw;
	delete mCurPoseDraw;

	mClips.clear();
}