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
		MessageCallback onFail;
		MessageCallback onSuccess;

	public:
		void queueJob(const AnyString& filePath);

		uint runAllJobs();

	private:
		void queueJobWL(const AnyString& filePath);

	private:
		Yuni::Mutex pMutex;
		JobList pJobs;

	}; // class Converter



} // namespace NanyFromC
