#pragma once

#include <yuni/yuni.h>
#include <yuni/io/file.h>


namespace NanyFromC
{

	class Parser
	{
	public:
		Parser(const AnyString& filePath): pFilePath(filePath)
		{}

		bool run();

		void* ast() { return nullptr; }
		Yuni::String error() const { return "Not yet implemented"; }

	private:
		AnyString pFilePath;
		Yuni::String pError;

	}; // class Parser



} // namespace NanyFromC
