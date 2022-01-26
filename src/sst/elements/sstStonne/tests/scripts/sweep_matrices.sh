#/bin/bash

M="2 4 16 64 128 256 512 1024 2048"

N="2 4 16 64 128 256 512 1024 2048"

K="64 128 256 512 1024 2048"

sparsity="0 10 20 30 40 50 60 70 80 90"

for m in $(echo $M)
do
    for n in $(echo $N)
     do
         for k in $(echo $K)
         do
             for mk_spa in $(echo $sparsity)
	     do
		     cycles=$(bash launch_single_simulation.sh $m $n $k $mk_spa $mk_spa sigma_128mses_128_bw.cfg sparseflex_op_128mses_128_bw.cfg)
                     echo "$m $n $k $mk_spa $cycles"
	     done
         done
     done
done
