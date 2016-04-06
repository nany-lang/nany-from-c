#pragma once

#include <yuni/yuni.h>
#include <yuni/core/bind.h>
#include <yuni/thread/mutex.h>
#include <vector>


namespace NanyFromC
{

	class Converter
	{
	public:
		//! Delegate type for the Refresh callback
		typedef Yuni::Bind<void (const AnyString&)>  MessageCallback;

		typedef std::vector<Yuni::String> JobList;

	public:
		enum Verbosity
		{
			vQuiet = 0,
			vError = 1,
			vWarning = 2,
			vInfo = 3,
			vDebug = 4,
		};

	public:
		MessageCallback onFail;
		MessageCallback onSuccess;

	public:
		void queueJob(const AnyString& filePath);

		uint runAllJobs();

		void setVerbosity(const AnyString& verbosityStr);

	private:
		void queueJobWL(const AnyString& filePath);
		void printMessage(const AnyString& message, Verbosity level);

	private:
		Yuni::Mutex pMutex;
		JobList pJobs;
		Verbosity pVerbosity = vWarning;

	}; // class Converter



} // namespace NanyFromC
