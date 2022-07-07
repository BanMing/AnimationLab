#include "Chapter13Sample03.h"
#include "../GLTF/GLTFLoader.h"
#include "../OpenGL/Uniform.h"
#include "../3rd/glad/glad.h"
#include "../Physics/Ray.h"
#include "../Animation/Blending.h"
#include <iostream>

#pragma region Helpers
void GetScalarValues(std::vector<float>& outScalars, unsigned int inComponentCount, const cgltf_accessor& inAccessor)
{
	outScalars.resize(inAccessor.count * inComponentCount);
	for (cgltf_size i = 0; i < inAccessor.count; i++)
	{
		cgltf_accessor_read_float(&inAccessor, i, &outScalars[i * inComponentCount], inComponentCount);
	}
}

void StaticMeshFromAttribute(Mesh& outMesh, cgltf_attribute& attribute)
{
	cgltf_attribute_type attributeType = attribute.type;
	cgltf_accessor& accessor = *attribute.data;

	unsigned int componentCount = 0;
	if (accessor.type == cgltf_type_vec2)
	{
		componentCount = 2;
	}
	else if (accessor.type == cgltf_type_vec3)
	{
		componentCount = 3;
	}
	else if (accessor.type == cgltf_type_vec4)
	{
		componentCount = 4;
	}
	else
	{
		std::cout << "Unknown data type\n";
		return;
	}

	std::vector<float> values;
	GetScalarValues(values, componentCount, accessor);
	unsigned int accessorCount = (unsigned int)accessor.count;

	std::vector<vec3>& positions = outMesh.GetPosition();
	std::vector<vec3>& normals = outMesh.GetNormal();
	std::vector<vec2>& texCoords = outMesh.GetTexCoord();

	for (unsigned int i = 0; i < accessorCount; i++)
	{
		int index = i * componentCount;
		switch (attributeType)
		{
		case cgltf_attribute_type_position:
			positions.push_back(vec3(values[index + 0], values[index + 1], values[index + 2]));
			break;
		case cgltf_attribute_type_normal:
		{
			vec3 normal = vec3(values[index + 0], values[index + 1], values[index + 2]);
			if (lenSq(normal) < 0.000001f)
			{
				normal = vec3(0, 1, 0);
			}
			normals.push_back(normalized(normal));
		}
		break;
		case cgltf_attribute_type_texcoord:
			texCoords.push_back(vec2(values[index + 0], values[index + 1]));
			break;
		}
	}
}

std::vector<Mesh> LoadStaticMeshes(cgltf_data* data)
{
	std::vector<Mesh> result;
	cgltf_node* nodes = data->nodes;
	unsigned int nodeCount = (unsigned int)data->nodes_count;
	for (unsigned int i = 0; i < nodeCount; i++)
	{
		cgltf_node* node = &nodes[i];
		if (node->mesh == 0)
		{
			continue;
		}

		unsigned int numPrims = (unsigned int)node->mesh->primitives_count;
		for (unsigned int j = 0; j < numPrims; j++)
		{
			result.push_back(Mesh());
			Mesh& mesh = result[result.size() - 1];

			cgltf_primitive* primitive = &node->mesh->primitives[j];
			unsigned int numAttributes = (unsigned int)primitive->attributes_count;
			for (unsigned int k = 0; k < numAttributes; k++)
			{
				cgltf_attribute* attribute = &primitive->attributes[k];
				StaticMeshFromAttribute(mesh, *attribute);
			}

			if (primitive->indices != 0)
			{
				unsigned int indexCount = (unsigned int)primitive->indices->count;
				std::vector<unsigned int>& indices = mesh.GetIndices();
				indices.resize(indexCount);

				for (unsigned int k = 0; k < indexCount; k++)
				{
					indices[k] = (unsigned int)cgltf_accessor_read_index(primitive->indices, k);
				}
			}

			mesh.UpdateOpenGLBuffers();
		}
	}

	return result;
}

#pragma endregion Helpers

