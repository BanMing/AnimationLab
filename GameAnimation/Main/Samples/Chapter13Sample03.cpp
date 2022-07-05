#include "Chapter13Sample03.h"
#include "../GLTF/GLTFLoader.h"
#include <iostream>
#include "../OpenGL/Uniform.h"
#include "../3rd/glad/glad.h"

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

	mCharacterDraw = new DebugDraw();
	mCharacterDraw->FromPose(mCurPose);
	mCharacterDraw->UpdateOpenGLBuffers();

	mSkeletonDraw = new DebugDraw();
	mSkeletonDraw->FromPose(mCurPose);
	mSkeletonDraw->UpdateOpenGLBuffers();

	mIsShowGround = true;
	mIsShowCharacter = true;
}

void Chapter13Sample03::Update(float inDeltaTime)
{
	// Increment time and sample the animation clip that moves the model on the level rails
	mWalkingTime += inDeltaTime * 0.3f;

	// Figure out the X and Z position of the model in world space 
	vec3 currentPosition = mMotionTrack.Sample(mWalkingTime, true);
	vec3 nextPosition = mMotionTrack.Sample(mWalkingTime + 0.1f, true);
	mCharacterTrans.position = currentPosition;

	// Figure out the forward direction of the character in world space
	vec3 direction = normalized(nextPosition - currentPosition);
	quat newDirection = lookRotation(direction, vec3(0, 1, 0));
	if (dot(mCharacterTrans.rotation, newDirection) < 0.0f)
	{
		newDirection = newDirection * -1.0f;
	}
	mCharacterTrans.rotation = nlerp(mCharacterTrans.rotation, newDirection, inDeltaTime * 10.0f);

	mPlayTime = mWalkClip.Sample(mCurPose, mPlayTime + inDeltaTime);

	mCharacterDraw->FromPose(mCurPose);
	mCharacterDraw->UpdateOpenGLBuffers();

	mCurPose.GetMatrixPalette(mPoseMatrixPalette);
	for (size_t i = 0; i < mPoseMatrixPalette.size(); i++)
	{
		mPoseMatrixPalette[i] = mPoseMatrixPalette[i] * mSkeleton.GetInvBindPose()[i];
	}

	mSkeletonDraw->FromPose(mCurPose);
	mSkeletonDraw->UpdateOpenGLBuffers();
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

	if (mIsShowSkeleton)
	{
		glDisable(GL_DEPTH_TEST);
		mSkeletonDraw->Draw(DebugDrawMode::Lines, vec3(0, 0, 1), mvp);
		glEnable(GL_DEPTH_TEST);
	}
}

void Chapter13Sample03::OnGUI()
{
	ImGui::Begin("IK Dome");
	ImGui::Checkbox("Show Ground", &mIsShowGround);
	ImGui::Checkbox("Show Character", &mIsShowCharacter);
	ImGui::Checkbox("Show Skeleton", &mIsShowSkeleton);
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

	delete mCharacterDraw;

	delete mSkeletonDraw;

	delete mDiffuseTexture;
	delete mStaticShader;
	delete mDynamicShader;
}
