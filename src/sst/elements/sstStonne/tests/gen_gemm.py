#!/usr/bin/python

import random;
import struct;


generate_result=1
test_output_file="result_test.out"

M = 3;
N = 3;
K = 20;


file_name="gemm_mem.ini"
data_width=4;

rand_smallest=0;
rand_largest=10;

matrixA_size=int(M*K);
matrixB_size=int(N*K);
matrixC_size=int(M*N);

matrixA=[]
matrixB=[]
matrixC=list(range(0,matrixC_size));
random.seed(a=0, version=2)


with open(file_name, "w") as f:
    for i in range(matrixA_size):  # Row major
        value = float(random.randint(rand_smallest, rand_largest));
        ba = bytearray(struct.pack(">f", value))  # generating list of bytes
        my_int = int.from_bytes(ba, "big")
        f.write(str(my_int))
        f.write(",")
        if(generate_result):
            matrixA.append(value);
    for i in range(matrixB_size):   # Row major format (interpreted by the simulator)
        value = float(random.randint(rand_smallest, rand_largest));
        ba = bytearray(struct.pack(">f", value))
        my_int = int.from_bytes(ba, "big");
        f.write(str(my_int));
        f.write(",")
        if(generate_result):
            matrixB.append(value);
    f.write(str(0)); # adding a final value which will not be used just to avoid have a last comma without value


print("Offset matrix A: "+str(0));
print("Offset matrix B: "+str(matrixA_size*data_width));
print("Offset matrix C: "+str((matrixA_size+matrixB_size)*data_width))
print("File "+file_name+" generated correctly");

if(generate_result):
    for i in range(0, M ):
        for j in range(0, N):
            matrixC[i*N+j]=float(0.0)
            for k in range(0,K):
                matrixC[i*N+j]+= matrixA[i*K+k]*matrixB[j*K+k] # row-major order in both matrices. (i.e., KN matrix is transposed)
    with open(test_output_file, "w") as f:
        for i in range(0,matrixC_size):
            value = float(matrixC[i])
            f.write(str(value))
            f.write(",")

