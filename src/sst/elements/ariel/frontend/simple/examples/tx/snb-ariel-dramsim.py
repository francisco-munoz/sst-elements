import sst
import os
import ConfigParser, argparse

# Command line args
parser = argparse.ArgumentParser()
parser.add_argument("-c", "--config", help="config file", required=True)
parser.add_argument("-f", "--statfile", help="stat output file", default="./stats-snb-ariel.csv")
args = parser.parse_args();

cfgFile = args.config
statFile = args.statfile

next_core_id = 0
next_network_id = 0
next_memory_ctrl_id = 0
next_l3_cache_id = 0

clock = "2660MHz"
memory_clock = "200MHz"
coherence_protocol = "MESI"

cores_per_group = 2
memory_controllers_per_group = 1
groups = 4

os.environ['OMP_NUM_THREADS'] = str(cores_per_group * groups)

l3cache_blocks_per_group = 2
l3cache_block_size = "2MB"

l3_cache_per_core  = int(l3cache_blocks_per_group / cores_per_group)
l3_cache_remainder = l3cache_blocks_per_group - (l3_cache_per_core * cores_per_group)

ring_latency = "300ps" # 2.66 GHz time period plus slack for ringstop latency
ring_bandwidth = "96GiB/s" # 2.66GHz clock, moves 64-bytes per cycle, plus overhead = 36B/c
ring_flit_size = "8B"

memory_network_bandwidth = "96GiB/s"

mem_interleave_size = 64	# Do 4K page level interleaving
memory_capacity = 16384 	# Size of memory in MBs
page_size = 4                   # In KB 
num_pages = memory_capacity * 1024 / page_size

streamN = 1000000

l1_prefetch_params = {
	"prefetcher": "cassini.StridePrefetcher",
       	"prefetcher.reach": 4,
	"prefetcher.detect_range" : 1
}

l2_prefetch_params = {
       	"prefetcher": "cassini.StridePrefetcher",
       	"prefetcher.reach": 16,
	"prefetcher.detect_range" : 1
}

ringstop_params = {
        "torus:shape" : groups * (cores_per_group + memory_controllers_per_group + l3cache_blocks_per_group),
        "output_latency" : "25ps",
        "xbar_bw" : ring_bandwidth,
        "input_buf_size" : "2KiB",
        "input_latency" : "25ps",
        "num_ports" : "3",
        "torus:local_ports" : "1",
        "flit_size" : ring_flit_size,
       	"output_buf_size" : "2KiB",
       	"link_bw" : ring_bandwidth,
       	"torus:width" : "1",
        "topology" : "merlin.torus"
}

# ariel cpu
ariel = sst.Component("a0", "ariel.ariel")
ariel.addParams({
    "verbose" : 1,
#    "alloctracker" : 1,
    "clock" : clock,
    "maxcorequeue" : 256,
    "maxissuepercycle" : 3,
    "pipetimeout" : 0,
    "corecount" : groups * cores_per_group,
    "memorylevels" : 1,
    "pagecount0" : num_pages,
    "pagesize0" : page_size * 1024,
#    "arielstack" : 1,
    "defaultlevel" : 0,
    "arielinterceptcalls" : 0
})

# Get app args from the config file
cp = ConfigParser.ConfigParser()
if not cp.read(cfgFile):
    raise Exception('Could not read file "%s"'%cfgFile)

print "Getting appArgs from %s"%cfgFile

appArgs = dict(cp.items('app'))
ariel.addParams(appArgs);

l1_params = {
        "cache_frequency": clock,
        "cache_size": "32KiB",
        "associativity": 8,
        "access_latency_cycles": 4,
       	"L1": 1,
        # Default params
        # "cache_line_size": 64,
	# "coherence_protocol": coherence_protocol,
        # "replacement_policy": "lru",
        "maxRequestDelay" : "1000000",
}

l2_params = {
        "cache_frequency": clock,
        "cache_size": "256KiB",
        "associativity": 8,
        "access_latency_cycles": 6,
        "mshr_num_entries" : 16,
	"network_bw": ring_bandwidth,
        # Default params
        #"cache_line_size": 64,
        #"coherence_protocol": coherence_protocol,
        #"replacement_policy": "lru",
}

