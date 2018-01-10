obj-m += nulll.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	sudo insmod nulll.ko
	sudo chmod 666 /dev/nulll
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	sudo rmmod nulll
test:
	sudo dmesg -C
	sudo insmod nulll.ko
	sudo rmmod nulll.ko
	dmesg