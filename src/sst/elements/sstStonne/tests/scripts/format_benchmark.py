import scipy.io
import matplotlib.pyplot as plt
import numpy as np
import sys
import struct


def ensureNoEmptyRows(array_data, array_pointers, array_indices):
    # Calculating the size of the new array
    nnz=array_data.shape[0]
    n_rows=array_pointers.shape[0]-1
    new_size=nnz
    for i in range(n_rows):
        size_row=array_pointers[i+1]-array_pointers[i]
        #print("Size row "+str(i)+":"+str(size_row))
        if(size_row < 3):
            gap=3 - size_row;
            new_size+=gap
            #print("Detected an empty row. nedded to increase "+str(gap))

    # Creating the new modified arrays
    new_array_data = np.zeros(new_size, dtype=np.float32)
    new_array_pointers = np.zeros(n_rows+1, dtype=np.int32)
    new_array_indices = np.zeros(new_size, dtype=np.int32)

    # Allocating data into the new arrays
    new_index=0
    new_array_pointers[0]=0 # Initializing the first pointer to the first element
    for i in range(n_rows):
        size_row=array_pointers[i+1]-array_pointers[i]
        if(size_row > 0): # If there are elements, copy them
            for j in range(array_pointers[i], array_pointers[i+1]):
                new_array_data[new_index]=array_data[j]
                new_array_indices[new_index]=array_indices[j]
                new_index+=1;
        if(size_row < 3):
            gap = 3 - size_row
            for j in range(gap): # Introduces the remaining elements
                new_array_data[new_index]=4.0 # 4 is just a random number
                new_array_indices[new_index]=j # WATCH OUT! This might collide with a current index?
                new_index+=1

        new_array_pointers[i+1]=new_index # Updating the row pointer

    return new_array_data,new_array_pointers, new_array_indices


if len(sys.argv) != 2:
    print("Usage: ./script name_benchmark");
    exit(1)


name_benchmark=sys.argv[1]
mat = scipy.io.loadmat(name_benchmark)
data_width=4 # Assuming 4 bytes per element

print("Generating files for benchmark "+name_benchmark)

# Flags to select the required dataflows to be generated
generate_inner_product=True
generate_outer_product=True
generate_gustavsons=True

#Name files
# Inner Product
innerProduct_mem_file="bitmapSpMSpM_gemm_mem.ini";
innerProduct_a_bitmap_file="bitmapSpMSpM_file_bitmapA.in";
innerProduct_b_bitmap_file="bitmapSpMSpM_file_bitmapB.in";


#Outer Product
outerProduct_mem_file="outerproduct_gemm_mem.ini";
outerProduct_a_rowpointer_file="outerproduct_gemm_rowpointerA.in";
outerProduct_a_colpointer_file="outerproduct_gemm_colpointerA.in";
outerProduct_b_rowpointer_file="outerproduct_gemm_rowpointerB.in";
outerProduct_b_colpointer_file="outerproduct_gemm_colpointerB.in";

#Gustavsons Product
gustavsons_mem_file="gustavsons_gemm_mem.ini";
gustavsons_a_rowpointer_file="gustavsons_gemm_rowpointerA.in";
gustavsons_a_colpointer_file="gustavsons_gemm_colpointerA.in";
gustavsons_b_rowpointer_file="gustavsons_gemm_rowpointerB.in";
gustavsons_b_colpointer_file="gustavsons_gemm_colpointerB.in";


if type(mat['Problem'][0][0][2]) == scipy.sparse.csc.csc_matrix:
    matrixA_csr=mat['Problem'][0][0][2].tocsr();
    matrixB_csr=mat['Problem'][0][0][2].tocsr().transpose().tocsr();
    matrixA_csc=mat['Problem'][0][0][2].tocsc();
    matrixB_csc=mat['Problem'][0][0][2].tocsc().transpose().tocsc();

elif type(mat['Problem'][0][0][1]) == scipy.sparse.csc.csc_matrix:
    matrixA_csr=mat['Problem'][0][0][1].tocsr();
    matrixB_csr=mat['Problem'][0][0][1].tocsr().transpose().tocsr();
    matrixA_csc=mat['Problem'][0][0][1].tocsc();
    matrixB_csc=mat['Problem'][0][0][1].tocsc().transpose().tocsc();

elif type(mat['Problem'][0][0][0]) == scipy.sparse.csc.csc_matrix:
    matrixA_csr=mat['Problem'][0][0][0].tocsr();
    matrixB_csr=mat['Problem'][0][0][0].tocsr().transpose().tocsr();
    matrixA_csc=mat['Problem'][0][0][0].tocsc();
    matrixB_csc=mat['Problem'][0][0][0].tocsc().transpose().tocsc();