l3_params = {
      	"access_latency_cycles" : "12",
      	"cache_frequency" : clock,
      	"associativity" : "16",
      	"cache_size" : l3cache_block_size,
      	"mshr_num_entries" : "4096",
      	"network_bw": ring_bandwidth,
      	# Distributed caches
        "num_cache_slices" : str(groups * l3cache_blocks_per_group),
      	"slice_allocation_policy" : "rr",
        # Default params
      	# "replacement_policy" : "lru",
      	# "cache_line_size" : "64",
      	# "coherence_protocol" : coherence_protocol,
}

mem_params = {
	"backend.mem_size" : memory_capacity / (groups * memory_controllers_per_group),
        "backend" : "memHierarchy.dramsim",
        "clock" : memory_clock,
	"network_bw": ring_bandwidth,
        "max_requests_per_cycle" : 1, # Somehow need to make sure that even though we're at a low mem clock, the rate of requests to dram is right
        "do_not_back" : 1,
        "backend.system_ini" : "/home/grvosku/experiments/snb-dramsim/system.ini",
        "backend.device_ini" : "/home/grvosku/experiments/snb-dramsim/DDR3_micron_32M_8B_x4_sg125.ini",
}

dc_params = {
        "interleave_size": str(mem_interleave_size) + "B",
        "interleave_step": str((groups * memory_controllers_per_group) * (mem_interleave_size)) + "B",
        "entry_cache_size": 256*1024*1024, #Entry cache size of mem/blocksize
        "clock": memory_clock,
       	"network_bw": ring_bandwidth,
        # Default params
	# "coherence_protocol": coherence_protocol,
}

router_map = {}

print "Configuring Ring Network-on-Chip..."

for next_ring_stop in range((cores_per_group + memory_controllers_per_group + l3cache_blocks_per_group) * groups):
	ring_rtr = sst.Component("rtr." + str(next_ring_stop), "merlin.hr_router")
        ring_rtr.addParams(ringstop_params)
        ring_rtr.addParams({
               	"id" : next_ring_stop
               	})
        router_map["rtr." + str(next_ring_stop)] = ring_rtr

for next_ring_stop in range((cores_per_group + memory_controllers_per_group + l3cache_blocks_per_group) * groups):
	if next_ring_stop == 0:
               	rtr_link_positive = sst.Link("rtr_pos_" + str(next_ring_stop))
               	rtr_link_positive.connect( (router_map["rtr.0"], "port0", ring_latency), (router_map["rtr.1"], "port1", ring_latency) )
               	rtr_link_negative = sst.Link("rtr_neg_" + str(next_ring_stop))
                rtr_link_negative.connect( (router_map["rtr.0"], "port1", ring_latency), (router_map["rtr." + str(((cores_per_group + memory_controllers_per_group + l3cache_blocks_per_group) * groups) - 1)], "port0", ring_latency) )
       	elif next_ring_stop == ((cores_per_group + memory_controllers_per_group + l3cache_blocks_per_group) * groups) - 1:
               	rtr_link_positive = sst.Link("rtr_pos_" + str(next_ring_stop))
               	rtr_link_positive.connect( (router_map["rtr." + str(next_ring_stop)], "port0", ring_latency), (router_map["rtr.0"], "port1", ring_latency) )
               	rtr_link_negative = sst.Link("rtr_neg_" + str(next_ring_stop))
               	rtr_link_negative.connect( (router_map["rtr." + str(next_ring_stop)], "port1", ring_latency), (router_map["rtr." + str(next_ring_stop-1)], "port0", ring_latency) )
       	else:
               	rtr_link_positive = sst.Link("rtr_pos_" + str(next_ring_stop))
               	rtr_link_positive.connect( (router_map["rtr." + str(next_ring_stop)], "port0", ring_latency), (router_map["rtr." + str(next_ring_stop+1)], "port1", ring_latency) )
               	rtr_link_negative = sst.Link("rtr_neg_" + str(next_ring_stop))
                rtr_link_negative.connect( (router_map["rtr." + str(next_ring_stop)], "port1", ring_latency), (router_map["rtr." + str(next_ring_stop-1)], "port0", ring_latency) )

