import sst

# Define SST core options
sst.setProgramOption("timebase", "1 ps")
sst.setProgramOption("stopAtCycle", "10000s")

# Define the simulation components
comp_stonne = sst.Component("stonne1", "sstStonne.MAERI")
comp_stonne.addParams({
    "hardware_configuration" : "maeri_128mses_128_bw.cfg",
    "kernelOperation" : "CONV",
    "R" : 3,
    "S" : 3,
    "C" : 3,
    "K" : 1,
    "G" : 1,
    "N" : 1,
    "X" : 6,
    "Y" : 6,
    "T_R" : 3,
    "T_S" : 3,
    "T_C" : 1,
    "T_K" : 1,
    "T_G" : 1,
    "T_N" : 1,
    "T_X_" : 1,
    "T_Y_" : 1,
    "mem_matrix_a_init" : "conv_file_matrixA_3_3_31_1_6_6.in",
    "mem_matrix_b_init" : "conv_file_matrixB_3_3_31_1_6_6.in",
    "mem_matrix_c_init" : "result.out"

})

sst.setStatisticLoadLevel(4)

# Enable statistics outputs

