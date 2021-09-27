#!/usr/bin/python

import random;

M = 3;
N = 3;
K = 20;

sparsity_ratio_a=40;
sparsity_ratio_b=10;

in_file_matrix_a="bitmapSpMSpM_file_matrixA_"+str(M)+"_"+str(N)+"_"+str(K)+".in";
in_file_matrix_b="bitmapSpMSpM_file_matrixB_"+str(M)+"_"+str(N)+"_"+str(K)+".in";

in_file_bitmap_a="bitmapSpMSpM_file_bitmapA_"+str(M)+"_"+str(N)+"_"+str(K)+".in";
in_file_bitmap_b="bitmapSpMSpM_file_bitmapB_"+str(M)+"_"+str(N)+"_"+str(K)+".in";


rand_smallest=1;
rand_largest=10;



# Generating matrix A
with open(in_file_matrix_a, "w") as fd, open(in_file_bitmap_a, "w") as fb:
    for m in range(M):  # Row major
        for k in range(K):
            sparse_prob=random.randint(0,100);
            if(sparse_prob > sparsity_ratio_a):  # value is generated
                if((m==0) and (k==0)):  # this is to insert comma.
                    fb.write(str(1)); #writing a 1 in bitmap
                    fd.write(str(random.randint(rand_smallest, rand_largest))); # a new data element is included
                else:
                    fb.write(","+str(1));
                    fd.write(","+str(random.randint(rand_smallest, rand_largest)))
            else:
                if((m==0) and (k==0)): # this is to insert a comma
                    fb.write(str(0)); #writing a 1
                    # note no data element is inserted in this case
                else:
                    # note no data element is inserted in this case
                    fb.write(","+str(0));



print("File "+in_file_matrix_a+" generated correctly");
print("File "+in_file_bitmap_a+" generated correctly");

# Generating matrix B
with open(in_file_matrix_b, "w") as fd, open(in_file_bitmap_b, "w") as fb:
    for k in range(K):  # Row major
        for n in range(N):
            sparse_prob=random.randint(0,100);
            if(sparse_prob > sparsity_ratio_b):  # value is generated
                if((k==0) and (n==0)):  # this is to insert comma.
                    fb.write(str(1)); #writing a 1 in bitmap
                    fd.write(str(random.randint(rand_smallest, rand_largest))); # a new data element is included
                else:
                    fb.write(","+str(1));
                    fd.write(","+str(random.randint(rand_smallest, rand_largest)))
            else:
                if((k==0) and (n==0)): # this is to insert a comma
                    fb.write(str(0)); #writing a 1
                    # note no data element is inserted in this case
                else:
                    # note no data element is inserted in this case
                    fb.write(","+str(0));


print("File "+in_file_matrix_b+" generated correctly");
print("File "+in_file_bitmap_b+" generated correctly");
