/*

cprof_example.cpp

Example of usage
*/

#include <math.h>

#include "cprof.h"

int main()
{
	//Initialize cprof environment to take 10 tests
	cprof::Environment environment(10);
	
	//Begin latency test. Parameters are cprof::Environment and const char* for test name
	CPROF_BEGIN_LATENCY_TEST(environment, "math.h sqrtf() test");
	
	for(int i = 0; i < 100; i++)
	{
		sqrtf((float)i);
	}
	
	//End latency test
	CPROF_END_LATENCY_TEST(environment);
	
	CPROF_BEGIN_LATENCY_TEST(environment, "math.h absf() test");
	
	for(int i = 0; i < 100; i++)
	{
		absf((float)i);
	}
	
	CPROF_END_LATENCY_TEST(environment);
	
	//Print results
	environment.PrintResults();
	
	//Save data to XML
	environment.SaveToXML("example_tests.xml");
	
	//Save data to CSV
	environment.SaveToCSV("example_tests.csv");
	
	//Dispose test results
	environment.DisposeTests();
	
	return 0;
}
