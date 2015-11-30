#pragma once
#include <yuni/yuni.h>
#include <iostream>

namespace NanyFromC
{

	//! Manage indentation level and print indentation
	class Indenter
	{
	public:
		/*!
		** By default, write one tab per level.
		*/
		Indenter(bool useSpaces = false, uint indentSize = 1)
			: pWhitespace(useSpaces ? ' ' : '\t')
			, pIndentSize(indentSize)
			, pLevel(0u)
		{}

		Indenter(const Indenter& other) = default;
		Indenter& operator = (const Indenter& other) = default;

		//! Current indent level
		uint level() const { return pLevel; }

		void print(std::ostream& out) const
		{
			for (uint i = 0; i < pLevel * pIndentSize; ++i)
				out << '\t';
		}

		//! Increase indent level (prefix)
		Indenter& operator++()  { ++pLevel; return *this; }
		//! Increase indent level (postfix)
		Indenter operator++(int)
		{
			Indenter temp(*this);
   			++pLevel;
   			return temp;
		}

		//! Decrease indent level (prefix)
		Indenter& operator--() { if (pLevel > 0u) --pLevel; return *this; }
		//! Decrease indent level (postfix)
		Indenter operator--(int)
		{
			Indenter temp(*this);
			if (pLevel > 0u)
   				--pLevel;
   			return temp;
		}

		void useSpaces(bool doUseSpaces) { pWhitespace = (doUseSpaces ? ' ' : '\t'); }
		bool useSpaces() const { return ' ' == pWhitespace; }

	private:
		//! Use spaces or tabs
		char pWhitespace;
		//! Number of tabs / spaces per indent level
		uint pIndentSize;
		//! Indent level
		uint pLevel;

	}; // class Indenter


	inline std::ostream& operator << (std::ostream& out, const Indenter& indenter)
	{
		indenter.print(out);
		return out;
	}

} // namespace NanyFromC