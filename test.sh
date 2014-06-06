#########################################################################
# File Name: test.sh
# Author: lax
# Mail: xingkungao@gmail.com
# Created Time: Mon 05 May 2014 11:39:44 PM CST
#########################################################################
#!/bin/bash
make
echo 'Basic test case:'
for i in Test/test_*.c
do 
	printf '_______________%s_______________\n' "$i"
	./parser $i
	echo ''
done
