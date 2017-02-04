#include <linux/module.h>
#include <linux/random.h>
#include <linux/slab.h>
#include <linux/kfifo.h>
#include "project2.h"

/**
* @brief Context for map test
*/
typedef struct project2_map_context_t {
	int *data_ptr; /*Data ptr to hold the data inserted*/
	struct idr *map_ptr; /*map pointer for accessing map*/
	int lower_bound;
	int upper_bound;
} project2_map_context;


/**
* @brief Add size number of Random Integers to the map
*
* @param context Context information for the map
* @param size Number of Random Integers to be inserted
*
* @return 0 if successful otherwise appropriate error codes
*/
static int add_map(void *context, int size)
{
	int id = 0;
	int tmp_size = size;
	project2_map_context *map_context = (project2_map_context *) context;

	if (!map_context) {
		printk(KERN_INFO "context to add_map is NULL\n");
		return -EINVAL;
	}

	if (size && !map_context->map_ptr) {
		printk(KERN_INFO "map_ptr in add_map is NULL\n");
		return -EINVAL;
	}

	if (size && !map_context->data_ptr) {
		printk(KERN_INFO "data_ptr in add_map is NULL\n");
		return -EINVAL;
	}

	while(size--) {

		map_context->data_ptr[size] = project2_get_next_integer(tmp_size);

		idr_preload(GFP_KERNEL);

		id = idr_alloc(map_context->map_ptr,
						&(map_context->data_ptr[size]),
						map_context->lower_bound,
						map_context->upper_bound, GFP_KERNEL);

		idr_preload_end();

		if (id < 0) {
			if(id == -ENOSPC) {
				printk(KERN_INFO "No space in the range\n");
			} else if(id == -ENOMEM) {
				printk(KERN_INFO "Memory allocation failure for the UID\n");
			} else {
				printk(KERN_INFO "UID Allocation failed\n");
			}
			return id;
		}
		printk(KERN_INFO "MAP_ADD<id,value>: <%d, %d>\n", id, map_context->data_ptr[size]);
	}

	printk(KERN_INFO "\n");

	return 0;
}

/**
* @brief Prints the contents of the map
*
* @param context Context of the map
*/
static void show_map(void *context)
{
	project2_map_context *map_context = (project2_map_context *) context;
	int *curr = NULL;
	int id = 0;

	if (!map_context) {
		printk(KERN_INFO "context to show_map is NULL\n");
		return;
	}

	if (!map_context->map_ptr) {
		printk(KERN_INFO "map_ptr in show_map is NULL\n");
		return;
	}

	idr_for_each_entry(map_context->map_ptr, curr, id)
		printk(KERN_INFO "MAP_SHOW<id,value>: <%d, %d>\n", id, *curr);

	printk(KERN_INFO "\n");
}

/**
* @brief Destroys the entire map
*
* @param context Context of the map
*
* @return 0 for success and appropriate error codes on failure
*/
static int remove_map(void *context)
{
	project2_map_context *map_context = (project2_map_context *) context;

	if (!context) {
		printk(KERN_INFO "context to remove_map is NULL\n");
		return -EINVAL;
	}

	if (map_context->map_ptr) {
		idr_destroy(map_context->map_ptr);
		printk(KERN_INFO "Destroyed entire map\n");
	}

	show_map(context);

	return 0;
}

/**
* @brief Deallocates the context
*
* @param context Context for the map
*/
static void deinit_map(void *context)
{
	project2_map_context *map_context = (project2_map_context *) context;

	if (context && map_context->map_ptr) {
		kfree(map_context->map_ptr);
	}

	if (context && map_context->data_ptr)
		kfree(map_context->data_ptr);

	if (context)
		kfree(context);
}

/**
* @brief Initializes the context by adding map_ptr & data_ptr
*
* @param size Numbers of the random Integers to be inserted.
* @param context Context to be initialized.
*
* @return 0 for success, otherwise appropriate error code.
*/
static int init_map(int size, void **context)
{
	project2_map_context *map_context =
				kmalloc(sizeof(project2_map_context), GFP_KERNEL);

	if (!map_context) {
		printk (KERN_INFO "memory allocation for map head failed\n");
		return -ENOMEM;
	}

	map_context->map_ptr = NULL;
	map_context->data_ptr = NULL;

	map_context->lower_bound = 0;
	map_context->upper_bound = size;

	printk( KERN_INFO "Using range for id as [%d, %d)", 0, size);

	map_context->data_ptr = kmalloc(sizeof(int) * size, GFP_KERNEL);

	if (map_context->data_ptr == NULL) {
		printk (KERN_INFO "memory allocation for map data buffer failed\n");
		deinit_map(map_context);
		return -ENOMEM;
	}

	map_context->map_ptr = kmalloc(sizeof(struct idr), GFP_KERNEL);

	if (map_context->map_ptr == NULL) {
		printk (KERN_INFO "memory allocation for map head failed\n");
		deinit_map(map_context);
		return -ENOMEM;
	}

	idr_init(map_context->map_ptr);

	*context = map_context;

	return 0;
}

// Generates the handles for the map test-case
PROJECT2_GENERATE_HANDLE(map);

// Module related macros
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Abhishek Chauhan <zxcve@vt.edu>");
MODULE_DESCRIPTION("Project2 for manipulation of map data structures\n");
