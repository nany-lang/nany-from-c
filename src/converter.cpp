#include <yuni/yuni.h>
#include <yuni/job/queue/service.h>
#include <yuni/thread/utility.h>
#include <vector>
#include "converter.h"
#include "parser/parser.h"
//#include "writer/writer.h"


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
		return nbErrors;
	}




} // namespace NanyFromC
