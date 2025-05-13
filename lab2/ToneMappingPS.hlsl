

Texture2D HDRTexture : register(t0);
Texture2D AvgLuminance : register(t1);
SamplerState Sampler : register(s0);

cbuffer AdaptationBuffer : register(b0)
{
    float DeltaTime;
    float AdaptationSpeed;
};

static const float A = 0.15;
static const float B = 0.50;
static const float C = 0.10;
static const float D = 0.20;
static const float E = 0.02;
static const float F = 0.30;
static const float W = 11.2;

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};

float3 Uncharted2Tonemap(float3 x)
{
    return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

float4 main(PS_INPUT input) : SV_Target
{
    // read hdr color
    float3 hdrColor = HDRTexture.Sample(Sampler, input.Tex).rgb;
    
    // avg
    float avgLum = AvgLuminance.Sample(Sampler, float2(0.5, 0.5)).r;
    
    // јдаптаци€ €ркости
    static float adaptedLum = 1.0;
    adaptedLum = lerp(adaptedLum, avgLum, DeltaTime * AdaptationSpeed);
    
    // calc exp
    float exposure = 1.0 / (adaptedLum + 0.0001);
    float3 exposedColor = hdrColor * exposure;
    
    // tone
    float3 curr = Uncharted2Tonemap(exposedColor);
    float3 whiteScale = 1.0 / Uncharted2Tonemap(W);
    float3 result = curr * whiteScale;
    
    // gamma
    result = pow(result, 1.0 / 2.2);
    
    return float4(result, 1.0);
}
