#include "visitor.h"

namespace NanyFromC
{

	bool NanyConverterVisitor::VisitDecl(clang::Decl* decl)
	{
		std::cerr << ">> Decl" << std::endl;
		if (!decl)
			return true;

		std::cerr << ">> Not null" << std::endl;
		// As a syntax visitor, we want to ignore declarations for
		// implicitly-defined declarations (ones not typed explicitly by the
		// user).
		if (decl->isImplicit())
			return true;

		std::cerr << ">> Not implicit" << std::endl;

		clang::Decl* child;
		while ((child = decl->getNextDeclInContext()) != nullptr)
		{
			std::cerr << ">> Next is : " << child->getDeclKindName() << std::endl;
			switch (child->getKind())
			{
			case clang::Decl::Var:
				if (!VisitVarDecl(static_cast<clang::VarDecl*>(child)))
					return false;
				else
					break;
			case clang::Decl::ParmVar:
				VisitParmVarDecl(static_cast<clang::ParmVarDecl*>(child));
				return true;
			case clang::Decl::Function:
				VisitFunctionDecl(static_cast<clang::FunctionDecl*>(child));
				return true;
			case clang::Decl::CXXMethod:
				VisitCXXMethodDecl(static_cast<clang::CXXMethodDecl*>(child));
				return true;
			case clang::Decl::Record:
				VisitRecordDecl(static_cast<clang::RecordDecl*>(child));
				return true;
			case clang::Decl::CXXRecord:
				VisitCXXRecordDecl(static_cast<clang::CXXRecordDecl*>(child));
				return true;
			case clang::Decl::Typedef:
				VisitTypedefDecl(static_cast<clang::TypedefDecl*>(child));
				return true;
			}
		}

		std::cerr << ">> No child managed" << std::endl;

		// Visit any attributes attached to this declaration.
		for (auto* attr : decl->attrs())
		{
			if (not VisitAttr(attr))
				return false;
		}

		if (decl->hasBody())
			VisitStmt(decl->getBody());
		return true;
	}

	bool NanyConverterVisitor::VisitTranslationUnitDecl(clang::TranslationUnitDecl* decl)
	{
		std::cerr << ">> TranslationUnitDecl" << std::endl;
		VisitDecl(decl);
		return true;
	}

	bool NanyConverterVisitor::VisitVarDecl(clang::VarDecl* decl)
	{
		std::cerr << ">> VarDecl" << std::endl;

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
			VisitStmt(decl->getInit());
		}
		std::cout << ";\n";
		return true;
	}

	bool NanyConverterVisitor::VisitParmVarDecl(clang::ParmVarDecl* decl)
	{
		std::cerr << ">> ParmVarDecl" << std::endl;
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
				VisitParmVarDecl(param);
				if (i++ + 1 < decl->param_size())
					std::cout << ", ";
			}
			std::cout << ")";
		}
		if (not decl->isNoReturn())
			std::cout << " : " << clang::QualType::getAsString(decl->getReturnType().split());
		std::cout << "\n{\n";
		if (decl->hasBody())
			VisitStmt(decl->getBody());
		std::cout << "}\n";
		return true;
	}

	bool NanyConverterVisitor::VisitTypedefDecl(clang::TypedefDecl* decl)
	{
		std::cerr << ">> TypedefDecl" << std::endl;

		std::cout << "typedef " << decl->getNameAsString() << " : ";
		VisitType(decl->getTypeForDecl());
		return true;
	}

	bool NanyConverterVisitor::VisitRecordDecl(clang::RecordDecl* decl)
	{
		if (decl->getPreviousDecl() != nullptr)
			return true;
		std::cout << "[[C]]\nclass "
			<< decl->getDefinition()->getNameAsString()
			<< "\n{\n";
		for (auto* field : decl->fields())
		{
			VisitDecl(field);
		}
		std::cout << "}\n";
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
			VisitStmt(arg);
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
		VisitStmt(stmt->getRetValue());
		std::cout << ";\n";
		return true;
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



	bool NanyConverterVisitor::VisitAttr(clang::Attr *attr)
	{
		return true;
	}

	bool NanyConverterVisitor::VisitType(const clang::Type* type)
	{
		std::cout << type->getTypeClassName();
		return true;
	}


} // namespace NanyFromC