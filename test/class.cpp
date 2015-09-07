
class A
{
public:
	enum Validity
	{
		valid,
		invalid,
		maybe
	};

public:
	A(): pValid(valid) {}
	~A() {}

    Validity foo() { return pValid; }

private:
	Validity pValid;
};
