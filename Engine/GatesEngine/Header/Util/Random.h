#pragma once
#include <random>
namespace GE
{
	class RandomMaker
	{
	private:
		static std::mt19937_64 rnd;
	public:
		static void ResetSeed();
		static void SetSeed(int seed);

		static int GetInt(int minValue , int maxValue);
		static float GetFloat(float minValue, float maxValue);
	};
}