Texture2D HDRTexture : register(t0);
SamplerState Sampler : register(s0);

static const float A = 0.15f;
static const float B = 0.50f;
static const float C = 0.10f;
static const float D = 0.20f;
static const float E = 0.02f;
static const float F = 0.30f;
static const float W = 11.2f;

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};

// Функция тонального отображения Uncharted 2
float3 Uncharted2Tonemap(float3 x)
{
    return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

static const float WhiteLumen = 11.2f;
static const float lumMin = 0.1;
static const float lumMax = 100.0;

float getExposition(float avgLuminance)
{
    float keyValue = 1.03 - 2.0 / (2.0 + log2(avgLuminance + 1));
    return keyValue / clamp(avgLuminance, lumMin, lumMax);
}

float4 main(PS_INPUT input) : SV_Target
{
    float3 hdrColor = HDRTexture.Sample(Sampler, input.Tex).rgb;

    // Получите среднюю яркость из вычислений (например, через отдельный буфер)
    float avgLuminance = 1.0; // Замените на реальное значение
    float E = getExposition(avgLuminance);
    
    float3 curr = Uncharted2Tonemap(hdrColor * E);
    float3 whiteScale = 1.0f / Uncharted2Tonemap(WhiteLumen);
    float3 result = curr * whiteScale;

    result = pow(result, 1.0f / 2.2f);
    return float4(result, 1.0f);
}