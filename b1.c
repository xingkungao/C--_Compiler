/*************************************************************************
    > File Name: b1.c
    > Author: lax
    > Mail: xingkungao@gmail.com
    > Created Time: Thu 05 Jun 2014 11:52:10 PM CST
 ************************************************************************/

int main()
{
int i, j, k;
i = read();
j = read();
k = read();
if (i <= 0 || j <= 0 || k <= 0)
write(-1);
else
{
if ((i + j) <= k)
write(-1);
else if ((i + k) <= j)
write(-1);
else if ((j + k) <= i)
write(-1);
else
write(1);
}
return 0;
}
