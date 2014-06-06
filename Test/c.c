struct Complex{ int re, im;};
int sum(struct Complex x){
	return x.re + x.im;
}
int main(){
	struct Complex c;
	int f;
	c.re = 5;
	c.im = 3;
	f = sum(c);
	write(f);
	return 0;
}
