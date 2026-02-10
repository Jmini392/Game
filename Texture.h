#pragma once
class Texture
{
public:
    Texture();
    virtual ~Texture();

private:
    int m_nReferences = 0;

    // 텍스처 리소스 배열
    ID3D12Resource* m_pd3dTexture;
    // 업로드 버퍼 배열
    ID3D12Resource* m_pd3dTextureUploadBuffer;

    // SRV 서술자 힙의 시작 핸들
    D3D12_GPU_DESCRIPTOR_HANDLE m_d3dGpuSrvDescriptorHandle;

public:
    void AddRef() { m_nReferences++; }
    void Release() { if (--m_nReferences <= 0) delete this; }

    void SetGpuSrvDescriptorHandle(D3D12_GPU_DESCRIPTOR_HANDLE d3dGpuSrvDescriptorHandle) { m_d3dGpuSrvDescriptorHandle = d3dGpuSrvDescriptorHandle; }
    D3D12_GPU_DESCRIPTOR_HANDLE GetGpuSrvDescriptorHandle() { return m_d3dGpuSrvDescriptorHandle; }

    void LoadTextureFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const std::wstring& wstrFileName);
    void ReleaseUploadBuffers();
};