#include "Randomizer.hpp"

namespace akl::stacker
{

Randomizer::Randomizer()
: gen(rd())
{

}

int Randomizer::GetRandomInt(int base, int variance)
{
	if (variance > 0)
	{
		std::uniform_int_distribution<> dis(base - variance, base + variance);
		const int value = dis(gen);
		return std::max(0, value);
	}
	else
	{
		return base;
	}
}

}