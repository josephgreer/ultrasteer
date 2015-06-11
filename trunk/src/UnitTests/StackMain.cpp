#include "CppUnitLite/TestHarness.h"
#include <stdio.h>
#include <string.h>

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
  } else if(argc > 1) {
    SimpleString test(argv[1]);
    if(argc == 3) {
      test = SimpleString(strcat(argv[2], argv[1]));
    }
    TestRegistry::runSpecificTest(tr, test);
  } else {
    printUsage();
  }

  return 0;
}