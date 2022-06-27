#include "Chapter13Sample02.h"

void Chapter13Sample02::Initialize()
{
	mTarget.position = vec3(1, 2, 0);
	mIKSolver.Resize(kJointNum);

	mIKSolver.SetLocalTransform(0, Transform(vec3(0, 5, 0), quat(), vec3(1, 1, 1)));
	mIKSolver.SetLocalTransform(1, Transform(vec3(0.3, -1, 0), quat(), vec3(1, 1, 1)));
	mIKSolver.SetLocalTransform(2, Transform(vec3(0.1f, -.5f, 0), quat(), vec3(1, 1, 1)));
	mIKSolver.SetLocalTransform(3, Transform(vec3(-0.1f, .5f, 0), quat(), vec3(1, 1, 1)));
	mIKSolver.SetLocalTransform(4, Transform(vec3(-0.3f, -.3f, 0), quat(), vec3(1, 1, 1)));

	mPointDraw = new  DebugDraw();
	mTargetDraw[0] = new DebugDraw();
	mTargetDraw[1] = new DebugDraw();
	mTargetDraw[2] = new DebugDraw();
	mLineDraw = new DebugDraw();

	for (unsigned int i = 0; i < kJointNum; ++i)
	{
		Transform world = mIKSolver.GetGlobalTransform(i);
		mPointDraw->Push(world.position);
	}
	mPointDraw->UpdateOpenGLBuffers();

	mLineDraw->Push(mIKSolver.GetGlobalTransform(0).position);
	mLineDraw->Push(mIKSolver.GetGlobalTransform(1).position);

	mLineDraw->Push(mIKSolver.GetGlobalTransform(1).position);
	mLineDraw->Push(mIKSolver.GetGlobalTransform(2).position);
	
	mLineDraw->Push(mIKSolver.GetGlobalTransform(2).position);
	mLineDraw->Push(mIKSolver.GetGlobalTransform(3).position);

	mLineDraw->Push(mIKSolver.GetGlobalTransform(3).position);
	mLineDraw->Push(mIKSolver.GetGlobalTransform(4).position);

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

void Chapter13Sample02::Update(float inDeltaTime)
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
		(*mPointDraw)[i] = mIKSolver.GetGlobalTransform(i).position;
	}
	mPointDraw->UpdateOpenGLBuffers();

	(*mLineDraw)[0] = mIKSolver.GetGlobalTransform(0).position;
	(*mLineDraw)[1] = mIKSolver.GetGlobalTransform(1).position;

	(*mLineDraw)[2] = mIKSolver.GetGlobalTransform(1).position;
	(*mLineDraw)[3] = mIKSolver.GetGlobalTransform(2).position;
	
	(*mLineDraw)[4] = mIKSolver.GetGlobalTransform(2).position;
	(*mLineDraw)[5] = mIKSolver.GetGlobalTransform(3).position;
	
	(*mLineDraw)[6] = mIKSolver.GetGlobalTransform(3).position;
	(*mLineDraw)[7] = mIKSolver.GetGlobalTransform(4).position;

	mLineDraw->UpdateOpenGLBuffers();
}

void Chapter13Sample02::Render(float inAspectRatio)
{
	vec3 cameraPos(
		mCamDist * cosf(mCamYaw * QUAT_DEG2RAD) * sinf(mCamPitch * QUAT_DEG2RAD),
		mCamDist * cosf(mCamPitch * QUAT_DEG2RAD),
		mCamDist * sinf(mCamYaw * QUAT_DEG2RAD) * sinf(mCamPitch * QUAT_DEG2RAD)
	);
	mat4 projection = perspective(60.0f, inAspectRatio, 0.01f, 1000.0f);
	mat4 view = lookAt(cameraPos, vec3(0, 4, 0), vec3(0, 1, 0));
	mat4 mvp = projection * view; // No model

	mTargetDraw[0]->Draw(DebugDrawMode::Lines, vec3(1, 0, 0), mvp);
	mTargetDraw[1]->Draw(DebugDrawMode::Lines, vec3(0, 1, 0), mvp);
	mTargetDraw[2]->Draw(DebugDrawMode::Lines, vec3(0, 0, 1), mvp);

	for (unsigned int i = 0; i < kJointNum; ++i)
	{
		mPointDraw->Draw(DebugDrawMode::Points, vec3(1, 0, 1), mvp);
	}

	mLineDraw->Draw(DebugDrawMode::Lines, vec3(0, 1, 1), mvp);
}

void Chapter13Sample02::OnGUI()
{
	ImGui::Begin("FABR IK Solver");
	ImGui::SliderFloat("Target x", &mTarget.position.x, -5, 5);
	ImGui::SliderFloat("Target y", &mTarget.position.y, 0, 8);
	ImGui::SliderFloat("Target z", &mTarget.position.z, -7, 7);

	ImGui::SliderFloat("Pitch", &mCamPitch, kOrientationMin, kOrientationMax);
	ImGui::SliderFloat("Yaw", &mCamYaw, kOrientationMin, kOrientationMax);
	ImGui::SliderFloat("Distance", &mCamDist, kOrientationMin, kOrientationMax);
	ImGui::End();
}

void Chapter13Sample02::Shutdown()
{
	delete mPointDraw;
	delete mTargetDraw[0];
	delete mTargetDraw[1];
	delete mTargetDraw[2];
	delete mLineDraw;
}
