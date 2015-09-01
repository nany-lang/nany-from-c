#pragma once

#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <iostream>


namespace NanyFromC
{

	class NanyConverterVisitor final
	{
	public:
		NanyConverterVisitor(clang::ASTContext* context): pContext(context)
		{}

		bool VisitAttr(clang::Attr *attr);
		bool VisitType(const clang::Type* type);

		bool VisitDecl(clang::Decl* decl);
		bool VisitTranslationUnitDecl(clang::TranslationUnitDecl* decl);
		bool VisitVarDecl(clang::VarDecl* decl);
		bool VisitParmVarDecl(clang::ParmVarDecl* decl);
		bool VisitFunctionDecl(clang::FunctionDecl* decl);
		bool VisitCXXMethodDecl(clang::CXXMethodDecl* decl);
		bool VisitTypedefDecl(clang::TypedefDecl* decl);
		bool VisitRecordDecl(clang::RecordDecl* decl);
		bool VisitCXXRecordDecl(clang::CXXRecordDecl* decl);

		bool VisitCallExpr(clang::CallExpr* expr);

		bool VisitStmt(clang::Stmt* stmt);
		bool VisitReturnStmt(clang::ReturnStmt* stmt);
		bool VisitCharacterLiteral(clang::CharacterLiteral* stmt);
		bool VisitIntegerLiteral(clang::IntegerLiteral* stmt);

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
			// Start AST visit
			pVisitor.VisitTranslationUnitDecl(context.getTranslationUnitDecl());
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
