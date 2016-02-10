#pragma once
#include <yuni/yuni.h>
#include <yuni/core/dictionary.h>
#include <yuni/core/logs.h>
#include <yuni/core/string.h>
#include <yuni/io/filename-manipulation.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include "indenter.h"


namespace NanyFromC
{

	class NanyConverterVisitor final
	{
	public:
		NanyConverterVisitor(clang::ASTContext* context)
			: pContext(context)
			, pIndent(0u)
			, pStatementStart(false)
		{
			// Filter out debug and warning messages
			pLog.verbosityLevel = Yuni::Logs::Verbosity::Error::level;
			// Get the compilation unit's main file
			clang::FileID mainFileID = pContext->getSourceManager().getMainFileID();
			const clang::FileEntry* fileEntry = pContext->getSourceManager().getFileEntryForID(mainFileID);
			// Store its path to detect other files from the project (this is naive but better than nothing)
			Yuni::IO::ExtractAbsoluteFilePath(pMainFilePath, fileEntry->getName());
		}

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
		bool visitCXXMethodDecl(const clang::CXXMethodDecl* decl);
		bool visitCXXConstructorDecl(const clang::CXXConstructorDecl* decl);
		bool visitCXXDestructorDecl(const clang::CXXDestructorDecl* decl);
		bool visitTypedefDecl(const clang::TypedefDecl* decl);
		//! Visibility specifier
		bool visitAccessSpecDecl(const clang::AccessSpecDecl* decl);
		//! Enum type declaration
		bool visitEnumDecl(const clang::EnumDecl* decl);
		//! Enum value declaration
		bool visitEnumConstantDecl(const clang::EnumConstantDecl* decl);
		//! C struct
		bool visitRecordDecl(const clang::RecordDecl* decl);
		//! C++ struct / class
		bool visitCXXRecordDecl(const clang::CXXRecordDecl* decl);
		//! extern "C" declarations
		bool visitLinkageSpecDecl(const clang::LinkageSpecDecl* decl);
		//! Common code for records and enums, called specifically by visitRecordDecl and visitEnumDecl
		bool visitTagDecl(const clang::TagDecl* decl, Yuni::String& name);

		//! Default behaviour for statements (and exprs)
		bool visitStmt(const clang::Stmt* stmt);
		//! Empty statement ";"
		bool visitNullStmt(const clang::Stmt* stmt);
		bool visitDeclStmt(const clang::DeclStmt* stmt);
		bool visitReturnStmt(const clang::ReturnStmt* stmt);
		bool visitIfStmt(const clang::IfStmt* stmt);
		bool visitWhileStmt(const clang::WhileStmt* stmt);
		bool visitDoStmt(const clang::DoStmt* stmt);
		bool visitForStmt(const clang::ForStmt* stmt);
		bool visitSwitchStmt(const clang::SwitchStmt* stmt);
		bool visitCaseStmt(const clang::CaseStmt* stmt);
		bool visitDefaultStmt(const clang::DefaultStmt* stmt);
		bool visitBreakStmt(const clang::BreakStmt* stmt);
		bool visitContinueStmt(const clang::ContinueStmt* stmt);
		bool visitCompoundStmt(const clang::CompoundStmt* stmt);

