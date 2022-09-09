#include "DefaultShaderInfo.hlsli"

Texture2D<float4> tex : register(t0);

cbuffer TextureAnimationInfo : register(b4)
{
	// 切り抜きサイズ
	float2 clipSize;
	// 切り抜きサイズでの切り抜き位置(0なら左上)
	float2 pivot;
};

float4 main(DefaultSpriteVSOutput input) : SV_TARGET
{
	float2 textureSize = float2(0,0);
	tex.GetDimensions(textureSize.x, textureSize.y);

	float2 clipUV = clipSize * pivot / textureSize;
	float clipUVValue = clipUV.x + clipUV.y;

	if (clipUVValue <= 0)clipUV = (1, 1);

	float4 texColor = tex.Sample(wrapPointSampler,clipUV + input.uv * clipUV);
	return texColor * color;
}