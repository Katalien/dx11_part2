struct VS_INPUT
{
    uint VertexID : SV_VertexID;
};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;
    float2 texCoord = float2((input.VertexID << 1) & 2, input.VertexID & 2);
    output.Position = float4(texCoord * float2(2.0, -2.0) + float2(-1.0, 1.0), 0.0, 1.0);
    output.TexCoord = texCoord;
    return output;
}
