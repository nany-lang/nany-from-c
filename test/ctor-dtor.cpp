struct A
{
	A(int c)
	{
		value = new int(24 + c);
	}
	A(const A& other) : value(new int(*(other.value)))
	{}
	~A()
	{
		delete value;
	}

	int* value;
};

int main()
{
	A a(24);
	return A(a).value;
}
