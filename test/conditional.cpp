
int main()
{
	int a;
	if (2 > 0)
		a = 0;
	if (a == 1)
	{
		a = 1;
	}
	else if (a != 2)
	{
		a = 2;
	}
	a = (a < 3 ? 4 : 5);
	if (int b = (a + 1))
		return b;
	return a;
}
