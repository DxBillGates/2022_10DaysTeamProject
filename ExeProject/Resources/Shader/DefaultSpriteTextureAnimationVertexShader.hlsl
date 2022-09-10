#include "DefaultShaderInfo.hlsli"

cbuffer TextureAnimationInfo : register(b4)
{
	// 切り抜きサイズ
	float2 clipSize;
	// 切り抜きサイズでの切り抜き位置(0なら左上)
	float2 pivot;
	// 元テクスチャサイズ
	float2 textureSize;
};

DefaultSpriteVSOutput main(DefaultSpriteVSInput input)
{
	DefaultSpriteVSOutput output;
	output.svpos = mul(projMatrix, mul(viewMatrix, mul(billboard, mul(modelMatrix, input.pos))));

	float2 ancerPoint = pivot / (textureSize / clipSize);
	float2 size = clipSize / textureSize;
	output.uv = input.uv * size * ancerPoint;
	return output;
}