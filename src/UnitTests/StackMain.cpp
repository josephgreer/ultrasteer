#include "CppUnitLite/TestHarness.h"
#include <stdio.h>

static void printUsage()
{
	printf("UnitTest usage:  \n");
	printf("UnitTest.exe\n");
	printf("UnitTest.exe GroupName TestName\n");
}

int main(int argc, char *argv[])
{
    TestResult tr;

	if(argc == 1) {
		TestRegistry::runAllTests(tr);
	} else if(argc == 2) {
		TestRegistry::runSpecificTest(tr, SimpleString(argv[1]));
	} else {
		printUsage();
	}

  return 0;
}