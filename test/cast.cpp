#include <cstdlib>

int main()
{
	char* str = 0;
	str = NULL;
	str = nullptr;
	str = (char*)0;
	str = (char*)NULL;
	str = (char*)nullptr;

	typedef void (*FuncPtr)(void);
	FuncPtr ptr = 0;
	ptr = NULL;
	ptr = nullptr;
	ptr = (FuncPtr)0;
	ptr = (FuncPtr)NULL;
	ptr = (FuncPtr)nullptr;

	if (ptr == 0)
		return 1;
	if (ptr == NULL)
		return 1;
	if (ptr == nullptr)
		return 1;
	return 0;
}
