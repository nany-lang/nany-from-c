#include "visitor.h"

namespace NanyFromC
{

	bool NanyConverterVisitor::VisitDecl(clang::Decl* decl)
	{
		decl->dump();
		return true;
	}

	bool NanyConverterVisitor::VisitCXXRecordDecl(clang::CXXRecordDecl* decl)
	{
		decl->dump();
		return true;
	}


} // namespace NanyFromC