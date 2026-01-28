#include "stdafx.h"
#include "GameFramework.h"
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

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

	BuildObjects();

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

	md3dCommandList->SetGraphicsRootSignature(mRootSignature.Get());
	md3dCommandList->SetGraphicsRootConstantBufferView(0, mConstantBuffer->GetGPUVirtualAddress());
	md3dCommandList->SetPipelineState(mPipelineState.Get());

	md3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	md3dCommandList->IASetVertexBuffers(0, 1, &mVertexBufferView);
	md3dCommandList->IASetIndexBuffer(&mIndexBufferView);

	md3dCommandList->DrawIndexedInstanced(mIndexCount, 1, 0, 0, 0);

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
	mTheta += 0.005f;
	if (mTheta > XM_2PI) mTheta -= XM_2PI; // XM_2PI는 2π 상수

	XMMATRIX world = XMMatrixRotationRollPitchYaw(0.0f, mTheta, 0.0f); // 월드 행렬(회전 행렬), x, y, z 축 순서로 회전

	XMVECTOR pos = XMVectorSet(0.0f, 0.0f, -3.0f, 1.0f); // 카메라 위치 벡터
	XMVECTOR target = XMVectorZero(); // 카메라가 바라보는 지점(원점)
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // 카메라의 업 벡터
	XMMATRIX view = XMMatrixLookAtLH(pos, target, up); // 뷰 행렬

	float aspectRatio = static_cast<float>(mClientWidth) / static_cast<float>(mClientHeight);
	XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspectRatio, 1.0f, 1000.0f); // 투영 행렬, XM_PIDIV4는 π/4 상수

	XMMATRIX worldViewProj = world * view * proj; // 월드-뷰-투영 행렬

	ObjectConstants cbData; // 상수 버퍼에 전달할 데이터
	XMStoreFloat4x4(&cbData.worldViewProj, XMMatrixTranspose(worldViewProj)); // 행렬을 전치하여 저장, 셰이더에서 열 우선으로 사용하기 때문
	// XMSttoreFloat4x4는 XMMATRIX를 XMFLOAT4X4로 변환하여 저장하는 함수

	memcpy(mMappedData, &cbData, sizeof(ObjectConstants)); // 상수 버퍼에 데이터 복사
}

void GameFramework::Release()
{

}

bool GameFramework::BuildObjects()
{
	D3D12_ROOT_PARAMETER rootParam[1]; // 루트 파라미터 배열

	D3D12_ROOT_DESCRIPTOR rootCBVDescriptor; // 루트 CBV 디스크립터
	rootCBVDescriptor.RegisterSpace = 0;
	rootCBVDescriptor.ShaderRegister = 0; // b0 레지스터

	rootParam[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // CBV 타입, 상수 버퍼 뷰
	rootParam[0].Descriptor = rootCBVDescriptor;
	rootParam[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // 버텍스 셰이더에서만 접근 가능

	D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
	rootSigDesc.NumParameters = 1; // 루트 파라미터 개수
	rootSigDesc.pParameters = rootParam; // 루트 파라미터 배열
	rootSigDesc.NumStaticSamplers = 0;
	rootSigDesc.pStaticSamplers = nullptr;
	rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;

	if (FAILED(D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, signature.GetAddressOf(), error.GetAddressOf())))
	{
		if (error) OutputDebugStringA((char*)error->GetBufferPointer());
		return false;
	}

	if (FAILED(md3dDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&mRootSignature))))
		return false;

	ComPtr<ID3D10Blob> vertexShader;
	ComPtr<ID3D10Blob> pixelShader;

	UINT compileFlags = 0;
