/******************************************************************
** @File         : Chapter13Sample01.cpp
** @Author       : BanMing
** @Date         : 2022/06/20
** @Description  :
*******************************************************************/

#include "Chapter13Sample01.h"
#include <iostream>

#define QUAT_DEG2RAD 0.0174533f

void Chapter13Sample01::Initialize()
{
	mTarget.position = vec3(1, 2, 0);
	mIKSolver.Resize(kJointNum);

	mIKSolver[0] = Transform(vec3(0, 5, 0), quat(), vec3(1, 1, 1));
	mIKSolver[1] = Transform(vec3(0.3, -1, 0), quat(), vec3(1, 1, 1));
	mIKSolver[2] = Transform(vec3(0.5f, -1.5f, 0), quat(), vec3(1, 1, 1));

	mPointDraw = new  DebugDraw();
	mTargetDraw[0] = new DebugDraw();
	mTargetDraw[1] = new DebugDraw();
	mTargetDraw[2] = new DebugDraw();
	mLineDraw = new DebugDraw();

	for (unsigned int i = 0; i < kJointNum; ++i)
	{
		Transform world = mIKSolver.GetGlobalTransform(i);
		printf("chan [%d] worl position:[%f,%f,%f] \n", i, world.position.x, world.position.y, world.position.z);
		printf("chan [%d] local position:[%f,%f,%f] \n", i, mIKSolver[i].position.x, mIKSolver[i].position.y, mIKSolver[i].position.z);
		mPointDraw->Push(world.position);
	}
	mPointDraw->UpdateOpenGLBuffers();

	mLineDraw->Push(mIKSolver.GetGlobalTransform(0).position);
	mLineDraw->Push(mIKSolver.GetGlobalTransform(1).position);
	mLineDraw->Push(mIKSolver.GetGlobalTransform(1).position);
	mLineDraw->Push(mIKSolver.GetGlobalTransform(2).position);

	mLineDraw->UpdateOpenGLBuffers();

	mTargetDraw[0]->Push(mTarget.position.v + vec3(kTargetLineOffset, 0, 0));
	mTargetDraw[0]->Push(mTarget.position.v + vec3(-kTargetLineOffset, 0, 0));
	mTargetDraw[0]->UpdateOpenGLBuffers();

	mTargetDraw[1]->Push(mTarget.position.v + vec3(0, kTargetLineOffset, 0));
	mTargetDraw[1]->Push(mTarget.position.v + vec3(0, -kTargetLineOffset, 0));
	mTargetDraw[1]->UpdateOpenGLBuffers();

	mTargetDraw[2]->Push(mTarget.position.v + vec3(0, 0, kTargetLineOffset));
	mTargetDraw[2]->Push(mTarget.position.v + vec3(0, 0, -kTargetLineOffset));
	mTargetDraw[2]->UpdateOpenGLBuffers();
	
	mCamPitch = 45.0f;
	mCamYaw = 60.0f;
	mCamDist = 7.0f;
}

void Chapter13Sample01::Update(float inDeltaTime)
{
	mIKSolver.Solve(mTarget);
	(*mTargetDraw[0])[0] = mTarget.position.v + vec3(kTargetLineOffset, 0, 0);
	(*mTargetDraw[0])[1] = mTarget.position.v + vec3(-kTargetLineOffset, 0, 0);
	mTargetDraw[0]->UpdateOpenGLBuffers();

	(*mTargetDraw[1])[0] = mTarget.position.v + vec3(0, kTargetLineOffset, 0);
	(*mTargetDraw[1])[1] = mTarget.position.v + vec3(0, -kTargetLineOffset, 0);
	mTargetDraw[1]->UpdateOpenGLBuffers();

	(*mTargetDraw[2])[0] = mTarget.position.v + vec3(0, 0, kTargetLineOffset);
	(*mTargetDraw[2])[1] = mTarget.position.v + vec3(0, 0, -kTargetLineOffset);
	mTargetDraw[2]->UpdateOpenGLBuffers();

	for (unsigned int i = 0; i < kJointNum; ++i)
	{
		Transform world = mIKSolver.GetGlobalTransform(i);
		mPointDraw->operator[](i) = world.position.v;
	}
	mPointDraw->UpdateOpenGLBuffers();

	mLineDraw->operator[](0) = mIKSolver.GetGlobalTransform(0).position.v;
	mLineDraw->operator[](1) = mIKSolver.GetGlobalTransform(1).position.v;
	mLineDraw->operator[](2) = mIKSolver.GetGlobalTransform(1).position.v;
	mLineDraw->operator[](3) = mIKSolver.GetGlobalTransform(2).position.v;
	mLineDraw->UpdateOpenGLBuffers();
}

void Chapter13Sample01::Render(float inAspectRatio)
{
	vec3 cameraPos(
		mCamDist * cosf(mCamYaw * QUAT_DEG2RAD) * sinf(mCamPitch * QUAT_DEG2RAD),
		mCamDist * cosf(mCamPitch * QUAT_DEG2RAD),
		mCamDist * sinf(mCamYaw * QUAT_DEG2RAD) * sinf(mCamPitch * QUAT_DEG2RAD)
	);
	mat4 projection = perspective(60.0f, inAspectRatio, 0.01f, 1000.0f);
	mat4 view = lookAt(cameraPos, vec3(0, 4, 0), vec3(0, 1, 0));
	mat4 mvp = projection * view; // No model

	mPointDraw->Draw(DebugDrawMode::Points, vec3(0, 1, 1), mvp);
	mLineDraw->Draw(DebugDrawMode::Lines, vec3(1, 0, 1), mvp);
	mTargetDraw[0]->Draw(DebugDrawMode::Lines, vec3(1, 0, 0), mvp);
	mTargetDraw[1]->Draw(DebugDrawMode::Lines, vec3(0, 1, 0), mvp);
	mTargetDraw[2]->Draw(DebugDrawMode::Lines, vec3(0, 0, 1), mvp);
}

void Chapter13Sample01::OnGUI()
{
	ImGui::Begin("CCD IK Solver");
	ImGui::SliderFloat("Target x", &mTarget.position.x, -5, 5);
	ImGui::SliderFloat("Target y", &mTarget.position.y, 0, 8);
	ImGui::SliderFloat("Target z", &mTarget.position.z, -7, 7);


	ImGui::SliderFloat("Pitch", &mCamPitch, kOrientationMin, kOrientationMax);
	ImGui::SliderFloat("Yaw", &mCamYaw, kOrientationMin, kOrientationMax);
	ImGui::SliderFloat("Distance", &mCamDist, kOrientationMin, kOrientationMax);
	ImGui::End();
}

void Chapter13Sample01::Shutdown()
{
	delete mPointDraw;
	delete mTargetDraw[0];
	delete mTargetDraw[1];
	delete mTargetDraw[2];
	delete mLineDraw;
}
