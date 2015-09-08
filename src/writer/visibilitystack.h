#pragma once

#include <yuni/yuni.h>
#include <vector>
#include <cassert>


namespace NanyFromC
{
	enum Visibility
	{
		vPublic,
		vProtected,
		vPrivate
	};


	class VisibilityStack
	{
	public:
		typedef std::vector<Visibility> Stack;

	public:
		VisibilityStack() : pCurrent(vPublic)
		{}

		void push(Visibility last)
		{
			pStack.push_back(pCurrent);
			pCurrent = last;
		}

		Visibility pop()
		{
			assert(pStack.size() > 0 && "Popping empty stack !");
			Visibility current = pCurrent;
			pCurrent = pStack.back();
			pStack.pop_back();
			return current;
		}

		Visibility current() const { return pCurrent; }
		void current(Visibility newValue) { pCurrent = newValue; }

	private:
		Stack pStack;
		Visibility pCurrent;
	}; // VisibilityStack


	inline std::ostream& operator << (std::ostream& out, const Visibility& visibility)
	{
		switch (visibility)
		{
			case vPublic:
				out << "public";
				break;
			case vProtected:
				out << "protected";
				break;
			case vPrivate:
				out << "private";
				break;
		}
		return out;
	}


} // namespace NanyFromC