elif type(mat['Problem'][0][0][3]) == scipy.sparse.csc.csc_matrix:
    matrixA_csr=mat['Problem'][0][0][3].tocsr();
    matrixB_csr=mat['Problem'][0][0][3].tocsr().transpose().tocsr();
    matrixA_csc=mat['Problem'][0][0][3].tocsc();
    matrixB_csc=mat['Problem'][0][0][3].tocsc().transpose().tocsc();

elif type(mat['Problem'][0][0][4]) == scipy.sparse.csc.csc_matrix:
    matrixA_csr=mat['Problem'][0][0][4].tocsr();
    matrixB_csr=mat['Problem'][0][0][4].tocsr().transpose().tocsr();
    matrixA_csc=mat['Problem'][0][0][4].tocsc();
    matrixB_csc=mat['Problem'][0][0][4].tocsc().transpose().tocsc();

elif type(mat['Problem'][0][0][5]) == scipy.sparse.csc.csc_matrix:
    matrixA_csr=mat['Problem'][0][0][5].tocsr();
    matrixB_csr=mat['Problem'][0][0][5].tocsr().transpose().tocsr();
    matrixA_csc=mat['Problem'][0][0][5].tocsc();
    matrixB_csc=mat['Problem'][0][0][5].tocsc().transpose().tocsc();

# Extracting csr and csc data

matrixA_data_csr = matrixA_csr.data;
matrixA_pointers_csr = matrixA_csr.indptr;
matrixA_indices_csr = matrixA_csr.indices;
#Preprocess
matrixA_data_csr, matrixA_pointers_csr, matrixA_indices_csr = ensureNoEmptyRows(matrixA_data_csr, matrixA_pointers_csr, matrixA_indices_csr);

matrixB_data_csr = matrixB_csr.data;
matrixB_pointers_csr = matrixB_csr.indptr; # it includes the row n with elelements elements+1
matrixB_indices_csr = matrixB_csr.indices;
#Preprocess
matrixB_data_csr, matrixB_pointers_csr, matrixB_indices_csr = ensureNoEmptyRows(matrixB_data_csr, matrixB_pointers_csr, matrixB_indices_csr);

matrixA_data_csc = matrixA_csc.data;
matrixA_pointers_csc = matrixA_csc.indptr;
matrixA_indices_csc = matrixA_csc.indices;
#Preprocess
matrixA_data_csc, matrixA_pointers_csc, matrixA_indices_csc = ensureNoEmptyRows(matrixA_data_csc, matrixA_pointers_csc, matrixA_indices_csc);

matrixB_data_csc = matrixB_csc.data;
matrixB_pointers_csc = matrixB_csc.indptr;
matrixB_indices_csc = matrixB_csc.indices;
matrixB_data_csc, matrixB_pointers_csc, matrixB_indices_csc = ensureNoEmptyRows(matrixB_data_csc, matrixB_pointers_csc, matrixB_indices_csc);

# Calculating memory addresses
n_nnz_A=matrixA_data_csr.shape[0];
n_nnz_B=matrixB_data_csr.shape[0];
memory_address_A=0;
memory_address_B=n_nnz_A*data_width
memory_address_C=memory_address_B+(n_nnz_B*data_width);

if(generate_inner_product):
    # Extracting bitmap A
    matrixA_data_bitmap=matrixA_csr.data;
    dim_bitmap=matrixA_pointers_csr.shape[0]-1
    matrixA_bitmap_bitmap=np.zeros(dim_bitmap*dim_bitmap, dtype=np.int8)
    for i in range(dim_bitmap):
        for j in range(matrixA_pointers_csr[i], matrixA_pointers_csr[i+1]):
            matrixA_bitmap_bitmap[i*dim_bitmap+matrixA_indices_csr[j]]=1
    
    matrixB_data_bitmap=matrixB_csc.data; # The data goes in CSC but the bitmap in CSR
    dim_bitmap=matrixA_pointers_csr.shape[0]-1
    matrixB_bitmap_bitmap=np.zeros(dim_bitmap*dim_bitmap, dtype=np.int8)
    for i in range(dim_bitmap):
        for j in range(matrixB_pointers_csr[i], matrixB_pointers_csr[i+1]):
            matrixB_bitmap_bitmap[i*dim_bitmap+matrixB_indices_csr[j]]=1


    # Generating the files for Inner Product
    with open(innerProduct_mem_file, "w") as fd, open(innerProduct_a_bitmap_file, "w") as fbA, open(innerProduct_b_bitmap_file, "w") as fbB:
        # Writing memory
        for i in matrixA_data_bitmap:
            ba = bytearray(struct.pack(">f", i)) # Getting list of bytes
            my_int = int.from_bytes(ba, "big")
            fd.write(str(my_int)+",")
        for i in matrixB_data_bitmap:
            ba = bytearray(struct.pack(">f", i)) # Getting list of bytes
            my_int = int.from_bytes(ba, "big")
            fd.write(str(my_int)+",")
        # Writing bitmap A
        for i in matrixA_bitmap_bitmap:
            fbA.write(str(i)+",")
        #Writing bitmap B
        for i in matrixB_bitmap_bitmap:
            fbB.write(str(i)+",")

    print("Inner Product files generated correctly:")
    print("  -"+innerProduct_mem_file)
    print("  -"+innerProduct_a_bitmap_file)
    print("  -"+innerProduct_b_bitmap_file)

