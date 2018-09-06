#!/usr/bin/bash

declare -a tsrs=("1998DARPA" "nell2" "freebase_music" "freebase_sampled" "nell1" "delicious")
declare -a test_tsr_names=("nell2")
declare -a BLOCK_SIZES=("32" "64" "128" "256" "512" "1024")
declare -a TYPEFLAGS=("char" "short" "int" "long")
declare -a modes=("0" "1" "2")
declare -a precisions=("single" "double")

tsr_path="/BIGTENSORS"
out_path="/people/liji541/Codes/mttkrp-gpu/outputs/double"

for p in "${precisions[@]}"
do

	# for R in 8 16 32 64
	for R in 16
	do
		for tsr_name in "${test_tsr_names[@]}"
		do
			for bs in "${BLOCK_SIZES[@]}"
			do
				for tf in "${TYPEFLAGS[@]}"
				do
					for m in "${modes[@]}"
					do

						echo "./main_${tf}_${p} ${tsr_path}/${tsr_name}.tns ${R} ${bs} ${m} > ${out_path}/${tsr_name}-r${R}-b${bs}-m${m}_${p}.txt"
						# ./main_${tf}_${p} ${tsr_path}/${tsr_name}.tns ${R} ${bs} ${m} > ${out_path}/${tsr_name}-r${R}-b${bs}-m${m}_${p}.txt

					done
				done
			done
		done
	done

done
