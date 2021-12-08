#!/usr/bin/python

import random;
import struct;

generate_result=1
test_output_file="result_test.out"
R = 3;
S = 3;
C = 3;
K = 1;
N = 1;
G = 1;
X = 6;
Y = 6;
strides = 1
X_= int((X - R + strides) / strides);
Y_= int((Y - S + strides) / strides);

file_name="conv_mem.ini"
data_width=4;

rand_smallest=0;
rand_largest=10;

matrixA_size=N*X*Y*C;
matrixB_size=int(R*S*(C/G)*K);
matrixC_size=int(N*X_*Y_*K);

matrixA=[]
matrixB=[]
matrixC=list(range(0,matrixC_size));
random.seed(a=0, version=2)
# Generating matrix A
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
    OX=int((X - R + strides) / strides);
    OY=int((Y - S + strides) / strides);
    K=int(K/G);
    C=int(C/G);
    output_size_n = int(G*K*OX*OY);
    input_size_n = int(G*C*X*Y);
    filter_size=int(R*S*C);
    size_oy=int(OY*K*G);
    size_y=int(Y*G*C);
    for n in range(0,N): 
        for g in range(0,G): 
            for k in range(0,K): 
                for ox in range(0,OX):
                    for oy in range(0,OY):
                        #print(n*output_size_n + ox*size_oy + oy*K*G + g*K + k)
                        matrixC[int(n*output_size_n + ox*size_oy + oy*K*G + g*K + k)]=float(0.0);
                        for c in range(0, C):
                            for r in range(0, R):
                                for s in range(0,S):
                                    matrixC[n*output_size_n + ox*size_oy + oy*K*G + g*K + k] += matrixA[n*input_size_n+ ox*strides*size_y + oy*strides*C*G + r*size_y + s*C*G + g*C + c]*matrixB[g*K*filter_size + k*filter_size + r*S*C + s*C + c];
                                
                            
    with open(test_output_file, "w") as f:
        for i in range(0,matrixC_size):
            value = float(matrixC[i])
            f.write(str(value))
            f.write(",")




