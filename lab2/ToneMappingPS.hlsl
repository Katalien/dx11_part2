Texture2D HDRTexture : register(t0);
Texture2D AvgLuminance : register(t1);
Texture2D MinLuminance : register(t2);
Texture2D MaxLuminance : register(t3);
SamplerState Sampler : register(s0);

cbuffer AdaptationBuffer : register(b0)
{
    float DeltaTime;
    float AdaptationSpeed;
    float MinLum;
    float MaxLum;
};

static const float A = 0.15;
static const float B = 0.50;
static const float C = 0.10;
static const float D = 0.20;
static const float E = 0.02;
static const float F = 0.30;
static const float W = 11.2;

float3 Uncharted2Tonemap(float3 x)
{
    return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

float4 main(float4 pos : SV_POSITION, float2 texCoord : TEXCOORD) : SV_Target
{
    // hdr
    float3 hdrColor = HDRTexture.Sample(Sampler, texCoord).rgb;
    
    // read min max avg
    float avgLum = exp(AvgLuminance.Sample(Sampler, float2(0.5, 0.5)).r) - 1.0;
    float minLum = MinLuminance.Sample(Sampler, float2(0.5, 0.5)).r;
    float maxLum = MaxLuminance.Sample(Sampler, float2(0.5, 0.5)).r;
    
    // adapt -> exp
    float adaptedLum = avgLum;
    float adaptation = 1.0 - exp(-DeltaTime * AdaptationSpeed);
    adaptedLum = lerp(adaptedLum, avgLum, adaptation);
    
    // expose
    float keyValue = 1.03 - 2.0 / (2.0 + log(adaptedLum + 1.0));
    float exposure = keyValue / clamp(adaptedLum, minLum, maxLum);
    
    
    float3 exposedColor = hdrColor * exposure;
    
    // tone
    float3 curr = Uncharted2Tonemap(exposedColor);
    float3 whiteScale = 1.0 / Uncharted2Tonemap(W);
    float3 result = curr * whiteScale;
    
    // gamma
    result = pow(result, 1.0 / 2.2);
    
    return float4(result, 1.0);
}
