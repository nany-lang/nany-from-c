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
		NanyConverterVisitor(clang::ASTContext* context)
			: pContext(context)
			, pIndent(0u)
		{}

		//! Entry point for a whole compilation unit
		bool visitTranslationUnitDecl(clang::TranslationUnitDecl* decl);

	private:
		NanyConverterVisitor(const NanyConverterVisitor& other) = delete;
		NanyConverterVisitor(const NanyConverterVisitor&& other) = delete;
		NanyConverterVisitor& operator = (const NanyConverterVisitor& other) = delete;
		NanyConverterVisitor& operator = (const NanyConverterVisitor&& other) = delete;

	private:
		bool visitAttr(clang::Attr *attr);
		bool visitType(const clang::Type* type);
		bool visitType(const clang::QualType& type);

		//! Default behaviour for declarations
		bool visitDecl(clang::Decl* decl);
		//! Declaration contexts are scopes containing declarations
		bool visitDeclContext(clang::DeclContext* context);

		// Variable declaration
		bool visitVarDecl(clang::VarDecl* decl);
		//! Call parameters
		bool visitParmVarDecl(clang::ParmVarDecl* decl);
		bool visitFunctionDecl(clang::FunctionDecl* decl);
		bool visitCXXMethodDecl(clang::CXXMethodDecl* decl);
		bool visitTypedefDecl(clang::TypedefDecl* decl);
		//! C struct
		bool visitRecordDecl(clang::RecordDecl* decl);
		//! C++ struct / class
		bool visitCXXRecordDecl(clang::CXXRecordDecl* decl);

		//! Default behaviour for statements (and exprs)
		bool visitStmt(clang::Stmt* stmt);
		bool visitReturnStmt(clang::ReturnStmt* stmt);
		bool visitCompoundStmt(clang::CompoundStmt* stmt);

		//! Variable use
		bool visitDeclRefExpr(clang::DeclRefExpr* expr);
		//! Function call
		bool visitCallExpr(clang::CallExpr* expr);
		//! All unary operators, both prefix or postfix
		bool visitUnaryOperator(clang::UnaryOperator* expr);
		//! Constructor call
		bool visitCXXConstructExpr(clang::CXXConstructExpr* decl);
		//! All types of cast
		bool visitCastExpr(clang::CastExpr* expr);
		//! Implicit cast
		bool visitImplicitCastExpr(clang::ImplicitCastExpr* expr);
		//! All explicit casts : c-style or c++-style
		bool visitExplicitCastExpr(clang::ExplicitCastExpr* expr);

		bool visitCharacterLiteral(clang::CharacterLiteral* stmt);
		bool visitStringLiteral(clang::StringLiteral* stmt);
		bool visitIntegerLiteral(clang::IntegerLiteral* stmt);
		bool visitFloatingLiteral(clang::FloatingLiteral* stmt);

		//! Visit a declaration's children
		bool visitChildren(clang::Decl* decl);
		//! Dispatch the visitor to the correct dynamic type for this declaration
		bool visitRealDeclType(clang::Decl* decl);
		//! Dispatch the visitor to the correct dynamic type for this statement / expr
		bool visitRealStmtType(clang::Stmt* stmt);

		Yuni::String convertType(const clang::QualType& type) const;
		Yuni::String convertType(const clang::Type* type) const;

		void writeIndent() const
		{
			for (uint i = 0; i < pIndent; ++i)
				std::cout << '\t';
		}

		void increaseIndent() { ++pIndent; }
		void decreaseIndent() { --pIndent; }

	private:
		clang::ASTContext* pContext;
		//! Indent level
		uint pIndent;

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
