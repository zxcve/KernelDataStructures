obj-m += project2.o

KDIR := /lib/modules/$(shell uname -r)/build

PWD := $(shell pwd)

project2-objs:= project2_main.o \
				project2_list.o \
				project2_queue.o \
				project2_rbtree.o \
				project2_map.o \
				project2_utils.o

all:
	make -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	make -C $(KDIR) SUBDIRS=$(PWD) clean
