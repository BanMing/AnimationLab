#include "Crowd.h"

unsigned int Crowd::Size()
{
    return mCurrentPlayTimes.size();
}

void Crowd::Resize(unsigned int size)
{
}

Transform Crowd::GetActor(unsigned int index)
{
    return Transform();
}

void Crowd::SetActor(unsigned int index, const Transform& t)
{
}

void Crowd::Update(float deltaTime, FastClip& mClip, unsigned int texWidth)
{
}

void Crowd::SetUniforms(Shader* shader)
{
}

float Crowd::AdjustTime(float t, float start, float end, bool looping)
{
    return 0.0f;
}

void Crowd::UpdatePlaybackTimes(float dt, bool looping, float start, float end)
{
}

void Crowd::UpdateFrameIndices(float start, float duration, unsigned int texWidth)
{
}

void Crowd::UpdateInterpolationTimes(float start, float duration, unsigned int texWidth)
{
}
