#ifndef _H_GLTFLOADER_
#define _H_GLTFLOADER_

#include "../3rd/cgltf/cgltf.h"
#include "../Animation/Pose.h"
#include "../Animation/TClip.h"
#include "../Animation/Skeleton.h"
#include "../Animation/Mesh.h"
#include <string>
#include <vector>

cgltf_data* LoadGLTFFile(const char* path);
void FreeGLTFFile(cgltf_data* data);
Pose LoadRestPose(cgltf_data* data);

// useful for debugging
std::vector<std::string> LoadJointNames(cgltf_data* data);

std::vector<Clip> LoadAnimationClips(cgltf_data* data);
std::vector<FastClip> LoadFastClips(cgltf_data* data);

Pose LoadBindPose(cgltf_data* data);
Skeleton LoadSkeleton(cgltf_data* data);
std::vector<Mesh> LoadMeshes(cgltf_data* data);
#endif
