#pragma once

#include <cstdint>
#include <cstring>
#include <cmath>
#include <ios>
#include <string>
#include <sstream>

#include <akl/Price.hpp>

#include <KT01/SecDefs/CmeSecMasterV2.hpp>
#include <KT01/SecDefs/CfeSecMaster.hpp>

namespace akl::stacker
{

class PriceConverter
{
public:
	PriceConverter(const KT01::SECDEF::CME::CmeSecDef &instr)
	: Symbol(instr.symbol)
	, tickScale(GetTickScale(instr))
	, tickDenom(instr.mainFraction)
	, tickSubDenom(instr.subFraction)
	, numDecimals(countDecimals(instr))
	, tickIncrement(Price::FromUnshifted(instr.mintick))
	, useDecimal(tickDenom <= 1 || !isPowerOfTwo(tickDenom) || instr.exch != "CME")
	{

	}

	PriceConverter(const KT01::SECDEF::CFE::CfeSecDef &instr)
	: Symbol(instr.symbol)
	, tickScale(-2)
	, tickDenom(1)
	, tickSubDenom(1)
	, numDecimals(2)
	, tickIncrement(Price::FromUnshifted(instr.mintick))
	, useDecimal(true)
	{

	}

	std::string DisplayPrice(const Price px) const
	{
		if (useDecimal)
		{
			return decimalDisplayPrice(px);
		}
		else
		{
			return fractionalDisplayPrice(px);
		}
	}

	bool TryParse(const char *str, Price &px) const
	{
		if (useDecimal)
		{
			char copy[9];
			strncpy(copy, str, sizeof(copy) - 1);
			copy[8] = '\0';
			double doublePx = atof(copy);
			if (doublePx != 0)
			{
				doublePx /= pow(10, tickScale);
				px = Price::FromUnshifted(doublePx);
				px = Price::FromShifted(px.AsShifted() / tickIncrement.AsShifted() * tickIncrement.AsShifted());
				return true;
			}
			else
			{
				size_t i = 0;
				while (i < sizeof(copy) && copy[i] != '\0')
				{
					switch (copy[i])
					{
						case '0':
						case ',':
						case '.':
						case '-':
							break;
						default:
							return false;
					}
					++i;
				}
				px = Price::FromShifted(0);
				return true;
			}
		}
		else
		{
			char copy[16];
			size_t i = 0;
			size_t j = 0;
			bool tickFound = false;
			bool numberFound = false;
			while (i < sizeof(copy) - 1 && str[i] != '\0')
			{
				switch (str[i])
				{
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9':
						copy[j++] = str[i];
						numberFound = true;
						break;
					case '\'':
						if (!tickFound)
						{
							tickFound = true;
							copy[j++] = str[i];
						}
						break;
					case '-':
						if (j == 0)
						{
							copy[j++] = str[i];
						}
						break;
					default:
						// ignore other characters
						break;
				}
				++i;
			}

			if (!numberFound)
			{
				return false;
			}

			copy[j] = '\0';

			j = 0;
			const bool negative = copy[j] == '-';
			if (negative)
			{
				++j;
			}

			size_t tickIndex = j;
			while (copy[tickIndex] != '\0')
			{
				if (copy[tickIndex] == '\'')
				{
					break;
				}
				++tickIndex;
			}

			if (copy[tickIndex] == '\'')
			{
				copy[tickIndex] = '\0';
				double integerPart = atof(copy + j);

				constexpr int MAX = 99999999;
				if (integerPart > MAX)
				{
					integerPart = MAX;
				}

				double firstFraction = 0;
				double secondFraction = 0;
				
				if (tickIndex < sizeof(copy) - 1)
				{
					size_t fractionIndex = tickIndex + 1;

					const int firstDenom = tickDenom;
					
					int len;
					if (firstDenom == 8)
					{
						len = 1;
					}
					else
					{
						len = 2;
					}

					firstFraction = ParseDigits(copy + fractionIndex, len);

					if (fractionIndex + len < sizeof(copy))
					{
						secondFraction = ParseDigits(copy + fractionIndex + len, 1);
					}

					if (secondFraction == 2 || secondFraction == 7)
					{
						secondFraction += 0.5;
					}
					else if (secondFraction == 1 || secondFraction == 6)
					{
						secondFraction += 0.25;
					}
					else if (secondFraction == 3 || secondFraction == 8)
					{
						secondFraction += 0.75;
					}
				}

				double doublePx = secondFraction / 10.0;
				doublePx += firstFraction;
				doublePx /= tickDenom;
				doublePx += integerPart;

				px = Price::FromUnshifted(doublePx);
				px = Price::FromShifted(px.AsShifted() / tickIncrement.AsShifted() * tickIncrement.AsShifted());
				if (negative)
				{
					px = -px;
				}
				return true;
			}
			else
			{
				double doublePx = atof(copy + j);
				constexpr int MAX = 99999999;
				if (doublePx > MAX)
				{
					doublePx = MAX;
				}
				px = Price::FromUnshifted(doublePx);
				if (negative)
				{
					px = -px;
				}
				return true;
			}

			return false;
		}
	}

