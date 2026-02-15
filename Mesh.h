#pragma once
// 재질을 표현하기 위한 구조체이다.
struct Material
{
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;  // w = Shininess
	XMFLOAT4 Emissive;

	Material() {
		Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
		Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		Emissive = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	}
};
//정점을 표현하기 위한 클래스를 선언한다. 
class Vertex
{
public:
	//정점의 위치 벡터이다(모든 정점은 최소한 위치 벡터를 가져야 한다).
	XMFLOAT3 m_xmf3Position;
	XMFLOAT3 m_xmf3Normal;
	XMFLOAT2 m_xmf2UV;
	XMFLOAT4 m_xmf4Diffuse;
public:
	Vertex() {
		m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_xmf3Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_xmf2UV = XMFLOAT2(0.0f, 0.0f);
		m_xmf4Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	Vertex(XMFLOAT3 xmf3Position) {
		m_xmf3Position = xmf3Position;
		m_xmf3Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_xmf2UV = XMFLOAT2(0.0f, 0.0f);
		m_xmf4Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	Vertex(XMFLOAT3 xmf3Position, XMFLOAT4 xmf4Diffuse, XMFLOAT3 xmfNormal = XMFLOAT3(0, 0, 0)) {
		m_xmf3Position = xmf3Position;
		m_xmf3Normal = xmfNormal;
		m_xmf2UV = XMFLOAT2(0.0f, 0.0f);
		m_xmf4Diffuse = xmf4Diffuse;
	}
	~Vertex() {}
};

class Mesh
{
public:
	Mesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	Mesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		const std::vector<Vertex>& vertices, const std::vector<UINT>& indices);
	Mesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, std::string MeshFile, int obj);
	virtual ~Mesh();
private:
	int m_nReferences = 0;
public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
	void ReleaseUploadBuffers();
protected:
	Material m_Material;
	ID3D12Resource* m_pd3dVertexBuffer = NULL;
	ID3D12Resource* m_pd3dVertexUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW m_d3dVertexBufferView;
	D3D12_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	UINT m_nSlot = 0;
	UINT m_nVertices = 0;
	UINT m_nStride = 0;
	UINT m_nOffset = 0;
protected:
	ID3D12Resource* m_pd3dIndexBuffer = NULL;
	ID3D12Resource* m_pd3dIndexUploadBuffer = NULL;
	/*인덱스 버퍼(인덱스의 배열)와 인덱스 버퍼를 위한 업로드 버퍼에 대한 인터페이스 포인터이다. 인덱스 버퍼는 정점
	버퍼(배열)에 대한 인덱스를 가진다.*/
	D3D12_INDEX_BUFFER_VIEW m_d3dIndexBufferView;
	UINT m_nIndices = 0;
	//인덱스 버퍼에 포함되는 인덱스의 개수이다.
	UINT m_nStartIndex = 0;
	//인덱스 버퍼에서 메쉬를 그리기 위해 사용되는 시작 인덱스이다. 
	int m_nBaseVertex = 0;
	//인덱스 버퍼의 인덱스에 더해질 인덱스이다. 
public:
	Material GetMaterial() const { return m_Material; }
	void SetMaterial(const Material& mat) { m_Material = mat; }
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);
};


class GroundMesh : public Mesh
{
public:
	GroundMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth, float fHeight);
	virtual ~GroundMesh();
};

