#include "Camera.h"

Camera::Camera()
{
	mTheta = 1.5f * XM_PI;
	mPhi = XM_PIDIV4;
	mRadius = 5.0f;

	XMStoreFloat4x4(&mView, XMMatrixIdentity());
	XMStoreFloat4x4(&mProj, XMMatrixIdentity());
}

void Camera::SetLens(float fovY, float aspect, float zn, float zf)
{
	XMMATRIX proj = XMMatrixPerspectiveFovLH(fovY, aspect, zn, zf);
	XMStoreFloat4x4(&mProj, proj);
}

void Camera::Update(float dt)
{
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		mTheta -= dt;
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		mTheta += dt;
	if (GetAsyncKeyState(VK_UP) & 0x8000)
		mPhi -= dt;
	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
		mPhi += dt;

	if (GetAsyncKeyState('W') & 0x8000)
		mRadius -= dt * 2.0f;
	if (GetAsyncKeyState('S') & 0x8000)
		mRadius += dt * 2.0f;

	if (mPhi <= 0.1f) mPhi = 0.1f;
	if (mPhi >= XM_PI - 0.1f) mPhi = XM_PI - 0.1f;

	float x = mRadius * sinf(mPhi) * cosf(mTheta);
	float y = mRadius * cosf(mPhi);
	float z = mRadius * sinf(mPhi) * sinf(mTheta);

	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, view);
}

XMMATRIX Camera::GetViewProj() const
{
	return XMLoadFloat4x4(&mView) * XMLoadFloat4x4(&mProj);
}