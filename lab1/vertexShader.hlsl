cbuffer WorldMatrixBuffer : register(b0)
{
    float4x4 mWorldMatrix;
};

cbuffer SceneMatrixBuffer : register(b1)
{
    float4x4 mViewProjectionMatrix;
};

struct VS_INPUT
{
    float3 position : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0; // �������� UV-����������
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0; // ��������� UV-����������
};

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;
    output.position = mul(mViewProjectionMatrix, mul(mWorldMatrix, float4(input.position, 1.0f)));
    output.color = input.color;
    output.uv = input.uv; // ��������� UV-����������
    return output;
}

