import sst

# Define SST core options
sst.setProgramOption("timebase", "1 ps")
sst.setProgramOption("stopAtCycle", "10000s")

# Define the simulation components
comp_stonne = sst.Component("stonne1", "sstStonne.MAERI")
comp_stonne.addParams({
    "hardware_configuration" : "sigma_128mses_128_bw.cfg",
    "kernelOperation" : "bitmapSpMSpM",
    "GEMM_K" : 20,
    "GEMM_N" : 3,
    "GEMM_M" : 3,
    "mem_matrix_a_init" : "bitmapSpMSpM_file_matrixA_3_3_20.in",
    "mem_matrix_b_init" : "bitmapSpMSpM_file_matrixB_3_3_20.in",
    "mem_matrix_c_init" : "result.out",
    "bitmap_matrix_a_init" : "bitmapSpMSpM_file_bitmapA_3_3_20.in",
    "bitmap_matrix_b_init" : "bitmapSpMSpM_file_bitmapB_3_3_20.in",


})

sst.setStatisticLoadLevel(4)

# Enable statistics outputs

