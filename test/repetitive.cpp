int main()
{
	float f = 1; // Implicit cast
	while (24 == f) // rvalue to lvalue
		f = 23.0f;

	unsigned int i = 4;
	while (--i) // Implicit cast to bool
	{}

	while (bool b = false) // var decl in condition
	{
		int a = 2;
		return 1;
	}

	for (int j = 0; j < 100; ++j)
	{
		j += 1;
	}
	return 0;
}
