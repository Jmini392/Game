#include "stdafx.h"
#include "GameFramework.h"

GameFramework::GameFramework()
{
}

GameFramework::~GameFramework()
{
}

bool GameFramework::Initialize(HWND hwnd, int width, int height)
{
	mhMainWnd = hwnd;
	mClientWidth = width;
	mClientHeight = height;

	// 디버그 레이어 활성화
#if defined(_DEBUG)
	ComPtr<ID3D12Debug> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) debugController->EnableDebugLayer();
#endif

	// DXGI 팩토리 생성(그래픽 카드 검색?)
	if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&mdxgiFactory)))) return false;

	// 디바이스 생성
	if (FAILED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&md3dDevice))))
	{
		ComPtr<IDXGIAdapter> pWarpAdapter; // WARP로 생성 시도. CPU 기반 가상 어댑터
		mdxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter));
		if (FAILED(D3D12CreateDevice(pWarpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&md3dDevice)))) return false;
	}

	// 펜스 생성(CPU와 GPU간의 락같은 거)
	if (FAILED(md3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&md3dFence)))) return false;

	// 디스크립터 크기 얻기(힙에서 오프셋 계산할 때 필요)
	mRtvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	mDsvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	mCbvSrvUavDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// 커맨드 큐, 어로케이터, 리스트 생성
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	if (FAILED(md3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&md3dCommandQueue)))) return false;
	if (FAILED(md3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&md3dCommandAllocator)))) return false;
	if (FAILED(md3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, md3dCommandAllocator.Get(), nullptr, IID_PPV_ARGS(&md3dCommandList)))) return false;

	md3dCommandList->Close(); // 커맨드 리스트 닫기(초기화 시 반드시 필요). 추가된 명령이 없으므로 바로 닫음. 사용 시에는 Reset() 필요

	// 스왑체인 생성
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferDesc.Width = mClientWidth;
	swapChainDesc.BufferDesc.Height = mClientHeight;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60; // 0으로 설정 시 모니터 주사율에 맞춤
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 픽셀의 포맷
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SampleDesc.Count = 1; // 멀티샘플링 설정(안함)
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = SwapChainBufferCount;
	swapChainDesc.OutputWindow = mhMainWnd;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	ComPtr<IDXGISwapChain> swapChain;
	if (FAILED(mdxgiFactory->CreateSwapChain(md3dCommandQueue.Get(), &swapChainDesc, swapChain.GetAddressOf()))) return false;

	swapChain.As(&mSwapChain);

	// 렌더 타겟 뷰 디스크립터 힙 생성
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = SwapChainBufferCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	if (FAILED(md3dDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&mRtvHeap)))) return false;

	// 렌더 타겟 뷰 생성
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRtvHeap->GetCPUDescriptorHandleForHeapStart());

	for (UINT i = 0; i < SwapChainBufferCount; ++i)
	{
		if (FAILED(mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mSwapChainBuffer[i])))) return false;
		md3dDevice->CreateRenderTargetView(mSwapChainBuffer[i].Get(), nullptr, rtvHandle);
		rtvHandle.ptr += mRtvDescriptorSize;
	}

	// 깊이-스텐실 뷰 디스크립터 힙 생성
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	if (FAILED(md3dDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&mDsvHeap)))) return false;

	// 깊이-스텐실 리소스 생성
	D3D12_RESOURCE_DESC depthStencilDesc = {};
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = mClientWidth;
	depthStencilDesc.Height = mClientHeight;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE optClear = {};
	optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;

	D3D12_HEAP_PROPERTIES heapProps = {};
	heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProps.CreationNodeMask = 1;
	heapProps.VisibleNodeMask = 1;

	if(FAILED(md3dDevice->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &depthStencilDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE, &optClear, IID_PPV_ARGS(&mDepthStencilBuffer)))) return false;

	// 깊이-스텐실 뷰 생성
	md3dDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(), nullptr, mDsvHeap->GetCPUDescriptorHandleForHeapStart());

	md3dCommandList->Reset(md3dCommandAllocator.Get(), nullptr);

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = mDepthStencilBuffer.Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	md3dCommandList->ResourceBarrier(1, &barrier);
	md3dCommandList->Close();

	ID3D12CommandList* cmdsLists[] = { md3dCommandList.Get() };
	md3dCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	mCurrentFence++;
	md3dCommandQueue->Signal(md3dFence.Get(), mCurrentFence);

	if (md3dFence->GetCompletedValue() < mCurrentFence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
		md3dFence->SetEventOnCompletion(mCurrentFence, eventHandle);
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}

	// 뷰포트 설정
	mScreenViewport.TopLeftX = 0;
	mScreenViewport.TopLeftY = 0;
	mScreenViewport.Width = static_cast<float>(mClientWidth);
	mScreenViewport.Height = static_cast<float>(mClientHeight);
	mScreenViewport.MinDepth = 0.0f;
	mScreenViewport.MaxDepth = 1.0f;

	// 시저 사각형 설정(화면 전체)
	mScissorRect = { 0, 0, mClientWidth, mClientHeight };

	return true;
}

void GameFramework::Render()
{
	// 명령 할당자 리셋

	if (FAILED(md3dCommandAllocator->Reset())) return;

	// 명령 리스트 리셋
	if (FAILED(md3dCommandList->Reset(md3dCommandAllocator.Get(), nullptr))) return;

	// 리소스 배리어
	// 후면 버퍼를 렌더 타겟으로 전환
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = mSwapChainBuffer[mCurrBackBuffer].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	md3dCommandList->ResourceBarrier(1, &barrier);

	// 뷰포트와 시저 사각형 설정
	md3dCommandList->RSSetViewports(1, &mScreenViewport);
	md3dCommandList->RSSetScissorRects(1, &mScissorRect);

	// 렌더 타겟과 깊이-스텐실 뷰 설정
	// 렌더 타겟 뷰 핸들
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRtvHeap->GetCPUDescriptorHandleForHeapStart());
	rtvHandle.ptr += mCurrBackBuffer * mRtvDescriptorSize;

	// 깊이-스텐실 뷰 핸들
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle(mDsvHeap->GetCPUDescriptorHandleForHeapStart());

	md3dCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

	// 화면 클리어
	const float clearColor[] = { 0.2f, 0.3f, 0.4f, 1.0f };
	md3dCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	// 깊이-스텐실 뷰 클리어
	md3dCommandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	// 리소스 배리어
	// 후면 버퍼를 프레젠트 상태로 전환
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

	md3dCommandList->ResourceBarrier(1, &barrier);

	// 명령 리스트 닫기
	if (FAILED(md3dCommandList->Close())) return;

	// 명령 리스트 실행
	ID3D12CommandList* cmdsLists[] = { md3dCommandList.Get() };
	md3dCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// 화면 출력(프레젠트)
	mSwapChain->Present(1, 0);

	// 펜스 신호 보내기(동기화, GPU 작업 완료 대기)

	mCurrentFence++;
	md3dCommandQueue->Signal(md3dFence.Get(), mCurrentFence);

	if (md3dFence->GetCompletedValue() < mCurrentFence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
		md3dFence->SetEventOnCompletion(mCurrentFence, eventHandle);
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}

	mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;
}

void GameFramework::Update()
{

}

void GameFramework::Release()
{

}