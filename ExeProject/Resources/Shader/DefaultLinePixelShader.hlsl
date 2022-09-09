#include "DefaultShaderInfo.hlsli"

float4 main(DefaultLineVSOutput i) : SV_TARGET
{
	return i.color * color;
}