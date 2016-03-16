#!/bin/bash

function count {

    counter=1
	sum=0.0
    while [ $counter -le 16 ]
        do
            
        	if [ $counter -eq 1 ] || [ $counter -eq 2 ] || [ $counter -eq 4 ] || [ $counter -eq 8 ] || [ $counter -eq 16 ]
			then
				sum=0.0
			    echo "P: $counter, K=2"
				var=$(./kmeans 2 $counter kmeansMedium.txt outputMedium.txt)
				echo $var
				sum=$(echo "scale=5; $sum + $var" | bc)
				var=$(./kmeans 2 $counter kmeansMedium.txt outputMedium.txt)
				echo $var
				sum=$(echo "scale=5; $sum + $var" | bc)
				var=$(./kmeans 2 $counter kmeansMedium.txt outputMedium.txt)
				echo $var
				sum=$(echo "scale=5; $sum + $var" | bc)
				echo "scale=5; $sum / 3" | bc
				
				sum=0.0
				echo "P: $counter, K=5"
				var=$(./kmeans 5 $counter kmeansMedium.txt outputMedium.txt)
				echo $var
				sum=$(echo "scale=5; $sum + $var" | bc)
				var=$(./kmeans 5 $counter kmeansMedium.txt outputMedium.txt)
				echo $var
				sum=$(echo "scale=5; $sum + $var" | bc)
				var=$(./kmeans 5 $counter kmeansMedium.txt outputMedium.txt)
				echo $var
				sum=$(echo "scale=5; $sum + $var" | bc)
				echo "scale=5; $sum / 3" | bc
				
				sum=0.0
				echo "P: $counter, K=10"
				var=$(./kmeans 10 $counter kmeansMedium.txt outputMedium.txt)
				echo $var
				sum=$(echo "scale=5; $sum + $var" | bc)
				var=$(./kmeans 10 $counter kmeansMedium.txt outputMedium.txt)
				echo $var
				sum=$(echo "scale=5; $sum + $var" | bc)
				var=$(./kmeans 10 $counter kmeansMedium.txt outputMedium.txt)
				echo $var
				sum=$(echo "scale=5; $sum + $var" | bc)
				echo "scale=5; $sum / 3" | bc
				
				sum=0.0
				echo "P: $counter, K=25"
				var=$(./kmeans 25 $counter kmeansMedium.txt outputMedium.txt)
				echo $var
				sum=$(echo "scale=5; $sum + $var" | bc)
				var=$(./kmeans 25 $counter kmeansMedium.txt outputMedium.txt)
				echo $var
				sum=$(echo "scale=5; $sum + $var" | bc)
				var=$(./kmeans 25 $counter kmeansMedium.txt outputMedium.txt)
				echo $var
				sum=$(echo "scale=5; $sum + $var" | bc)
				echo "scale=5; $sum / 3" | bc
			
			
			fi
	            ((counter++))
        done
}
count
