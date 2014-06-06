/*************************************************************************
    > File Name: a4.c
    > Author: lax
    > Mail: xingkungao@gmail.com
    > Created Time: Fri 06 Jun 2014 12:38:09 PM CST
 ************************************************************************/

int print(int a, int b) {
	write(a);
	write(b);
	return 0;
}

int main() {
	int i, j, k;
	i = 2; 
	j = 3;
	k = print(i, j);
	return 0;
}
