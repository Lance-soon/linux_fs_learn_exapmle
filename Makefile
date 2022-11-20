# 
# Makefile for Linux wangdehuifs
#

obj-m += wangdehuifs.o
wangdehuifs-objs := super.o inode.o file.o

all:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(shell pwd)

clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build/ M=$(shell pwd) clean

