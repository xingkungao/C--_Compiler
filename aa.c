struct A{
	int x[2];
	int y[3][4];
};
/*
int c(struct A a) {
	return 0;
}
int g(int b) {
	return 0;
}

int f(int a) {
	return 0;
}

int xf(int a) {
	return 0;
}
int sf(int a[2][3]) {
	return 0;
}

*/
int main() {
	struct A a;
	int b;
	a.y[2][3] = 5;
	return 0;
}
