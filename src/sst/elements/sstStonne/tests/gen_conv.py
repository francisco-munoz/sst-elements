#!/usr/bin/python

import random;

R = 3;
S = 3;
C = 20;
K = 1;
G = 1;
X = 6;
Y = 6;

in_file_matrix_a="conv_file_matrixA_"+str(R)+"_"+str(S)+"_"+str(C)+str(K)+"_"+str(G)+"_"+str(X)+"_"+str(Y)+".in";
in_file_matrix_b="conv_file_matrixB_"++str(R)+"_"+str(S)+"_"+str(C)+str(K)+"_"+str(G)+"_"+str(X)+"_"+str(Y)+".in";

rand_smallest=0;
rand_largest=10;

matrixA_size=N*X*Y*C;
matrixB_size=R*S*(C/G)*K;
# Generating matrix A
with open(in_file_matrix_a, "w") as f:
    for i in range(matrixA_size):  # Row major
        f.write(str(random.randint(rand_smallest, rand_largest)));
        if(i < (matrixA_size-1)):
            f.write(",");

print("File "+in_file_matrix_a+" generated correctly");

# Generating matrix B
with open(in_file_matrix_b, "w") as f:
    for i in range(matrixB_size):   # Row major format (interpreted by the simulator)
        f.write(str(random.randint(rand_smallest, rand_largest)));
        if(i < (matrixB_size-1)):
            f.write(",");

print("File "+in_file_matrix_b+" generated correctly");
