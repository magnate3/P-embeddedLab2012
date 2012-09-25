int factorial(int n)
{
	int c;
	int result = 1;
	for (c = 1; c <= n; c++)
		result *= c;
	return result;
}

int main()
{
	int i;
	int n;
	for (i = 0; i < 10; ++i) {
		n = factorial(i);
		printf("factorial(%d) = %d\n", i, n);
	}
}
