/******************************************************************
** Mesh.h
** @Author       : BanMing 
** @Date         : 8/21/2021 11:05:17 PM
** @Description  : 
*******************************************************************/

#pragma once

#include <d3d12.h>

class Mesh
{
public:
	Mesh();
	Mesh(char fileName[]);
	~Mesh();
	HRESULT Load(char fileName[]);
	void Render();
	void Release();

private:
	//ID3D12Mesh


};

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}