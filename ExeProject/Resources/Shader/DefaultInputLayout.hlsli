struct DefaultMeshVSInput
{
	float4 pos : POSITION;
	float2 uv : UV;
	float3 normal : NORMAL;
};

struct DefaultMeshVSOutput
{
	float4 svpos : SV_POSITION;
	float2 uv : UV;
	float3 normal : NORMAL;
	float4 worldPosition : WORLD_POSITION;
};

struct DefaultSpriteVSInput
{
	float4 pos : POSITION;
	float2 uv : UV;
};

struct DefaultSpriteVSOutput
{
	float4 svpos : SV_POSITION;
	float2 uv : UV;
};

struct DefaultLineVSInput
{
	float4 pos : POSITION;
	float4 color : COLOR;
};

struct DefaultLineVSOutput
{
	float4 svpos : SV_POSITION;
	float4 pos : POSITION;
	float4 color : COLOR;
};