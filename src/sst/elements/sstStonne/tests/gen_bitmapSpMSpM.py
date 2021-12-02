#!/usr/bin/python

import random;
import struct;

generate_result=1
test_output_file="result_test.out"


M = 3;
N = 3;
K = 20;

sparsity_ratio_a=40;
sparsity_ratio_b=10;

file_name="bitmapSpMSpM_gemm_mem.ini"
data_width=4;

in_file_bitmap_a="bitmapSpMSpM_file_bitmapA_"+str(M)+"_"+str(N)+"_"+str(K)+".in";
in_file_bitmap_b="bitmapSpMSpM_file_bitmapB_"+str(M)+"_"+str(N)+"_"+str(K)+".in";


rand_smallest=1;
rand_largest=10;

if(generate_result):
    matrixA_size=int(M*K);
    matrixB_size=int(N*K);
    matrixC_size=int(M*N);

    matrixA=[]
    matrixB=[]
    matrixC=list(range(0,matrixC_size));
random.seed(a=0, version=2)




# Generating matrix A
with open(file_name, "w") as fd, open(in_file_bitmap_a, "w") as fbA, open(in_file_bitmap_b, "w") as fbB:
    #generating matrixA
    n_nonzeros=0
    for m in range(M):  # Row major
        for k in range(K):
            sparse_prob=random.randint(0,100);
            if(sparse_prob > sparsity_ratio_a):  # value is generated
                if(n_nonzeros==0):  # this is to insert comma.
                    fbA.write(str(1)); #writing a 1 in bitmap
                else: # we do not insert a comma 
                    fbA.write(","+str(1));
                value = float(random.randint(rand_smallest, rand_largest));
                ba = bytearray(struct.pack(">f", value))  # generating list of bytes
                my_int = int.from_bytes(ba, "big")
                fd.write(str(my_int))
                fd.write(",")
                n_nonzeros+=1;
                if(generate_result):
                    matrixA.append(value);
            else:
                if((m==0) and (k==0)): # this is to insert a comma
                    fbA.write(str(0)); #writing a 1
                    # note no data element is inserted in this case
                else:
                    # note no data element is inserted in this case
                    fbA.write(","+str(0));
                if(generate_result):
                    matrixA.append(float(0.0));

    #Generating matrix B
    n_nonzeros=0;
    for k in range(K):  # Row major
        for n in range(N):
            sparse_prob=random.randint(0,100);
            if(sparse_prob > sparsity_ratio_b):  # value is generated
                if(n_nonzeros==0):  # this is to insert comma.
                    fbB.write(str(1)); #writing a 1 in bitmap
                else:
                    fbB.write(","+str(1));
                value = float(random.randint(rand_smallest, rand_largest));
                ba = bytearray(struct.pack(">f", value))  # generating list of bytes
                my_int = int.from_bytes(ba, "big")
                fd.write(str(my_int))
                fd.write(",")
                n_nonzeros+=1;
                if(generate_result):
                    matrixB.append(value);
            else:
                if((k==0) and (n==0)): # this is to insert a comma
                    fbB.write(str(0)); #writing a 1
                    # note no data element is inserted in this case
                else:
                    # note no data element is inserted in this case
                    fbB.write(","+str(0));
                if(generate_result):
                    matrixB.append(float(0.0));
    
    fd.write(str(0)) # Adding a final 0 to the memory which will never be used. This is just to avoid having a last comma.



print("File "+file_name+" generated correctly");
print("File "+in_file_bitmap_a+" generated correctly");


print("File "+in_file_bitmap_b+" generated correctly");

if(generate_result):
    for i in range(0, M ):
        for j in range(0, N):
            matrixC[i*N+j]=float(0.0)
            for k in range(0,K):
                matrixC[i*N+j]+= matrixA[i*K+k]*matrixB[k*N+j] # row-major order in both matrices. (i.e., KN matrix is transposed)
    with open(test_output_file, "w") as f:
        for i in range(0,matrixC_size):
            value = float(matrixC[i])
            f.write(str(value))
            f.write(",")

