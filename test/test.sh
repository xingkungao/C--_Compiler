#########################################################################
# File Name: test.sh
# Author: lax
# Mail: xingkungao@gmail.com
# Created Time: Mon 05 May 2014 11:39:44 PM CST
#########################################################################
#!/bin/bash
cd ../
make 
cd 'test'
current=`pwd`
for i in current/*.s
do
	rm $i
done
for i in current/*.c
do 
	printf '_______________%s_______________\n' "$i"
	./parser $i ${i%%.*}.s
	echo ''
done

#for j in Test/*.s
#do
#	printf '_______________%s_______________\n' "$j"
#	spim -file $j;
#	echo ''
#done


