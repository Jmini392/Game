#include "stdafx.h"
#include "Texture.h"
#include "DirectXTex.h" // DirectXTex 헤더 포함

Texture::Texture()
{
}

Texture::~Texture()
{
    // 소멸자에서 리소스 해제
    if (m_pd3dTexture) m_pd3dTexture->Release();
    // 업로드 버퍼는 ReleaseUploadBuffers에서 이미 해제되었을 수 있음
    if (m_pd3dTextureUploadBuffer) m_pd3dTextureUploadBuffer->Release();
}

void Texture::LoadTextureFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const std::wstring& wstrFileName)
{
    using namespace DirectX;

    // 1. WIC(Windows Imaging Component)를 사용하여 이미지 파일 로드
    TexMetadata metadata;
    ScratchImage scratchImage;
    HRESULT hr = LoadFromWICFile(wstrFileName.c_str(), WIC_FLAGS_NONE, &metadata, scratchImage);
    if (FAILED(hr))
    {
        // 파일 로드 실패 처리
        return;
    }

    // 2. 최종 텍스처 리소스 생성 (GPU 기본 힙)
    D3D12_HEAP_PROPERTIES d3dHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    D3D12_RESOURCE_DESC d3dResourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(metadata.format, metadata.width, metadata.height, (UINT)metadata.arraySize, (UINT)metadata.mipLevels);

    pd3dDevice->CreateCommittedResource(
        &d3dHeapProperties,
        D3D12_HEAP_FLAG_NONE,
        &d3dResourceDesc,
        D3D12_RESOURCE_STATE_COPY_DEST, // 초기 상태: 복사 대상으로 설정
        nullptr,
        IID_PPV_ARGS(&m_pd3dTexture));

    // 3. 업로드용 버퍼 생성 (CPU에서 접근 가능한 업로드 힙)
    UINT64 nUploadBufferSize = GetRequiredIntermediateSize(m_pd3dTexture, 0, (UINT)scratchImage.GetImageCount());

    d3dHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    d3dResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(nUploadBufferSize);

    pd3dDevice->CreateCommittedResource(
        &d3dHeapProperties,
        D3D12_HEAP_FLAG_NONE,
        &d3dResourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_pd3dTextureUploadBuffer));

    // 4. 이미지 데이터를 업로드 버퍼를 통해 최종 텍스처 리소스로 복사
    D3D12_SUBRESOURCE_DATA d3dSubResourceData = {};
    d3dSubResourceData.pData = scratchImage.GetPixels();
    d3dSubResourceData.RowPitch = scratchImage.GetImage(0, 0, 0)->rowPitch;
    d3dSubResourceData.SlicePitch = scratchImage.GetImage(0, 0, 0)->slicePitch;

    UpdateSubresources(pd3dCommandList, m_pd3dTexture, m_pd3dTextureUploadBuffer, 0, 0, (UINT)scratchImage.GetImageCount(), &d3dSubResourceData);

    // 5. 텍스처 리소스의 상태를 픽셀 셰이더에서 읽을 수 있도록 변경
    D3D12_RESOURCE_BARRIER d3dResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pd3dTexture, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);
}

void Texture::ReleaseUploadBuffers()
{
    if (m_pd3dTextureUploadBuffer)
    {
        m_pd3dTextureUploadBuffer->Release();
        m_pd3dTextureUploadBuffer = nullptr;
    }
}