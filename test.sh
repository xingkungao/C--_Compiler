#########################################################################
# File Name: test.sh
# Author: lax
# Mail: xingkungao@gmail.com
# Created Time: Mon 05 May 2014 11:39:44 PM CST
#########################################################################
#!/bin/bash
make 
for i in 'test'/*.s
do
	rm $i
done
for i in 'test'/*.c
do 
	printf '_______________%s_______________\n' "$i"
	./compiler $i ${i%%.*}.s
	echo ''
done

#for j in Test/*.s
#do
#	printf '_______________%s_______________\n' "$j"
#	spim -file $j;
#	echo ''
#done


