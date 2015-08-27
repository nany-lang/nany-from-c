#include <yuni/yuni.h>
#include <yuni/io/file.h>
#include <vector>
#include <string>
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Frontend/FrontendActions.h"
#include "parser.h"


namespace NanyFromC
{
	/*
	namespace // anonymous
	{

		class ExampleFrontendAction : public ASTFrontendAction
		{
		public:
			virtual ASTConsumer* CreateASTConsumer(CompilerInstance &ci, llvm::StringRef file)
			{
				return new ExampleASTConsumer(&CI); // pass CI pointer to ASTConsumer
			}
		};

	}
	*/

	// Apply a custom category to all command-line options so that they are the
	// only ones displayed.
	static llvm::cl::OptionCategory NanyToolCategory("nany-from-c options");


	bool Parser::run()
	{
		if (!Yuni::IO::File::Exists(pFilePath))
		{
			pError = "File not found";
			return false;
		}

		const char* argv[2] = {
			"dummy",
			pFilePath.data()
		};
		int argc = 2;
		clang::tooling::CommonOptionsParser optionParser(argc, argv, NanyToolCategory);
		clang::tooling::ClangTool tool(
			optionParser.getCompilations(), optionParser.getSourcePathList());

		int result = tool.run(
			clang::tooling::newFrontendActionFactory<clang::SyntaxOnlyAction>().get());
		return true;
	}


} // namespace NanyFromC
