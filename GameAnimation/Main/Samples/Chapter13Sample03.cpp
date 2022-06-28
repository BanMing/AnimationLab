#include "Chapter13Sample03.h"
#include "../GLTF/GLTFLoader.h"
#include <iostream>
#include "../OpenGL/Uniform.h"

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
	cgltf_data* gltf = LoadGLTFFile("Assets/IKCourse.gltf");
	mIKGround = LoadStaticMeshes(gltf);
	FreeGLTFFile(gltf);
	mCourseTexture = new Texture("Assets/uv.png");

	mStaticShader = new Shader("Shaders/static.vert", "Shaders/lit.frag");

	mIsShowGround = true;
}

void Chapter13Sample03::Update(float inDeltaTime)
{
}

void Chapter13Sample03::Render(float inAspectRatio)
{
	mat4 projection = perspective(60.0f, inAspectRatio, 0.01f, 1000.0f);
	mat4 view = lookAt(vec3(0, 5, 10), vec3(0, 3, 0), vec3(0, 1, 0));

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
}

void Chapter13Sample03::OnGUI()
{
	ImGui::Begin("IK Dome");
	ImGui::Checkbox("Show Ground", &mIsShowGround);
	ImGui::End();
}

void Chapter13Sample03::Shutdown()
{
	delete mCourseTexture;
	//delete mCurPoseDraw;
	delete mStaticShader;
}
