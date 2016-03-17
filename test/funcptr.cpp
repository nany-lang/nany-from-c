
class A
{
public:
	bool isTrue() const { return true; }
};

int foo(int a)
{
	return a;
}

int main()
{
	// Declare a function pointer
	int (*funcPtr)(int a);
	// typedef a function pointer
	typedef void (*FuncPtrType)();
	// typedef a function pointer
	typedef bool (*FuncPtrType2)(int a, float b, char* c);

	// Method pointers
	A a;
	typedef bool (A::*MethodPtrType)();
	bool (A::*methodPtr)() const = 0;

	methodPtr = &A::isTrue;
	if ((a.*methodPtr)())
		return 1;
	funcPtr = &foo;
	return funcPtr(funcPtr(0));
}