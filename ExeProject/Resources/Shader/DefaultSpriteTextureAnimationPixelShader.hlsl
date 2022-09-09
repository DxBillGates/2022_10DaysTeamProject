#include "DefaultShaderInfo.hlsli"

Texture2D<float4> tex : register(t0);

cbuffer TextureAnimationInfo : register(b4)
{
	// �؂蔲���T�C�Y
	float2 clipSize;
	// �؂蔲���T�C�Y�ł̐؂蔲���ʒu(0�Ȃ獶��)
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