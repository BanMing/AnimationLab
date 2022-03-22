#ifndef _H_CHAPTER06_SAMPLE01_
#define _H_CHAPTER06_SAMPLE01_

#include "../Math/vec3.h"
#include "../Math/vec2.h"

#include "Application.h"
#include "../OpenGL/Shader.h"
#include "../OpenGL/Attribute.h"
#include "../OpenGL/IndexBuffer.h"
#include "../OpenGL/Texture.h"

class Chapter06Sample01 :public Application
{
protected:
	Shader* mShader;
	Attribute<vec3>* mVertexPositions;
	Attribute<vec3>* mVertexNormals;
	Attribute<vec2>* mVertexTexCoords;
	IndexBuffer* mIndexBuffer;
	Texture* mDisplayTexture;
	float mRotation;
public:
	void Initialize();
	void Update(float inDeltaTime);
	void Render(float inAspectRatio);
	void Shutdown();
};

#endif