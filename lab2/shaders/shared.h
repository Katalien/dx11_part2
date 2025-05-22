#ifdef __cplusplus
#include "Dev.h"
using float2 = DirectX::XMFLOAT2;
using float3 = DirectX::XMFLOAT3;
using float4 = DirectX::XMFLOAT4;
using float4x4 = DirectX::XMFLOAT4X4;

using uint =  uint32_t;
using uint2 = DirectX::XMUINT2;
using uint3 = DirectX::XMUINT3;
using uint4 = DirectX::XMUINT4;

#define SHADER_CB_SLOT(val) val
#define SHADER_SRV_SLOT(val) val
#define SHADER_SAMPLER_SLOT(val) val

#else
	struct VSDefaultIn {
		float3 pos : POSITION;
		float3 normal : NORMAL;
		float2 texCoord : TEXCOORD;
	};

#define SHADER_CB_SLOT(val) b ## val
#define SHADER_SRV_SLOT(val) t ## val
#define SHADER_SAMPLER_SLOT(val) s ## val

#endif

#define PER_FRAME_CB_SLOT SHADER_CB_SLOT(0)
#define PER_MESH_CB_SLOT SHADER_CB_SLOT(1)
#define PER_MATERIAL_CB_SLOT SHADER_CB_SLOT(2)
#define TONEMAPPING_CB_SLOT SHADER_CB_SLOT(3)
#define LIGHTS_CB_SLOT SHADER_CB_SLOT(4)

#define TEXTURE_COLOR_SRV_SLOT SHADER_SRV_SLOT(0)
#define TEXTURE_AMBIENT_SRV_SLOT SHADER_SRV_SLOT(1)
#define TEXTURE_SPECULAR_SRV_SLOT SHADER_SRV_SLOT(2)

#define LINEAR_SAMPLER_SLOT SHADER_SAMPLER_SLOT(0)
#define NEAREST_SAMPLER_SLOT SHADER_SAMPLER_SLOT(1)

#define POINT_LIGHTS_MAX_AMOUNT 16
#define DIR_LIGHTS_MAX_AMOUNT 16

#define SHADE_MODEL_COLOR 0
#define SHADE_MODEL_PHONG 1
#define SHADE_MODEL_PBR 2
#ifdef __cplusplus
#pragma pack(push, 1)
#endif
struct TriangleData
{
	float3 color1;
	float _dummy1;
	float3 color2;
	float _dummy2;
	float3 color3;
	float _dummy3;
};

struct PerMaterialData
{
	uint4 usedTextures;
	float4 color;
	float3 ambient;
	float ph;
	float3 diffuse;
	uint shadeModel;
	float3 specular;
	float _padding1;
};

struct PerFrameData
{
	float4x4 viewProj;
	float3 cameraPos;
	uint _padding;
};

struct PerMeshData
{
	float4x4 transform;
};

struct ToneMappingCB
{
	float avgBrightnessLog;
	float sceneExposure;
	uint2 _padding;
};

struct PointLightData
{
	float3 color;
	float radius;

	float3 position;
	uint _padding;
};

struct DirLightData
{
	float3 color;
	uint _padding0;

	float3 direction;
	uint _padding1;
};

struct LightsData {
	PointLightData pointLights[POINT_LIGHTS_MAX_AMOUNT];
	DirLightData dirLights[DIR_LIGHTS_MAX_AMOUNT];
	uint pointAmount;
	uint dirAmount;
	uint2 _padding;
};

#ifdef __cplusplus
#pragma pack(pop)
#endif
