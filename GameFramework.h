#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

using namespace Microsoft::WRL;

class GameFramework
{
public:
	GameFramework();
	~GameFramework();

	bool Initialize(HWND hwnd, int width, int height);

	void Update();
	void Render();

	void Release();

private:
	ComPtr<IDXGIFactory4> mdxgiFactory;
	ComPtr<ID3D12Device> md3dDevice;
	ComPtr<ID3D12Fence> md3dFence;
	UINT64 mCurrentFence = 0;

	ComPtr<ID3D12CommandQueue> md3dCommandQueue;
	ComPtr<ID3D12CommandAllocator> md3dCommandAllocator;
	ComPtr<ID3D12GraphicsCommandList> md3dCommandList;

	ComPtr<IDXGISwapChain4> mSwapChain;
	int mClientWidth = 800;
	int mClientHeight = 600;

	UINT mRtvDescriptorSize = 0; // ·»´õ Å¸°Ù ºä µð½ºÅ©¸³ÅÍ Å©±â
	UINT mDsvDescriptorSize = 0; // ±íÀÌ-½ºÅÙ½Ç ºä µð½ºÅ©¸³ÅÍ Å©±â
	UINT mCbvSrvUavDescriptorSize = 0; // ¼ÎÀÌ´õ ¸®¼Ò½º ºä µð½ºÅ©¸³ÅÍ Å©±â

	HWND mhMainWnd = nullptr;
};

