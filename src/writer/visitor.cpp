#include "visitor.h"
#include <iostream>



namespace NanyFromC
{


	bool NanyConverterVisitor::visitRealDeclType(const clang::Decl* decl)
	{
		if (not decl)
			return true;

		switch (decl->getKind())
		{
		case clang::Decl::Var:
			return visitVarDecl(static_cast<const clang::VarDecl*>(decl));
		case clang::Decl::ParmVar:
			// ParmVarDecl nodes are visited on-the-fly during function decl / call
			return true;
		case clang::Decl::Field:
			return visitFieldDecl(static_cast<const clang::FieldDecl*>(decl));
		case clang::Decl::Function:
			return visitFunctionDecl(static_cast<const clang::FunctionDecl*>(decl));
		case clang::Decl::CXXConstructor:
			return visitCXXConstructorDecl(static_cast<const clang::CXXConstructorDecl*>(decl));
		case clang::Decl::CXXDestructor:
			return visitCXXDestructorDecl(static_cast<const clang::CXXDestructorDecl*>(decl));
		case clang::Decl::CXXMethod:
			return visitCXXMethodDecl(static_cast<const clang::CXXMethodDecl*>(decl));
		case clang::Decl::Record:
			return visitRecordDecl(static_cast<const clang::RecordDecl*>(decl));
		case clang::Decl::CXXRecord:
			return visitCXXRecordDecl(static_cast<const clang::CXXRecordDecl*>(decl));
		case clang::Decl::Typedef:
			return visitTypedefDecl(static_cast<const clang::TypedefDecl*>(decl));
		case clang::Decl::AccessSpec:
			return visitAccessSpecDecl(static_cast<const clang::AccessSpecDecl*>(decl));
		case clang::Decl::Enum:
			return visitEnumDecl(static_cast<const clang::EnumDecl*>(decl));
		case clang::Decl::EnumConstant:
			return visitEnumConstantDecl(static_cast<const clang::EnumConstantDecl*>(decl));
		default:
			std::cerr << "Node \"" << decl->getDeclKindName() << "\" is not yet implemented !" << std::endl;
			return true;
		}
		return true;
	}

	bool NanyConverterVisitor::visitRealStmtType(const clang::Stmt* stmt)
	{
		if (not stmt)
			return true;

		switch (stmt->getStmtClass())
		{
		case clang::Stmt::NoStmtClass:
			break;
		case clang::Stmt::DeclStmtClass:
			return visitDeclStmt(static_cast<const clang::DeclStmt*>(stmt));
		case clang::Stmt::CompoundStmtClass:
			return visitCompoundStmt(static_cast<const clang::CompoundStmt*>(stmt));
		case clang::Stmt::ReturnStmtClass:
			return visitReturnStmt(static_cast<const clang::ReturnStmt*>(stmt));
		case clang::Stmt::IfStmtClass:
			return visitIfStmt(static_cast<const clang::IfStmt*>(stmt));
		case clang::Stmt::WhileStmtClass:
			return visitWhileStmt(static_cast<const clang::WhileStmt*>(stmt));
		case clang::Stmt::ForStmtClass:
			return visitForStmt(static_cast<const clang::ForStmt*>(stmt));
		case clang::Stmt::DeclRefExprClass:
			return visitDeclRefExpr(static_cast<const clang::DeclRefExpr*>(stmt));
		case clang::Stmt::MemberExprClass:
			return visitMemberExpr(static_cast<const clang::MemberExpr*>(stmt));
		case clang::Stmt::CallExprClass:
			return visitCallExpr(static_cast<const clang::CallExpr*>(stmt));
		case clang::Stmt::CXXNewExprClass:
			return visitCXXNewExpr(static_cast<const clang::CXXNewExpr*>(stmt));
		case clang::Stmt::CXXDeleteExprClass:
			return visitCXXDeleteExpr(static_cast<const clang::CXXDeleteExpr*>(stmt));
		case clang::Stmt::UnaryOperatorClass:
			return visitUnaryOperator(static_cast<const clang::UnaryOperator*>(stmt));
		case clang::Stmt::BinaryOperatorClass:
			return visitBinaryOperator(static_cast<const clang::BinaryOperator*>(stmt));
		case clang::Stmt::ConditionalOperatorClass:
			return visitConditionalOperator(static_cast<const clang::ConditionalOperator*>(stmt));
		case clang::Stmt::CompoundAssignOperatorClass:
			return visitCompoundAssignOperator(static_cast<const clang::CompoundAssignOperator*>(stmt));
		case clang::Stmt::CXXConstructExprClass:
			return visitCXXConstructExpr(static_cast<const clang::CXXConstructExpr*>(stmt));
		case clang::Stmt::ImplicitCastExprClass:
			return visitImplicitCastExpr(static_cast<const clang::ImplicitCastExpr*>(stmt));
		case clang::Stmt::CStyleCastExprClass:
		case clang::Stmt::CXXFunctionalCastExprClass:
		case clang::Stmt::CXXStaticCastExprClass:
			return visitExplicitCastExpr(static_cast<const clang::ExplicitCastExpr*>(stmt));
		case clang::Stmt::ParenExprClass:
			return visitParenExpr(static_cast<const clang::ParenExpr*>(stmt));
		case clang::Stmt::CXXBoolLiteralExprClass:
			return visitCXXBoolLiteralExpr(static_cast<const clang::CXXBoolLiteralExpr*>(stmt));
		case clang::Stmt::CharacterLiteralClass:
			return visitCharacterLiteral(static_cast<const clang::CharacterLiteral*>(stmt));
		case clang::Stmt::StringLiteralClass:
			return visitStringLiteral(static_cast<const clang::StringLiteral*>(stmt));
		case clang::Stmt::IntegerLiteralClass:
			return visitIntegerLiteral(static_cast<const clang::IntegerLiteral*>(stmt));
		case clang::Stmt::FloatingLiteralClass:
			return visitFloatingLiteral(static_cast<const clang::FloatingLiteral*>(stmt));
		default:
			std::cerr << "Node \"" << stmt->getStmtClassName() << "\" is not yet implemented !" << std::endl;
			return true;
		}
		return true;
	}

