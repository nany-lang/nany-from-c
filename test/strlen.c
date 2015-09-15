
static unsigned long long mystrlen(const char* str)
{
	if (!str)
		return 0;

	unsigned long long i = 0;
	for (; *str; ++str, ++i)
		;
	return i;
}

int main()
{
	unsigned long long sz = mystrlen("Hello world!");
	return mystrlen((void*)0);
}
