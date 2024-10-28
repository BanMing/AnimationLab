#ifndef _H_ANIMTEXTURE_
#define _H_ANIMTEXTURE_

#include "../Math/vec3.h"
#include "../Math/quat.h"

class AnimTexture
{
public:
	AnimTexture();
	AnimTexture(const AnimTexture&);
	AnimTexture& operator=(const AnimTexture&);
	~AnimTexture();

	void Load(const char* path);
	void Save(const char* path);

	void UploadTextureDataToGPU();
	unsigned int Size();
	void Resize(unsigned int newSize);
	float* GetData();

	void SetTexel(unsigned int x, unsigned int y, const vec3& v);
	void SetTexel(unsigned int x, unsigned int y, const quat& q);
	vec4 GetTexel(unsigned int x, unsigned int y);

	void Set(unsigned int uniform, unsigned int texture);
	void UnSet(unsigned int textureIndex);
	unsigned int GetHandle();
protected:
	float* mData;
	unsigned int mSize;
	unsigned int mHandle;
};
#endif // _H_ANIMTEXTURE_
