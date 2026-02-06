#pragma once
#include "Shader.h"
#include "Camera.h"

class Object;

class Scene
{
private:
	std::wstring m_wstrName;

protected:
	Camera* m_pCamera;
	std::vector<Object*> arrObj[(UINT)GROUP_TYPE::END];

public:
	Scene();
	virtual ~Scene();

	void SetName(const std::wstring& strName) { m_wstrName = strName; }
	const std::wstring& GetName() { return m_wstrName; }

	virtual void Update();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void Enter(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) = 0;
	virtual void Exit() = 0;

	Camera* GetCamera() { return m_pCamera; }

public:
	void AddObject(Object* obj, GROUP_TYPE type) {
		arrObj[(UINT)type].push_back(obj);
	}
};

