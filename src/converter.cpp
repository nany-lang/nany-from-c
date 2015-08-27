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
		std::vector<Yuni::Thread::IThread::Ptr> threads;
		uint nbErrors = 0;

		for (const auto& file : pJobs)
		{
			threads.push_back(Yuni::spawn([&]() {
				Parser parser(file);
				if (!parser.run())
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
			}));
		}

		for (const auto& thread : threads)
		{
			thread->wait();
		}

		pJobs.clear();
		return nbErrors;
	}




} // namespace NanyFromC
