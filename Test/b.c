int main(){
int a[2][3], i = 1, j = 2;
a[0][0] = i; a[0][1] = j;
a[i][j] = a[0][0] + a[0][1];
write(a[i][j]);
return 0;
}
