
#define TESTING_SUITE_ENABLED

#ifndef TESTING_SUITE_ENABLED
#define DO_TEST_RIGHT return 0xFFFF;
#else
#define DO_TEST_RIGHT
#endif

int tests_main()
{
	DO_TEST_RIGHT
	printk("test","Entered Kernel Function Tester...\n");
	printk("test","=> It appears the kernel function tester is running, at least!\n");
	return 0;
}