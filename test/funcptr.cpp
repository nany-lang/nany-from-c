
class A
{
public:
	bool isTrue() { return true; }
};

int main()
{
	// Declare a function pointer
	void (*funcPtr)();
	// typedef a function pointer
	typedef bool (*FuncPtrType)(int a);
	// typedef a function pointer
	typedef unsigned int (*FuncPtrType2)(int a, float b, char* c);

	// Method pointers
	A a;
	typedef bool (A::*MethodPtrType)();
	bool (A::*methodPtr)();
	return 0;
}