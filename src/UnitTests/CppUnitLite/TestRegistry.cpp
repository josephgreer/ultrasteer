

#include "Test.h"
#include "TestResult.h"
#include "TestRegistry.h"
#include <string.h>


void TestRegistry::addTest (Test *test) 
{
	instance ().add (test);
}


void TestRegistry::runAllTests (TestResult& result) 
{
	instance ().run (result);
}

void TestRegistry::runSpecificTest(TestResult& result, SimpleString &testName)
{
	for(Test *test=instance().tests; test!=0; test=test->getNext()) {
		if(_strcmpi(testName.asCharString(),test->getName().asCharString()) == 0) {
			test->run(result);
		}
	}
}


TestRegistry& TestRegistry::instance () 
{
	static TestRegistry registry;
	return registry;
}


void TestRegistry::add (Test *test) 
{
	if (tests == 0) {
		tests = test;
		return;
	}
	
	test->setNext (tests);
	tests = test;
}


void TestRegistry::run (TestResult& result) 
{
	result.testsStarted ();

	for (Test *test = tests; test != 0; test = test->getNext ())
		test->run (result);
	result.testsEnded ();
}



