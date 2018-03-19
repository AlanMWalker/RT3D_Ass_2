#ifndef XMVECTOR_UTILS_H
#define XMVECTOR_UTILS_H

#include <DirectXMath.h>

inline DirectX::XMFLOAT3 operator+ (const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2)
{
	return DirectX::XMFLOAT3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

inline DirectX::XMFLOAT3 operator- (const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2)
{
	return DirectX::XMFLOAT3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

#endif