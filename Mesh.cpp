#include "stdafx.h"
#include "Mesh.h"
#include <fstream>
#include <sstream>
Mesh::Mesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}

Mesh::Mesh(ID3D12Device* pd3dDevice,
    ID3D12GraphicsCommandList* pd3dCommandList,
    void* pVertices, UINT nVertices,
    void* pIndices, UINT nIndices)
{
    m_nVertices = nVertices;

    m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, sizeof(Vertex) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

    m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
    m_d3dVertexBufferView.StrideInBytes = sizeof(Vertex);
    m_d3dVertexBufferView.SizeInBytes = sizeof(Vertex) * m_nVertices;

    m_nIndices = nIndices;

    m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);

    m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
    m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
    m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
}

Mesh::Mesh(ID3D12Device* pd3dDevice,
    ID3D12GraphicsCommandList* pd3dCommandList,
    std::string MeshFile)
{
    std::ifstream file(MeshFile);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open mesh file: " + MeshFile);
    }

    std::string line;

    // OBJ raw data
    std::vector<XMFLOAT3> positions;
    std::vector<XMFLOAT3> normals;
    std::vector<XMFLOAT2> uvs;

    // Final GPU data
    std::vector<Vertex> vertices;
    std::vector<UINT> indices;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;

        // ------------------------
        // Position
        // ------------------------
        if (prefix == "v") {
            XMFLOAT3 pos;
            ss >> pos.x >> pos.y >> pos.z;
            positions.push_back(pos);
        }
        // ------------------------
        // Texcoord
        // ------------------------
        else if (prefix == "vt") {
            XMFLOAT2 uv;
            ss >> uv.x >> uv.y;
            uv.y = 1.0f - uv.y; // DirectX 좌표계 보정
            uvs.push_back(uv);
        }
        // ------------------------
        // Normal
        // ------------------------
        else if (prefix == "vn") {
            XMFLOAT3 normal;
            ss >> normal.x >> normal.y >> normal.z;
            normals.push_back(normal);
        }
        // ------------------------
        // Face
        // ------------------------
        else if (prefix == "f") {
            std::string vertexStr;
            std::vector<UINT> faceIndices;

            while (ss >> vertexStr) {
                int v = 0, vt = 0, vn = 0;

                // OBJ 형식 파싱: v, v/vt, v/vt/vn, v//vn
                size_t slash1 = vertexStr.find('/');
                if (slash1 == std::string::npos) {
                    // "v" 형식
                    v = std::stoi(vertexStr);
                }
                else {
                    size_t slash2 = vertexStr.find('/', slash1 + 1);

                    // 첫 번째 인덱스 (위치)
                    v = std::stoi(vertexStr.substr(0, slash1));

                    if (slash2 == std::string::npos) {
                        // "v/vt" 형식
                        vt = std::stoi(vertexStr.substr(slash1 + 1));
                    }
                    else {
                        // "v/vt/vn" 또는 "v//vn" 형식
                        if (slash2 > slash1 + 1) {
                            // "v/vt/vn"
                            vt = std::stoi(vertexStr.substr(slash1 + 1, slash2 - slash1 - 1));
                        }
                        // 노멀 인덱스
                        vn = std::stoi(vertexStr.substr(slash2 + 1));
                    }
                }

                Vertex vertex;

                // 위치 (필수)
                if (v > 0 && v <= (int)positions.size()) {
                    vertex.m_xmf3Position = positions[v - 1];
                }
                else {
                    vertex.m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
                }

                // UV (선택)
                if (vt > 0 && vt <= (int)uvs.size()) {
                    vertex.m_xmf2UV = uvs[vt - 1];
                }
                else {
                    vertex.m_xmf2UV = XMFLOAT2(0.0f, 0.0f);
                }

                // 노멀 (선택)
                if (vn > 0 && vn <= (int)normals.size()) {
                    vertex.m_xmf3Normal = normals[vn - 1];
                }
                else {
                    // 노멀이 없으면 나중에 계산
                    vertex.m_xmf3Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
                }

                vertex.m_xmf4Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

                vertices.push_back(vertex);
                faceIndices.push_back((UINT)vertices.size() - 1);
            }

            // Fan triangulation
            for (size_t i = 1; i + 1 < faceIndices.size(); ++i) {
                indices.push_back(faceIndices[0]);
                indices.push_back(faceIndices[i]);
                indices.push_back(faceIndices[i + 1]);
            }
        }
    }

    file.close();

    // ========================
    // 노멀이 없는 정점 처리
    // ========================
    bool hasNormals = false;
    for (const auto& v : vertices) {
        if (v.m_xmf3Normal.x != 0.0f || v.m_xmf3Normal.y != 0.0f || v.m_xmf3Normal.z != 0.0f) {
            hasNormals = true;
            break;
        }
    }

    if (!hasNormals && !indices.empty()) {
        // 노멀 계산: 각 삼각형의 면 노멀을 정점에 누적
        std::vector<XMFLOAT3> vertexNormals(vertices.size(), XMFLOAT3(0.0f, 0.0f, 0.0f));

        for (size_t i = 0; i < indices.size(); i += 3) {
            UINT i0 = indices[i];
            UINT i1 = indices[i + 1];
            UINT i2 = indices[i + 2];

            XMVECTOR v0 = XMLoadFloat3(&vertices[i0].m_xmf3Position);
            XMVECTOR v1 = XMLoadFloat3(&vertices[i1].m_xmf3Position);
            XMVECTOR v2 = XMLoadFloat3(&vertices[i2].m_xmf3Position);

            XMVECTOR edge1 = XMVectorSubtract(v1, v0);
            XMVECTOR edge2 = XMVectorSubtract(v2, v0);
            XMVECTOR faceNormal = XMVector3Cross(edge1, edge2);

            XMFLOAT3 fn;
            XMStoreFloat3(&fn, faceNormal);

            vertexNormals[i0].x += fn.x; vertexNormals[i0].y += fn.y; vertexNormals[i0].z += fn.z;
            vertexNormals[i1].x += fn.x; vertexNormals[i1].y += fn.y; vertexNormals[i1].z += fn.z;
            vertexNormals[i2].x += fn.x; vertexNormals[i2].y += fn.y; vertexNormals[i2].z += fn.z;
        }

        // 정규화
        for (size_t i = 0; i < vertices.size(); ++i) {
            XMVECTOR n = XMLoadFloat3(&vertexNormals[i]);
            n = XMVector3Normalize(n);
            XMStoreFloat3(&vertices[i].m_xmf3Normal, n);
        }
    }

    // ========================
    // Vertex Buffer
    // ========================
    m_nVertices = (UINT)vertices.size();
    m_nStride = sizeof(Vertex);
    m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    m_pd3dVertexBuffer = ::CreateBufferResource(
        pd3dDevice,
        pd3dCommandList,
        vertices.data(),
        m_nStride * m_nVertices,
        D3D12_HEAP_TYPE_DEFAULT,
        D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
        &m_pd3dVertexUploadBuffer
    );

    m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
    m_d3dVertexBufferView.StrideInBytes = m_nStride;
    m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

    // ========================
    // Index Buffer
    // ========================
    m_nIndices = (UINT)indices.size();

    m_pd3dIndexBuffer = ::CreateBufferResource(
        pd3dDevice,
        pd3dCommandList,
        indices.data(),
        sizeof(UINT) * m_nIndices,
        D3D12_HEAP_TYPE_DEFAULT,
        D3D12_RESOURCE_STATE_INDEX_BUFFER,
        &m_pd3dIndexUploadBuffer
    );

    m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
    m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
    m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
}


