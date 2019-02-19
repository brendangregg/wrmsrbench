obj-m += wrmsrbench.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

run:
	insmod wrmsrbench.ko
	rmmod wrmsrbench
	dmesg | tail -1
	perf stat -a -- date > /dev/null 2>&1	# reset MSRs