		//! Variable use
		bool visitDeclRefExpr(const clang::DeclRefExpr* expr);
		//! Class member use
		bool visitMemberExpr(const clang::MemberExpr* expr);
		//! Function call
		bool visitCallExpr(const clang::CallExpr* expr);
		//! Call to `new`
		bool visitCXXNewExpr(const clang::CXXNewExpr* expr);
		//! Call to `delete`
		bool visitCXXDeleteExpr(const clang::CXXDeleteExpr* expr);
		//! Special expression that initializes short-life objects
		bool visitExprWithCleanups(const clang::ExprWithCleanups* expr);
		//! Indexed array access
		bool visitArraySubscriptExpr(const clang::ArraySubscriptExpr* expr);
		//! All unary operators, both prefix or postfix
		bool visitUnaryOperator(const clang::UnaryOperator* expr);
		//! All binary operators
		bool visitBinaryOperator(const clang::BinaryOperator* expr);
		//! Ternary operator "? :"
		bool visitConditionalOperator(const clang::ConditionalOperator* expr);
		//! X= operators
		bool visitCompoundAssignOperator(const clang::CompoundAssignOperator* expr);
		//! Constructor call
		bool visitCXXConstructExpr(const clang::CXXConstructExpr* decl);
		//! All types of cast
		bool visitCastExpr(const clang::CastExpr* expr);
		//! Implicit cast
		bool visitImplicitCastExpr(const clang::ImplicitCastExpr* expr);
		//! All explicit casts : c-style or c++-style
		bool visitExplicitCastExpr(const clang::ExplicitCastExpr* expr);
		//! Parentheses
		bool visitParenExpr(const clang::ParenExpr* expr);
		//! sizeof / alignof
		bool visitUnaryExprOrTypeTraitExpr(const clang::UnaryExprOrTypeTraitExpr* expr);

		bool visitCXXBoolLiteralExpr(const clang::CXXBoolLiteralExpr* expr);
		bool visitCharacterLiteral(const clang::CharacterLiteral* stmt);
		bool visitStringLiteral(const clang::StringLiteral* stmt);
		bool visitIntegerLiteral(const clang::IntegerLiteral* stmt);
		bool visitFloatingLiteral(const clang::FloatingLiteral* stmt);

		//! Dispatch the visitor to the correct dynamic type for this declaration
		bool visitRealDeclType(const clang::Decl* decl);
		//! Dispatch the visitor to the correct dynamic type for this statement / expr
		bool visitRealStmtType(const clang::Stmt* stmt);

		Yuni::String convertType(const clang::QualType& type) const;
		Yuni::String convertType(const clang::Type* type) const;

		//! Get the type size in bits
		uint64_t typeSize(const clang::Type* type) const;
		uint64_t typeSize(const clang::QualType& type) const;
		//! Is a given expression of pointer type ?
		bool isPtrType(const clang::Expr* expr) const;

		//! Write a single character literal, with proper escaping
		void writeCharacter(char c);

		bool definedInProject(const clang::Decl* decl) const
		{
			return isProjectFile(pContext->getSourceManager().getFileID(decl->getLocation()));
		}

		bool definedInProject(const clang::Stmt* stmt) const
		{
			return isProjectFile(pContext->getSourceManager().getFileID(stmt->getLocStart()));
		}

		void addProjectFile(clang::FileID id)
		{
			unsigned hash = id.getHashValue();
			if (pProjectFiles.end() == pProjectFiles.find(hash))
				pProjectFiles[hash] = true;
		}

		bool isProjectFile(clang::FileID id) const
		{
			// Search the cache first
			const auto it = pProjectFiles.find(id.getHashValue());
			if (pProjectFiles.end() != it)
				return it->second;

			// Check that the file's absolute path starts with the main file's absolute path
			const clang::FileEntry* fileEntry = pContext->getSourceManager().getFileEntryForID(id);
			if (not fileEntry)
				return false;
			Yuni::String path;
			Yuni::IO::ExtractAbsoluteFilePath(path, fileEntry->getName());
			bool isInProjectDirectory = path.startsWith(pMainFilePath);
			pProjectFiles[id.getHashValue()] = isInProjectDirectory;
			return isInProjectDirectory;
		}

	private:
		//! AST Context holds additional separate information about the AST
		clang::ASTContext* pContext;
		//! Logger
		Yuni::Logs::Logger<> pLog;
		//! Indenting management
		Indenter pIndent;
		//! Use this to mark when a future simple expression is actually used as a statement
		bool pStatementStart;
		//! Store the absolute path to the main file for this compilation unit
		Yuni::String pMainFilePath;
		//! Cache file Ids with a boolean to distinguish files from the project (true) from library includes (false)
		mutable Yuni::Dictionary<unsigned, bool>::Hash pProjectFiles;

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
