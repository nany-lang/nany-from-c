#include <yuni/yuni.h>
#include <yuni/core/getopt.h>
#include <iostream>
#include "converter.h"


int main(int argc, char* argv[])
{
	if (argc < 2)
		return 1;

	// Option parser
	Yuni::GetOpt::Parser options;
	// Verbosity option
	Yuni::String optionVerbose = "warning";
	options.add(optionVerbose, 'v', "verbose", "Message verbosity. Values are : quiet, error, warning, info, debug. Default is : warning");
	// Catch all remaining arguments
	Yuni::String::Vector remainingArgs;
	options.remainingArguments(remainingArgs);
	if (!options(argc, argv))
		return options.errors() ? EXIT_FAILURE : 0;

	// Create the converter
	NanyFromC::Converter converter;
	converter.setVerbosity(optionVerbose);
	converter.onFail = [](const AnyString& message) {
		std::cerr << message << std::endl;
	};
	converter.onSuccess = [](const AnyString& message) {
		std::cout << message << std::endl;
	};

	// Queue conversion jobs
	for (const Yuni::String& filePath : remainingArgs)
		converter.queueJob(filePath);

	// Run
	uint nbErrors = converter.runAllJobs();
	return nbErrors > 0 ? 1 : 0;
}