	bool NanyConverterVisitor::visitDecl(const clang::Decl* decl)
	{
		if (not decl)
			return true;

		// As a syntax visitor, we want to ignore declarations for
		// implicitly-defined declarations (ones not typed explicitly by the
		// user).
		if (decl->isImplicit())
			return true;

		if (!visitRealDeclType(decl))
			return true;

		// Visit any attributes attached to this declaration.
		for (auto* attr : decl->attrs())
		{
			if (not visitAttr(attr))
				return false;
		}
		return true;
	}

	bool NanyConverterVisitor::visitDeclContext(const clang::DeclContext* context)
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

	bool NanyConverterVisitor::visitTranslationUnitDecl(const clang::TranslationUnitDecl* decl)
	{
		pLog.debug() << "TranslationUnitDecl";
		return visitDeclContext(decl);
	}

	bool NanyConverterVisitor::visitVarDecl(const clang::VarDecl* decl)
	{
		pLog.debug() << "VarDecl";

		// Do not dump forward declarations
		if (not decl || not decl->isThisDeclarationADefinition())
			return true;
		// Specific VarDecls are managed in each specific derived class
		// Only manage function-local decls here
		if (!decl->isLocalVarDecl())
			return true;

		std::cout << pIndent;
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

	bool NanyConverterVisitor::visitParmVarDecl(const clang::ParmVarDecl* decl)
	{
		pLog.debug() << "ParmVarDecl";
		if (not decl)
			return true;
		std::cout << decl->getNameAsString();
		return true;
	}

	bool NanyConverterVisitor::visitFieldDecl(const clang::FieldDecl* decl)
	{
		pLog.debug() << "FieldDecl";
		if (not decl)
			return true;
		std::cout << pIndent << pVisibilities.current()
			<< (decl->getType().isConstant(*pContext) ? " const " : " var ")
			<< decl->getNameAsString() << ";\n";
		return true;
	}

	bool NanyConverterVisitor::visitFunctionDecl(const clang::FunctionDecl* decl)
	{
		pLog.debug() << "FunctionDecl";
		if (not decl)
			return true;
		// Ignore forward declarations
		if (not decl->isThisDeclarationADefinition())
			return true;

		std::cout << pIndent << pVisibilities.current() << " func " << decl->getNameAsString();
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
		std::cout << '\n' << pIndent << "{\n";
		++pIndent;
		if (decl->getBody())
			if (not visitStmt(decl->getBody()))
				return false;
		--pIndent;
		std::cout << pIndent << "}\n\n";
		return true;
	}

	bool NanyConverterVisitor::visitCXXConstructorDecl(const clang::CXXConstructorDecl* decl)
	{
		pLog.debug() << "CXXConstructorDecl";
		if (not decl)
			return true;
		// Ignore forward declarations
		if (not decl->isThisDeclarationADefinition())
			return true;
		// Ignore default constructors
		if (decl->isDefaultConstructor())
			return true;

		std::cout << pIndent << pVisibilities.current() << " func new";
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
		std::cout << '\n' << pIndent << "{\n";
		++pIndent;
		if (not visitDeclContext(decl))
			return false;
		if (decl->hasBody())
			if (not visitStmt(decl->getBody()))
				return false;
		--pIndent;
		std::cout << pIndent << "}\n\n";
		return true;
	}

	bool NanyConverterVisitor::visitCXXDestructorDecl(const clang::CXXDestructorDecl* decl)
	{
		pLog.debug() << "CXXDestructorDecl";
		if (not decl)
			return true;
		// Ignore forward declarations
		if (not decl->isThisDeclarationADefinition())
			return true;

		std::cout << pIndent << pVisibilities.current() << " func delete\n";
		std::cout << pIndent << "{\n";
		++pIndent;
		if (not visitDeclContext(decl))
			return false;
		if (decl->hasBody())
			if (not visitStmt(decl->getBody()))
				return false;
		--pIndent;
		std::cout << pIndent << "}\n\n";
		return true;
		return true;
	}

	bool NanyConverterVisitor::visitCXXMethodDecl(const clang::CXXMethodDecl* decl)
	{
		pLog.debug() << "CXXMethodDecl";
		return visitFunctionDecl(decl);
	}

	bool NanyConverterVisitor::visitTypedefDecl(const clang::TypedefDecl* decl)
	{
		pLog.debug() << "TypedefDecl";

		std::cout << pIndent;
		std::cout << "typedef " << decl->getNameAsString() << " : ";
		visitType(decl->getUnderlyingType());
		std::cout << ";\n";
		return true;
	}

	bool NanyConverterVisitor::visitAccessSpecDecl(const clang::AccessSpecDecl* decl)
	{
		pLog.debug() << "AccessSpecDecl";
		if (not decl)
			return true;
		switch (decl->getAccess())
		{
			case clang::AS_public:
				pVisibilities.current(Visibility::vPublic);
				break;
			case clang::AS_protected:
				pVisibilities.current(Visibility::vProtected);
				break;
			case clang::AS_private:
				pVisibilities.current(Visibility::vPrivate);
				break;
			default:
				break;
		}
		return true;
	}

	bool NanyConverterVisitor::visitEnumDecl(const clang::EnumDecl* decl)
	{
		pLog.debug() << "EnumDecl";
		if (not decl)
			return true;

		std::cout << pIndent << pVisibilities.current() << " enum " << decl->getNameAsString() << '\n';
		std::cout << pIndent << "{\n";
		++pIndent;
		visitDeclContext(decl);
		--pIndent;
		std::cout << pIndent << "}\n\n";
		return true;
	}

	bool NanyConverterVisitor::visitEnumConstantDecl(const clang::EnumConstantDecl* decl)
	{
		pLog.debug() << "EnumConstantDecl";
		if (not decl)
			return true;

		std::cout << pIndent << decl->getNameAsString();
		if (nullptr != decl->getInitExpr())
		{
			std::cout << " = ";
			visitStmt(decl->getInitExpr());
		}
		std::cout << ",\n";
		return true;
	}

	bool NanyConverterVisitor::visitRecordDecl(const clang::RecordDecl* decl)
	{
		if (not decl)
			return true;
		// Ignore forward declarations
		if (not decl->isThisDeclarationADefinition())
			return true;

		if (not llvm::isa<clang::CXXRecordDecl>(decl))
		{
			std::cout << pIndent;
			std::cout << "[[C]]\n";
		}
		if (decl->isInAnonymousNamespace())
			std::cout << "private ";
		else
			std::cout << "public ";
		std::cout << "class "
			<< decl->getDefinition()->getNameAsString()
			<< '\n' << pIndent << "{\n";
		++pIndent;
		// Default visibility in classes is Public
		pVisibilities.push(Visibility::vPublic);
		visitDeclContext(decl);
		pVisibilities.pop();
		--pIndent;
		std::cout << pIndent << "}\n";
		return true;
	}

	bool NanyConverterVisitor::visitCXXRecordDecl(const clang::CXXRecordDecl* decl)
	{
		pLog.debug() << "CXXRecordDecl";
		return visitRecordDecl(decl);
	}


	bool NanyConverterVisitor::visitStmt(const clang::Stmt* stmt)
	{
		return visitRealStmtType(stmt);
	}

	bool NanyConverterVisitor::visitDeclStmt(const clang::DeclStmt* stmt)
	{
		return visitDecl(stmt->getSingleDecl());
	}

	bool NanyConverterVisitor::visitReturnStmt(const clang::ReturnStmt* stmt)
	{
		pStatementStart = false;
		pLog.debug() << "ReturnStmt";
		std::cout << pIndent;
		std::cout << "return ";
		visitStmt(stmt->getRetValue());
		std::cout << ";\n";
		return true;
	}

	bool NanyConverterVisitor::visitIfStmt(const clang::IfStmt* stmt)
	{
		pLog.debug() << "IfStmt";
		pStatementStart = false;
		std::cout << pIndent << "if ";
		// An if statement may declare a variable :
		if (stmt->getConditionVariableDeclStmt())
		{
			visitStmt(stmt->getConditionVariableDeclStmt());
			std::cout << " ";
		}
		visitStmt(stmt->getCond());
		std::cout << " then\n";
		if (!llvm::isa<clang::CompoundStmt>(stmt->getThen()))
		{
			pStatementStart = true;
			++pIndent;
			visitStmt(stmt->getThen());
			--pIndent;
		}
		else
		{
			std::cout << pIndent << "{\n";
			++pIndent;
			visitStmt(stmt->getThen());
			--pIndent;
			std::cout << pIndent << "}\n";
		}
		if (stmt->getElse())
		{
			std::cout << pIndent << "else\n";
			++pIndent;
			visitStmt(stmt->getElse());
			--pIndent;
		}
		pStatementStart = false;
		return true;
	}

	bool NanyConverterVisitor::visitWhileStmt(const clang::WhileStmt* stmt)
	{
		pLog.debug() << "WhileStmt";
		if (not stmt)
			return true;

		pStatementStart = false;
		std::cout << pIndent << "while ";
		if (stmt->getConditionVariableDeclStmt())
			visitStmt(stmt->getConditionVariableDeclStmt());
		visitStmt(stmt->getCond());
		std::cout << " do\n";
		if (!llvm::isa<clang::CompoundStmt>(stmt->getBody()))
		{
			pStatementStart = true;
			++pIndent;
			visitStmt(stmt->getBody());
			--pIndent;
		}
		else
		{
			std::cout << pIndent << "{\n";
			++pIndent;
			visitStmt(stmt->getBody());
			--pIndent;
			std::cout << pIndent << "}\n";
		}
		pStatementStart = false;
		return true;
	}

	bool NanyConverterVisitor::visitForStmt(const clang::ForStmt* stmt)
	{
		pLog.debug() << "ForStmt";
		if (not stmt)
			return true;

		pStatementStart = false;
		visitStmt(stmt->getInit());
		std::cout << pIndent << "while ";
		if (stmt->getConditionVariableDeclStmt())
			visitStmt(stmt->getConditionVariableDeclStmt());
		visitStmt(stmt->getCond());
		std::cout << " do\n";
		if (!llvm::isa<clang::CompoundStmt>(stmt->getBody()) && stmt->getInc() == nullptr)
		{
			pStatementStart = true;
			++pIndent;
			visitStmt(stmt->getBody());
			--pIndent;
		}
		else
		{
			std::cout << pIndent << "{\n";
			++pIndent;
			visitStmt(stmt->getBody());
			pStatementStart = true;
			visitStmt(stmt->getInc());
			--pIndent;
			std::cout << pIndent << "}\n";
		}
		pStatementStart = false;
		return true;
	}

	bool NanyConverterVisitor::visitCompoundStmt(const clang::CompoundStmt* stmt)
	{
		pLog.debug() << "CompoundStmt";
		if (not stmt)
			return true;

		for (auto* child : stmt->body())
		{
			pStatementStart = true;
			if (!visitStmt(child))
				return true;
		}
		pStatementStart = false;
		return true;
	}


	bool NanyConverterVisitor::visitDeclRefExpr(const clang::DeclRefExpr* expr)
	{
		pLog.debug() << "DeclRefExpr";
		if (not expr)
			return true;

		std::cout << expr->getFoundDecl()->getNameAsString();
		return true;
	}

	bool NanyConverterVisitor::visitMemberExpr(const clang::MemberExpr* expr)
	{
		pLog.debug() << "MemberExpr";
		if (not expr)
			return true;

		std::cout << expr->getMemberDecl()->getNameAsString();
		return true;
	}

	bool NanyConverterVisitor::visitCallExpr(const clang::CallExpr* expr)
	{
		pLog.debug() << "CallExpr";
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

	bool NanyConverterVisitor::visitCXXNewExpr(const clang::CXXNewExpr* expr)
	{
		pLog.debug() << "CXXNewExpr";
		if (not expr)
			return true;
		std::cout << "new";
		if (expr->getConstructExpr())
		{
			std::cout << ' ';
			visitStmt(expr->getConstructExpr());
		}
		return true;
	}

	bool NanyConverterVisitor::visitCXXDeleteExpr(const clang::CXXDeleteExpr* expr)
	{
		pLog.debug() << "CXXDeleteExpr";
		if (not expr)
			return true;
		// Manual deletes have no meaning in Nany
		return true;
	}

	bool NanyConverterVisitor::visitUnaryOperator(const clang::UnaryOperator* expr)
	{
		pLog.debug() << "UnaryOperator";
		if (not expr)
			return true;

		bool isStmt = pStatementStart;
		pStatementStart = false;
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
				pLog.error() << "Unary operator is not yet implemented : \'" << clang::UnaryOperator::getOpcodeStr(expr->getOpcode()).data() << '\'';
				break;
		}
		if (isStmt)
			std::cout << pIndent;
		if (not expr->isPostfix())
			std::cout << op;
		visitStmt(expr->getSubExpr());
		if (expr->isPostfix())
			std::cout << op;
		if (isStmt)
			std::cout << ";\n";
		return true;
	}

	bool NanyConverterVisitor::visitBinaryOperator(const clang::BinaryOperator* expr)
	{
		pLog.debug() << "UnaryOperator";
		if (not expr)
			return true;

		bool isStmt = pStatementStart;
		pStatementStart = false;
		const char* op = "";
		switch (expr->getOpcode())
		{
		case clang::BO_Rem:
			op = "mod";
			break;
		case clang::BO_And:
		case clang::BO_LAnd:
			op = "and";
			break;
		case clang::BO_Xor:
			op = "xor";
			break;
		case clang::BO_Or:
		case clang::BO_LOr:
			op = "or";
			break;
		case clang::BO_PtrMemD:
		case clang::BO_PtrMemI:
		case clang::BO_Shl:
		case clang::BO_Shr:
		case clang::BO_Comma:
			std::cerr << "Binary operator \"" << expr->getOpcodeStr().data() << "\" is not yet implemented";
			return true;
		default:
			// All other binary operators are the same in Nany and can be copied directly
			op = expr->getOpcodeStr().data();
			break;
		}
		if (isStmt)
			std::cout << pIndent;
		visitStmt(expr->getLHS());
		std::cout << ' ' << op << ' ';
		visitStmt(expr->getRHS());
		if (isStmt)
			std::cout << ";\n";
		return true;
	}

	bool NanyConverterVisitor::visitConditionalOperator(const clang::ConditionalOperator* expr)
	{
		pLog.debug() << "ConditionalOperator";
		bool isStmt = pStatementStart;
		pStatementStart = false;
		if (isStmt)
			std::cout << pIndent;
		std::cout << "if ";
		visitStmt(expr->getCond());
		std::cout << " then ";
		visitStmt(expr->getLHS());
		std::cout << " else ";
		visitStmt(expr->getRHS());
		if (isStmt)
			std::cout << ";\n";
		return true;
	}

	bool NanyConverterVisitor::visitCompoundAssignOperator(const clang::CompoundAssignOperator* expr)
	{
		pLog.debug() << "ConditionalOperator";
		bool isStmt = pStatementStart;
		pStatementStart = false;
		const char* op = "";
		switch (expr->getOpcode())
		{
		case clang::BO_MulAssign:
		case clang::BO_DivAssign:
		case clang::BO_AddAssign:
		case clang::BO_SubAssign:
			op = expr->getOpcodeStr().data();
			break;
		case clang::BO_RemAssign:
		case clang::BO_ShlAssign:
		case clang::BO_ShrAssign:
		default:
			std::cerr << "Compound Assignment operator \"" << expr->getOpcodeStr().data() << "\" is not yet implemented";
			return true;
		}

		if (isStmt)
			std::cout << pIndent;
		visitStmt(expr->getLHS());
		std::cout << ' ' << op << ' ';
		visitStmt(expr->getRHS());
		if (isStmt)
			std::cout << ";\n";
		return true;
	}

	bool NanyConverterVisitor::visitCXXConstructExpr(const clang::CXXConstructExpr* expr)
	{
		pLog.debug() << "CXXConstructExpr";
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

	bool NanyConverterVisitor::visitCastExpr(const clang::CastExpr* expr)
	{
		if (clang::CK_LValueToRValue == expr->getCastKind())
		{
			// Ignore the cast when possible
			visitStmt(expr->getSubExpr());
		}
		else
		{
			// In the default case, use the copy constructor to cast
			std::cout << "new ";
			visitType(expr->getType());
			std::cout << "(";
			visitStmt(expr->getSubExpr());
			std::cout << ")";
		}
		return true;
	}

	bool NanyConverterVisitor::visitImplicitCastExpr(const clang::ImplicitCastExpr* expr)
	{
		pLog.debug() << "ImplicitCastExpr";
		return visitCastExpr(expr);
	}

	bool NanyConverterVisitor::visitExplicitCastExpr(const clang::ExplicitCastExpr* expr)
	{
		pLog.debug() << "ExplicitCastExpr";
		return visitCastExpr(expr);
	}

	bool NanyConverterVisitor::visitParenExpr(const clang::ParenExpr* expr)
	{
		pLog.debug() << "ParenExpr";
		std::cout << '(';
		visitStmt(expr->getSubExpr());
		std::cout << ')';
		return true;
	}


	bool NanyConverterVisitor::visitCXXBoolLiteralExpr(const clang::CXXBoolLiteralExpr* expr)
	{
		pLog.debug() << "CXXBoolLiteralExpr";
		std::cout << (expr->getValue() ? "true" : "false");
		return true;
	}

	bool NanyConverterVisitor::visitCharacterLiteral(const clang::CharacterLiteral* stmt)
	{
		pLog.debug() << "CharacterLiteral";
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

	bool NanyConverterVisitor::visitStringLiteral(const clang::StringLiteral* stmt)
	{
		pLog.debug() << "StringLiteral";
		std::cout << '"' << stmt->getString().data() << '"';
		return true;
	}

	bool NanyConverterVisitor::visitIntegerLiteral(const clang::IntegerLiteral* stmt)
	{
		pLog.debug() << "IntegerLiteral";
		std::cout << *(stmt->getValue().getRawData());
		return true;
	}

	bool NanyConverterVisitor::visitFloatingLiteral(const clang::FloatingLiteral* stmt)
	{
		pLog.debug() << "FloatingLiteral";
		llvm::SmallVector<char, 32> dest;
		stmt->getValue().toString(dest);
		// TODO : discriminate 32 / 64
		std::cout << std::string(dest.begin(), dest.end()) << "f32";
		return true;
	}


	bool NanyConverterVisitor::visitAttr(const clang::Attr *attr)
	{
		return true;
	}

	bool NanyConverterVisitor::visitType(const clang::Type* type)
	{
		pLog.debug() << "Type";
		if (not type)
			return true;
		std::cout << convertType(type);
		return true;
	}

	bool NanyConverterVisitor::visitType(const clang::QualType& type)
	{
		pLog.debug() << "QualType";
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
			// Get the type size in bits
			clang::TypeInfo typeInfo = pContext->getTypeInfo(type);
    		uint64_t typeSize = typeInfo.Width; // in bits

			switch (type->getScalarTypeKind())
			{
				case clang::Type::STK_Bool:
					return "__bool";
				case clang::Type::STK_Integral:
					return Yuni::String(type->isSignedIntegerType()	? "__i" : "__u") << typeSize;
				case clang::Type::STK_Floating:
					return Yuni::String("__f") << typeSize;
				case clang::Type::STK_IntegralComplex:
					return "std.cplx<__i32>";
				case clang::Type::STK_FloatingComplex:
					return "std.cplx<__f32>";
				case clang::Type::STK_CPointer:
					return Yuni::String("__pointer<") << convertType(type->getPointeeType()) << '>';
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