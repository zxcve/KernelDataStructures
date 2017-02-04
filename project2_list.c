#include <linux/module.h>
#include <linux/list.h>
#include <linux/slab.h>
#include "project2.h"

/**
* @brief Node encapsulating link list.
*/
typedef struct project2_list_t {
	int data;
	struct list_head list;
} project2_list;


/**
* @brief Helper API to perform insertion in the list.
*
* @param head Head of the list.
* @param data Data which is to be inserted.
*
* @return 0 for success and appropriate error codes for failure
*/
static int __add_list(struct list_head *head, int data)
{
	project2_list *tmp;

	if (head == NULL) {
		printk (KERN_INFO "NULL head passed to add helper\n");
		return -EINVAL;
	}

	tmp  = kmalloc(sizeof(project2_list), GFP_KERNEL);

	if (tmp == NULL) {
		printk (KERN_INFO "memory allocation for list addition failed\n");
		return -ENOMEM;
	}

	tmp->data = data;

	list_add_tail(&tmp->list, head);

	printk(KERN_INFO "LIST_ADD: %d\n", tmp->data);

	return 0;
}

/**
* @brief Add size number of random numbers to the list
*
* @param context Context information for the list
* @param size Number of Random Integers to be inserted
*
* @return 0 if successful otherwise appropriate error codes
*/
static int add_list(void *context, int size)
{
	int data;
	struct list_head *head = context;
	int ret = 0;
	int tmp_size = size;

	if (!context) {
		printk(KERN_INFO "context to add_list is NULL\n");
		return -EINVAL;
	}

	while (tmp_size--) {

		data = project2_get_next_integer(size);

		ret = __add_list(head, data);

		if (ret)
			break;
	}

	printk(KERN_INFO "\n");

	return ret;
}

/**
* @brief Prints the contents of the list from head
*
* @param context Context of the list
*/
static void show_list(void *context)
{
	struct list_head *head = context;
	project2_list *tmp;

	if (!context) {
		printk(KERN_INFO "context to show_list is NULL\n");
		return;
	}

	if (list_empty(head))
		return;

	list_for_each_entry(tmp, head, list) {
		printk(KERN_INFO "LIST_SHOW: %d\n", tmp->data);
	}

	printk(KERN_INFO "\n");
}

/**
* @brief Removes the entire list.
*
* @param context Context of the list.
*
* @return 0 for success and appropriate error codes on failure
*/
static int remove_list(void *context)
{
	struct list_head *head = context;
	project2_list *curr;
	project2_list *next;

	if (!context) {
		printk(KERN_INFO "context to remove_list is NULL\n");
		return -EINVAL;
	}

	if (list_empty(head))
		return 0;

	list_for_each_entry_safe(curr, next, head, list)
	{
		list_del(&curr->list);
		printk(KERN_INFO "LIST_DEL: %d\n", curr->data);
		kfree (curr);
	}

	show_list(context);

	printk(KERN_INFO "\n");

	return 0;
}

/**
* @brief Initializes the context by adding a head for the test
*
* @param size Numbers of the random Integers to be inserted.
* @param context Context to be initialized.
*
* @return 0 for success, otherwise appropriate error code.
*/
static int init_list (int size, void **context)
{
	struct list_head *head = kmalloc(sizeof(struct list_head), GFP_KERNEL);

	if (head == NULL) {
		printk (KERN_INFO "memory allocation for list head failed\n");
		return -ENOMEM;
	}

	INIT_LIST_HEAD(head);

	*context = head;

	return 0;
}

/**
* @brief Deallocates the context
*
* @param context Context for the List
*/
static void deinit_list(void *context)
{
	if (context)
		kfree(context);
}

/**
* @brief Executes all list functions in 1 function
*
* @param size Number of integers to be inserted
*
* @return 0 for success or appropriate error code on failure.
*/
int project2_list_standalone(int size)
{
	LIST_HEAD(my_head);
	int tmp_size = size;
	int data;
	project2_list *tmp;
	project2_list *next;

	printk(KERN_INFO "##################################\n");
	printk(KERN_INFO "Running single function list test\n");

	while (tmp_size--) {

		data = project2_get_next_integer(size);

		tmp  = kmalloc(sizeof(project2_list), GFP_KERNEL);

		if (tmp == NULL) {
			printk (KERN_INFO "memory allocation for list addition failed\n");
			return -ENOMEM;
		}

		tmp->data = data;

		list_add_tail(&tmp->list, &my_head);

		printk(KERN_INFO "LIST1_ADD: %d\n", tmp->data);
	}

	printk(KERN_INFO "\n");

	list_for_each_entry(tmp, &my_head, list) {
		printk(KERN_INFO "LIST1_SHOW: %d\n", tmp->data);
	}

	printk(KERN_INFO "\n");

	list_for_each_entry_safe(tmp, next, &my_head, list)
	{
		list_del(&tmp->list);
		printk(KERN_INFO "LIST1_DEL: %d\n", tmp->data);
		kfree (tmp);
	}

	printk(KERN_INFO "\n");
	printk(KERN_INFO "##################################\n");

	return 0;
}

// Generates the handles for the list test-case
PROJECT2_GENERATE_HANDLE(list);

// Module related macros
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Abhishek Chauhan <zxcve@vt.edu>");
MODULE_DESCRIPTION("Project2 for manipulation of list data structures\n");
