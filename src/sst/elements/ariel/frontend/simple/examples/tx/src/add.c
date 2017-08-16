#include <stdio.h>
#include <stdint.h>
#include "/home/chughes/sst/src/sst-elements/src/sst/elements/ariel/api/arielapi.h"

#define SIZE 10
#define X_VAL 10000

int main()
{
	int32_t counter, temp;
	static int32_t acc[SIZE];

	ariel_enable();

	txBegin();
	for(temp = 0; temp < X_VAL; temp++)
	{
		for(counter = 0; counter < SIZE - 1; counter++)
		{
//			acc[counter] = 42;

//			acc[counter + 1] = 11;

			__asm__ __volatile__("SET_VAL_:");
			acc[0] = 11;
			__asm__ __volatile__("END_SET_VAL_:");
			//printf("temp %d, counter %d, addr %p\n", temp, counter, &acc[counter]);
		}
	}
	txEnd();



	return 0;
}

