#include "stdafx.h"
#include "ResourceMgr.h"
#include "Mesh.h"
#include <codecvt>
#include <functional>

ResourceMgr::ResourceMgr() : m_pFbxManager(nullptr) {}
ResourceMgr::~ResourceMgr() {
	if (m_pFbxManager) m_pFbxManager->Destroy();

	for (auto& m : m_mapMesh) {
		m.second->Release();
	}
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
		return nullptr;
	}

	FbxMesh* pFbxMesh = (FbxMesh*)vecMeshNodes[0]->GetNodeAttribute();

	// 정점 위치 데이터 추출
	FbxVector4* pCtrlPoints = pFbxMesh->GetControlPoints();
	int iVtxCount = pFbxMesh->GetControlPointsCount();
	std::vector<Vertex> vecVtx(iVtxCount);

	for (int i = 0; i < iVtxCount; ++i)
	{
		vecVtx[i].m_xmf3Position = XMFLOAT3(
			(float)pCtrlPoints[i][0], 
			(float)pCtrlPoints[i][2], 
			-(float)pCtrlPoints[i][1]);
		vecVtx[i].m_xmf4Diffuse = XMFLOAT4(1.f, 1.f, 1.f, 1.f); // 기본값
	}

	// 컬러 데이터 추출
	FbxGeometryElementVertexColor* pColor = pFbxMesh->GetElementVertexColor(0);
	if (pColor)
	{
		for (int i = 0; i < iVtxCount; ++i)
		{
			int iColorIdx = (pColor->GetMappingMode() == FbxLayerElement::eByControlPoint) ? i : i;
			FbxColor color = pColor->GetDirectArray().GetAt(iColorIdx);
			vecVtx[i].m_xmf4Diffuse = XMFLOAT4((float)color.mRed, (float)color.mGreen, (float)color.mBlue, (float)color.mAlpha);
		}
	}

	// UV 데이터 추출
	FbxGeometryElementUV* pUV = pFbxMesh->GetElementUV(0);
	if (pUV)
	{
		for (int i = 0; i < iVtxCount; ++i)
		{
			int iUVIdx = (pUV->GetMappingMode() == FbxLayerElement::eByControlPoint) ? i : i;
			FbxVector2 uv = pUV->GetDirectArray().GetAt(iUVIdx);
			vecVtx[i].m_xmf2UV = XMFLOAT2((float)uv[0], 1.f - (float)uv[1]);
		}
	}

	// 노멀 데이터 추출 - Face-Vertex 인덱싱 고려
	FbxGeometryElementNormal* pNormal = pFbxMesh->GetElementNormal(0);
	if (pNormal)
	{
		FbxLayerElement::EMappingMode mappingMode = pNormal->GetMappingMode();
		
		if (mappingMode == FbxLayerElement::eByControlPoint)
		{
			// 정점 기반 노멀
			for (int i = 0; i < iVtxCount; ++i)
			{
				FbxVector4 normal = pNormal->GetDirectArray().GetAt(i);
				vecVtx[i].m_xmf3Normal = XMFLOAT3(
					(float)normal[0],
					(float)normal[2],
					-(float)normal[1]
				);
			}
		}
		else if (mappingMode == FbxLayerElement::eByPolygonVertex)
		{
			// Face-Vertex 인덱싱 - 인덱스 배열에서 노멀 읽기
			std::vector<XMFLOAT3> faceVertexNormals;
			int normalCount = pNormal->GetDirectArray().GetCount();
			
			for (int i = 0; i < normalCount; ++i)
			{
				FbxVector4 normal = pNormal->GetDirectArray().GetAt(i);
				faceVertexNormals.push_back(XMFLOAT3(
					(float)normal[0],
					(float)normal[2],
					-(float)normal[1]
				));
			}
			
			// 인덱스 배열이 있으면 사용, 없으면 직접 인덱스
			int vertexId = 0;
			int iPolyCount = pFbxMesh->GetPolygonCount();
			
			for (int polyIdx = 0; polyIdx < iPolyCount; ++polyIdx)
			{
				int polySize = pFbxMesh->GetPolygonSize(polyIdx);
				
				for (int vertIdx = 0; vertIdx < polySize; ++vertIdx)
				{
					int ctrlPointIdx = pFbxMesh->GetPolygonVertex(polyIdx, vertIdx);
					
					// 노멀 인덱스 가져오기
					int normalIdx = pNormal->GetIndexArray().GetAt(vertexId);
					if (normalIdx >= 0 && normalIdx < (int)faceVertexNormals.size())
					{
						vecVtx[ctrlPointIdx].m_xmf3Normal = faceVertexNormals[normalIdx];
					}
					vertexId++;
				}
			}
		}
	}

	// 인덱스 데이터 추출 (면 기반으로 정확히 읽기)
	std::vector<UINT> vecIdx;
	int iPolyCount = pFbxMesh->GetPolygonCount();
	
	for (int i = 0; i < iPolyCount; ++i)
	{
		int iPolySize = pFbxMesh->GetPolygonSize(i);  // 다각형의 정점 개수 확인
		
		// 사각형(4개)인 경우 삼각형 2개로 변환
		if (iPolySize == 4)
		{
			vecIdx.push_back(pFbxMesh->GetPolygonVertex(i, 0));
			vecIdx.push_back(pFbxMesh->GetPolygonVertex(i, 1));
			vecIdx.push_back(pFbxMesh->GetPolygonVertex(i, 2));
			
			vecIdx.push_back(pFbxMesh->GetPolygonVertex(i, 0));
			vecIdx.push_back(pFbxMesh->GetPolygonVertex(i, 2));
			vecIdx.push_back(pFbxMesh->GetPolygonVertex(i, 3));
		}
		// 삼각형(3개)인 경우 그대로 추가
		else if (iPolySize == 3)
		{
			vecIdx.push_back(pFbxMesh->GetPolygonVertex(i, 0));
			vecIdx.push_back(pFbxMesh->GetPolygonVertex(i, 1));
			vecIdx.push_back(pFbxMesh->GetPolygonVertex(i, 2));
		}
	}

	// 메쉬 생성 및 저장
	Mesh* pNewMesh = new Mesh(pd3dDevice, pd3dCommandList, vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	pNewMesh->AddRef();
	m_mapMesh.insert(std::make_pair(_strKey, pNewMesh));

	pScene->Destroy();

	return pNewMesh;
}