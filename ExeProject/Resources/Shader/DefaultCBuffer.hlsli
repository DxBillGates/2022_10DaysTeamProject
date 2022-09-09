cbuffer ModelInfo : register(b0)
{
	matrix modelMatrix;
}

cbuffer CameraInfo : register(b1)
{
	matrix viewMatrix;
	matrix projMatrix;
	matrix billboard;
	float4 cameraPos;
	float4 cameraDir;
}

cbuffer Material : register(b2)
{
	float4 ambient;
	float4 diffuse;
	float4 specular;
	float4 color;
}

cbuffer DirectionalLightInfo : register(b3)
{
	float4 worldLightDir;
	float4 worldLightColor;
}