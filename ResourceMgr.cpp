#include "stdafx.h"
#include "ResourceMgr.h"
#include "Mesh.h"
#include <codecvt>
#include <functional>

ResourceMgr::ResourceMgr() : m_pFbxManager(nullptr) {}
ResourceMgr::~ResourceMgr() {
	if (m_pFbxManager) m_pFbxManager->Destroy();

	for (auto& m : m_mapMesh) delete m.second;

	for (auto& t : m_mapTexture) delete t.second;
}

void ResourceMgr::Init()
{
	// FBX 매니저 생성
	m_pFbxManager = FbxManager::Create();
	FbxIOSettings* pIOSettings = FbxIOSettings::Create(m_pFbxManager, IOSROOT);
	m_pFbxManager->SetIOSettings(pIOSettings);
}

Mesh* ResourceMgr::LoadMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,const std::wstring& _strKey, const std::string& _strRelativePath)
{
	// 이미 로드된 리소스인지 확인
	auto it = m_mapMesh.find(_strKey);
	if (it != m_mapMesh.end())
	{
		return it->second; // 이미 로드된 리소스 반환
	}
	// FBX 파일 로드 및 파싱
	FbxImporter* pImporter = FbxImporter::Create(m_pFbxManager, "");

	if (!pImporter->Initialize(_strRelativePath.c_str(), -1, m_pFbxManager->GetIOSettings()))
	{
		// 로드 실패
		pImporter->Destroy();
		return nullptr;
	}
	FbxScene* pScene = FbxScene::Create(m_pFbxManager, "myScene");
	pImporter->Import(pScene);
	pImporter->Destroy();

	FbxNode* pRootNode = pScene->GetRootNode();
	std::vector<FbxNode*> vecMeshNodes;

	// 씬의 모든 노드를 순회하며 메쉬 노드만 수집
	std::function<void(FbxNode*)> Traverse =
		[&](FbxNode* _pNode)
		{
			if (_pNode->GetNodeAttribute() && _pNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eMesh)
			{
				vecMeshNodes.push_back(_pNode);
			}

			for (int i = 0; i < _pNode->GetChildCount(); ++i)
			{
				Traverse(_pNode->GetChild(i));
			}
		};

	Traverse(pRootNode);

	if (vecMeshNodes.empty())
	{
		pScene->Destroy();
		return nullptr; // 메쉬 노드가 없음
	}

	// 첫 번째 메쉬 노드만 사용
	FbxMesh* pFbxMesh = (FbxMesh*)vecMeshNodes[0]->GetNodeAttribute();

	// 정점 데이터 추출
	FbxVector4* pCtrlPoints = pFbxMesh->GetControlPoints();
	int iVtxCount = pFbxMesh->GetControlPointsCount();
	std::vector<Vertex> vecVtx(iVtxCount);

	for (int i = 0; i < iVtxCount; ++i)
	{
		vecVtx[i].m_xmf3Position = XMFLOAT3((float)pCtrlPoints[i][0], (float)pCtrlPoints[i][2], (float)pCtrlPoints[i][1]);
	}

	// 인덱스, 컬러, UV, 노멀 데이터 추출
	std::vector<UINT> vecIdx;
	int iPolyCount = pFbxMesh->GetPolygonCount();
	for (int i = 0; i < iPolyCount; ++i)
	{
		for (int j = 0; j < 3; ++j) // 삼각형 폴리곤 기준
		{
			int iVtxIdx = pFbxMesh->GetPolygonVertex(i, j);
			vecIdx.push_back(iVtxIdx);

			// 컬러
			FbxGeometryElementVertexColor* pColor = pFbxMesh->GetElementVertexColor(0);
			if (pColor)
			{
				FbxColor color = pColor->GetDirectArray().GetAt(pColor->GetMappingMode() == FbxLayerElement::eByControlPoint ? iVtxIdx : vecIdx.size() - 1);
				vecVtx[iVtxIdx].m_xmf4Diffuse = XMFLOAT4((float)color.mRed, (float)color.mGreen, (float)color.mBlue, (float)color.mAlpha);
			}
			else
			{
				vecVtx[iVtxIdx].m_xmf4Diffuse = XMFLOAT4(1.f, 1.f, 1.f, 1.f); // 기본 흰색
			}

			// UV
			FbxGeometryElementUV* pUV = pFbxMesh->GetElementUV(0);
			if (pUV)
			{
				FbxVector2 uv = pUV->GetDirectArray().GetAt(pUV->GetMappingMode() == FbxLayerElement::eByControlPoint ? iVtxIdx : pFbxMesh->GetTextureUVIndex(i, j));
				vecVtx[iVtxIdx].m_xmf2UV = XMFLOAT2((float)uv[0], 1.f - (float)uv[1]); // Y축 반전
			}

			// 노멀
			FbxGeometryElementNormal* pNormal = pFbxMesh->GetElementNormal(0);
			if (pNormal)
			{
				FbxVector4 normal = pNormal->GetDirectArray().GetAt(pNormal->GetMappingMode() == FbxLayerElement::eByControlPoint ? iVtxIdx : vecIdx.size() - 1);
				vecVtx[iVtxIdx].m_xmf3Normal = XMFLOAT3((float)normal[0], (float)normal[2], (float)normal[1]);
			}
		}
	}

	// 메쉬 생성 및 저장
	Mesh* pNewMesh = new Mesh(pd3dDevice, pd3dCommandList, vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	m_mapMesh.insert(std::make_pair(_strKey, pNewMesh));

	pScene->Destroy();

	return pNewMesh;
}