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
    
    // real size
    uint width, height;
    AvgTexture.GetDimensions(width, height);
    const float2 texelSize = 1.0f / float2(width, height);
    
    // 
    const float2 offsets[4] =
    {
        float2(-0.5f, -0.5f),
        float2(-0.5f, 0.5f),
        float2(0.5f, -0.5f),
        float2(0.5f, 0.5f)
    };
    
    
    float4 samplesAvg[4];
    float4 samplesMin[4];
    float4 samplesMax[4];
    
    [unroll]
    for (int i = 0; i < 4; ++i)
    {
        const float2 sampleCoord = texCoord + offsets[i] * texelSize;
        samplesAvg[i] = AvgTexture.Sample(Sampler, sampleCoord);
        samplesMin[i] = MinTexture.Sample(Sampler, sampleCoord);
        samplesMax[i] = MaxTexture.Sample(Sampler, sampleCoord);
    }
    
    // calc avg
    output.avg = (samplesAvg[0] + samplesAvg[1] + samplesAvg[2] + samplesAvg[3]) * 0.25f;
    
    // min
    output.min = samplesMin[0];
    [unroll]
    for (int j = 1; j < 4; ++j)
    {
        output.min = min(output.min, samplesMin[j]);
    }
    
    // max
    output.max = samplesMax[0];
    [unroll]
    for (int k = 1; k < 4; ++k)
    {
        output.max = max(output.max, samplesMax[k]);
    }
    
    return output;
}