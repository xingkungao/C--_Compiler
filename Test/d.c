int sum(int x[2], int n) {
	return x[0] + x[1];
}

int main(){
	int a[2];
	a[0] = 1;
	a[1] = 2;
	write( sum(a,2) );
	return 0;
}
