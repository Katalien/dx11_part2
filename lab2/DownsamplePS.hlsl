Texture2D AvgTexture : register(t0);
Texture2D MinTexture : register(t1);
Texture2D MaxTexture : register(t2);
SamplerState SamplerAvg : register(s0);
SamplerState SamplerMin : register(s1);
SamplerState SamplerMax : register(s2);

struct PS_OUTPUT
{
    float4 avg : SV_Target0;
    float4 min : SV_Target1;
    float4 max : SV_Target2;
};

PS_OUTPUT main(float4 pos : SV_POSITION, float2 texCoord : TEXCOORD)
{
    PS_OUTPUT output;
    
    output.avg = AvgTexture.Sample(SamplerAvg, texCoord);
    output.min = MinTexture.Sample(SamplerMin, texCoord);
    output.max = MaxTexture.Sample(SamplerMax, texCoord);
    
    return output;
}