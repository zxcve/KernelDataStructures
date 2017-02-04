#ifndef __PROJECT2_H__
#define __PROJECT2_H__

/**
* @brief Enum for the tests to be carried out.
*/
typedef enum project2_ds_type_t {
	PROJECT2_LIST = 0x0,
	PROJECT2_QUEUE,
	PROJECT2_MAP,
	PROJECT2_RBTREE
} project2_ds_type;


/**
* @brief Function Pointer table to carry out the test.
*/
typedef struct project2_handle_t {
	int (*init) (int size, void **context);
	int (*add) (void *context, int size);
	int (*remove) (void *context);
	void (*iterate) (void *context);
	void (*deinit) (void *context);
	void *context;
} project2_handle;

/**
* @brief Function pointer table for registering/freeing the handle
*/
typedef struct project2_ds_handle_t {
	int (*get_handle) (project2_handle **handle);
	void (*free_handle) (project2_handle *handle);
	char *type;
} project2_ds_handle;


/**
* @brief Generates the array of handles on the basis of type
*
* @param type Type of the test
*/
#define PROJECT2_GENERATE_HANDLE_ARRAY(type) \
	{project2_get_##type##_handle,  project2_free_##type##_handle, #type}

/**
* @brief Generates the handlw functions for the tests
*
* @param type Type of the test
*
* @return 0 for success or -ENOMEM in failure.
*/
#define PROJECT2_GENERATE_HANDLE(type) 										\
	int project2_get_##type##_handle(project2_handle **handle) 				\
	{ 																		\
		*handle = kmalloc (sizeof(project2_handle), GFP_KERNEL); 			\
		if (*handle == NULL) { 												\
			printk (KERN_INFO "memory allocation for" #type "handle failed\n");\
			return -ENOMEM; 												\
		} 																	\
		(*handle)->init = init_##type; 										\
		(*handle)->add = add_##type; 										\
		(*handle)->remove = remove_##type; 									\
		(*handle)->iterate = show_##type; 									\
		(*handle)->deinit = deinit_##type; 									\
		return 0; 															\
	} 																		\
																			\
	void project2_free_##type##_handle(project2_handle *handle) 			\
	{																		\
		if (handle)															\
			kfree(handle);													\
	}


/**
* @brief Generates the prototype to add in c sources
*
* @param type Type of the test
*/
#define PROJECT2_GENERATE_HANDLE_PROTOTYPE(type) 					\
		int project2_get_##type##_handle(project2_handle **handle); \
		void project2_free_##type##_handle(project2_handle *handle);

// Generates the prototypes for addition in c sources
PROJECT2_GENERATE_HANDLE_PROTOTYPE(list);
PROJECT2_GENERATE_HANDLE_PROTOTYPE(queue);
PROJECT2_GENERATE_HANDLE_PROTOTYPE(map);
PROJECT2_GENERATE_HANDLE_PROTOTYPE(rbtree);

/**
* @brief Returns a random integer from 0 to (size/size*4) based on size
*
* @param size Seed for defining boundary conditions
*
* @return Random Integer
*/
int project2_get_next_integer(int size);


/**
* @brief Executes all list functions in 1 function
*
* @param size Number of integers to be inserted
*
* @return 0 for success or appropriate error code on failure.
*/
int project2_list_standalone(int size);

#endif
