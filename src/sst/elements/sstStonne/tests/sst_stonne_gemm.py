import sst

# Define SST core options
sst.setProgramOption("timebase", "1 ps")
sst.setProgramOption("stopAtCycle", "10000s")

# Define the simulation components
comp_stonne = sst.Component("stonne1", "sstStonne.MAERI")
comp_stonne.addParams({
    "hardware_configuration" : "maeri_128mses_128_bw.cfg",
    "kernelOperation" : "GEMM",
    "GEMM_K" : 20,
    "GEMM_N" : 3,
    "GEMM_M" : 3,
    "GEMM_T_K" : 5,
    "GEMM_T_M" : 1,
    "GEMM_T_N" : 1,
    "mem_matrix_a_init" : "in_file_matrixA_3_3_20.in",
    "mem_matrix_b_init" : "in_file_matrixB_3_3_20.in",
    "mem_matrix_c_init" : "result.out"

})

sst.setStatisticLoadLevel(4)

# Enable statistics outputs

