cbuffer cbGameObjectInfo : register(b0)
{
    matrix gmtxWorld : packoffset(c0);
};

cbuffer cbCameraInfo : register(b1)
{
    matrix gmtxView : packoffset(c0);
    matrix gmtxProjection : packoffset(c4);
};

cbuffer cbLightInfo : register(b2)
{
    float4 gLightColor : packoffset(c0);
    float4 gLightDirection : packoffset(c1);
    float4 gCameraPosition : packoffset(c2);
    float4 gLightParams : packoffset(c3);
};


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

VS_OUTPUT VSDiffused(VS_INPUT input)
{
    VS_OUTPUT output;
    float4 worldPos = mul(float4(input.position, 1.0f), gmtxWorld);
    output.worldPos = worldPos.xyz;
    output.position = mul(mul(worldPos, gmtxView), gmtxProjection);
    output.normal = normalize(mul(input.normal, (float3x3) gmtxWorld));
    output.texcoord = input.texcoord;
    output.color = input.color;
    return (output);
}

float4 PSDiffused(VS_OUTPUT input) : SV_TARGET
{
    
    float ambientIntensity = 0.5f;
    float3 ambient = gLightColor.rgb * ambientIntensity;

    float3 lightDir = normalize(-gLightDirection.xyz);
    float diff = max(dot(normalize(input.normal), lightDir), 0.0f);
    float3 diffuse = gLightColor.rgb * diff;

    float3 resultColor = (ambient + diffuse) * input.color.rgb;

    return float4(resultColor, input.color.a);
}
//end