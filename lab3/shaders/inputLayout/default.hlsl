// Fake shader for generating input layout
#include "shaders/shared.h"

float4 VSMain(VSDefaultIn input) : SV_POSITION {
	return float4(0, 0, 0, 1);
}