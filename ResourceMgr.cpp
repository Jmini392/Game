#include "stdafx.h"
#include "ResourceMgr.h"
#include <locale>
#include <codecvt>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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
	const std::wstring& strKey) {
	Mesh* pMesh = FindMesh(strKey);
	if (pMesh != nullptr) {
		return pMesh;
	}
	return nullptr;
}

Mesh* ResourceMgr::LoadMeshObj(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList,
	const std::wstring& strKey,
	const std::string& strRelativePath) {
	Mesh* pMesh = FindMesh(strKey);
	if (pMesh != nullptr) {
		return pMesh;
	}
	pMesh = new Mesh(pd3dDevice, pd3dCommandList, strRelativePath);

	m_mapMesh.insert({ strKey, pMesh });

	return pMesh;
}

Mesh* ResourceMgr::LoadMeshFbx(ID3D12Device* pd3dDevice,
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

	if (scene->mNumMaterials > 0) {
		Material mat = ProcessMaterial(pd3dDevice, pd3dCommandList ,scene->mMaterials[0], strRelativePath);
		pMesh->SetMaterial(mat);
	}

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

	// 메쉬에 연결된 재질 가져오기
	XMFLOAT4 meshDiffuseColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	if (mesh->mMaterialIndex < scene->mNumMaterials) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		aiColor4D diffuse;
		if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse)) {
			meshDiffuseColor = XMFLOAT4(diffuse.r, diffuse.g, diffuse.b, diffuse.a);
		}
	}

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

		// 색상: 버텍스 컬러가 있으면 사용, 없으면 메쉬의 재질 Diffuse 색상 사용
		if (mesh->HasVertexColors(0)) {
			vertex.m_xmf4Diffuse.x = mesh->mColors[0][i].r;
			vertex.m_xmf4Diffuse.y = mesh->mColors[0][i].g;
			vertex.m_xmf4Diffuse.z = mesh->mColors[0][i].b;
			vertex.m_xmf4Diffuse.w = mesh->mColors[0][i].a;
		}
		else {
			// 메쉬의 재질 Diffuse 색상을 버텍스 컬러로 사용
			vertex.m_xmf4Diffuse = meshDiffuseColor;
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

// 재질 추출 함수 구현
Material ResourceMgr::ProcessMaterial(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList, 
	aiMaterial* material, 
	const std::wstring& strFbxFilePath)
{
	Material mat;

	aiColor4D color;

	// Diffuse 색상
	if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, color)) {
		mat.Diffuse = XMFLOAT4(color.r, color.g, color.b, color.a);
	}

	// Ambient 색상
	if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_AMBIENT, color)) {
		mat.Ambient = XMFLOAT4(color.r, color.g, color.b, color.a);
	}

	// Specular 색상
	if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_SPECULAR, color)) {
		mat.Specular = XMFLOAT4(color.r, color.g, color.b, mat.Specular.w);
	}

	// Shininess (광택)
	float shininess = 1.0f;
	if (AI_SUCCESS == material->Get(AI_MATKEY_SHININESS, shininess)) {
		mat.Specular.w = shininess;
	}

	// Emissive 색상
	if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_EMISSIVE, color)) {
		mat.Emissive = XMFLOAT4(color.r, color.g, color.b, color.a);
	}

	// Diffuse 텍스처 로드
	aiString texturePath;
	if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
		if (AI_SUCCESS == material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath)) {
			// FBX 파일의 디렉토리 경로 추출
			std::wstring directory = strFbxFilePath.substr(0, strFbxFilePath.find_last_of(L'/') + 1);

			// 텍스처 파일 이름(UTF-8)을 wstring으로 변환
			std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
			std::wstring wTexturePath = converter.from_bytes(texturePath.C_Str());

			// 전체 경로 조합 및 텍스처 로드
			std::wstring fullTexturePath = directory + wTexturePath;
			mat.pTexture = LoadTexture(pd3dDevice, pd3dCommandList, wTexturePath, fullTexturePath);
		}
	}

	return mat;
}

