#include "stdafx.h"
#include "ResourceMgr.h"
#include <locale>
#include <codecvt>


ResourceMgr::ResourceMgr() {}

ResourceMgr::~ResourceMgr() {
	Release();
	for (auto& m : m_mapMesh) {
		m.second->Release();
	}
}

void ResourceMgr::Release() {
	for (auto& pair : m_mapMesh) {
		if (pair.second) {
			pair.second->Release();
		}
	}
	m_mapMesh.clear();
}

Mesh* ResourceMgr::FindMesh(const std::wstring& strKey) {
	auto iter = m_mapMesh.find(strKey);
	if (iter != m_mapMesh.end()) {
		return iter->second;
	}
	return nullptr;
}

Mesh* ResourceMgr::LoadMesh(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList,
	const std::wstring& strKey,
	const std::wstring& strRelativePath)
{
	// 1. 캐시에서 먼저 찾기
	Mesh* pMesh = FindMesh(strKey);
	if (pMesh != nullptr) {
		return pMesh;
	}

	// 2. wstring -> string 변환 (Assimp는 std::string 사용)
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	std::string filePath = converter.to_bytes(strRelativePath);

	// 3. Assimp Importer 생성 및 파일 로드
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath,
		aiProcess_Triangulate |           // 삼각형으로 변환
		aiProcess_GenNormals |            // 노말 생성
		aiProcess_FlipUVs |               // UV 뒤집기 (DirectX용)
		aiProcess_CalcTangentSpace |      // 탄젠트 계산
		aiProcess_JoinIdenticalVertices   // 동일 정점 병합
	);

	// 4. 로드 실패 체크
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		OutputDebugStringA("Assimp Error: ");
		OutputDebugStringA(importer.GetErrorString());
		OutputDebugStringA("\n");
		return nullptr;
	}

	// 5. 정점 및 인덱스 데이터 추출
	std::vector<Vertex> vertices;
	std::vector<UINT> indices;
	ProcessNode(scene->mRootNode, scene, vertices, indices);

	// 6. 메쉬 생성 (Mesh 클래스에 새 생성자 필요)
	pMesh = new Mesh(pd3dDevice, pd3dCommandList, vertices, indices);
	pMesh->AddRef();

	// 7. 캐시에 저장
	m_mapMesh.insert({ strKey, pMesh });

	return pMesh;
}

void ResourceMgr::ProcessNode(aiNode* node, const aiScene* scene,
	std::vector<Vertex>& vertices, std::vector<UINT>& indices)
{
	// 현재 노드의 모든 메쉬 처리
	for (UINT i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		ProcessMesh(mesh, scene, vertices, indices);
	}

	// 자식 노드들 재귀 처리
	for (UINT i = 0; i < node->mNumChildren; i++) {
		ProcessNode(node->mChildren[i], scene, vertices, indices);
	}
}

void ResourceMgr::ProcessMesh(aiMesh* mesh, const aiScene* scene,
	std::vector<Vertex>& vertices, std::vector<UINT>& indices)
{
	UINT baseVertex = static_cast<UINT>(vertices.size());

	// 정점 데이터 추출
	for (UINT i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;

		// 위치
		vertex.m_xmf3Position.x = mesh->mVertices[i].x;
		vertex.m_xmf3Position.y = mesh->mVertices[i].z;
		vertex.m_xmf3Position.z = -mesh->mVertices[i].y;

		// 노말
		if (mesh->HasNormals()) {
			vertex.m_xmf3Normal.x = mesh->mNormals[i].x;
			vertex.m_xmf3Normal.y = mesh->mNormals[i].z;
			vertex.m_xmf3Normal.z = -mesh->mNormals[i].y;
		}

		// UV 좌표 (첫 번째 UV 채널만 사용)
		if (mesh->mTextureCoords[0]) {
			vertex.m_xmf2UV.x = mesh->mTextureCoords[0][i].x;
			vertex.m_xmf2UV.y = mesh->mTextureCoords[0][i].y;
		}

		// 색상 (있는 경우)
		if (mesh->HasVertexColors(0)) {
			vertex.m_xmf4Diffuse.x = mesh->mColors[0][i].r;
			vertex.m_xmf4Diffuse.y = mesh->mColors[0][i].g;
			vertex.m_xmf4Diffuse.z = mesh->mColors[0][i].b;
			vertex.m_xmf4Diffuse.w = mesh->mColors[0][i].a;
		}
		else {
			vertex.m_xmf4Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		}

		vertices.push_back(vertex);
	}

	// 인덱스 데이터 추출
	for (UINT i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (UINT j = 0; j < face.mNumIndices; j++) {
			indices.push_back(baseVertex + face.mIndices[j]);
		}
	}
}