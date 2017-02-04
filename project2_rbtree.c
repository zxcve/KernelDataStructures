#include <linux/module.h>
#include <linux/slab.h>
#include <linux/rbtree.h>
#include "project2.h"


/**
* @brief Context for Red-Black tree test
*/
typedef struct project2_rbtree_context_t {
	int start; /*Start of the range (INCLUSIVE) */
	int end;  /*End of the range (INCLUSIVE) */
	struct rb_root root; /*Root for the Red-Black tree */
} project2_rbtree_context;


/**
* @brief Red-Black tree node
*/
typedef struct my_rbnode_t {
	int value; /*Value to be stored */
	struct rb_node rbnode; /*Holds Meta-Data for Red-Black Node */
} my_rbnode;

/**
* @brief Helper API to perform insertion in Red-Black Tree.
*
* @param root Root for the Red-Black Tree
* @param entry Node which has to be inserted
*
* @return 0 for success and appropriate error codes for failure
*/
static int __add_rbtree_node(struct rb_root *root, my_rbnode *entry)
{
	struct rb_node **link = &root->rb_node;
	struct rb_node *parent = NULL;
	my_rbnode *myentry;

	if (root == NULL || entry == NULL) {
		printk("Invalid arguments passed to add helper routine\n");
		return -EINVAL;
	}

	while (*link) {
		parent = *link;
		myentry = rb_entry(parent, my_rbnode, rbnode);
		if (myentry->value > entry->value)
			link = &(*link)->rb_left;
		else if (myentry->value < entry->value)
			link = &(*link)->rb_right;
		else {
			//Node with same value already exists
			return -EEXIST;
		}
	}
	rb_link_node(&entry->rbnode, parent, link);
	rb_insert_color(&entry->rbnode, root);
	return 0;
}


/**
* @brief Helper function to find the node in the Red-Black Tree
*
* @param root Root of the Red-Black Tree
* @param value Value of the node being searched
*
* @return NULL if not found, Address of the node if found
*/
static struct rb_node* __find_node_rbtree (struct rb_root *root, int value)
{
	struct rb_node *node = root->rb_node;
	my_rbnode *entry;

	if (root == NULL) {
		printk("Invalid arguments passed to find  helper routine\n");
		return NULL;
	}

	while (node) {
		entry = rb_entry(node, my_rbnode, rbnode);
		if (entry->value > value)
			node = node->rb_left;
		else if (entry->value < value)
			node = node->rb_right;
		else
			return node;
	}
	return NULL;
}


/**
* @brief Add size number of Unique Random Integers to the tree
*
* @param context Context information for the Red-Black Tree
* @param size Number of Random Integers to be inserted
*
* @return 0 if successful otherwise appropriate error codes
*/
static int add_rbtree (void *context, int size)
{
	project2_rbtree_context *rbtree_context =
							(project2_rbtree_context *) context;
	my_rbnode *tmp_node = NULL;
	int tmp_size = size;
	int ret;

	if (!context) {
		printk(KERN_INFO "context to add_rbtree is NULL\n");
		return -EINVAL;
	}

	while (tmp_size--) {
		tmp_node = kmalloc(sizeof(my_rbnode), GFP_KERNEL);

		if (tmp_node == NULL) {
			printk (KERN_INFO "memory allocation for rbtree node failed\n");
			return -ENOMEM;
		}

		// Retry if the node was already inserted earlier.
		do {
			tmp_node->value = project2_get_next_integer(size);

			ret = __add_rbtree_node(&rbtree_context->root, tmp_node);

			if (ret == -EINVAL) {
				kfree(tmp_node);
				return -EINVAL;
			}
		} while (ret == -EEXIST);

		printk(KERN_INFO "RBTREE_ADD: %d\n", tmp_node->value);
	}
	printk(KERN_INFO "\n");
	return 0;
}

/**
* @brief Prints the contents of the tree in INORDER
*
* @param context Context of the Red-Black Tree
*/
static void show_rbtree (void *context)
{
	project2_rbtree_context *rbtree_context =
								(project2_rbtree_context *) context;
	struct rb_node *node;

	if (!context) {
		printk(KERN_INFO "context to show_rbtree is NULL\n");
		return;
	}

	// Inorder traversal of the tree.
	for (node = rb_first(&rbtree_context->root); node != NULL;
			node = rb_next(node))
		printk(KERN_INFO "RBTREE_SHOW: %d\n",
			rb_entry(node, my_rbnode, rbnode)->value);

	printk(KERN_INFO "\n");
}

/**
* @brief Removes the entire tree after first removing the numbers in the
*		range of the Context.
*
* @param context Context of the Red-Black Tree.
*
* @return 0 for success and appropriate error codes on failure
*/
static int remove_rbtree (void *context)
{
	int curr_index = 0;
	struct rb_node *node;
	my_rbnode *curr = NULL;
	my_rbnode *next = NULL;
	project2_rbtree_context *rbtree_context =
							(project2_rbtree_context *) context;

	if (!context) {
		printk(KERN_INFO "context to show_rbtree is NULL\n");
		return -EINVAL;
	}

	printk("Search and Erase over [%d,%d]\n",
			rbtree_context->start, rbtree_context->end);

	// Traverse across the range and erase any found node.
	for (curr_index = rbtree_context->start;
			curr_index <= rbtree_context->end; curr_index++) {

		node = __find_node_rbtree(&rbtree_context->root, curr_index);

		if (node == NULL)
			printk(KERN_INFO "%d not found in the rbtree\n", curr_index);
		else {
			curr = rb_entry(node, my_rbnode, rbnode);

			rb_erase(node, &rbtree_context->root);

			kfree(curr);

			printk(KERN_INFO "%d found and erased from rbtree\n", curr_index);
		}
	}

	printk(KERN_INFO "\nUpdated tree after previous erase\n");
	// Show the updated tree.
	show_rbtree(context);

	// Remove the entire tree.
	rbtree_postorder_for_each_entry_safe(curr, next,
								&rbtree_context->root, rbnode) {
		printk(KERN_INFO "RBTREE_REMOVE: %d\n", curr->value);
		kfree(curr);
	}

	// Required so that next show_rbtree cannot traverse the tree.
	rbtree_context->root = RB_ROOT;

	show_rbtree(context);

	return 0;
}

/**
* @brief Deallocates the context
*
* @param context Context for the Red-Black Tree
*/
static void deinit_rbtree (void *context)
{
	if (context)
		kfree(context);
}

/**
* @brief Initializes the context by adding a root and range for the test
*
* @param size Numbers of the random Integers to be inserted.
* @param context Context to be initialized.
*
* @return 0 for success, otherwise appropriate error code.
*/
static int init_rbtree (int size, void **context)
{
	project2_rbtree_context *rbtree_context =
				kmalloc(sizeof(project2_rbtree_context), GFP_KERNEL);

	if (!rbtree_context) {
		printk (KERN_INFO "memory allocation for rbtree context failed\n");
		return -ENOMEM;
	}

	// Taking range as [0, size] for the test.
	rbtree_context->start = 0;

	rbtree_context->end = size;

	// Initalizes the root.
	rbtree_context->root = RB_ROOT;

	*context = rbtree_context;

	return 0;

}

// Generates the handles for the rbtree test-case
PROJECT2_GENERATE_HANDLE(rbtree);

// Module related macros
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Abhishek Chauhan <zxcve@vt.edu>");
MODULE_DESCRIPTION("Project2 for manipulation of rbtree data structures\n");
