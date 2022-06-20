/******************************************************************
** @File         : Chapter13Sample01.cpp
** @Author       : BanMing
** @Date         : 2022/06/20
** @Description  :
*******************************************************************/

#include "Chapter13Sample01.h"

void Chapter13Sample01::Initialize()
{
	mJoints.resize(kJointNum);
	mIKSolver.Resize(kJointNum);

	mJoints[0] = Transform(vec3(13, 14, 0), quat(), vec3(1, 1, 1));

	mJoints[1] = Transform(vec3(12, 11, 0), quat(), vec3(1, 1, 1));
	//mJoints[1] = combine(mJoints[0], mJoints[1]);

	mJoints[2] = Transform(vec3(12.5f, 10, 0), quat(), vec3(1, 1, 1));
	//mJoints[2] = combine(mJoints[1], mJoints[2]);

	mDebugDraw = new  DebugDraw();

	for (unsigned int i = 0; i < kJointNum; ++i)
	{
		mDebugDraw->Push(mJoints[i].position);
	}
	mDebugDraw->UpdateOpenGLBuffers();

}

void Chapter13Sample01::Update(float inDeltaTime)
{
	//mDebugDraw->operator[](0) = mTargetPosition.v;
	mDebugDraw->UpdateOpenGLBuffers();
}

void Chapter13Sample01::Render(float inAspectRatio)
{
	mat4 model;
	mat4 view = lookAt(vec3(0, 0, 5), vec3(0, 0, 0), vec3(0, 1, 0));
	mat4 projection = ortho(0, inAspectRatio * 22.0f, 0, 22, 0.001f, 10);

	mat4 mvp = projection * view * model;

	mDebugDraw->Draw(DebugDrawMode::Points, vec3(0, 0, 1), mvp);
}

void Chapter13Sample01::OnGUI()
{
	ImGui::Begin("CCD IK Solver");
	ImGui::SliderFloat("Target x", &mTargetPosition.x, kPositionMin, kPositionMax);
	ImGui::SliderFloat("Target y", &mTargetPosition.y, kPositionMin, kPositionMax);
	ImGui::SliderFloat("Target z", &mTargetPosition.z, kPositionMin, kPositionMax);


	ImGui::SliderFloat("Pitch", &mTargetOrientation.x, kOrientationMin, kOrientationMax);
	ImGui::SliderFloat("Yaw", &mTargetOrientation.y, kOrientationMin, kOrientationMax);
	ImGui::SliderFloat("Distance", &mTargetOrientation.z, kOrientationMin, kOrientationMax);
	ImGui::End();
}

void Chapter13Sample01::Shutdown()
{
	mJoints.clear();
	mJoints.shrink_to_fit();
	delete mDebugDraw;
}
