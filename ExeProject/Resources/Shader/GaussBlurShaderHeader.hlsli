#include "DefaultShaderInfo.hlsli"

cbuffer GaussFilterData : register(b4)
{
	float4 data[16];
}