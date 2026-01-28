#pragma once

#include <d3d12.h>
#include <wrl.h>
#include <DirectXMath.h>
#include <vector>

using namespace Microsoft::WRL;
using namespace DirectX;

struct Vertex
{
	XMFLOAT3 position; // x, y, z ÁÂÇ¥
	XMFLOAT4 color;    // RGBA »ö»ó
	XMFLOAT2 uv;       // ÅØ½ºÃ³ ÁÂÇ¥
	XMFLOAT3 normal; // ¹ý¼± º¤ÅÍ
};

class Mesh
{
public:
	void Init(ID3D12Device* device, const std::vector<Vertex>& vertices, const std::vector<UINT16>& indices);
	void Render(ID3D12GraphicsCommandList* cmdList);

private:
	ComPtr<ID3D12Resource> mVertexBuffer;
	ComPtr<ID3D12Resource> mIndexBuffer;

	D3D12_VERTEX_BUFFER_VIEW mVertexBufferView;
	D3D12_INDEX_BUFFER_VIEW mIndexBufferView;

	UINT mIndexCount = 0;
};

