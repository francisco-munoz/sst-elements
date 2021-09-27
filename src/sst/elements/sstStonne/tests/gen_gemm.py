#!/usr/bin/python

import random;

M = 3;
N = 3;
K = 9;

in_file_matrix_a="gemm_file_matrixA_"+str(M)+"_"+str(N)+"_"+str(K)+".in";
in_file_matrix_b="gemm_file_matrixB_"+str(M)+"_"+str(N)+"_"+str(K)+".in";

rand_smallest=0;
rand_largest=10;


# Generating matrix A
with open(in_file_matrix_a, "w") as f:
    for m in range(M):  # Row major
        for k in range(K):
            f.write(str(random.randint(rand_smallest, rand_largest)));
            if((m*K+k) < (M*K-1)):
                f.write(",");

print("File "+in_file_matrix_a+" generated correctly");

# Generating matrix B
with open(in_file_matrix_b, "w") as f:
    for n in range(N):   # Row major format (interpreted by the simulator)
        for k in range(K):
            f.write(str(random.randint(rand_smallest, rand_largest)));
            if((n*K+k) < (N*K-1)):
                f.write(",");

print("File "+in_file_matrix_b+" generated correctly");
