struct VS_INPUT
{
    float3 Pos : POSITION;
    float2 Tex : TEXCOORD0;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;
    
    // Позиция напрямую в NDC пространстве
    output.Pos = float4(input.Pos, 1.0f);
    
    // Передача текстурных координат
    output.Tex = input.Tex;
    
    return output;
}