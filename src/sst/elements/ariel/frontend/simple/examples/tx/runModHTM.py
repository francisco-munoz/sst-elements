# -*- coding: utf-8 -*-
import sst
import os

#Global Params
memDebug = 1
memDebugLevel = 10
reqDelay = 50000

coherenceProtocol = "MESI"
l1_rplPolicy = "lru"
rplPolicy = "random"
busLat = "50 ps"
freeLat = "0 ps"
cacheFrequency = "2 Ghz"
defaultLevel = 0
cacheLineSize = 64

coreCount = 4

## Application Info
os.environ['SIM_DESC'] = 'ONE_CORES'
os.environ['OMP_NUM_THREADS'] = str(coreCount)

sst_root = os.environ[ "SST_ROOT" ]
#test_program = os.environ[ "TEST_PROG" ]
test_program = "/home/chughes/sst/src/sst-elements/src/sst/elements/ariel/frontend/simple/examples/newTests/src/add_tm.x"
program_name = os.path.basename(test_program)

## Statistics Init (<path>/<bin>.<coreCount>.csv
sst.setStatisticLoadLevel(1)
sst.setStatisticOutput("sst.statOutputCSV", {"filepath" : "./results." + program_name + "." + str(coreCount) + ".csv",
                                                         "separator" : ", "
                                            })

print "Running %s" %test_program

## -----------------------------------------------------------------------------------------------------------------
ariel = sst.Component("a0", "ariel.ariel")                                                                          
ariel.addParams({                                                                                                   
        "verbose"               : "1",
        "maxcorequeue"          : "256",                                                                            
        "maxissuepercycle"      : "2",                                                                              
        "pipetimeout"           : "0",                                                                              
        "executable"            : test_program,                                                                     
        "arielmode"             : "1",                                                                              
        "arielinterceptcalls"   : "1",                                                                              
        "corecount"             : coreCount,                                                                        
        "memmgr.memorylevels"   : "1",                                                                              
        "memmgr.defaultlevel"   : defaultLevel                                                                      
})                                                                                                                  


def memInit(numCores):

        membus = sst.Component("membus", "memHierarchy.Bus")
        membus.addParams({                                  
                "bus_frequency" : cacheFrequency            
        })                                                  

        memory = sst.Component("memory", "memHierarchy.MemController")
        memory.addParams({                                            
                "range_start"           : "0",                        
                "backend"               : "memHierarchy.dramsim",     
		"do_not_back"		: "1",
                "backend.mem_size"      : "4GiB",                     
                "backend.access_time"   : "100 ns",                   
                "backend.device_ini"    : "DDR3_micron_32M_8B_x4_sg125.ini",
                "backend.system_ini"    : "system.ini",                     
                "clock"                 : "1GHz",                           
                "coherence_protocol"    : coherenceProtocol,                
                "request_width"         : cacheLineSize,                    
                "device_ini"            : "DDR3_micron_32M_8B_x4_sg125.ini",
                "system_ini"            : "system.ini",                     
                "trace_file"            : "./mem_trace.out",                
                "debug"                 : memDebug                          
        })                                                                  

        ## Cache Init
        for core in range (numCores):
		htm = sst.Component("htm_%d"%core, "memHierarchy.HTM")
                htm.addParams({                                             
                        "cache_frequency"       : cacheFrequency,          
                        "cache_size"            : "32KiB",                 
                        "cache_line_size"       : cacheLineSize,           
                        "associativity"         : "1",                     
                        "access_latency_cycles" : "4",                     
                        "coherence_protocol"    : coherenceProtocol,       
                        "replacement_policy"    : l1_rplPolicy,            
                        "L1"                    : "0",
                        "tm_cache"              : "1",                     
                        "debug"                 : memDebug,                
                        "debug_level"           : memDebugLevel            
                })

                l1 = sst.Component("l1cache_%d"%core, "memHierarchy.Cache")
                l1.addParams({                                             
                        "cache_frequency"       : cacheFrequency,          
                        "cache_size"            : "32KiB",                 
                        "cache_line_size"       : cacheLineSize,           
                        "associativity"         : "1",                     
                        "access_latency_cycles" : "4",                     
                        "coherence_protocol"    : coherenceProtocol,       
                        "replacement_policy"    : l1_rplPolicy,            
                        "L1"                    : "1",
                        "debug"                 : memDebug,                
                        "debug_level"           : memDebugLevel,
                        "maxRequestDelay"       : reqDelay
                })                                                         

                l2 = sst.Component("l2cache_%d"%core, "memHierarchy.Cache")
                l2.addParams({                                             
                        "cache_frequency"       : cacheFrequency,          
                        "cache_size"            : "256KiB",                
                        "cache_line_size"       : cacheLineSize,           
                        "cache_type"		: "inclusive",
                        "associativity"         : "1",                     
                        "access_latency_cycles" : "10",                    
                        "coherence_protocol"    : coherenceProtocol,       
                        "replacement_policy"    : rplPolicy,               
                        "L1"                    : "0",                     
                        "debug"                 : memDebug,                
                        "debug_level"           : memDebugLevel,           
                        "mshr_num_entries"      : "16",
                        "maxRequestDelay"       : reqDelay
                })                                                         

                ## Bus Init
                proc_htm_link = sst.Link("proc_htm_%d"%core)
                proc_htm_link.connect( (ariel, "cache_link_%d"%core, busLat), (htm, "htm_high_link", busLat) )

		htm_l1_link = sst.Link("htm_cache_%d"%core)
		htm_l1_link .connect( (htm, "htm_low_link", busLat), (l1,  "high_network_0", busLat) )

                l1_l2_link = sst.Link("l1_l2_link_%d"%core)
                l1_l2_link.connect( (l1, "low_network_0", busLat), (l2, "high_network_0", busLat) ) 

