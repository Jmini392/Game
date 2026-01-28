#pragma once
#include <DirectXMath.h>
#include <Windows.h>

using namespace DirectX;

class Camera
{
public:
	Camera();

	void SetLens(float fovY, float aspect, float zn, float zf);

	void Update(float dt);

	XMMATRIX GetView() const { return XMLoadFloat4x4(&mView); }
	XMMATRIX GetProj() const { return XMLoadFloat4x4(&mProj); }
	XMMATRIX GetViewProj() const;

private:
	float mTheta; // 수평 각도
	float mPhi;   // 수직 각도
	float mRadius; // 카메라와 원점 사이의 거리

	XMFLOAT4X4 mView; // 뷰 행렬
	XMFLOAT4X4 mProj; // 투영 행렬
};

