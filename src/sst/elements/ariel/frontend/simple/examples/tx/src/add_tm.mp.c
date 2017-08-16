#include <stdio.h>
#include <stdint.h>
#include "/home/chughes/sst/src/sst-elements/src/sst/elements/ariel/api/arielapi.h"

#define SIZE 10
#define X_VAL 10000

int main()
{
	volatile static uint32_t testVal0 = 0;
	volatile static uint16_t testVal1 = 0;
	volatile static uint16_t testValA[2];

	uint32_t i;
	ariel_enable();

        #pragma omp parallel for
        for(i = 0; i < 2; ++i)
	{
		txBegin();
		__asm__ __volatile__("SET_VAL_:");
		testVal0 = 16;
		testVal1 = 32;

		testValA[0] = testVal0;
		testValA[1] = 8;
		__asm__ __volatile__("END_SET_VAL_:");
		txEnd();
	}

	testVal0 = testVal0 + testVal1 + testValA[0] + testValA[1];


	return 0;
}

