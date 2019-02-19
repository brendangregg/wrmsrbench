# wrmsrbench

Microbenchmark for the wrmsr instruction.

This loads a kernel module since wrmsr must run as ring 0 or 1. The MSR written can be modified in the source: be careful, as many will harm the system: eg, changing voltages and writing to firmware. I'm guessing that worst cases include bricking a system, and setting it on fire.

```
# make
[...]
# make run
insmod wrmsrbench.ko
rmmod wrmsrbench
dmesg | tail -1
[530060.004103] wrmsrbench: 58 ns per instruction, 2000000 instructions
perf stat -a -- date > /dev/null 2>&1	# reset MSRs
```

That run showed 58 nanoseconds per wrmsr. It works by loading a kernel module which runs the benchmark in its init routine, writes to dmesg, and then unloads the module. We fetch the output from dmesg.

WARNING: This is dangerous software. Do not use unless you know and accept the risks of writing to MSRs, which include rebooting or damaging systems.
