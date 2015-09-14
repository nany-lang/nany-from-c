#include <yuni/yuni.h>
#include <yuni/io/file.h>
#include <yuni/io/filename-manipulation.h>
#include <vector>
#include <string>
#include "writer/visitor.h"
#include <clang/Tooling/Tooling.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include "parser.h"


namespace NanyFromC
{

	// Apply a custom category to all command-line options so that they are the
	// only ones displayed.
	static llvm::cl::OptionCategory NanyToolCategory("nany-tool");


	bool Parser::run()
	{
		if (!Yuni::IO::File::Exists(pFilePath))
		{
			pError = "File not found";
			return false;
		}
/*
		const char* argv[2] = {
			"nany-from-c",
			pFilePath.data()
		};
		int argc = 2;
		clang::tooling::CommonOptionsParser optionParser(argc, argv, NanyToolCategory);
		clang::tooling::ClangTool tool(
			optionParser.getCompilations(), optionParser.getSourcePathList());
*/
		//int result = tool.run(
		//	clang::tooling::newFrontendActionFactory<clang::SyntaxOnlyAction>().get());
		//return 0 != tool.run(clang::tooling::newFrontendActionFactory<NanyConverterFrontendAction>().get());

		Yuni::String code;
		Yuni::IO::File::LoadFromFile(code, pFilePath);
//		Yuni::String outputFile(pFilePath);
//		if (!Yuni::IO::ReplaceExtension(outputFile, ".ny"))
//			outputFile.append(".ny");
		return clang::tooling::runToolOnCode(new NanyConverterFrontendAction(), code.c_str(), pFilePath.c_str());
	}


} // namespace NanyFromC
