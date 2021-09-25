/******************************************************************
** d3dUtil.cpp
** @Author       : BanMing
** @Date         : 9/25/2021 7:27:45 PM
** @Description  :
*******************************************************************/

#include "D3DUtil.h"
bool d3dUtil::IsKeyDown(int vkeyCode)
{
	return (GetAsyncKeyState(vkeyCode) & 0x8000) != 0;
}

Microsoft::WRL::ComPtr<ID3DBlob> d3dUtil::LoadBinary(const std::wstring& filename)
{
	std::ifstream fin(filename, std::ios::binary);

	fin.seekg(0, std::ios_base::end);
	std::ifstream::pos_type size = (int)fin.tellg();
	fin.seekg(0, std::ios_base::beg);

	Microsoft::WRL::ComPtr<ID3DBlob> blob;
	D3DCreateBlob(size, blob.GetAddressOf());

	fin.read((char*)blob->GetBufferPointer(), size);
	fin.close();

	return blob;
}