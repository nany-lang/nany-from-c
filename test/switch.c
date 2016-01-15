int main()
{
	switch (42)
	{
	case 2:
		break;
	case 3:
		return 1;
	case 4:
	case 5:
		return 2;
	case 6:
		if (0)
			return 1;
		return 0;
	default:
		return 3;
	}
	return 0;
}
