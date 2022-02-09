#!/usr/bin/python

import random;
import struct;

MIN_VALUES=3 # do not touch 

generate_result=1
test_output_file="result_test.out"


M = 16;
N = 32;
K = 1024;

sparsity_ratio_a=99;
sparsity_ratio_b=99;

file_name="gustavsons/gustavsons_gemm_mem.ini"
data_width=4;

rowpointer_a="gustavsons/gustavsons_gemm_rowpointerA.in";
colpointer_a="gustavsons/gustavsons_gemm_colpointerA.in";

rowpointer_b="gustavsons/gustavsons_gemm_rowpointerB.in";
colpointer_b="gustavsons/gustavsons_gemm_colpointerB.in";

address_matrix_a=0;
address_matrix_b=0; # to be updated in the code
address_matrix_c=0; # to be updated in the code

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
with open(file_name, "w") as fd, open(rowpointer_a, "w") as rpA, open(colpointer_a, "w") as cpA, open(rowpointer_b, "w") as rpB, open(colpointer_b, "w") as cpB:
    #generating matrixA
    n_nonzeros=0
    for m in range(M):  # row major
        initial_values=0
        rpA.write(str(n_nonzeros)+","); # writing the index of A
        for k in range(K):
            sparse_prob=random.randint(0,100);
            if((sparse_prob > sparsity_ratio_a) or (initial_values < MIN_VALUES)):  # value is generated
                if((m==(M-1)) and (k==(K-1))):
                    cpA.write(str(k))
                else:
                    cpA.write(str(k)+","); #writing the row index
                initial_values+=1;
                value = float(random.randint(rand_smallest, rand_largest));
                ba = bytearray(struct.pack(">f", value))  # generating list of bytes
                my_int = int.from_bytes(ba, "big")
                fd.write(str(my_int))
                fd.write(",")
                n_nonzeros+=1;
                if(generate_result):
                    matrixA.append(value);
            else:
                if(generate_result):
                    matrixA.append(float(0.0));
    rpA.write(str(n_nonzeros));
    address_matrix_b=n_nonzeros*data_width;
    #Generating matrix B
    n_nonzeros=0;
    for k in range(0,K):  # Row major
        initial_values=0;
        rpB.write(str(n_nonzeros)+","); # writing the index of A
        for n in range(0,N):
            sparse_prob=random.randint(0,100);
            if((sparse_prob > sparsity_ratio_b) or (initial_values < MIN_VALUES)):  # value is generated
                if((k==(K-1)) and (n==(N-1))):
                    cpB.write(str(n))
                else:
                    cpB.write(str(n)+","); #writing the row index

                initial_values+=1;
                value = float(random.randint(rand_smallest, rand_largest));
                ba = bytearray(struct.pack(">f", value))  # generating list of bytes
                my_int = int.from_bytes(ba, "big")
                fd.write(str(my_int))
                fd.write(",")
                n_nonzeros+=1;
                if(generate_result):
                    matrixB.append(value);
            else:
                # no data element is inserted in this case
                if(generate_result):
                    matrixB.append(float(0.0));
    rpB.write(str(n_nonzeros)) 
    fd.write(str(0)) # Adding a final 0 to the memory which will never be used. This is just to avoid having a last comma.
    address_matrix_c=address_matrix_b+(n_nonzeros*data_width);



print("Offset matrix A: "+str(address_matrix_a));
print("Offset matrix B: "+str(address_matrix_b));
print("Offset matrix C: "+str(address_matrix_c));

print("File "+file_name+" generated correctly");
print("File "+rowpointer_a+" generated correctly");
print("File "+colpointer_a+" generated correctly");

print("File "+rowpointer_b+" generated correctly");
print("File "+colpointer_b+" generated correctly");


#print("MatrixA: ")
#for i in range(matrixA_size):
#    print(matrixA[i])

#print("MatrixB: ")
#for i in range(matrixB_size):
#    print(matrixB[i])

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

