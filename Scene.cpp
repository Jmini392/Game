#include "stdafx.h"
#include "Scene.h"

#include "Object.h"

Scene::Scene()
	:m_pCamera(nullptr), m_pLight(nullptr)
{
	m_pCamera = new Camera();
	m_pLight = new Light();
}

Scene::~Scene()
{
	for (UINT i = 0; i < (UINT)GROUP_TYPE::END; ++i) {
		for (size_t j = 0; j < arrObj[i].size(); ++j) {
			delete arrObj[i][j];
		}
	}

	if (m_pCamera) {
		delete m_pCamera;
	}

	if (m_pLight) {
		delete m_pLight;
		m_pLight = nullptr;
	}
}

void Scene::Update(float fTimeElapsed)
{
	for (UINT i = 0; i < (UINT)GROUP_TYPE::END; ++i) {
		for (size_t j = 0; j < arrObj[i].size(); ++j) {
			arrObj[i][j]->Animate(fTimeElapsed);
		}
	}
}

void Scene::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pCamera) {
		m_pCamera->SetViewportsAndScissorRects(pd3dCommandList);
		m_pCamera->UpdateShaderVariables(pd3dCommandList);
	}

	if (m_pLight) {
		m_pLight->UpdateShaderVariables(pd3dCommandList);
	}

	for (UINT i = 0; i < (UINT)GROUP_TYPE::END; ++i) {
		for (size_t j = 0; j < arrObj[i].size(); ++j) {
			arrObj[i][j]->Render(pd3dCommandList, m_pCamera);
		}
	}
}

void Scene::Enter(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void Scene::Exit()
{
}