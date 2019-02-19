/*
 * wrmsrbench	Microbenchmark for WRMSR
 *
 * WARNING: Dangerous software. Do not use unless you know and accept the risks
 * of writing to MSRs, which include rebooting or damaging systems.
 *
 * Copyright Brendan Gregg, 2019.
 * Licensed under the terms of the GNU GPL License version 2
 *
 * 07-Feb-2019	Brendan Gregg	Created this.
 */

#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/ktime.h>	/* Needed for ktime_get */

void __attribute__((optimize("O0"))) do_wrmsr_benchmark(u64 iterations,
    int msr, int lo, int hi)
{
	u64 i;

	for (i = 0; i < iterations; i++) {
		// minimize loop instructions by unrolling wrmsr
		asm volatile ("wrmsr\n" : : "c"(msr), "a"(lo), "d"(hi));
		asm volatile ("wrmsr\n");
		asm volatile ("wrmsr\n");
		asm volatile ("wrmsr\n");
		asm volatile ("wrmsr\n");
		asm volatile ("wrmsr\n");
		asm volatile ("wrmsr\n");
		asm volatile ("wrmsr\n");
		asm volatile ("wrmsr\n");
		asm volatile ("wrmsr\n");
	}
}

int init_module(void)
{
	u64 iterations = 200 * 1000;
	int perloop = 10;	/* unrolled, as above */
	u64 instructions = iterations * perloop;
	ktime_t start, end;
	// choose MSR and value here:
	int msr = 0x38f, lo = 0, hi = 0;	// IA32_PERF_GLOBAL_CTRL

	/*
	 * MSR GPFs will panic the kernel, so check via a slower and safer
	 * call first.
	 */
	if (native_write_msr_safe(msr, lo, hi) != 0) {
		printk(KERN_INFO "wrmsrbench: error writing MSR (%x %x %x), bailing.\n",
		    msr, lo, hi);
		return 0;
	}

	/* main */
	start = ktime_get();
	do_wrmsr_benchmark(iterations, msr, lo, hi);
	end = ktime_get();
	printk(KERN_INFO "wrmsrbench: %llu ns per instruction, %llu instructions\n",
	    (ktime_to_ns(end) - ktime_to_ns(start)) / instructions, instructions);

	return 0;
}

void cleanup_module(void)
{
	;
}

MODULE_LICENSE("GPL");
