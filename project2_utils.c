#include <linux/module.h>
#include <linux/random.h>
#include "project2.h"

/**
* @brief Returns a random integer from 0 to (size/size*4) based on size
*
* @param size Seed for defining boundary conditions
*
* @return Random Integer
*/
int project2_get_next_integer(int size)
{
	int retval = get_random_int();

	if (retval < 0)
		retval = -(1 + retval);

	if (size <= 0)
		return retval % 100;

	if (size < INT_MAX / 4)
		retval = retval % (size * 4);
	else
		retval = retval % size;

	return retval;
}

// Module related macros
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Abhishek Chauhan <zxcve@vt.edu>");
MODULE_DESCRIPTION("Project2 for manipulation of rbtree data structures\n");
