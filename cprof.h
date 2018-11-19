/*
CPerf.h

C++ performance tools/macros
*/
#pragma once

#include <iostream>
#include <chrono>
#include <fstream>
#include <exception>
#include <iomanip>
#include <ios>
#include <istream>

namespace cprof
{
	struct latency_test_results
	{
		const char* testname;
		long long nanos;
	};

	/*
	cprof::Environment
	Handles tests and provides functions that allow for writing to files.
	*/
	class Environment
	{
	private:
		latency_test_results* results;
		unsigned max_tests;
		unsigned current_index;

	public:
		Environment(unsigned max_tests)
		{
			results = (latency_test_results*)calloc(max_tests, sizeof(latency_test_results));
			this->max_tests = max_tests;
			current_index = 0;
		}

		~Environment()
		{
			free(results);
		}

		unsigned int GetTestCount() const
		{
			for (unsigned int i = 0; i < max_tests; i++)
			{
				if (results[i].testname == NULL && results[i].nanos == 0)
					return i;
			}
			return max_tests;
		}

		latency_test_results* GetTest(unsigned index) const
		{
			return &results[index];
		}

		latency_test_results* GetTest(const char* name) const
		{
			for (unsigned i = 0; i < current_index; i++)
			{
				if (strcmp(name, results[i].testname) == 0)
					return &results[i];
			}

			return NULL;
		}

		bool HasTest(unsigned index) const
		{
			return index < current_index;
		}

		bool HasTest(const char* name) const
		{
			for (unsigned i = 0; i < current_index; i++)
			{
				if (strcmp(name, results[i].testname) == 0)
					return true;
			}
			return false;
		}

		void CreateTestEntry(latency_test_results test)
		{
			results[current_index] = test;
			current_index++;
		}

		void PutResults(std::ostream& outstream) const 
		{
			for (unsigned i = 0; i < current_index; i++)
			{
				outstream << "\nElapsed time for " << results[i].testname << "\n";
				outstream << "\t" << results[i].nanos << "ns\n";
				outstream << "\t" << (float)(results[i].nanos / 1000.0f) << "us\n";
				outstream << "\t" << (float)(results[i].nanos / 1000000.0f) << "ms\n";
				outstream << "\t" << (float)(results[i].nanos / 1000000000.0f) << "s\n" << std::endl;
				outstream.flush();
			}
		}

		void SaveToFile(const char* file) const
		{
			try
			{
				std::ofstream fs(file, std::ios_base::out);

				PutResults(fs);

				fs.close();
			}
			catch (std::exception e)
			{
				//oof
				return;
			}
		}

		void PrintResults() const 
		{
			PutResults(std::cout);
		}

		void DisposeTests()
		{
			current_index = 0;

			free(results);

			results = (latency_test_results*)calloc(max_tests, sizeof(latency_test_results));
		}

		void Destroy()
		{
			DisposeTests();
			delete this;
		}

		void SaveToXML(const char* file) const 
		{
			try
			{
				std::ofstream fs(file, std::ios_base::out);

				if (!fs.good()) 
				{
					fs.close();
					throw new std::exception("Unable to write to XML");
				}
					

				fs << "<?xml version = \"1.0\" encoding = \"utf - 8\"?>" << "\n";
				fs << "<!--nanos - Nanoseconds of the test\nmicros - Microseconds of the test\nmillis - Milliseconds of the test\nseconds - Seconds of the test -->\n";
				fs << "<tests>\n";

				for (unsigned i = 0; i < current_index; i++)
				{
					fs << std::setprecision(16) << std::fixed;

					fs << "\t<test name=\"" << results[i].testname << "\" number=\"" << i << "\">\n";

					fs << "\t\t<nanos>" << results[i].nanos << "</nanos>\n";
					fs << "\t\t<micros>" << (float)(results[i].nanos/1000.0f) << "</micros>\n";
					fs << "\t\t<millis>" << (float)(results[i].nanos / 1000000.0f) << "</millis>\n";
					fs << "\t\t<seconds>" << (float)(results[i].nanos / 1000000000.0f) << "</seconds>\n";

					fs << "\t</test>\n";
				}

				fs << "</tests>" << std::endl;

				fs.flush();
				fs.close();

			}
			catch (std::exception e)
			{
				//oof
			}
		}

		void SaveToCSV(const char* file) const
		{
			try
			{
				std::ofstream fs(file, std::ios_base::out);

				if (!fs.good())
				{
					fs.close();
					throw new std::exception("Unable to write to XML");
				}

				fs << "Test Number, Test Name, Duration(ns), Duration(us), Duration(ms), Duration(s)\n";

				for (unsigned i = 0; i < current_index; i++)
				{
					fs << std::setprecision(16) << std::fixed;

					fs << i << ", ";
					fs << results[i].testname << ", ";
					fs << results[i].nanos << ", ";
					fs << (float)(results[i].nanos / 1000.0f) << ", ";
					fs << (float)(results[i].nanos / 1000000.0f) << ", ";
					fs << (float)(results[i].nanos / 1000000000.0f) << "\n";
				}

				fs.flush();
				fs.close();

			}
			catch (std::exception e)
			{
				//oof
			}
		}
	};

}

#define CPROF_BEGIN_LATENCY_TEST(environment, name) { \
									cprof::latency_test_results __cprof__res__ = cprof::latency_test_results();	\
									__cprof__res__.testname = name;	\
									std::chrono::high_resolution_clock::time_point __cprof__point1__ = std::chrono::high_resolution_clock::now()

#define CPROF_END_LATENCY_TEST(environment)	std::chrono::high_resolution_clock::time_point __cprof__point2__ = std::chrono::high_resolution_clock::now();	\
									__cprof__res__.nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(__cprof__point2__ - __cprof__point1__).count();	\
									environment.CreateTestEntry(__cprof__res__); \
								 }
