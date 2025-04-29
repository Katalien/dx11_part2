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

float4 main(PS_INPUT input) : SV_Target
{
    // Чтение HDR-цвета из текстуры
    float3 hdrColor = HDRTexture.Sample(Sampler, input.Tex).rgb;

    // Применение тональной кривой
    float3 curr = Uncharted2Tonemap(hdrColor * 2.0f);
    
    // Коррекция белого уровня
    float3 whiteScale = 1.0f / Uncharted2Tonemap(float3(W, W, W));
    float3 result = curr * whiteScale;

    // Гамма-коррекция
    result = pow(result, 1.0f / 2.2f);

    return float4(result, 1.0f);
}