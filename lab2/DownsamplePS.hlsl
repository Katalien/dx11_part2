Texture2D AvgTexture : register(t0);
Texture2D MinTexture : register(t1);
Texture2D MaxTexture : register(t2);
SamplerState Sampler : register(s0);

struct PS_OUTPUT
{
    float4 avg : SV_Target0;
    float4 min : SV_Target1;
    float4 max : SV_Target2;
};

PS_OUTPUT main(float4 pos : SV_POSITION, float2 texCoord : TEXCOORD)
{
    PS_OUTPUT output;
    
    // 
    float2 texelSize = 1.0 / float2(1920, 1080); 
    
    float4 samplesAvg[4];
    samplesAvg[0] = AvgTexture.Sample(Sampler, texCoord + float2(-0.5, -0.5) * texelSize);
    samplesAvg[1] = AvgTexture.Sample(Sampler, texCoord + float2(-0.5, 0.5) * texelSize);
    samplesAvg[2] = AvgTexture.Sample(Sampler, texCoord + float2(0.5, -0.5) * texelSize);
    samplesAvg[3] = AvgTexture.Sample(Sampler, texCoord + float2(0.5, 0.5) * texelSize);
    
    float4 samplesMin[4];
    samplesMin[0] = MinTexture.Sample(Sampler, texCoord + float2(-0.5, -0.5) * texelSize);
    samplesMin[1] = MinTexture.Sample(Sampler, texCoord + float2(-0.5, 0.5) * texelSize);
    samplesMin[2] = MinTexture.Sample(Sampler, texCoord + float2(0.5, -0.5) * texelSize);
    samplesMin[3] = MinTexture.Sample(Sampler, texCoord + float2(0.5, 0.5) * texelSize);
    
    float4 samplesMax[4];
    samplesMax[0] = MaxTexture.Sample(Sampler, texCoord + float2(-0.5, -0.5) * texelSize);
    samplesMax[1] = MaxTexture.Sample(Sampler, texCoord + float2(-0.5, 0.5) * texelSize);
    samplesMax[2] = MaxTexture.Sample(Sampler, texCoord + float2(0.5, -0.5) * texelSize);
    samplesMax[3] = MaxTexture.Sample(Sampler, texCoord + float2(0.5, 0.5) * texelSize);
    
    // avg
    output.avg = (samplesAvg[0] + samplesAvg[1] + samplesAvg[2] + samplesAvg[3]) * 0.25;
    
    // min
    output.min = min(min(samplesMin[0], samplesMin[1]), min(samplesMin[2], samplesMin[3]));
    
    // max
    output.max = max(max(samplesMax[0], samplesMax[1]), max(samplesMax[2], samplesMax[3]));
    
    return output;
}