for next_group in range(groups):
	print "Configuring core and memory controller group " + str(next_group) + "..."

	for next_active_core in range(cores_per_group):
		for next_l3_cache_block in range(l3_cache_per_core):
			print "Creating L3 cache block " + str(next_l3_cache_id) + "..."
			
			l3cache = sst.Component("l3cache_" + str(next_l3_cache_id), "memHierarchy.Cache")
			l3cache.addParams(l3_params)

			l3cache.addParams({
       	        "network_address" : next_network_id,
				"slice_id" : str(next_l3_cache_id)
       	    })

			l3_ring_link = sst.Link("l3_" + str(next_l3_cache_id) + "_link")
			l3_ring_link.connect( (l3cache, "directory", ring_latency), (router_map["rtr." + str(next_network_id)], "port2", ring_latency) )		

			next_l3_cache_id = next_l3_cache_id + 1
			next_network_id = next_network_id + 1
		
		print "Creating Core " + str(next_active_core) + " in Group " + str(next_group)

		l1 = sst.Component("l1cache_" + str(next_core_id), "memHierarchy.Cache")
		l1.addParams(l1_params)
		#l1.addParams(l1_prefetch_params)

		l2 = sst.Component("l2cache_" + str(next_core_id), "memHierarchy.Cache")
		l2.addParams({
			"network_address" : next_network_id
			})
		l2.addParams(l2_params)
		#l2.addParams(l2_prefetch_params)

                arielL1Link = sst.Link("cpu_cache_link_" + str(next_core_id))
                arielL1Link.connect((ariel, "cache_link_%d"%next_core_id, ring_latency), (l1, "high_network_0", ring_latency))
		arielL1Link.setNoCut()

		l2_core_link = sst.Link("l2cache_" + str(next_core_id) + "_link")
       		l2_core_link.connect((l1, "low_network_0", ring_latency), (l2, "high_network_0", ring_latency))				
		l2_core_link.setNoCut()

		l2_ring_link = sst.Link("l2_ring_link_" + str(next_core_id))
       		l2_ring_link.connect((l2, "cache", ring_latency), (router_map["rtr." + str(next_network_id)], "port2", ring_latency))

		next_network_id = next_network_id + 1
		next_core_id = next_core_id + 1

	for next_l3_cache_block in range(l3_cache_remainder):
		print "Creating L3 cache block: " + str(next_l3_cache_id) + "..."

		l3cache = sst.Component("l3cache_" + str(next_l3_cache_id), "memHierarchy.Cache")
		l3cache.addParams(l3_params)

		l3cache.addParams({
       			"network_address" : next_network_id,
			"slice_id" : str(next_l3_cache_id)
     		})

		l3_ring_link = sst.Link("l3_" + str(next_l3_cache_id) + "_link")
		l3_ring_link.connect( (l3cache, "directory", ring_latency), (router_map["rtr." + str(next_network_id)], "port2", ring_latency) )		

		next_l3_cache_id = next_l3_cache_id + 1
		next_network_id = next_network_id + 1

	for next_mem_ctrl in range(memory_controllers_per_group):	
		local_size = memory_capacity / (groups * memory_controllers_per_group)

		mem = sst.Component("memory_" + str(next_memory_ctrl_id), "memHierarchy.MemController")
		mem.addParams(mem_params)

		dc = sst.Component("dc_" + str(next_memory_ctrl_id), "memHierarchy.DirectoryController")
		dc.addParams({
			"network_address" : next_network_id,
			"addr_range_start" : next_memory_ctrl_id * mem_interleave_size,
			"addr_range_end" : (memory_capacity * 1024 * 1024) - (groups * memory_controllers_per_group * mem_interleave_size) + (next_memory_ctrl_id * mem_interleave_size)
		})
		dc.addParams(dc_params)

		memLink = sst.Link("mem_link_" + str(next_memory_ctrl_id))
		memLink.connect((mem, "direct_link", ring_latency), (dc, "memory", ring_latency))

		netLink = sst.Link("dc_link_" + str(next_memory_ctrl_id))
		netLink.connect((dc, "network", ring_latency), (router_map["rtr." + str(next_network_id)], "port2", ring_latency))

		next_network_id = next_network_id + 1
		next_memory_ctrl_id = next_memory_ctrl_id + 1

# ===============================================================================

# Enable SST Statistics Outputs for this simulation
sst.setStatisticLoadLevel(16)
sst.enableAllStatisticsForAllComponents({"type":"sst.AccumulatorStatistic"})

sst.setStatisticOutput("sst.statOutputCSV")
sst.setStatisticOutputOptions( {
	"filepath"  : "./stats-snb-miranda.csv",
	"separator" : ", "
} )

print "Completed configuring the SST Sandy Bridge model"
