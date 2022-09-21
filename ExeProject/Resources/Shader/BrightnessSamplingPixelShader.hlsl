#include "DefaultShaderInfo.hlsli"

Texture2D<float4> tex : register(t0);

cbuffer SamplingInfo : register(b4)
{
	float value;
}

float4 main(DefaultSpriteVSOutput input) : SV_TARGET
{
	float4 texColor = tex.Sample(clampPointSampler,input.uv);

	// グレイスケールに変換
	float greyScale = (texColor.r * 0.299f + texColor.g * 0.587f + texColor.b * 0.114f);
	// グレイスケールからサンプリング以上の値を取得 0 or しきい値以上
	float diff = step(value,greyScale);

	return float4(texColor.rgb * diff, 1.0f);
}