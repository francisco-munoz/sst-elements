import sst

# Define SST core options
sst.setProgramOption("timebase", "1 ps")
sst.setProgramOption("stopAtCycle", "10000s")

# Define the simulation components
comp_stonne = sst.Component("stonne1", "sstStonne.MAERI")
comp_stonne.addParams({
    "hardware_configuration" : "magma_128mses_128_bw.cfg",
    "kernelOperation" : "csrSpMM",
    "GEMM_K" : 4,
    "GEMM_N" : 4,
    "GEMM_M" : 4,
    "GEMM_T_K" :4,
    "GEMM_T_N" : 1,
    "mem_matrix_a_init" : "csr/matrixAData",
    "mem_matrix_b_init" : "csr/matrixBData",
    "mem_matrix_c_init" : "result.out",
    "rowpointer_matrix_a_init" : "csr/rowpointermatrixA",
    "colpointer_matrix_a_init" : "csr/colpointermatrixA",


})

sst.setStatisticLoadLevel(4)

# Enable statistics outputs

