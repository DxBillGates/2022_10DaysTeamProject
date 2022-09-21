float3 GetPosition(matrix mat)
{
	return float3(-mat[0][3], -mat[1][3], -mat[2][3]);
}