#if defined( _DEBUG )
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	if (FAILED(D3DCompileFromFile(L"Shaders.hlsl", nullptr, nullptr, "VS", "vs_5_0", compileFlags, 0, &vertexShader, &error)))
	{
		if (error) OutputDebugStringA((char*)error->GetBufferPointer());
		return false;
	}

	if (FAILED(D3DCompileFromFile(L"Shaders.hlsl", nullptr, nullptr, "PS", "ps_5_0", compileFlags, 0, &pixelShader, &error)))
	{
		if (error) OutputDebugStringA((char*)error->GetBufferPointer());
		return false;
	}

	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};

	psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	psoDesc.pRootSignature = mRootSignature.Get();

	psoDesc.VS = { vertexShader->GetBufferPointer(), vertexShader->GetBufferSize() };
	psoDesc.PS = { pixelShader->GetBufferPointer(), pixelShader->GetBufferSize() };

	psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	psoDesc.RasterizerState.FrontCounterClockwise = FALSE;
	psoDesc.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	psoDesc.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	psoDesc.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	psoDesc.RasterizerState.DepthClipEnable = TRUE;
	psoDesc.RasterizerState.MultisampleEnable = FALSE;
	psoDesc.RasterizerState.AntialiasedLineEnable = FALSE;
	psoDesc.RasterizerState.ForcedSampleCount = 0;
	psoDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	psoDesc.BlendState.AlphaToCoverageEnable = FALSE;
	psoDesc.BlendState.IndependentBlendEnable = FALSE;
	const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc =
	{
		FALSE,FALSE,
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_LOGIC_OP_NOOP,
		D3D12_COLOR_WRITE_ENABLE_ALL,
	};
	for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
		psoDesc.BlendState.RenderTarget[i] = defaultRenderTargetBlendDesc;

	psoDesc.DepthStencilState.DepthEnable = TRUE;
	psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;

	psoDesc.DepthStencilState.StencilEnable = FALSE;

	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;

	if (FAILED(md3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPipelineState))))
		return false;

	Vertex vertices[] =
	{
		// 앞면 (Z = -0.5)
		{ { -0.5f,  0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f, 1.0f } }, // 0. 좌상 (빨강)
		{ {  0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f, 1.0f } }, // 1. 우상 (초록)
		{ {  0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f, 1.0f } }, // 2. 우하 (파랑)
		{ { -0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 0.0f, 1.0f } }, // 3. 좌하 (노랑)

		// 뒷면 (Z = +0.5)
		{ { -0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f, 1.0f, 1.0f } }, // 4. 좌상 (청록)
		{ {  0.5f,  0.5f,  0.5f }, { 1.0f, 0.0f, 1.0f, 1.0f } }, // 5. 우상 (자주)
		{ {  0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f, 1.0f } }, // 6. 우하 (흰색)
		{ { -0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f, 0.0f, 1.0f } }  // 7. 좌하 (검정)
	};

	const UINT vertexBufferSize = sizeof(vertices);

	// 인덱스 버퍼 데이터, 시계 방향 순서
	uint16_t indices[] =
	{
		// 앞면
		0, 1, 2,  0, 2, 3,
		// 뒷면
		4, 6, 5,  4, 7, 6,
		// 왼쪽면
		4, 5, 1,  4, 1, 0,
		// 오른쪽면
		3, 2, 6,  3, 6, 7,
		// 윗면
		1, 5, 6,  1, 6, 2,
		// 아랫면
		4, 0, 3,  4, 3, 7
	};

	const UINT indexBufferSize = sizeof(indices);
	mIndexCount = _countof(indices); // 36개

	D3D12_HEAP_PROPERTIES heapProps = {};
	heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProps.CreationNodeMask = 1;
	heapProps.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = vertexBufferSize;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	if (FAILED(md3dDevice->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&mVertexBuffer))))
		return false;

	UINT8* pVertexDataBegin;
	D3D12_RANGE readRange = { 0, 0 };
	if (FAILED(mVertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)))) // reinterpret_cast: 포인터 타입 변환
		return false;

	memcpy(pVertexDataBegin, vertices, sizeof(vertices));
	mVertexBuffer->Unmap(0, nullptr);

	mVertexBufferView.BufferLocation = mVertexBuffer->GetGPUVirtualAddress();
	mVertexBufferView.StrideInBytes = sizeof(Vertex);
	mVertexBufferView.SizeInBytes = vertexBufferSize;

	resourceDesc.Width = indexBufferSize;

	if (FAILED(md3dDevice->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&mIndexBuffer))))
		return false;

	UINT8* pIndexDataBegin;
	D3D12_RANGE indexReadRange = { 0, 0 };
	if (FAILED(mIndexBuffer->Map(0, &indexReadRange, reinterpret_cast<void**>(&pIndexDataBegin))))
		return false;

	memcpy(pIndexDataBegin, indices, sizeof(indices));
	mIndexBuffer->Unmap(0, nullptr);

	mIndexBufferView.BufferLocation = mIndexBuffer->GetGPUVirtualAddress();
	mIndexBufferView.Format = DXGI_FORMAT_R16_UINT;
	mIndexBufferView.SizeInBytes = indexBufferSize;

	UINT elementByteSize = (sizeof(ObjectConstants) + 255) & ~255; // 256바이트 정렬)

	D3D12_HEAP_PROPERTIES cbHeapProps = {}; // 상수 버퍼용 힙 속성
	cbHeapProps.Type = D3D12_HEAP_TYPE_UPLOAD; // 업로드 힙
	cbHeapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	cbHeapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	cbHeapProps.CreationNodeMask = 1;
	cbHeapProps.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC cbDesc = {}; // 상수 버퍼용 리소스 서술자
	cbDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbDesc.Width = elementByteSize; // 상수 버퍼 크기
	cbDesc.Height = 1;
	cbDesc.DepthOrArraySize = 1;
	cbDesc.MipLevels = 1;
	cbDesc.Format = DXGI_FORMAT_UNKNOWN;
	cbDesc.SampleDesc.Count = 1;
	cbDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	cbDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	if (FAILED(md3dDevice->CreateCommittedResource(
		&cbHeapProps,
		D3D12_HEAP_FLAG_NONE,
		&cbDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&mConstantBuffer)))) // IID_PPV_ARGS: COM 인터페이스 포인터 얻기 위한 매크로, _uuidof와 GetAddressOf()를 조합
		return false;

	D3D12_RANGE cbReadRange = { 0, 0 }; // CPU가 읽지 않을 것이므로 범위는 0으로 설정
	if (FAILED(mConstantBuffer->Map(0, &cbReadRange, reinterpret_cast<void**>(&mMappedData))))
		return false;

	return true;
}