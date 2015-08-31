#include "visitor.h"

namespace NanyFromC
{

	bool NanyConverterVisitor::VisitDecl(clang::Decl* decl)
	{
		//decl->dump();
		return true;
	}

	bool NanyConverterVisitor::VisitVarDecl(clang::VarDecl* decl)
	{
		// Do not dump forward declarations
		if (not decl->isThisDeclarationADefinition())
			return true;
		// Specific vardecls are managed in each specific derived class
		// Only manage function-local decls here
		if (!decl->isLocalVarDecl())
			return true;

		if (decl->isStaticDataMember())
			std::cout << "class ";
		std::cout << (decl->isConstexpr() ? "const " : "var ") << decl->getNameAsString();
		if (not decl->hasInit())
			// If we do not have an initializer, we need a type
			std::cout << " : " << clang::QualType::getAsString(decl->getType().split());
		else
		{
			std::cout << " = ";
			TraverseStmt(decl->getInit());
		}
		std::cout << ";\n";
		return false;
	}

	bool NanyConverterVisitor::VisitParmVarDecl(clang::ParmVarDecl* decl)
	{
		std::cout << decl->getNameAsString();
		return true;
	}

	bool NanyConverterVisitor::VisitCXXMethodDecl(clang::CXXMethodDecl* decl)
	{
		if (decl->isStatic())
			std::cout << "class ";
		std::cout << "func " << decl->getNameAsString();
		if (not decl->isNoReturn())
			std::cout << " : " << clang::QualType::getAsString(decl->getReturnType().split());
		std::cout << "\n{" << "}\n";
		return false;
	}

	bool NanyConverterVisitor::VisitFunctionDecl(clang::FunctionDecl* decl)
	{
		std::cout << "func " << decl->getNameAsString();
		if (decl->param_size() > 0)
		{
			uint i = 0;
			std::cout << "(";
			for (const auto& param : decl->params())
			{
				TraverseParmVarDecl(param);
				if (i++ + 1 < decl->param_size())
					std::cout << ", ";
			}
			std::cout << ")";
		}
		if (not decl->isNoReturn())
			std::cout << " : " << clang::QualType::getAsString(decl->getReturnType().split());
		std::cout << "\n{\n";
		if (decl->hasBody())
			TraverseStmt(decl->getBody());
		std::cout << "}\n";
		return false;
	}

	bool NanyConverterVisitor::VisitRecordDecl(clang::RecordDecl* decl)
	{
		return true;
	}

	bool NanyConverterVisitor::VisitCXXRecordDecl(clang::CXXRecordDecl* decl)
	{
		if (decl->getPreviousDecl() != nullptr)
			return true;
		std::cout << "class " << decl->getDefinition()->getNameAsString();
		std::cout << "\n{\n";
		std::cout << "}\n";
		return true;
	}


	bool NanyConverterVisitor::VisitCallExpr(clang::CallExpr* expr)
	{
		if (expr->getCallee() != nullptr)
			std::cout << expr->getCalleeDecl()->getAsFunction()->getNameAsString() << '(';
		uint i = 0;
		for (const auto& arg : expr->arguments())
		{
			TraverseStmt(arg);
			if (i++ + 1 < expr->getNumArgs())
				std::cout << ", ";
		}
		std::cout << ");";
		return true;
	}

	bool NanyConverterVisitor::VisitStmt(clang::Stmt* stmt)
	{
		return true;
	}

	bool NanyConverterVisitor::VisitReturnStmt(clang::ReturnStmt* stmt)
	{
		std::cout << "return ";
		TraverseStmt(stmt->getRetValue());
		std::cout << ";\n";
		return false;
	}

	bool NanyConverterVisitor::VisitCharacterLiteral(clang::CharacterLiteral* stmt)
	{
		std::cout << stmt->getValue();
		return true;
	}

	bool NanyConverterVisitor::VisitIntegerLiteral(clang::IntegerLiteral* stmt)
	{
		std::cout << *(stmt->getValue().getRawData());
		return true;
	}


} // namespace NanyFromC