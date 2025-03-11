struct PointLight
{
    float4 Position; // В мировых координатах
    float4 Color;
    float Intensity;
    
    //доавбляем выравнивание
    float3 Padding;
    
};

cbuffer LightBuffer : register(b2)
{
    PointLight Lights[3];
};

struct PS_INPUT
{
    float4 sv_position : SV_POSITION;
    float4 color : COLOR; // Порядок должен совпадать с VS_OUTPUT!
    float2 uv : TEXCOORD0;
    float3 world_position : TEXCOORD1;
};


float4 CalculateLighting(PS_INPUT input, PointLight light)
{
    float3 lightDir = normalize(light.Position.xyz - input.world_position);
    float distance = length(light.Position.xyz - input.world_position);
    
    // Уменьшенное затухание для большей зоны влияния
    float attenuation = 1.0 / (distance * 0.5 + 1.0);
    
    float3 lightColor = light.Color.rgb * light.Intensity * attenuation;
    return float4(lightColor * input.color.rgb, input.color.a);
}

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 finalColor = float4(0, 0, 0, input.color.a);
    for (int i = 0; i < 3; i++)
    {
        finalColor += CalculateLighting(input, Lights[i]);
    }
    return finalColor;
}