Texture* ResourceMgr::LoadTexture(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList,
	const std::wstring& strKey,
	const std::wstring& strRelativePath)
{
	// 1. 캐시에서 텍스처 찾기
	Texture* pTexture = FindTexture(strKey);
	if (pTexture)
		return pTexture;

	// 2. 텍스처 파일 경로 조합 및 로드
	wchar_t szFullPath[256] = {};
	wcscpy_s(szFullPath, 256, L"Res/Textures/");
	wcscat_s(szFullPath, 256, strRelativePath.c_str());

	char cPath[256];
	WideCharToMultiByte(CP_ACP, 0, szFullPath, -1, cPath, 256, NULL, NULL);

	int width, height, channels;
	unsigned char* pImageData = stbi_load(cPath, &width, &height, &channels, 4);
	if (!pImageData)
	{
		assert(NULL);
		return nullptr;
	}

	// 4. D3D12 텍스처 리소스 생성
	D3D12_RESOURCE_DESC textureDesc = {};
	textureDesc.MipLevels = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	textureDesc.DepthOrArraySize = 1;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

	Texture newTexture;

	// CD3DX12_HEAP_PROPERTIES 대체
	D3D12_HEAP_PROPERTIES defaultHeapProps = {};
	defaultHeapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
	defaultHeapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	defaultHeapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	defaultHeapProps.CreationNodeMask = 1;
	defaultHeapProps.VisibleNodeMask = 1;

	pd3dDevice->CreateCommittedResource(
		&defaultHeapProps,
		D3D12_HEAP_FLAG_NONE,
		&textureDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&newTexture.pResource));

	// 5. 업로드 힙 생성 및 데이터 복사
	// GetRequiredIntermediateSize 대체
	UINT64 uploadBufferSize;
	pd3dDevice->GetCopyableFootprints(&textureDesc, 0, 1, 0, nullptr, nullptr, nullptr, &uploadBufferSize);

	// CD3DX12_HEAP_PROPERTIES 대체
	D3D12_HEAP_PROPERTIES uploadHeapProps = {};
	uploadHeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
	uploadHeapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	uploadHeapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	uploadHeapProps.CreationNodeMask = 1;
	uploadHeapProps.VisibleNodeMask = 1;

	// CD3DX12_RESOURCE_DESC::Buffer 대체
	D3D12_RESOURCE_DESC bufferDesc = {};
	bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	bufferDesc.Alignment = 0;
	bufferDesc.Width = uploadBufferSize;
	bufferDesc.Height = 1;
	bufferDesc.DepthOrArraySize = 1;
	bufferDesc.MipLevels = 1;
	bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
	bufferDesc.SampleDesc.Count = 1;
	bufferDesc.SampleDesc.Quality = 0;
	bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	bufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	pd3dDevice->CreateCommittedResource(
		&uploadHeapProps,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&newTexture.pUploadBuffer));

	// UpdateSubresources 대체
	D3D12_SUBRESOURCE_DATA textureData = {};
	textureData.pData = pImageData;
	textureData.RowPitch = width * 4;
	textureData.SlicePitch = textureData.RowPitch * height;

	D3D12_PLACED_SUBRESOURCE_FOOTPRINT layout;
	UINT numRows;
	UINT64 rowSizeInBytes;
	pd3dDevice->GetCopyableFootprints(&textureDesc, 0, 1, 0, &layout, &numRows, &rowSizeInBytes, nullptr);

	void* pMappedData;
	newTexture.pUploadBuffer->Map(0, nullptr, &pMappedData);
	for (UINT i = 0; i < numRows; ++i)
	{
		BYTE* pDestSlice = reinterpret_cast<BYTE*>(pMappedData) + layout.Offset + i * layout.Footprint.RowPitch;
		const BYTE* pSrcSlice = reinterpret_cast<const BYTE*>(textureData.pData) + i * textureData.RowPitch;
		memcpy(pDestSlice, pSrcSlice, rowSizeInBytes);
	}
	newTexture.pUploadBuffer->Unmap(0, nullptr);

	D3D12_TEXTURE_COPY_LOCATION destLocation = { newTexture.pResource, D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX, 0 };
	D3D12_TEXTURE_COPY_LOCATION srcLocation = { newTexture.pUploadBuffer, D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT, layout };
	pd3dCommandList->CopyTextureRegion(&destLocation, 0, 0, 0, &srcLocation, nullptr);


	// 6. 리소스 상태 전이
	// CD3DX12_RESOURCE_BARRIER::Transition 대체
	D3D12_RESOURCE_BARRIER resourceBarrier = {};
	resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	resourceBarrier.Transition.pResource = newTexture.pResource;
	resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	pd3dCommandList->ResourceBarrier(1, &resourceBarrier);

	// 7. 임시 데이터 해제
	stbi_image_free(pImageData);

	// 8. 텍스처 맵에 저장 및 반환
	auto iter = m_mapTexture.insert({ strKey, newTexture });
	return &iter.first->second;
}

Texture* ResourceMgr::FindTexture(const std::wstring& strKey) {
	auto iter = m_mapTexture.find(strKey);
	if (iter != m_mapTexture.end()) {
		return &iter->second;
	}
	return nullptr;
}