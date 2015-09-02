#include "visitor.h"

namespace NanyFromC
{

	bool NanyConverterVisitor::visitChildren(clang::Decl* decl)
	{
		return visitDeclContext(decl->getDeclContext());
	}

	bool NanyConverterVisitor::visitRealDeclType(clang::Decl* decl)
	{
		if (not decl)
			return true;

		switch (decl->getKind())
		{
		case clang::Decl::Var:
			return visitVarDecl(static_cast<clang::VarDecl*>(decl));
		case clang::Decl::ParmVar:
			return true; // ParmVarDecl nodes are visited on-the-fly during function decl / call
		case clang::Decl::Function:
			return visitFunctionDecl(static_cast<clang::FunctionDecl*>(decl));
		case clang::Decl::CXXMethod:
			return visitCXXMethodDecl(static_cast<clang::CXXMethodDecl*>(decl));
		case clang::Decl::Record:
			return visitRecordDecl(static_cast<clang::RecordDecl*>(decl));
		case clang::Decl::CXXRecord:
			return visitCXXRecordDecl(static_cast<clang::CXXRecordDecl*>(decl));
		case clang::Decl::Typedef:
			return visitTypedefDecl(static_cast<clang::TypedefDecl*>(decl));
		default:
			std::cerr << "Node \"" << decl->getDeclKindName() << "\" is not yet implemented !" << std::endl;
			return true;
		}
		return true;
	}

	bool NanyConverterVisitor::visitRealStmtType(clang::Stmt* stmt)
	{
		if (not stmt)
			return true;

		switch (stmt->getStmtClass())
		{
		case clang::Stmt::NoStmtClass:
			break;
		case clang::Stmt::DeclStmtClass:
			return visitDeclStmt(static_cast<clang::DeclStmt*>(stmt));
		case clang::Stmt::CompoundStmtClass:
			return visitCompoundStmt(static_cast<clang::CompoundStmt*>(stmt));
		case clang::Stmt::ReturnStmtClass:
			return visitReturnStmt(static_cast<clang::ReturnStmt*>(stmt));
		case clang::Stmt::ImplicitCastExprClass:
			return visitImplicitCastExpr(static_cast<clang::ImplicitCastExpr*>(stmt));
		case clang::Stmt::CStyleCastExprClass:
		case clang::Stmt::CXXFunctionalCastExprClass:
		case clang::Stmt::CXXStaticCastExprClass:
			return visitExplicitCastExpr(static_cast<clang::ExplicitCastExpr*>(stmt));
		case clang::Stmt::CharacterLiteralClass:
			return visitCharacterLiteral(static_cast<clang::CharacterLiteral*>(stmt));
		case clang::Stmt::StringLiteralClass:
			return visitStringLiteral(static_cast<clang::StringLiteral*>(stmt));
		case clang::Stmt::IntegerLiteralClass:
			return visitIntegerLiteral(static_cast<clang::IntegerLiteral*>(stmt));
		case clang::Stmt::FloatingLiteralClass:
			return visitFloatingLiteral(static_cast<clang::FloatingLiteral*>(stmt));
		default:
			std::cerr << "Node \"" << stmt->getStmtClassName() << "\" is not yet implemented !" << std::endl;
			return true;
		}
		return true;
	}

	bool NanyConverterVisitor::visitDecl(clang::Decl* decl)
	{
		if (not decl)
			return true;

		// As a syntax visitor, we want to ignore declarations for
		// implicitly-defined declarations (ones not typed explicitly by the
		// user).
		if (decl->isImplicit())
			return true;

		if (!visitChildren(decl))
			return false;

		// Visit any attributes attached to this declaration.
		for (auto* attr : decl->attrs())
		{
			if (not visitAttr(attr))
				return false;
		}
		return true;
	}

	bool NanyConverterVisitor::visitDeclContext(clang::DeclContext* context)
	{
		if (not context)
			return true;

		for (auto* child : context->decls())
		{
			// BlockDecls and CapturedDecls are traversed through BlockExprs and
			// CapturedStmts respectively.
			if (!llvm::isa<clang::BlockDecl>(child) && !llvm::isa<clang::CapturedDecl>(child))
				visitRealDeclType(child);
		}
		return true;
	}

	bool NanyConverterVisitor::visitTranslationUnitDecl(clang::TranslationUnitDecl* decl)
	{
		std::cerr << ">> TranslationUnitDecl" << std::endl;
		return visitDeclContext(decl);
	}

