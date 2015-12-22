#!bin/bash

echo "bash script for quicksort"
versions=(0 1 2 3)
probSize=(100 1000 10000 50000 100000 200000 300000 400000 500000 1000000)
times=10
len=${#versions[@]}
num=${#probSize[@]}
for ((i=0; i<${len}; i++))
do
	for ((j=0; j<${num}; j++))
	do
		for ((t=1; t<${times}; t++))
		do
			./myquicksort ${versions[$i]} ${probSize[$j]}		
	        	str1="#version is "
			str2=${versions[$i]}
			str3=" ,#problem size is "
			str4=${probSize[$j]}
			str=$str1$str2$str3$str4
			echo $str
		done
	done
done
