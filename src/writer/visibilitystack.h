#pragma once

#include <yuni/yuni.h>
#include <vector>
#include <cassert>


namespace NanyFromC
{

	class VisibilityStack
	{
	public:
		enum Visibility
		{
			vPublic,
			vProtected,
			vPrivate
		};

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

	private:
		Stack pStack;
		Visibility pCurrent;
	}; // VisibilityStack

} // namespace NanyFromC