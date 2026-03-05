/*
 * Helpers.hpp
 *
 *  Created on: May 24, 2021
 *      Author: centos
 */

#ifndef SRC_BOE3_HELPERS_HPP_
#define SRC_BOE3_HELPERS_HPP_

#include <stdio.h>
#include <algorithm>
#include <iostream>
#include <string>

using namespace std;
namespace KTN
{
	namespace CFE
	{
		namespace BOE3
		{
			class Helpers
			{
			public:
				inline static void hexdump(void *ptr, int buflen)
				{
					unsigned char *buf = (unsigned char *)ptr;
					int i, j;
					for (i = 0; i < buflen; i += 16)
					{
						printf("%06x: ", i);
						for (j = 0; j < 16; j++)
							if (i + j < buflen)
								printf("%02x ", buf[i + j]);
							else
								printf("   ");
						printf(" ");
						for (j = 0; j < 16; j++)
							if (i + j < buflen)
								printf("%c", isprint(buf[i + j]) ? buf[i + j] : '.');
						printf("\n");
					}
				}

				// convert the order id ... etc to base36 string
				inline static string ConvertFrom10(long value, int base)
				{
					if (base < 2 || base > 36)
						return "0";

					bool isNegative = (value < 0);
					if (isNegative)
						value *= -1;

					// NOTE: it's probably possible to reserve string based on value
					string output;

					do
					{
						char digit = value % base;

						// Convert to appropriate base character
						// 0-9
						if (digit < 10)
							digit += '0';
						// A-Z
						else
							digit = digit + 'A' - 10;

						// Append digit to string (in reverse order)
						output += digit;

						value /= base;

					} while (value > 0);

					if (isNegative)
						output += '-';

					// Reverse the string - NOTE: could be done with std::reverse
					reverse(output.begin(), output.end());
					// cout << value << " TO BASE " << base << ": " << output << endl;
					return output;
				}
			};

		} // namespace BOE3
	} // namespace CFE
} // namespace KTN


#endif /* SRC_BOE3_HELPERS_HPP_ */
