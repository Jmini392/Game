#pragma once
#include <map>
#include <string>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"



class ResourceMgr
{
	SINGLE(ResourceMgr);
private:
	std::map<std::wstring, Mesh*> m_mapMesh;
	std::map<std::wstring, Texture> m_mapTexture;
	
private:
	// Assimp로부터 메쉬 데이터 추출
	void ProcessNode(aiNode* node, const aiScene* scene, 
		std::vector<Vertex>& vertices, std::vector<UINT>& indices);
	void ProcessMesh(aiMesh* mesh, const aiScene* scene,
		std::vector<Vertex>& vertices, std::vector<UINT>& indices);
	Material ProcessMaterial(ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList, 
		aiMaterial* material, 
		const std::wstring& strFbxFilePath);

	// 캐시에서 메쉬 찾기
	Mesh* FindMesh(const std::wstring& strKey);
	// 캐시에서 텍스처 찾기
	Texture* FindTexture(const std::wstring& strKey);

public:
	void Release();
	// 직접 메쉬 로드 (캐싱 지원)
	Mesh* LoadMesh(ID3D12Device* pd3dDevice, 
		ID3D12GraphicsCommandList* pd3dCommandList, 
		const std::wstring& strKey);
	void AddMesh(const std::wstring& strKey, Mesh* pMesh) { m_mapMesh.insert({ strKey, pMesh }); }
	// 메쉬 로드 Obj
	Mesh* LoadMeshObj(ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList,
		const std::wstring& strKey,
		const std::string& strRelativePath);
	// 메쉬 로드 (캐싱 지원) Fbx
	Mesh* LoadMeshFbx(ID3D12Device* pd3dDevice, 
		ID3D12GraphicsCommandList* pd3dCommandList, 
		const std::wstring& strKey, 
		const std::wstring& strRelativePath);
	// 텍스처 로드 (캐싱 지원)
	Texture* LoadTexture(ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList, 
		const std::wstring& strKey, 
		const std::wstring& strRelativePath);
};