void Chapter13Sample03::Initialize()
{
	// Motion track, this position from the 4 corners of the ground
	mMotionTrack.Resize(5);
	mMotionTrack.SetInterpolation(Interpolation::Linear);
	mMotionTrack[0].mTime = 0.0f;
	mMotionTrack[0].mValue[0] = 0; mMotionTrack[0].mValue[2] = 1;
	mMotionTrack[1].mTime = 1.0f;
	mMotionTrack[1].mValue[0] = 0; mMotionTrack[1].mValue[2] = 10;
	mMotionTrack[2].mTime = 3.0f;
	mMotionTrack[2].mValue[0] = 22; mMotionTrack[2].mValue[2] = 10;
	mMotionTrack[3].mTime = 4.0f;
	mMotionTrack[3].mValue[0] = 22; mMotionTrack[3].mValue[2] = 2;
	mMotionTrack[4].mTime = 6.0f;
	mMotionTrack[4].mValue[0] = 0; mMotionTrack[4].mValue[2] = 1;

	cgltf_data* data = LoadGLTFFile("Assets/IKCourse.gltf");
	mIKGround = LoadStaticMeshes(data);
	FreeGLTFFile(data);
	mCourseTexture = new Texture("Assets/uv.png");
	mTriangles = MeshesToTriangles(mIKGround);

	data = LoadGLTFFile("Assets/Woman.gltf");
	mSkeleton = LoadSkeleton(data);
	std::vector<FastClip> clips = LoadFastClips(data);
	for (size_t i = 0; i < clips.size(); i++)
	{
		if (clips[i].GetName() == "Walking")
		{
			mWalkClip = clips[i];
			break;
		}
	}
	mCharacterMeshes = LoadMeshes(data);
	mCurPose = mSkeleton.GetRestPose();
	FreeGLTFFile(data);
	mDiffuseTexture = new Texture("Assets/Woman.png");

	mStaticShader = new Shader("Shaders/static.vert", "Shaders/lit.frag");
	mDynamicShader = new Shader("Shaders/preskinned.vert", "Shaders/lit.frag");

	mLeftLeg = new IKLeg(mSkeleton, "LeftUpLeg", "LeftLeg", "LeftFoot", "LeftToeBase");
	mLeftLeg->SetAnkleOffset(0.2f);
	mRightLeg = new IKLeg(mSkeleton, "RightUpLeg", "RightLeg", "RightFoot", "RightToeBase");
	mRightLeg->SetAnkleOffset(0.2f);

	ScalarTrack& rightTrack = mRightLeg->GetTrack();
	rightTrack.SetInterpolation(Interpolation::Cubic);
	rightTrack.Resize(4);
	rightTrack[0].mValue[0] = 1;
	rightTrack[1].mValue[0] = 0;
	rightTrack[2].mValue[0] = 0;
	rightTrack[3].mValue[0] = 1;
	rightTrack[0].mTime = 0.0f;
	rightTrack[1].mTime = 0.3f;
	rightTrack[2].mTime = 0.7f;
	rightTrack[3].mTime = 1.0f;

	ScalarTrack& leftTrack = mLeftLeg->GetTrack();
	leftTrack.SetInterpolation(Interpolation::Cubic);
	leftTrack.Resize(4);
	leftTrack[0].mValue[0] = 0;
	leftTrack[1].mValue[0] = 1;
	leftTrack[2].mValue[0] = 1;
	leftTrack[3].mValue[0] = 0;
	leftTrack[0].mTime = 0.0f;
	leftTrack[1].mTime = 0.4f;
	leftTrack[2].mTime = 0.6f;
	leftTrack[3].mTime = 1.0f;

	mCharacterDraw = new DebugDraw();
	mCharacterDraw->FromPose(mCurPose);
	mCharacterDraw->UpdateOpenGLBuffers();

	mCurPoseDraw = new DebugDraw();
	mCurPoseDraw->FromPose(mCurPose);
	mCurPoseDraw->UpdateOpenGLBuffers();

	mIsShowGround = true;
	mIsShowCharacter = true;

	// Start the character clamped to the ground.
	Ray groundRay(vec3(mCharacterTrans.position.x, kRayOriginY, mCharacterTrans.position.z));
	unsigned int numTriangles = (unsigned int)mTriangles.size();
	vec3 hitPoint;
	for (unsigned int i = 0; i < numTriangles; i++)
	{
		if (RaycastTriangle(groundRay, mTriangles[i], hitPoint))
		{
			mCharacterTrans.position = hitPoint;
			break;
		}
	}
	// Move down a little bit so it`s not perfectly up
	mCharacterTrans.position.y -= kSinkIntoGround;
	mLastCharacterY = mCharacterTrans.position.y;
}

