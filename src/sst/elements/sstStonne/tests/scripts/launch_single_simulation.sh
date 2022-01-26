#/bin/bash

if [ "$#" -ne 7 ]
then
    echo "Usage: ./$0 M N K MK_sparsity KN_sparsity stonne_inner_conf_file stonne_outer_conf_file"
    exit 1
fi

#echo "Configuring M to $1"
#echo "Configuring N to $2"
#echo "Configuring K to $3"
#echo "Configuring sparsity of MK to $4"
#echo "Configuring sparsity of KN to $5"
#echo "Configuring stonne inner product hardware configuration file to $6"
#echo "Configuring stonne outer product hardware configuration file to $7"

M=$1
N=$2
K=$3
MK_SPARSITY=$4
KN_SPARSITY=$5
STONNE_INNER_CONF_FILE=$6
STONNE_OUTER_CONF_FILE=$7

#echo "Generating temporal files"
cp template_gen_outerProductGEMM.py temporal_gen_outerProductGEMM.py
cp template_gen_bitmapSpMSpM.py temporal_gen_bitmapSpMSpM.py
cp template_sst_stonne_outerProduct.py temporal_sst_stonne_outerProduct.py
cp template_sst_stonne_bitmapSpMSpM.py temporal_sst_stonne_bitmapSpMSpM.py

sed -i "s/GEMM_M_PARAMETER/$M/g" temporal_*
sed -i "s/GEMM_N_PARAMETER/$N/g" temporal_*
sed -i "s/GEMM_K_PARAMETER/$K/g" temporal_*
sed -i "s/SPARSITY_RATIO_A_PARAMETER/$MK_SPARSITY/g" temporal_*
sed -i "s/SPARSITY_RATIO_B_PARAMETER/$KN_SPARSITY/g" temporal_*
sed -i "s/STONNE_INNER_PRODUCT_FILE/$STONNE_INNER_CONF_FILE/g" temporal_*
sed -i "s/STONNE_OUTER_PRODUCT_FILE/$STONNE_OUTER_CONF_FILE/g" temporal_*

# Generating matrices and addresses for inner product
output_cmd=$(python temporal_gen_bitmapSpMSpM.py)
address_b=$(echo "$output_cmd" | grep "Offset matrix B" | tr -d ' ' | cut -f 2 -d:)
address_c=$(echo "$output_cmd" | grep "Offset matrix C" | tr -d ' ' | cut -f 2 -d:)
     #  Replacing address in sst conf file 
     sed -i "s/MATRIX_B_DRAM_ADDRESS_PARAMETER/$address_b/g" temporal_sst_stonne_bitmapSpMSpM.py
     sed -i "s/MATRIX_C_DRAM_ADDRESS_PARAMETER/$address_c/g" temporal_sst_stonne_bitmapSpMSpM.py

# Generating matrices and addresses for outer product
output_cmd=$(python temporal_gen_outerProductGEMM.py)
address_b=$(echo "$output_cmd" | grep "Offset matrix B" | tr -d ' ' | cut -f 2 -d:)
address_c=$(echo "$output_cmd" | grep "Offset matrix C" | tr -d ' ' | cut -f 2 -d:)
     #  Replacing address in sst conf file
     sed -i "s/MATRIX_B_DRAM_ADDRESS_PARAMETER/$address_b/g" temporal_sst_stonne_outerProduct.py
     sed -i "s/MATRIX_C_DRAM_ADDRESS_PARAMETER/$address_c/g" temporal_sst_stonne_outerProduct.py

#echo "Temporal files generated successfully"
#echo "Running simulations"
#running inner product
sst temporal_sst_stonne_bitmapSpMSpM.py > /dev/null 
cycles_inner=$(cat *.counters | head -1 | cut -f 2 -d=)
l1_hits_inner=$(cat output.csv | grep hit | grep -o "Sum.u64 = [0-9]*" | cut -f 2 -d= | tr -d ' ' | paste -s -d'+' | bc)
l1_miss_inner=$(cat output.csv | grep miss | grep -o "Sum.u64 = [0-9]*" | cut -f 2 -d= | tr -d ' ' | paste -s -d'+' | bc)
idle_cycles_ms_inner=$(cat *.txt | grep "Idle_cycles_mswitch" | tr -d , | tr -d ' ' | cut -f 2 -d: | paste -d+ -s | bc)
idle_cycles_as_inner=$(cat *.txt | grep "Idle_cycles_aswitch" | tr -d , | tr -d ' ' | cut -f 2 -d: | paste -d+ -s | bc)
rm *.counters
rm *.txt
rm output.csv
sst temporal_sst_stonne_outerProduct.py > /dev/null
cycles_outer=$(cat *.counters | head -1 | cut -f 2 -d=)
l1_hits_outer=$(cat output.csv | grep hit | grep -o "Sum.u64 = [0-9]*" | cut -f 2 -d= | tr -d ' ' | paste -s -d'+' | bc)
l1_miss_outer=$(cat output.csv | grep miss | grep -o "Sum.u64 = [0-9]*" | cut -f 2 -d= | tr -d ' ' | paste -s -d'+' | bc)
n_cycles_multiplying_outer=$(cat *.txt | grep N_cycles_multiplying | tr -d , | tr -d ' ' | cut -f 2 -d:)
n_cycles_merging_outer=$(cat *.txt | grep N_cycles_merging | tr -d , | tr -d ' ' | cut -f 2 -d:)
idle_cycles_ms_outer=$(cat *.txt | grep "Idle_cycles_mswitch" | tr -d , | tr -d ' ' | cut -f 2 -d: | paste -d+ -s | bc)
idle_cycles_as_outer=$(cat *.txt | grep "Idle_cycles_aswitch" | tr -d , | tr -d ' ' | cut -f 2 -d: | paste -d+ -s | bc)

rm *.counters
rm *.txt
rm output.csv


#echo "Deleting temporal files"
rm -f temporal_*
rm -f *.in
rm -f *.ini
echo "$cycles_inner $cycles_outer $idle_cycles_ms_inner $idle_cycles_ms_outer $idle_cycles_as_inner $idle_cycles_as_outer $n_cycles_multiplying_outer $n_cycles_merging_outer $l1_hits_inner $l1_hits_outer $l1_miss_inner $l1_miss_outer"




