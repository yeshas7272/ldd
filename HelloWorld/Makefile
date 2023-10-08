all:
	make -C /lib/modules/`uname -r`/build M=$(PWD) modules

obj-m += HelloWorld.o

clean:
	rm -rf *.o *.ko *.mod.c .an* .lab* .tmp_versions *.order *.symvers *.mod