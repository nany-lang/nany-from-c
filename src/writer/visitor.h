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

		bool visitAttr(clang::Attr *attr);
		bool visitType(const clang::Type* type);

		bool visitDecl(clang::Decl* decl);
		bool visitDeclContext(clang::DeclContext* context);

		bool visitTranslationUnitDecl(clang::TranslationUnitDecl* decl);
		bool visitVarDecl(clang::VarDecl* decl);
		bool visitParmVarDecl(clang::ParmVarDecl* decl);
		bool visitFunctionDecl(clang::FunctionDecl* decl);
		bool visitCXXMethodDecl(clang::CXXMethodDecl* decl);
		bool visitTypedefDecl(clang::TypedefDecl* decl);
		bool visitRecordDecl(clang::RecordDecl* decl);
		bool visitCXXRecordDecl(clang::CXXRecordDecl* decl);

		bool visitCallExpr(clang::CallExpr* expr);

		bool visitStmt(clang::Stmt* stmt);
		bool visitDeclStmt(clang::DeclStmt* stmt);
		bool visitReturnStmt(clang::ReturnStmt* stmt);
		bool visitCompoundStmt(clang::CompoundStmt* stmt);

		bool visitCastExpr(clang::CastExpr* expr);
		bool visitImplicitCastExpr(clang::ImplicitCastExpr* expr);
		bool visitExplicitCastExpr(clang::ExplicitCastExpr* expr);

		bool visitCharacterLiteral(clang::CharacterLiteral* stmt);
		bool visitStringLiteral(clang::StringLiteral* stmt);
		bool visitIntegerLiteral(clang::IntegerLiteral* stmt);
		bool visitFloatingLiteral(clang::FloatingLiteral* stmt);

	private:
		bool visitChildren(clang::Decl* decl);
		bool visitRealDeclType(clang::Decl* decl);
		bool visitRealStmtType(clang::Stmt* stmt);

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
			pVisitor.visitTranslationUnitDecl(context.getTranslationUnitDecl());
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