Mesh::~Mesh()
{
	if (m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release();
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	if (m_pd3dIndexBuffer) m_pd3dIndexBuffer->Release();
	if (m_pd3dIndexUploadBuffer) m_pd3dIndexUploadBuffer->Release();
	m_pd3dIndexUploadBuffer = NULL;
}


void Mesh::ReleaseUploadBuffers()
{
	//정점 버퍼를 위한 업로드 버퍼를 소멸시킨다.
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	m_pd3dVertexUploadBuffer = NULL;
};

void Mesh::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);
	if (m_pd3dIndexBuffer)
	{
		pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
		pd3dCommandList->DrawIndexedInstanced(m_nIndices, 1, 0, 0, 0);
		//인덱스 버퍼가 있으면 인덱스 버퍼를 파이프라인(IA: 입력 조립기)에 연결하고 인덱스를 사용하여 렌더링한다.
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	}
}

GroundMesh::GroundMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth, float fHeight)
	: Mesh(pd3dDevice,pd3dCommandList)
{
	//직육면체는 꼭지점(정점)이 8개이다. 
	m_nVertices = 4;
	m_nStride = sizeof(Vertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	XMFLOAT4 COLOR = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	XMFLOAT3 NORMAL = XMFLOAT3(0.0f, 1.0f, 0.0f);
	float fx = fWidth * 0.5f,fy = -6.0f, fz = fHeight * 0.5f;
	//정점 버퍼는 직육면체의 꼭지점 8개에 대한 정점 데이터를 가진다. 
	Vertex pVertices[4];
	pVertices[0] = Vertex(XMFLOAT3(-fx, +fy, -fz), COLOR, NORMAL);
	pVertices[1] = Vertex(XMFLOAT3(+fx, +fy, -fz), COLOR, NORMAL);
	pVertices[2] = Vertex(XMFLOAT3(+fx, +fy, +fz), COLOR, NORMAL);
	pVertices[3] = Vertex(XMFLOAT3(-fx, +fy, +fz), COLOR, NORMAL);
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
	/*인덱스 버퍼는 직육면체의 6개의 면(사각형)에 대한 기하 정보를 갖는다. 삼각형 리스트로 직육면체를 표현할 것이므로 각 면은 2개의 삼각형을 가지고 각 삼각형은 3개의 정점이 필요하다. 즉, 인덱스 버퍼는 전체 36(=6*2*3)개의 인덱스를 가져야 한다.*/
	m_nIndices = 6;
	UINT pnIndices[6];
	//ⓐ 앞면(Front) 사각형의 위쪽 삼각형
	pnIndices[0] = 3; pnIndices[1] = 1; pnIndices[2] = 0;
	//ⓑ 앞면(Front) 사각형의 아래쪽 삼각형
	pnIndices[3] = 2; pnIndices[4] = 1; pnIndices[5] = 3;
	//인덱스 버퍼를 생성한다. 
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices,
		sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER,
		&m_pd3dIndexUploadBuffer);
	//인덱스 버퍼 뷰를 생성한다. 
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
}

GroundMesh::~GroundMesh()
{
}