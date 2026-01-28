cbuffer cbPerObject : register(b0) // 상수 버퍼 선언
{
    float4x4 worldViewProj; // 월드-뷰 투영 행렬
}

Texture2D gDiffuseMap : register(t0); // 디퓨즈 텍스처

SamplerState gsamPoint : register(s0); // 샘플러, 텍스처를 어떻게 읽을지 결정

struct VertexInput // 정점 입력 구조체
{
    float3 position : POSITION; // 위치 (x,y,z)
    float4 color : COLOR;      // 색상 (r,g,b,a)
    float2 uv : TEXCOORD; // 텍스처 좌표 (u,v)
    float3 normal : NORMAL; // 법선 벡터 (x,y,z)
};

struct PixelInput // 픽셀 입력 구조체
{
    float4 position : SV_POSITION; // 화면 위치 (x,y,z,w), w는 동차 좌표
    float4 color : COLOR;          // 색상 (r,g,b,a)
    float2 uv : TEXCOORD; // 텍스처 좌표 (u,v)
    float3 normal : NORMAL; // 법선 벡터 (x,y,z)
};

PixelInput VS(VertexInput input) // 정점 셰이더
{
    PixelInput output;
    
    float4 pos = float4(input.position, 1.0f); // 위치를 동차 좌표로 변환, w가 1.0f여야 위치 계산에 올바르게 사용됨
    output.position = mul(pos, worldViewProj); // 변환된 위치 계산, 행렬 곱셈
    
    output.color = input.color;                     // 색상 전달
    output.uv = input.uv; // 텍스처 좌표 전달
    
    output.normal = input.normal; // 법선 벡터 전달
    
    return output;
}

float4 PS(PixelInput input) : SV_TARGET // 픽셀 셰이더
{
    float4 texColor = gDiffuseMap.Sample(gsamPoint, input.uv); // 텍스처 샘플링
    
    float3 lightDir = normalize(float3(1.0f, 1.0f, -1.0f)); // 광원 방향 벡터, 빛의 방향은 위쪽, 오른쪽, 앞쪽
    
    float3 normal = normalize(input.normal); // 법선 벡터 정규화
    
    float diffuseBrightness = saturate(dot(normal, lightDir)); // 디퓨즈 밝기 계산, 0~1 범위로 제한
    
    float3 lighting = texColor.rgb * (diffuseBrightness + 0.3f); // 텍스처 색상에 조명 적용, 기본 밝기 0.3 추가
    
    return float4(lighting, texColor.a); // 최종 색상 계산 및 반환
}