	bool NanyConverterVisitor::visitVarDecl(clang::VarDecl* decl)
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
			visitStmt(decl->getInit());
		}
		std::cout << ";\n";
		return true;
	}

	bool NanyConverterVisitor::visitParmVarDecl(clang::ParmVarDecl* decl)
	{
		std::cerr << ">> ParmVarDecl" << std::endl;
		std::cout << decl->getNameAsString();
		return true;
	}

	bool NanyConverterVisitor::visitFunctionDecl(clang::FunctionDecl* decl)
	{
		std::cerr << ">> FunctionDecl" << std::endl;
		if (not decl)
			return true;
		// Ignore forward declarations
		if (not decl->isThisDeclarationADefinition())
			return true;

		std::cout << "public func " << decl->getNameAsString();
		if (decl->param_size() > 0)
		{
			uint i = 0;
			std::cout << "(";
			for (const auto& param : decl->params())
			{
				visitParmVarDecl(param);
				if (i++ + 1 < decl->param_size())
					std::cout << ", ";
			}
			std::cout << ")";
		}
		if (not decl->isNoReturn())
			std::cout << " : " << clang::QualType::getAsString(decl->getReturnType().split());
		std::cout << "\n{\n";
		if (not visitDeclContext(decl))
			return false;
		if (decl->hasBody())
			if (not visitStmt(decl->getBody()))
			return false;
		std::cout << "}\n\n";
		return true;
	}

	bool NanyConverterVisitor::visitCXXMethodDecl(clang::CXXMethodDecl* decl)
	{
		std::cerr << ">> CXXMethodDecl" << std::endl;
		return visitFunctionDecl(decl);
	}

	bool NanyConverterVisitor::visitTypedefDecl(clang::TypedefDecl* decl)
	{
		std::cerr << ">> TypedefDecl" << std::endl;

		std::cout << "typedef " << decl->getNameAsString() << " : ";
		//visitType(decl->getTypeForDecl());
		std::cout << ";\n";
		return true;
	}

	bool NanyConverterVisitor::visitRecordDecl(clang::RecordDecl* decl)
	{
		if (not decl)
			return true;
		// Ignore forward declarations
		if (not decl->isThisDeclarationADefinition())
			return true;

		if (not llvm::isa<clang::CXXRecordDecl>(decl))
			std::cout << "[[C]]\n";
		if (decl->isInAnonymousNamespace())
			std::cout << "private ";
		else
			std::cout << "public ";
		std::cout << "class "
			<< decl->getDefinition()->getNameAsString()
			<< "\n{\n";
		for (auto* field : decl->fields())
		{
			visitDecl(field);
		}
		std::cout << "}\n";
		return true;
	}

	bool NanyConverterVisitor::visitCXXRecordDecl(clang::CXXRecordDecl* decl)
	{
		std::cerr << ">> CXXRecordDecl" << std::endl;
		return visitRecordDecl(decl);
	}


	bool NanyConverterVisitor::visitStmt(clang::Stmt* stmt)
	{
		return visitRealStmtType(stmt);
	}

	bool NanyConverterVisitor::visitDeclStmt(clang::DeclStmt* stmt)
	{
		if (not stmt)
			return true;

		for (const auto& child : stmt->children())
		{
			visitStmt(child);
		}
		return true;
	}

	bool NanyConverterVisitor::visitReturnStmt(clang::ReturnStmt* stmt)
	{
		std::cerr << ">> ReturnStmt" << std::endl;
		std::cout << "return ";
		visitStmt(stmt->getRetValue());
		std::cout << ";\n";
		return true;
	}

	bool NanyConverterVisitor::visitCompoundStmt(clang::CompoundStmt* stmt)
	{
		std::cerr << ">> CompoundStmt" << std::endl;
		if (not stmt)
			return true;

		for (auto* child : stmt->body())
		{
			visitStmt(child);
		}
		return true;
	}


	bool NanyConverterVisitor::visitCallExpr(clang::CallExpr* expr)
	{
		std::cerr << ">> CallExpr" << std::endl;
		if (not expr)
			return true;

		if (expr->getCallee() != nullptr)
			std::cout << expr->getCalleeDecl()->getAsFunction()->getNameAsString() << '(';
		uint i = 0;
		for (const auto& arg : expr->arguments())
		{
			visitStmt(arg);
			if (i++ + 1 < expr->getNumArgs())
				std::cout << ", ";
		}
		std::cout << ");";
		return true;
	}

	bool NanyConverterVisitor::visitCastExpr(clang::CastExpr* expr)
	{
		std::cout << "new ";
		// TODO : Get the type
		std::cout << "(";
		visitStmt(expr->getSubExpr());
		std::cout << ")";
		return true;
	}

	bool NanyConverterVisitor::visitImplicitCastExpr(clang::ImplicitCastExpr* expr)
	{
		std::cerr << ">> ImplicitCastExpr" << std::endl;
		return visitCastExpr(expr);
	}

	bool NanyConverterVisitor::visitExplicitCastExpr(clang::ExplicitCastExpr* expr)
	{
		std::cerr << ">> ExplicitCastExpr" << std::endl;
		return visitCastExpr(expr);
	}


	bool NanyConverterVisitor::visitCharacterLiteral(clang::CharacterLiteral* stmt)
	{
		std::cout << stmt->getValue();
		return true;
	}

	bool NanyConverterVisitor::visitStringLiteral(clang::StringLiteral* stmt)
	{
		std::cout << '"' << stmt->getString().data() << '"';
		return true;
	}

	bool NanyConverterVisitor::visitIntegerLiteral(clang::IntegerLiteral* stmt)
	{
		std::cout << *(stmt->getValue().getRawData());
		return true;
	}

	bool NanyConverterVisitor::visitFloatingLiteral(clang::FloatingLiteral* stmt)
	{
		llvm::SmallVector<char, 32> dest;
		stmt->getValue().toString(dest);
		std::cout << std::string(dest.begin(), dest.end());
		return true;
	}



	bool NanyConverterVisitor::visitAttr(clang::Attr *attr)
	{
		return true;
	}

	bool NanyConverterVisitor::visitType(const clang::Type* type)
	{
		if (not type)
			return true;
		std::cout << type->getTypeClassName();
		return true;
	}


} // namespace NanyFromC