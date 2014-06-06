struct Complex{ 
	int real, image;
};
int main(){
	struct Complex c;
	int f;
	c.real = 5;
	c.image = 3;
	f = c.real + c.image;
	write(f);
	return 0;
}
