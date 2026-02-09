//게임 객체의 정보를 위한 상수 버퍼를 선언한다. 
cbuffer cbGameObjectInfo : register(b0)
{
    matrix gmtxWorld : packoffset(c0);
};

//카메라의 정보를 위한 상수 버퍼를 선언한다.
cbuffer cbCameraInfo : register(b1)
{
    matrix gmtxView : packoffset(c0);
    matrix gmtxProjection : packoffset(c4);
};

//조명 정보를 위한 상수 버퍼를 선언한다.
cbuffer cbLightInfo : register(b2)
{
    float4 gLightColor : packoffset(c0); // 광원 색상
    float4 gLightDirection : packoffset(c1); // 광원 방향 또는 위치
    float4 gCameraPosition : packoffset(c2); // 카메라 위치
    float4 gLightParams : packoffset(c3); // 광원 파라미터
};

//정점 셰이더의 입력을 위한 구조체를 선언한다. 
struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD0;
    float4 color : COLOR;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float3 worldPos : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD0;
    float4 color : COLOR;
};

//정점 셰이더를 정의한다.
VS_OUTPUT VSDiffused(VS_INPUT input)
{
    VS_OUTPUT output;
    //정점을 변환(월드 변환, 카메라 변환, 투영 변환)한다. 
    float4 worldPos = mul(float4(input.position, 1.0f), gmtxWorld);
    output.worldPos = worldPos.xyz;
    output.position = mul(mul(worldPos, gmtxView), gmtxProjection);
    
    // 노멀을 월드 공간으로 변환
    output.normal = normalize(mul(input.normal, (float3x3) gmtxWorld));
    
    output.texcoord = input.texcoord;
    output.color = input.color;
    return (output);
}

//픽셀 셰이더를 정의한다. 
float4 PSDiffused(VS_OUTPUT input) : SV_TARGET
{
    /*
    디버그: 노멀을 색상으로 출력
    // return float4(input.normal * 0.5f + 0.5f, 1.0f);
    
    // 디버그: 버텍스 컬러만 출력
    // return input.color;
    
    // 조명이 제대로 들어오는지 확인
    // return float4(gLightColor.rgb, 1.0f);
    */
    
    // Ambient 조명 계산
    float ambientIntensity = 0.3f;
    float3 ambient = gLightColor.rgb * ambientIntensity;
    
    // Diffuse 조명 계산
    float3 lightDir = normalize(-gLightDirection.xyz);
    float diff = max(dot(normalize(input.normal), lightDir), 0.0f);
    float3 diffuse = gLightColor.rgb * diff;
    
    // 최종 색상 계산
    float3 resultColor = (ambient + diffuse) * input.color.rgb;
    
    return float4(resultColor, input.color.a);
}