void Chapter13Sample03::Update(float inDeltaTime)
{
	// Increment time and sample the animation clip that moves the model on the level rails
	// The Y position is a lie, it`s a trackt hat only makes sense from an ortho top view
	mWalkingTime += inDeltaTime * 0.3f;

	// Figure out the X and Z position of the character in world space 
	vec3 currentPosition = mMotionTrack.Sample(mWalkingTime, true);
	vec3 nextPosition = mMotionTrack.Sample(mWalkingTime + 0.1f, true);

	// Keep the Y position the same as last frame for both to  properly orient the character
	float lastYPos = mCharacterTrans.position.y;
	currentPosition.y = lastYPos;
	nextPosition.y = lastYPos;

	mCharacterTrans.position = currentPosition;

	// Figure out the forward direction of the character in world space
	vec3 direction = normalized(nextPosition - currentPosition);
	quat newDirection = lookRotation(direction, vec3(0, 1, 0));
	if (dot(mCharacterTrans.rotation, newDirection) < 0.0f)
	{
		newDirection = newDirection * -1.0f;
	}
	mCharacterTrans.rotation = nlerp(mCharacterTrans.rotation, newDirection, inDeltaTime * 10.0f);
	vec3 characterForward = mCharacterTrans.rotation * vec3(0, 0, 1);

	// Figure out the Y position of the model in world spcae
	Ray groundRay(vec3(mCharacterTrans.position.x, kRayOriginY, mCharacterTrans.position.z));
	unsigned int numTriangles = (unsigned int)mTriangles.size();
	vec3 hitPoint;
	for (unsigned int i = 0; i < numTriangles; i++)
	{
		if (RaycastTriangle(groundRay, mTriangles[i], hitPoint))
		{
			// Sink the character a little bit into the ground to avoid hyper extending it`s legs
			mCharacterTrans.position = hitPoint - vec3(0, kSinkIntoGround, 0);
			break;
		}
	}

	// Sample the current animation, update the current pose draw
	mPlayTime = mWalkClip.Sample(mCurPose, mPlayTime + inDeltaTime);
	mCharacterDraw->FromPose(mCurPose);
	mCharacterDraw->UpdateOpenGLBuffers();

	// Figure out where the left and right leg are in their up/down animation cycle
	float normalizedTime = (mPlayTime - mWalkClip.GetStartTime()) / mWalkClip.GetDuration();
	if (normalizedTime < 0.0f) { std::cout << "should not be < 0\n"; normalizedTime = 0.0f; }
	if (normalizedTime > 1.0f) { std::cout << "should not be < 0\n"; normalizedTime = 1.0f; }
	float leftMotion = mLeftLeg->GetTrack().Sample(normalizedTime, true);
	float rightMotion = mRightLeg->GetTrack().Sample(normalizedTime, true);

	// Construct a ray for the left ankle, store the world position and the predictive position
	// of the ankle.This is in case the raycasts below don`t hit anything
	vec3 worldLeftAnkle = combine(mCharacterTrans, mCurPose.GetGlobalTransform(mLeftLeg->Ankle())).position;
	Ray leftAnkleRay(worldLeftAnkle + vec3(0, 2, 0));
	vec3 predictiveLeftAnkle = worldLeftAnkle;

	// Construct a ray for the right ankle, store the world position and the predictive position
	// of the ankle. This is in case the raycasts below don`t hit anything.
	vec3 worldRightAnkle = combine(mCharacterTrans, mCurPose.GetGlobalTransform(mRightLeg->Ankle())).position;
	Ray rightAnkleRay(worldRightAnkle + vec3(0, 2, 0));
	vec3 predictiveRightAnkle = worldRightAnkle;

	// Perform some raycasts for the feet, these are done in world space and
	// will define the IK based target points. For each ankle, we need to know
	// the current position (raycast from knee height to the sole of the foot height)
	// and the predictive position(infinate ray cast). The target point will be
	// between these two goals
	vec3 groundReference = mCharacterTrans.position;
	for (unsigned int i = 0; i < numTriangles; i++)
	{
		if (RaycastTriangle(leftAnkleRay, mTriangles[i], hitPoint))
		{
			if (lenSq(hitPoint - leftAnkleRay.origin) < kRayHeight * kRayHeight)
			{
				worldLeftAnkle = hitPoint;
				if (hitPoint.y < groundReference.y)
				{
					groundReference = hitPoint - vec3(0, kSinkIntoGround, 0);
				}
			}
			predictiveLeftAnkle = hitPoint;
		}

		if (RaycastTriangle(rightAnkleRay, mTriangles[i], hitPoint))
		{
			if (lenSq(hitPoint - rightAnkleRay.origin) < kRayHeight * kRayHeight)
			{
				worldRightAnkle = hitPoint;

				if (hitPoint.y < groundReference.y)
				{
					groundReference = hitPoint - vec3(0, kSinkIntoGround, 0);
				}
			}
			predictiveRightAnkle = hitPoint;
		}
	}

	// Lerp the Y position of the mcharacter over a small period of time
	// Just to avoid poping
	mCharacterTrans.position.y = mLastCharacterY;
	mCharacterTrans.position = lerp(mCharacterTrans.position, groundReference, inDeltaTime * 10.f);
	mLastCharacterY = mCharacterTrans.position.y;

	// Now that we know the position of the model, as well as the ankle we can solve the feet.
	mLeftLeg->SolveForLeg(mCharacterTrans, mCurPose, worldLeftAnkle);
	mRightLeg->SolveForLeg(mCharacterTrans, mCurPose, worldRightAnkle);

	// Apply the solved feet
	Blend(mCurPose, mCurPose, mLeftLeg->GetAdjustedPose(), 1, mLeftLeg->Hip());
	Blend(mCurPose, mCurPose, mRightLeg->GetAdjustedPose(), 1, mRightLeg->Hip());

	// The toes are still wrong, let`s fix those. First, construct some rays for the toes
	Transform leftAnkleWorld = combine(mCharacterTrans, mCurPose.GetGlobalTransform(mLeftLeg->Ankle()));
	Transform rightAnkleWorld = combine(mCharacterTrans, mCurPose.GetGlobalTransform(mRightLeg->Ankle()));

	vec3 worldLeftToe = combine(mCharacterTrans, mCurPose.GetGlobalTransform(mLeftLeg->Toe())).position;
	vec3 leftToeTarget = worldLeftToe;
	vec3 predictiveLeftToe = worldLeftToe;

	vec3 worldRightToe = combine(mCharacterTrans, mCurPose.GetGlobalTransform(mRightLeg->Toe())).position;
	vec3 rightToeTarget = worldRightToe;
	vec3 predictiveRightToe = worldRightToe;

	vec3 origin = leftAnkleWorld.position;
	origin.y = worldLeftToe.y;
	Ray leftToeRay(origin + characterForward * kToeLength + vec3(0, 1, 0));

	origin = rightAnkleWorld.position;
	origin.y = worldRightToe.y;
	Ray rightToeRay(origin + characterForward * kToeLength + vec3(0, 1, 0));


	//Next, see if the toes hit anything
	float ankleRayHeight = 1.1f;
	for (unsigned int i = 0; i < numTriangles; i++)
	{
		if (RaycastTriangle(leftToeRay, mTriangles[i], hitPoint))
		{
			if (lenSq(hitPoint - leftToeRay.origin) < ankleRayHeight * ankleRayHeight)
			{
				leftToeTarget = hitPoint;
			}
			predictiveLeftToe = hitPoint;
		}
		if (RaycastTriangle(rightToeRay, mTriangles[i], hitPoint))
		{
			if (lenSq(hitPoint - rightToeRay.origin) < ankleRayHeight * ankleRayHeight)
			{
				rightToeTarget = hitPoint;
			}
			predictiveRightToe = hitPoint;
		}
	}

	// Place the toe target at the right location
	leftToeTarget = lerp(leftToeTarget, predictiveLeftToe, leftMotion);
	rightToeTarget = lerp(rightToeTarget, predictiveRightToe, rightMotion);

	// If the left or right toe hit, adjust the ankle rotation approrpaiteley
	vec3 leftAnkleToCurrentToe = worldLeftToe - leftAnkleWorld.position;
	vec3 leftAnkleToDesiredToe = leftToeTarget - leftAnkleWorld.position;
	if (dot(leftAnkleToCurrentToe, leftAnkleToDesiredToe) > 0.00001f)
	{
		quat ankleRotator = fromTo(leftAnkleToCurrentToe, leftAnkleToDesiredToe);
		Transform ankleLocal = mCurPose.GetLocalTransform(mLeftLeg->Ankle());

		quat worldRotatedAnkle = leftAnkleWorld.rotation * ankleRotator;
		quat localRotatedAnkle = worldRotatedAnkle * inverse(leftAnkleWorld.rotation);

		ankleLocal.rotation = localRotatedAnkle * ankleLocal.rotation;
		mCurPose.SetLocalTransform(mLeftLeg->Ankle(), ankleLocal);
	}

	vec3 rightAnkleToCurrentToe = worldRightToe - rightAnkleWorld.position;
	vec3 rightAnkleToDesiredToe = rightToeTarget - rightAnkleWorld.position;
	if (dot(rightAnkleToCurrentToe, rightAnkleToDesiredToe) > 0.00001f)
	{
		quat ankleRotator = fromTo(rightAnkleToCurrentToe, rightAnkleToDesiredToe);
		Transform ankleLocal = mCurPose.GetLocalTransform(mRightLeg->Ankle());

		quat worldRotatedAnkle = rightAnkleWorld.rotation * ankleRotator;
		quat localRotatedAnkle = worldRotatedAnkle * inverse(rightAnkleWorld.rotation);

		ankleLocal.rotation = localRotatedAnkle * ankleLocal.rotation;
		mCurPose.SetLocalTransform(mRightLeg->Ankle(), ankleLocal);
	}


	mCurPose.GetMatrixPalette(mPoseMatrixPalette);
	for (size_t i = 0; i < mPoseMatrixPalette.size(); i++)
	{
		mPoseMatrixPalette[i] = mPoseMatrixPalette[i] * mSkeleton.GetInvBindPose()[i];
	}
}

