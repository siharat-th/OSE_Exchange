#pragma once

#include <random>

namespace akl::stacker
{

class Randomizer
{
public:
	Randomizer();
	int GetRandomInt(int base, int variance);

private:
	std::random_device rd;
	std::mt19937 gen;
};

}