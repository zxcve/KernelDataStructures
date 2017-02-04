#include <linux/module.h>
#include <linux/random.h>
#include <linux/slab.h>
#include <linux/kfifo.h>
#include "project2.h"


/**
* @brief Returns the ceiling power 2 for the given size
*
* @param size Size to be processed
*
* @return Ceiling power 2 for the input size
*/
static int __get_queue_size(int size)
{
	int ret_size = 1;

	if (size == 1) {
		printk("Changed size from %d to %d\n", size, ret_size);
		return 2;
	}

	if (size > INT_MAX / 2)
		return -EINVAL;

	while (size > ret_size)
		ret_size *= 2;

	printk("Changed size from %d to %d\n", size, ret_size);
	return ret_size;
}

/**
* @brief Add size number of random numbers to the queue
*
* @param context Context information for the queue
* @param size Number of Random Integers to be inserted
*
* @return 0 if successful otherwise appropriate error codes
*/
static int add_queue(void *context, int size)
{
	int data;
	int ret = 0;
	struct kfifo *my_queue = (struct kfifo *)context;
	int tmp_size = size;

	if (!context) {
		printk(KERN_INFO "context to enqueue is NULL\n");
		return -EINVAL;
	}

	while (tmp_size--) {

		data = project2_get_next_integer(size);

		ret = kfifo_in(my_queue, &data, sizeof(int));

		if (ret != sizeof(int)) {
			printk(KERN_INFO "enqueue failed due to less space\n");
			return -ENOMEM;
		}

		printk(KERN_INFO "ENQUEUE: %d\n", data);
	}

	printk(KERN_INFO "\n");

	return 0;
}

/**
* @brief Prints the contents of the queue
*
* @param context Context of the queue
*/
static void show_queue(void *context)
{
	// not implemented
}

/**
* @brief Dequeues the entire queue.
*
* @param context Context of the queue.
*
* @return 0 for success and appropriate error codes on failure
*/
static int remove_queue(void *context)
{
	int data;
	int ret = 0;
	struct kfifo *my_queue = (struct kfifo *)context;


	if (!context) {
		printk(KERN_INFO "context to dequeue is NULL\n");
		return -EINVAL;
	}

	while (!kfifo_is_empty(my_queue)) {

		ret = kfifo_out(my_queue, &data, sizeof(int));

		if (ret != sizeof(int)) {
			printk(KERN_INFO "dequeue failed due to less space in queue\n");
			return -ENOMEM;
		}

		printk(KERN_INFO "DEQUEUE: %d\n", data);
	}

	printk(KERN_INFO "\n");

	return 0;
}

/**
* @brief Initializes the context by adding a queue object for the test
*
* @param size Numbers of the random Integers to be inserted.
* @param context Context to be initialized.
*
* @return 0 for success, otherwise appropriate error code.
*/
static int init_queue(int size, void **context)
{
	// Get the ceiling power 2 for the size.
	int qsize = __get_queue_size(size);
	struct kfifo *my_queue;

	if (qsize < 0) {
		printk (KERN_INFO "integer overflow while getting power of 2\n");
		return -EINVAL;
	}

	my_queue = kmalloc(sizeof(struct kfifo), GFP_KERNEL);
	if (my_queue == NULL) {
		printk (KERN_INFO "memory allocation for queue kmalloc failed\n");
		return -ENOMEM;
	}


	if (kfifo_alloc(my_queue, sizeof(int) * qsize, GFP_KERNEL)) {
		kfree(my_queue);
		printk (KERN_INFO "memory allocation for queue kfifo_alloc failed\n");
		return -ENOMEM;
	}

	*context = my_queue;
	return 0;
}

/**
* @brief Deallocates the context
*
* @param context Context for the queue
*/
static void deinit_queue(void *context)
{
	struct kfifo *my_queue = (struct kfifo *)(context);

	if (my_queue) {
		kfifo_free(my_queue);
		kfree(my_queue);
	}
}

// Generates the handles for the queue test-case
PROJECT2_GENERATE_HANDLE(queue);

// Module related macros
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Abhishek Chauhan <zxcve@vt.edu>");
MODULE_DESCRIPTION("Project2 for manipulation of queue data structures\n");