	const std::string Symbol;

	static int GetTickScale(const KT01::SECDEF::CME::CmeSecDef &instr)
	{
		double displayFactor = instr.displayFactor;
		if (!std::isnan(displayFactor) && displayFactor > 0 && displayFactor < 1)
		{
			int tickScale = 0;
			while (static_cast<int>(displayFactor + 0.5) < 1)
			{
				tickScale -= 1;
				displayFactor *= 10;
			}
			return tickScale;
		}
		else
		{
			return 0;
		}
	}

	int ParseDigits(const char *str, int numDigits) const
	{
		int num = 0;
		int i = 0;
		while (str[i] != '\0' && i < numDigits)
		{
			num *= 10;
			switch (str[i])
			{
				case '0':
					break;
				case '1':
					num += 1;
					break;
				case '2':
					num += 2;
					break;
				case '3':
					num += 3;
					break;
				case '4':
					num += 4;
					break;
				case '5':
					num += 5;
					break;
				case '6':
					num += 6;
					break;
				case '7':
					num += 7;
					break;
				case '8':
					num += 8;
					break;
				case '9':
					num += 9;
					break;
				default:
					i = numDigits;
					break;
			}
			++i;
		}
		return num;
	}

private:
	static constexpr double Multiplier { Price::Multiplier };
	const int tickScale;
	const int tickDenom;
	const int tickSubDenom;
	const int numDecimals;
	const Price tickIncrement;
	const bool useDecimal;

	static int countDecimals(const KT01::SECDEF::CME::CmeSecDef &instr)
	{
		int64_t tickDenom = instr.mainFraction * instr.subFraction;
		if (tickDenom >= 0)
		{
			int numDecimals = 0;
			double tickSize = 1.0 / static_cast<double>(tickDenom);
			tickSize -= static_cast<int64_t>(tickSize);
			while (tickSize > 0 && numDecimals < 8)
			{
				++numDecimals;
				tickSize *= 10;
				tickSize -= (int)tickSize;
			}
			return numDecimals;
		}
		else
		{
			return 0;
		}
	}

	static constexpr bool isPowerOfTwo(int n)
	{
		return n > 0 && (n & (n - 1)) == 0;
	}

	std::string decimalDisplayPrice(const Price px) const
	{
		double doublePx = px.AsShifted() * (double)pow(10, tickScale) / Multiplier;
		
		std::stringstream ss;
		if (tickScale < 0)
		{
			
			ss.precision(-tickScale);
		}
		else
		{
			ss.precision(numDecimals);
		}

		ss << std::fixed << doublePx;
		return ss.str();
	}

	std::string fractionalDisplayPrice(Price px) const
	{
		const int firstDenom = tickDenom;
		const int secondDenom = tickSubDenom;

		std::stringstream ss;

		if (px < Price::FromShifted(0))
		{
			px = -px;
			ss << "-";
		}
		double doublePx = px.AsUnshifted();
		int integerPart = static_cast<int>(doublePx);
		ss << integerPart << '\'';
		doublePx -= integerPart;

		doublePx *= firstDenom;

		int firstFraction = static_cast<int>(doublePx);
		if (firstDenom == 8)
		{
			ss << firstFraction;
		}
		else
		{
			if (firstFraction < 10)
			{
				ss << '0' << firstFraction;
			}
			else
			{
				ss << firstFraction;
			}
		}

		if (secondDenom > 1)
		{
			doublePx -= firstFraction;
			doublePx *= 10;
			int secondFraction = static_cast<int>(doublePx);
			ss << secondFraction;
		}

		return ss.str();
	}
};

}