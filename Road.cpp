#include "stdafx.h"
#include "Road.h"
#include "Mesh.h"
#include "Shader.h"

Road::Road()
    : m_fWidth(4.0f)        
    , m_fLength(10.0f)      
    , m_fHeight(0.1f)       
    , m_xmf3Position(0.0f, 0.0f, 0.0f)
    , m_xmf4Color(0.4f, 0.4f, 0.4f, 1.0f)  
{
    XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
}

Road::Road(float fWidth, float fLength, float fHeight)
    : m_fWidth(fWidth)
    , m_fLength(fLength)
    , m_fHeight(fHeight)
    , m_xmf3Position(0.0f, 0.0f, 0.0f)
    , m_xmf4Color(0.4f, 0.4f, 0.4f, 1.0f)
{
    XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
}

Road::~Road()
{
}

void Road::CreateRoad(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
    CreateRoadMesh(pd3dDevice, pd3dCommandList);
    UpdateWorldMatrix();
}

void Road::SetRoadDimensions(float fWidth, float fLength, float fHeight)
{
    m_fWidth = fWidth;
    m_fLength = fLength;
    m_fHeight = fHeight;
}

void Road::SetRoadColor(XMFLOAT4 color)
{
    m_xmf4Color = color;
}

void Road::CreateRoadMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
    CubeMeshDiffused* pRoadMesh = new CubeMeshDiffused(
        pd3dDevice,
        pd3dCommandList,
        m_fWidth,  
        m_fHeight,  
        m_fLength, 
        m_xmf4Color.x,  
        m_xmf4Color.y,   
        m_xmf4Color.z  
    );

    SetMesh(pRoadMesh);
}

void Road::SetPosition(float x, float y, float z)
{
    m_xmf3Position = XMFLOAT3(x, y, z);
    UpdateWorldMatrix();
}

void Road::SetPosition(XMFLOAT3 position)
{
    m_xmf3Position = position;
    UpdateWorldMatrix();
}

void Road::UpdateWorldMatrix()
{
    XMMATRIX mtxTranslation = XMMatrixTranslation(
        m_xmf3Position.x,
        m_xmf3Position.y,
        m_xmf3Position.z
    );
    XMStoreFloat4x4(&m_xmf4x4World, mtxTranslation);
}

void Road::Update()
{
    Object::Update();
}

void Road::Animate(float fTimeElapsed)
{
    Object::Animate(fTimeElapsed);
}

// 🎨 렌더링 준비
void Road::OnPrepareRender()
{
    Object::OnPrepareRender();
}