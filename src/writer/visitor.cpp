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
			// ParmVarDecl nodes are visited on-the-fly during function decl / call
			return true;
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
			// DeclStmts are ignored
			// They are redundant with simple decls and stmts
			return true;
		case clang::Stmt::CompoundStmtClass:
			return visitCompoundStmt(static_cast<clang::CompoundStmt*>(stmt));
		case clang::Stmt::ReturnStmtClass:
			return visitReturnStmt(static_cast<clang::ReturnStmt*>(stmt));
		case clang::Stmt::DeclRefExprClass:
			return visitDeclRefExpr(static_cast<clang::DeclRefExpr*>(stmt));
		case clang::Stmt::CallExprClass:
			return visitCallExpr(static_cast<clang::CallExpr*>(stmt));
		case clang::Stmt::UnaryOperatorClass:
			return visitUnaryOperator(static_cast<clang::UnaryOperator*>(stmt));
		case clang::Stmt::CXXConstructExprClass:
			return visitCXXConstructExpr(static_cast<clang::CXXConstructExpr*>(stmt));
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
		// Specific VarDecls are managed in each specific derived class
		// Only manage function-local decls here
		if (!decl->isLocalVarDecl())
			return true;

		writeIndent();
		if (decl->isStaticDataMember())
			std::cout << "class ";
		std::cout << (decl->isConstexpr() || decl->getType().isConstant(*pContext) ? "const " : "var ") << decl->getNameAsString();
		if (not decl->hasInit())
		{
			// If we do not have an initializer, we need a type
			std::cout << " : ";
			visitType(decl->getType());
		}
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

		writeIndent();
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
		increaseIndent();
		if (not visitDeclContext(decl))
			return false;
		if (decl->hasBody())
			if (not visitStmt(decl->getBody()))
				return false;
		decreaseIndent();
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

		writeIndent();
		std::cout << "typedef " << decl->getNameAsString() << " : ";
		visitType(decl->getUnderlyingType());
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

	bool NanyConverterVisitor::visitReturnStmt(clang::ReturnStmt* stmt)
	{
		std::cerr << ">> ReturnStmt" << std::endl;
		writeIndent();
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


	bool NanyConverterVisitor::visitDeclRefExpr(clang::DeclRefExpr* expr)
	{
		std::cerr << ">> DeclRefExpr" << std::endl;
		if (not expr)
			return true;

		std::cout << expr->getFoundDecl()->getNameAsString();
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
		std::cout << ")";
		return true;
	}

	bool NanyConverterVisitor::visitUnaryOperator(clang::UnaryOperator* expr)
	{
		std::cerr << ">> UnaryOperator" << std::endl;
		if (not expr)
			return true;

		const char* op = "";
		switch (expr->getOpcode())
		{
			case clang::UO_PreInc:
			case clang::UO_PostInc:
				op = "++";
				break;
			case clang::UO_PreDec:
			case clang::UO_PostDec:
				op = "--";
				break;
			case clang::UO_Plus:
				op = "+";
				break;
			case clang::UO_Minus:
				op = "-";
				break;
			case clang::UO_Not:
			case clang::UO_LNot:
				op = "not ";
				break;
			// UO_AddrOf (&), UO_Deref (*), ...
			default:
				std::cout << "Unary operator is not yet implemented !" << std::endl;
				break;
		}
		if (not expr->isPostfix())
			std::cout << op;
		visitStmt(expr->getSubExpr());
		if (expr->isPostfix())
			std::cout << op;
		return true;
	}

	bool NanyConverterVisitor::visitCXXConstructExpr(clang::CXXConstructExpr* expr)
	{
		std::cerr << ">> CXXConstructExpr" << std::endl;
		std::cout << "new " << expr->getConstructor()->getNameAsString() << '(';
		uint i = 0;
		for (const auto& arg : expr->arguments())
		{
			visitStmt(arg);
			if (i++ + 1 < expr->getNumArgs())
				std::cout << ", ";
		}
		std::cout << ")";
		return true;
	}

	bool NanyConverterVisitor::visitCastExpr(clang::CastExpr* expr)
	{
		std::cout << "new ";
		visitType(expr->getType());
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
		std::cerr << ">> CharacterLiteral" << std::endl;
		char c = static_cast<char>(stmt->getValue());
		std::cout << '\'';
		switch (c)
		{
			case '\a':
				std::cout << "\\a";
				break;
			case '\b':
				std::cout << "\\b";
				break;
			case '\f':
				std::cout << "\\f";
				break;
			case '\t':
				std::cout << "\\t";
				break;
			case '\v':
				std::cout << "\\v";
				break;
			case '\r':
				std::cout << "\\r";
				break;
			case '\n':
				std::cout << "\\n";
				break;
			case '\\':
				std::cout << "\\\\";
				break;
			case '\'':
				std::cout << "\\\'";
				break;
			case '\"':
				std::cout << "\\\"";
				break;
			case '\?':
				std::cout << "\\\?";
				break;
			default:
				std::cout << c;
				break;
		}
		std::cout << '\'';
		return true;
	}

	bool NanyConverterVisitor::visitStringLiteral(clang::StringLiteral* stmt)
	{
		std::cerr << ">> StringLiteral" << std::endl;
		std::cout << '"' << stmt->getString().data() << '"';
		return true;
	}

	bool NanyConverterVisitor::visitIntegerLiteral(clang::IntegerLiteral* stmt)
	{
		std::cerr << ">> IntegerLiteral" << std::endl;
		std::cout << *(stmt->getValue().getRawData());
		return true;
	}

	bool NanyConverterVisitor::visitFloatingLiteral(clang::FloatingLiteral* stmt)
	{
		std::cerr << ">> FloatingLiteral" << std::endl;
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
		std::cerr << ">> Type" << std::endl;
		if (not type)
			return true;
		std::cout << convertType(type);
		return true;
	}

	bool NanyConverterVisitor::visitType(const clang::QualType& type)
	{
		std::cerr << ">> QualType" << std::endl;
		std::cout << convertType(type.getTypePtr());
		return true;
	}

	Yuni::String NanyConverterVisitor::convertType(const clang::QualType& type) const
	{
		return convertType(type.getTypePtr());
	}

	Yuni::String NanyConverterVisitor::convertType(const clang::Type* type) const
	{
		if (type->isScalarType())
		{
			switch (type->getScalarTypeKind())
			{
			case clang::Type::STK_Bool:
				return "__bool";
			case clang::Type::STK_Integral:
				if (type->isSignedIntegerType())
					return "__i32";
				else
					return "__u32";
			case clang::Type::STK_Floating:
				return "__f32";
			case clang::Type::STK_IntegralComplex:
				return "std.cplx<__i32>";
			case clang::Type::STK_FloatingComplex:
				return "std.cplx<__f32>";
			case clang::Type::STK_CPointer:
			{
				const auto& pointee = type->getPointeeType();
				return Yuni::String("std.c.pointer<") << convertType(pointee) << '>';
			}
			case clang::Type::STK_BlockPointer:
			case clang::Type::STK_ObjCObjectPointer:
			case clang::Type::STK_MemberPointer:
			default:
				std::cerr << "Unmanaged Scalar kind for type \"" << type->getTypeClassName() << "\"" << std::endl;
				return type->getTypeClassName();
			}
		}
		return type->getTypeClassName();
	}


} // namespace NanyFromC