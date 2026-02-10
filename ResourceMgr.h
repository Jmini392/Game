#pragma once
// 리소스 로드 및 파싱
// 리소스 캐싱
// 리소스 생명주기 관리
#include <map>
#include <string>

class Mesh; // 전방 선언
class Texture; // 전방 선언

class ResourceMgr
{
	SINGLE(ResourceMgr);
private:
	FbxManager* m_pFbxManager;

	// Key - 리소스 식별자, Value - 리소스 포인터
	std::map<std::wstring, Mesh*> m_mapMesh;
	std::map<std::wstring, Texture*> m_mapTexture;
public:
	void Init();
	Mesh* LoadMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const std::wstring& _strKey, const std::string& _strRelativePath);
	// Texture* LoadTexture(const std::wstring& _strKey, const std::string& _strRelativePath);
};

