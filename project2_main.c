#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include "project2.h"

/**
* @brief Argument to control the number of integers to insert
*/
static int dstruct_size __initdata = 5;


/**
* @brief Register dstruct_size as an argument to be taken
*/
module_param(dstruct_size, int, 0);
MODULE_PARM_DESC(dstruct_size, "Number of random numbers to insert in DS");

/**
* @brief List of Handles to be executed
*/
static project2_ds_handle ds_handle[] __initdata = {
	PROJECT2_GENERATE_HANDLE_ARRAY(list),
	PROJECT2_GENERATE_HANDLE_ARRAY(queue),
	PROJECT2_GENERATE_HANDLE_ARRAY(map),
	PROJECT2_GENERATE_HANDLE_ARRAY(rbtree)
};

/**
* @brief Test case executor function
*
* @param handle Handle for the test to be executed
* @param size Number of integers to be inserted
* @param context Context of the test being executed
*
* @return 0 for success or appropriate error codes on failure.
*/
static int execute_test(project2_handle *handle, int size, void *context)
{
	int ret_add = 0;
	int ret_del = 0;

	/* Performs addition of size number of integers */
	ret_add = handle->add(context, size);
	if (ret_add) {
		printk (KERN_INFO "adding elements failed %d\n", ret_add);
	}

	/* Prints the current state of the data structure*/
	handle->iterate(context);

	/* Removes all the integers from the data structure*/
	ret_del = handle->remove(context);
	if (ret_del) {
		printk (KERN_INFO "removing elements failed %d\n", ret_del);
	}

	/* Return error if any of them failed */
	return ret_add | ret_del;
}

/**
* @brief Runs the test by initializing the handles and calling execute.
*
* @param type Type of the test to be run.
* @param size Number of integers to be inserted.
*
* @return 0 for success or appropriate error codes on failure.
*/
static int run_test(project2_ds_type type, int size)
{
	int ret = 0;
	project2_handle *handle = NULL;

	if (type >= PROJECT2_LIST && type <= PROJECT2_RBTREE) {
		do {
			/* Get the handle for the test */
			ret = ds_handle[type].get_handle(&handle);

			/* Break if handle is not initialized successfully */
			if (ret) {
				break;
			}

			printk(KERN_INFO "##################################\n");
			printk(KERN_INFO "Running %s Test Suite\n", ds_handle[type].type);

			/* Initialize the context for the test */
			ret = handle->init(size, &handle->context);

			/* Break if context is not initialized successfully */
			if (ret) {
				break;
			}

			/* Do not Break if execution failed as deinit is needed */
			ret = execute_test(handle, size, handle->context);

			handle->deinit(handle->context);

		} while (0);

		printk(KERN_INFO "##################################\n");

	/* Free Handle if it was initialized */
	if (handle)
		ds_handle[type].free_handle(handle);
	}
	return ret;
}

/**
* @brief Init function for the module
*
* @return 0 for success or appropriate error codes on failure.
*/
static int __init project2_init(void)
{
	project2_ds_type type;

	printk(KERN_INFO "Starting Project2 for %d integers\n", dstruct_size);

	/* Error check for <= 0 size */
	if (dstruct_size <= 0) {
		printk (KERN_INFO "invalid size %d\n", dstruct_size);
		return -EINVAL;
	}

	project2_list_standalone(dstruct_size);

	/* Iterate over all data structures and perform the test
	* Ignore the errors as we want to run all the test-cases.
	*/
	for (type = PROJECT2_LIST; type <= PROJECT2_RBTREE; type++)
		if (run_test(type, dstruct_size)) {
			printk (KERN_INFO "%s test failed\n", ds_handle[type].type);
		}

	return 0;
}

/**
* @brief Exit function for the module/
*/
static void __exit project2_exit(void)
{
	printk(KERN_INFO "Module exiting \n");
}


/**
* @brief Registers init and exit functions.
*/
module_init (project2_init);
module_exit (project2_exit);

// Module related macros
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Abhishek Chauhan <zxcve@vt.edu>");
MODULE_DESCRIPTION("Project2 for manipulation of kernel data structures\n");
