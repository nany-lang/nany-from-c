#pragma once

#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/AST/DataRecursiveASTVisitor.h>
#include <iostream>


namespace NanyFromC
{

		class NanyConverterVisitor final : public clang::DataRecursiveASTVisitor<NanyConverterVisitor>
		{
		public:
			typedef clang::DataRecursiveASTVisitor<NanyConverterVisitor>  ParentT;

		public:
			NanyConverterVisitor(clang::ASTContext* context): pContext(context)
			{}

			bool VisitDecl(clang::Decl* decl)
			{
				decl->dump();
				return true;
			}

			bool VisitCXXRecordDecl(clang::CXXRecordDecl* decl)
			{
				decl->dump();
				return true;
			}

		private:
			clang::ASTContext* pContext;
		}; // class NanyConverterVisitor


		class NanyConverterASTConsumer final : public clang::ASTConsumer
		{
		public:
			NanyConverterASTConsumer(clang::ASTContext* context): pVisitor(context)
			{}

			virtual void HandleTranslationUnit(clang::ASTContext& context) override
			{
				std::cout << "Starting AST visit" << std::endl;
				pVisitor.TraverseDecl(context.getTranslationUnitDecl());
			}

		private:
			NanyConverterVisitor pVisitor;
		}; // class NanyConverterASTConsumer


		class NanyConverterFrontendAction final : public clang::ASTFrontendAction
		{
		public:
			virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance& ci, llvm::StringRef file) override
			{
				return std::unique_ptr<clang::ASTConsumer>(new NanyConverterASTConsumer(&ci.getASTContext()));
			}
		}; // class NanyConverterFrontendAction


} // namespace NanyFromC
