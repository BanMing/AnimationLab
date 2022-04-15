#ifndef _H_MESH_
#define _H_MESH_

#include "../Math/vec2.h"
#include "../Math/vec3.h"
#include "../Math/vec4.h"
#include "../Math/mat4.h"
#include "../OpenGL/Attribute.h"
#include "../OpenGL/IndexBuffer.h"
#include "Skeleton.h"
#include "Pose.h"
#include <vector>

// Maintain a copy of the mesh data on the CPU as well as on the GPU
class Mesh
{
	//define each vertex of mesh
protected:
	std::vector<vec3> mPosition;
	std::vector<vec3> mNormal;
	std::vector<vec2> mTexCoord;
	// 4 bones for each vertex
	std::vector<vec4> mWeights;
	std::vector<ivec4> mInfluences;
	std::vector<unsigned int> mIndices;

	//each of the vectors need to be  set appropriate attributes.
protected:
	Attribute<vec3>* mPosAttrib;
	Attribute<vec3>* mNormAttrib;
	Attribute<vec2>* mUvAttrib;
	Attribute<vec4>* mWeightAttrib;
	Attribute<ivec4>* mInfluenceAttrib;
	IndexBuffer* mIndexBuffer;
	
	// Add an additional copy of the pose and normal data, 
	// as well as a matrix palette to use for CPU skinning
protected:
	std::vector<vec3> mSkinnedPosition;
	std::vector<vec3> mSkinnedNormal;
	std::vector<mat4> mPosePalette;

public:
	Mesh();
	Mesh(const Mesh&);
	Mesh& operator=(const Mesh&);
	~Mesh();

	std::vector<vec3>& GetPosition();
	std::vector<vec3>& GetNormal();
	std::vector<vec2>& GetTexCoord();
	std::vector<vec4>& GetWeights();
	std::vector<ivec4>& GetInfluences();
	std::vector<unsigned int>& GetIndices();

	// CPU skinning is useful if the platform you are developing for has a limited number of uniform registers or a small uniform buffer.
	void CPUSkin(Skeleton& skeleton, Pose& pose);
	void CPUSKin(std::vector<mat4>& animatePose);
	// syncs the vectors holding data to the GPU
	void UpdateOpenGLBuffers();

	void Bind(int position, int normal, int texCoord, int weight, int influcence);
	void Draw();
	void DrawInstanced(unsigned int numInstances);
	void UnBind(int position, int normal, int texCoord, int weight, int influcence);
};

#endif // !_H_MESH_