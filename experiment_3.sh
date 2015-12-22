#!bin/bash

echo "bash script for quicksort3"
versions=(1 2)
#probSize=(100 1000 10000 50000 100000 200000 300000 400000 500000 1000000)
times=1
len=${#versions[@]}
#num=${#probSize[@]}
for ((i=0; i<${len}; i++))
do
	for ((t=0; t<${times}; t++))
	do
        	str1="#version is "
		str2=${versions[$i]}
		str=$str1$str2
		echo $str
		./quicksort3 ${versions[$i]} 	
	done
done