#                proc_l1_link = sst.Link("proc_cache_%d"%core)
#                proc_l1_link.connect( (ariel, "cache_link_%d"%core, busLat), (l1, "high_network_0", busLat) )

#		l1_htm_link = sst.Link("htm_cache_%d"%core)
#		l1_htm_link.connect( (l1, "low_network_0", freeLat), (htm, "htm_high_link", freeLat) )

#		htm_l2_link = sst.Link("htm_l2_link_%d"%core)
#		htm_l2_link.connect( (htm, "htm_low_link", freeLat), (l2, "high_network_0", busLat) )

                l2_bus_link = sst.Link("l2_membus_%d"%core)
                l2_bus_link.connect( (l2, "low_network_0", busLat), (membus, "high_network_%d"%core, busLat) )

                l1.enableAllStatistics()
                l2.enableAllStatistics()


        ## LLC Init
        llc = sst.Component("llc", "memHierarchy.Cache")
        llc.addParams({
                "cache_frequency"       : cacheFrequency,
                "cache_size"            : "8MiB",
                "cache_line_size"       : cacheLineSize,
                "cache_type"		: "noninclusive",
                "associativity"         : "1",
                "access_latency_cycles" : "20",
                "coherence_protocol"    : coherenceProtocol,
                "replacement_policy"    : rplPolicy,
                "L1"                    : "0",
                "debug"                 : memDebug,
                "debug_level"           : memDebugLevel,
                "mshr_num_entries"      : "16",
                "maxRequestDelay"       : reqDelay                     
        })

        # Bus to LLC and LLC <-> MM
        bus_llc_link = sst.Link("bus_llc_link")
        bus_llc_link.connect( (membus, "low_network_0", busLat), (llc, "high_network_0", busLat) )
        llc_memCTRL_link = sst.Link("llc_memCTRL_link")
        llc_memCTRL_link.connect( (llc, "low_network_0", busLat), (memory, "direct_link", busLat) )

        llc.enableAllStatistics()
        memory.enableAllStatistics()

memInit(coreCount)

ariel.enableAllStatistics()

