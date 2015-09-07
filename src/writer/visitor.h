#pragma once
#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include "indenter.h"
#include "visibilitystack.h"


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
		bool visitTranslationUnitDecl(const clang::TranslationUnitDecl* decl);

	private:
		NanyConverterVisitor(const NanyConverterVisitor& other) = delete;
		NanyConverterVisitor(NanyConverterVisitor&& other) = delete;
		NanyConverterVisitor& operator = (const NanyConverterVisitor& other) = delete;
		NanyConverterVisitor& operator = (NanyConverterVisitor&& other) = delete;

	private:
		bool visitAttr(const clang::Attr* attr);
		bool visitType(const clang::Type* type);
		bool visitType(const clang::QualType& type);

		//! Default behaviour for declarations
		bool visitDecl(const clang::Decl* decl);
		//! Declaration contexts are scopes containing declarations
		bool visitDeclContext(const clang::DeclContext* context);

		// Variable declaration
		bool visitVarDecl(const clang::VarDecl* decl);
		//! Call parameters
		bool visitParmVarDecl(const clang::ParmVarDecl* decl);
		bool visitFieldDecl(const clang::FieldDecl* decl);
		bool visitFunctionDecl(const clang::FunctionDecl* decl);
		bool visitCXXConstructorDecl(const clang::CXXConstructorDecl* decl);
		bool visitCXXDestructorDecl(const clang::CXXDestructorDecl* decl);
		bool visitCXXMethodDecl(const clang::CXXMethodDecl* decl);
		bool visitTypedefDecl(const clang::TypedefDecl* decl);
		//! Enum type declaration
		bool visitEnumDecl(const clang::EnumDecl* decl);
		//! Enum value declaration
		bool visitEnumConstantDecl(const clang::EnumConstantDecl* decl);
		//! C struct
		bool visitRecordDecl(const clang::RecordDecl* decl);
		//! C++ struct / class
		bool visitCXXRecordDecl(const clang::CXXRecordDecl* decl);

		//! Default behaviour for statements (and exprs)
		bool visitStmt(const clang::Stmt* stmt);
		bool visitReturnStmt(const clang::ReturnStmt* stmt);
		bool visitCompoundStmt(const clang::CompoundStmt* stmt);

		//! Variable use
		bool visitDeclRefExpr(const clang::DeclRefExpr* expr);
		//! Class member use
		bool visitMemberExpr(const clang::MemberExpr* expr);
		//! Function call
		bool visitCallExpr(const clang::CallExpr* expr);
		bool visitCXXNewExpr(const clang::CXXNewExpr* expr);
		bool visitCXXDeleteExpr(const clang::CXXDeleteExpr* expr);
		//! All unary operators, both prefix or postfix
		bool visitUnaryOperator(const clang::UnaryOperator* expr);
		//! Constructor call
		bool visitCXXConstructExpr(const clang::CXXConstructExpr* decl);
		//! All types of cast
		bool visitCastExpr(const clang::CastExpr* expr);
		//! Implicit cast
		bool visitImplicitCastExpr(const clang::ImplicitCastExpr* expr);
		//! All explicit casts : c-style or c++-style
		bool visitExplicitCastExpr(const clang::ExplicitCastExpr* expr);

		bool visitCharacterLiteral(const clang::CharacterLiteral* stmt);
		bool visitStringLiteral(const clang::StringLiteral* stmt);
		bool visitIntegerLiteral(const clang::IntegerLiteral* stmt);
		bool visitFloatingLiteral(const clang::FloatingLiteral* stmt);

		//! Visit a declaration's children
		bool visitChildren(const clang::Decl* decl);
		//! Dispatch the visitor to the correct dynamic type for this declaration
		bool visitRealDeclType(const clang::Decl* decl);
		//! Dispatch the visitor to the correct dynamic type for this statement / expr
		bool visitRealStmtType(const clang::Stmt* stmt);

		Yuni::String convertType(const clang::QualType& type) const;
		Yuni::String convertType(const clang::Type* type) const;

	private:
		//! AST Context holds additional separate information about the AST
		clang::ASTContext* pContext;
		//! Indenting management
		Indenter pIndent;
		//! Stack of current visibilities by scope
		VisibilityStack pVisibilities;

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
