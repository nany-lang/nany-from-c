#include <yuni/yuni.h>
#include <yuni/job/queue/service.h>
#include <yuni/thread/utility.h>
#include <vector>
#include <iostream>
#include "converter.h"
#include "parser/parser.h"


namespace NanyFromC
{


	void Converter::queueJob(const AnyString& filePath)
	{
		pJobs.push_back(filePath);
	}


	uint Converter::runAllJobs()
	{
		Yuni::Job::QueueService queueservice;
		queueservice.start();
		uint nbErrors = 0;

		for (const auto& file : pJobs)
		{
			Yuni::async(queueservice, [&]()
			{
				Parser parser(file);
				if (not parser.run())
				{
					onFail(Yuni::String(file) << " : Parse error : " << parser.error());
					Yuni::MutexLocker lock(pMutex);
					++nbErrors;
					return;
				}
				// assert(parser.ast() != nullptr);
				// Writer writer(parser.ast());
				// if (!writer.run())
				// {
				//		onFail(Yuni::String(file) << " : Parse error : " << parser.error());
				//		Yuni::MutexLocker lock(pMutex);
				//		++nbErrors;
				//		return;
				// }
			});
		}

		queueservice.wait(Yuni::qseIdle);
		pJobs.clear();

		if (nbErrors > 0 && pVerbosity >= vError)
			std::cerr << "Conversion failed on " << nbErrors << " files." << std::endl;

		return nbErrors;
	}


	void Converter::setVerbosity(const AnyString& verbosityStr)
	{
		if (verbosityStr == "quiet")
			pVerbosity = vQuiet;
		else if (verbosityStr == "error")
			pVerbosity = vError;
		else if (verbosityStr == "warning")
			pVerbosity = vWarning;
		else if (verbosityStr == "info")
			pVerbosity = vInfo;
		else if (verbosityStr == "debug")
			pVerbosity = vDebug;
	}

	void Converter::printMessage(const AnyString& message, Verbosity level)
	{
		assert(level > vQuiet && "Quiet verbosity makes no sesnse for a message");
		if (pVerbosity >= level)
			std::cerr << message << std::endl;
	}


} // namespace NanyFromC