void Chapter13Sample03::Render(float inAspectRatio)
{
	vec3 characterPosOffset = vec3(mCharacterTrans.position.x, 0, mCharacterTrans.position.z);
	mat4 projection = perspective(60.0f, inAspectRatio, 0.01f, 1000.0f);
	mat4 view = lookAt(vec3(0, 5, 10) + characterPosOffset,
					   vec3(0, 3, 0) + characterPosOffset,
					   vec3(0, 1, 0));

	mat4 model = transformToMat4(mCharacterTrans);
	mat4 mvp = projection * view * model;
	mat4 vp = projection * view;

	if (mIsShowCharacter)
	{
		Shader* characterShader = mDynamicShader;
		characterShader->Bind();
		Uniform<mat4>::Set(characterShader->GetUniform("model"), model);
		Uniform<mat4>::Set(characterShader->GetUniform("view"), view);
		Uniform<mat4>::Set(characterShader->GetUniform("projection"), projection);
		Uniform<vec3>::Set(characterShader->GetUniform("light"), vec3(1, 1, 1));
		Uniform<mat4>::Set(characterShader->GetUniform("animated"), mPoseMatrixPalette);

		mDiffuseTexture->Set(characterShader->GetUniform("tex0"), 0);
		for (size_t i = 0; i < mCharacterMeshes.size(); i++)
		{
			mCharacterMeshes[i].Bind(characterShader->GetAttribute("position"),
									 characterShader->GetAttribute("normal"),
									 characterShader->GetAttribute("texCoord"),
									 characterShader->GetAttribute("weights"),
									 characterShader->GetAttribute("joints"));
			mCharacterMeshes[i].Draw();
			mCharacterMeshes[i].UnBind(characterShader->GetAttribute("position"),
									   characterShader->GetAttribute("normal"),
									   characterShader->GetAttribute("texCoord"),
									   characterShader->GetAttribute("weights"),
									   characterShader->GetAttribute("joints"));

		}
		mDiffuseTexture->UnSet(0);
		characterShader->UnBind();
	}

	// Ground Render
	if (mIsShowGround)
	{
		Shader* groundShader = mStaticShader;
		groundShader->Bind();
		Uniform<mat4>::Set(groundShader->GetUniform("model"), mat4());
		Uniform<mat4>::Set(groundShader->GetUniform("view"), view);
		Uniform<mat4>::Set(groundShader->GetUniform("projection"), projection);
		Uniform<vec3>::Set(groundShader->GetUniform("light"), vec3(1, 1, 1));
		mCourseTexture->Set(groundShader->GetUniform("tex0"), 0);
		for (unsigned int i = 0, size = (unsigned int)mIKGround.size(); i < size; ++i)
		{
			mIKGround[i].Bind(groundShader->GetAttribute("position"), groundShader->GetAttribute("normal"), groundShader->GetAttribute("texCoord"), -1, -1);
			mIKGround[i].Draw();
			mIKGround[i].UnBind(groundShader->GetAttribute("position"), groundShader->GetAttribute("normal"), groundShader->GetAttribute("texCoord"), -1, -1);
		}
		mCourseTexture->UnSet(0);
		groundShader->UnBind();
	}

	glDisable(GL_DEPTH_TEST);
	if (mIsShowCurPose)
	{
		mCurPoseDraw->Draw(DebugDrawMode::Lines, vec3(0, 0, 1), mvp);
	}

	if (mIsShowIKLeg)
	{
		mLeftLeg->Draw(mvp, vec3(1, 0, 0));
		mRightLeg->Draw(mvp, vec3(1, 0, 0));
	}

	glEnable(GL_DEPTH_TEST);
}

void Chapter13Sample03::OnGUI()
{
	ImGui::Begin("IK Dome");
	ImGui::Checkbox("Show Ground", &mIsShowGround);
	ImGui::Checkbox("Show Character", &mIsShowCharacter);
	ImGui::Checkbox("Show Current Pose", &mIsShowCurPose);
	ImGui::Checkbox("Show IK Leg", &mIsShowIKLeg);
	ImGui::End();
}

void Chapter13Sample03::Shutdown()
{
	mIKGround.clear();
	mIKGround.shrink_to_fit();

	mCharacterMeshes.clear();
	mCharacterMeshes.shrink_to_fit();

	mPoseMatrixPalette.clear();
	mPoseMatrixPalette.shrink_to_fit();

	delete mCourseTexture;

	delete mLeftLeg;
	delete mRightLeg;

	delete mCharacterDraw;

	delete mCurPoseDraw;

	delete mDiffuseTexture;
	delete mStaticShader;
	delete mDynamicShader;
}
