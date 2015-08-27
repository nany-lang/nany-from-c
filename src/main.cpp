#include <yuni/yuni.h>
#include <yuni/core/getopt.h>
#include "converter.h"


int main(int argc, char* argv[])
{
	if (argc < 2)
		return 1;

	NanyFromC::Converter converter;

	converter.onFail = [](const AnyString& message) {
		std::cerr << message << std::endl;
	};

	converter.onSuccess = [](const AnyString& message) {
		std::cout << message << std::endl;
	};

	for (uint i = 1; i < static_cast<uint>(argc); ++i)
		converter.queueJob(argv[i]);

	uint nbErrors = converter.runAllJobs();
	if (nbErrors > 0)
	{
		std::cerr << "Conversion failed on " << nbErrors << " files." << std::endl;
		return 1;
	}

	return 0;
}
