import sst

# Define SST core options
sst.setProgramOption("timebase", "1 ps")
sst.setProgramOption("stopAtCycle", "10000s")

statLevel = 16
max_addr_gb = 1
tile_clk_mhz = 1


# Define the simulation components
comp_stonne = sst.Component("stonne1", "sstStonne.MAERI")
comp_stonne.addParams({
    "hardware_configuration" : "sigma_128mses_128_bw.cfg",
    "kernelOperation" : "bitmapSpMSpM",
    "GEMM_K" : 2048,
    "GEMM_N" : 2,
    "GEMM_M" : 2,
    "bitmap_matrix_a_init" : "bitmapSpMSpM_file_bitmapA_2_2_2048.in",
    "bitmap_matrix_b_init" : "bitmapSpMSpM_file_bitmapB_2_2_2048.in",
    "mem_init" : "bitmapSpMSpM_gemm_mem.ini",
    "matrix_a_dram_address" : 0,
    "matrix_b_dram_address" : 3300,
    "matrix_c_dram_address" : 6404,
    "mem_matrix_c_file_name" : "result.out"

})

df_l1cache = sst.Component("l1cache", "memHierarchy.Cache")
df_l1cache.addParams({
    "access_latency_cycles" : "2",
    "cache_frequency" : str(tile_clk_mhz) + "GHz",
    "replacement_policy" : "lru",
    "coherence_protocol" : "MESI",
    "associativity" : "1",
    "cache_line_size" : "16",
    "verbose" : 10,
    "debug" : 1,
    "debug_level" : 100,
    "L1" : "1",
    "cache_size" : "512B"
})

df_memory = sst.Component("memory", "memHierarchy.MemController")
df_memory.addParams({
    "backing" : "mmap",
    "verbose" : 10,
    "debug" : 1,
    "debug_level" : 100,
    "clock" : str(tile_clk_mhz) + "GHz",
})

backend = df_memory.setSubComponent("backend", "memHierarchy.simpleMem")
backend.addParams({
    "access_time" : "4 ns",
    "mem_size" : str(max_addr_gb) + "GiB"
    #"mem_size" : str(1048576) + "B", # 1 MB
})

# Enable SST Statistics Outputs for this simulation
sst.setStatisticLoadLevel(statLevel)
sst.enableAllStatisticsForAllComponents({"type":"sst.AccumulatorStatistic"})
sst.setStatisticOutput("sst.statOutputTXT", { "filepath" : "output.csv" })

# Define the simulation links
link_df_cache_link = sst.Link("link_cpu_cache_link")
link_df_cache_link.connect( (comp_stonne, "cache_link", "1ps"), (df_l1cache, "high_network_0", "1ps") )
link_df_cache_link.setNoCut()

link_mem_bus_link = sst.Link("link_mem_bus_link")
link_mem_bus_link.connect( (df_l1cache, "low_network_0", "5ps"), (df_memory, "direct_link", "5ps") )



#sst.setStatisticLoadLevel(4)


# Enable statistics outputs

