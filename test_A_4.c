int foo(int x)
{
	if(x>=0) return x;
	else return -x;
}

int foo(float x2)
{
	return 1;
}

int main()
{
	int a = 0;
	return foo(a);
}