# Generating the files for outer product
if(generate_outer_product):
    with open(outerProduct_mem_file, "w") as fd, open(outerProduct_a_rowpointer_file, "w") as rpA, open(outerProduct_a_colpointer_file, "w") as cpA, open(outerProduct_b_rowpointer_file, "w") as rpB, open(outerProduct_b_colpointer_file, "w") as cpB:
        # Writing memory
        for i in matrixA_data_csc:
            ba = bytearray(struct.pack(">f", i)) # Getting list of bytes
            my_int = int.from_bytes(ba, "big")
            fd.write(str(my_int)+",")
        for i in matrixB_data_csr:
            ba = bytearray(struct.pack(">f", i)) # Getting list of bytes
            my_int = int.from_bytes(ba, "big")
            fd.write(str(my_int)+",")
        # Writing pointers for matrixA
        for i in matrixA_pointers_csc:
            rpA.write(str(i)+",")
        # Writing indices for matrixA
        for i in matrixA_indices_csc:
            cpA.write(str(i)+",")
        # Writing pointers for matrixB
        for i in matrixB_pointers_csr:
            rpB.write(str(i)+",")
        # Writing indices for matrixB
        for i in matrixB_indices_csr:
            cpB.write(str(i)+",")

    print("Outer Product files generated correctly:")
    print("  -"+outerProduct_mem_file)
    print("  -"+outerProduct_a_rowpointer_file)
    print("  -"+outerProduct_a_colpointer_file)
    print("  -"+outerProduct_b_rowpointer_file)
    print("  -"+outerProduct_b_colpointer_file)

# Generating the files for gustavsons 
if(generate_gustavsons):
    with open(gustavsons_mem_file, "w") as fd, open(gustavsons_a_rowpointer_file, "w") as rpA, open(gustavsons_a_colpointer_file, "w") as cpA, open(gustavsons_b_rowpointer_file, "w") as rpB, open(gustavsons_b_colpointer_file, "w") as cpB:
        # Writing memory
        for i in matrixA_data_csr:
            ba = bytearray(struct.pack(">f", i)) # Getting list of bytes
            my_int = int.from_bytes(ba, "big")
            fd.write(str(my_int)+",")
        for i in matrixB_data_csr:
            ba = bytearray(struct.pack(">f", i)) # Getting list of bytes
            my_int = int.from_bytes(ba, "big")
            fd.write(str(my_int)+",")
        # Writing pointers for matrixA
        for i in matrixA_pointers_csr:
            rpA.write(str(i)+",")
        # Writing indices for matrixA
        for i in matrixA_indices_csr:
            cpA.write(str(i)+",")
        # Writing pointers for matrixB
        for i in matrixB_pointers_csr:
            rpB.write(str(i)+",")
        # Writing indices for matrixB
        for i in matrixB_indices_csr:
            cpB.write(str(i)+",")

    print("Gustavsons files generated correctly:")
    print("  -"+gustavsons_mem_file)
    print("  -"+gustavsons_a_rowpointer_file)
    print("  -"+gustavsons_a_colpointer_file)
    print("  -"+gustavsons_b_rowpointer_file)
    print("  -"+gustavsons_b_colpointer_file)

print("Script ended successfully")
print("PARAMETER M:"+str(dim_bitmap))
print("PARAMETER K:"+str(dim_bitmap))
print("PARAMETER N:"+str(dim_bitmap))
print("PARAMETER MEMORY_ADDRESS_A:"+str(memory_address_A))
print("PARAMETER MEMORY_ADDRESS_B:"+str(memory_address_B))
print("PARAMETER MEMORY_ADDRESS_C:"+str(memory_address_C))

