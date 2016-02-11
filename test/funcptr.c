
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
	typedef unsigned int (*FuncPtrType2)(int a, float b, char* c);

	funcPtr = &foo;
	return funcPtr(funcPtr(0));
}