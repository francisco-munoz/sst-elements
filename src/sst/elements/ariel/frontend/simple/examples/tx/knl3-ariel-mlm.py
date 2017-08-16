import os
import sst
from sst.merlin import *
import ConfigParser, argparse

# Command line args
parser = argparse.ArgumentParser()
parser.add_argument("-c", "--config", help="config file", required=True)
parser.add_argument("-f", "--statfile", help="stat output file", default="./stats-snb-ariel.csv")
parser.add_argument("-d", "--ddr", help="dram backend name", default="timing")
parser.add_argument("-h", "--hbm", help="hbm backedn name", default="vaultsim")
args = parser.parse_args();

cfgFile = args.config
statFile = args.statfile
backend = args.ddr
hbmBackend = args.hbm

print "Using DRAM model: %s"%backend
print "Using HBM model: %s"%hbmBackend

corecount = 72

os.environ['OMP_NUM_THREADS'] = str(64)

# Params to configure - currently config for HMC only
hbmCapacity = 16    # In GB
ddrCapacity = 48    # In GB -> needs to be 8 * 6
hbmPageSize = 4     # In KB 
hbmNumPages = hbmCapacity * 1024 * 1024 / hbmPageSize 
ddrPageSize = 4     # In KB
ddrNumPages = ddrCapacity * 1024 * 1024 / ddrPageSize

next_tile_id       = 0
next_core_id       = 0
next_network_id    = 0
next_hbm_id        = 0

mesh_stops_x       = 6
mesh_stops_y       = 6

mesh_clock         = 1700
mesh_flit          = 36
mesh_link_latency  = "100ps"    # Note, used to be 50ps, didn't seem to make a difference when bumping it up to 100
mesh_link_bw       = str( (mesh_clock * 1000 * 1000 * mesh_flit) ) + "B/s"

core_clock         = "1400MHz"
coherence_protocol = "MESI"

mesh_shape         = str(mesh_stops_x + 2) + "x" + str(mesh_stops_y + 2)
topo               = topoMesh()

sst.merlin._params["link_lat"]         = mesh_link_latency
sst.merlin._params["link_bw"]          = mesh_link_bw
sst.merlin._params["xbar_bw"]          = mesh_link_bw
sst.merlin._params["flit_size"]        = str(mesh_flit) + "B"
sst.merlin._params["input_latency"]    = mesh_link_latency
sst.merlin._params["output_latency"]   = mesh_link_latency
sst.merlin._params["input_buf_size"]   = "2KB"
sst.merlin._params["output_buf_size"]  = "2KB"
sst.merlin._params["num_dims"]         = 2
sst.merlin._params["mesh:local_ports"] = 1
sst.merlin._params["mesh:width"]       = "1x1"
sst.merlin._params["mesh:shape"]       = mesh_shape

topo.prepParams()

# Debug parameters for memH
debugAll = 0
debugL1 = max(debugAll, 0)
debugL2 = max(debugAll, 0)
debugHBMDC = max(debugAll, 0)
debugDDRDC = max(debugAll, 0)
debugLev = 10


l1_cache_params = {
    "cache_frequency"    : core_clock,
    "coherence_protocol" : coherence_protocol,
    "replacement_policy" : "lru",
    "cache_size"         : "32KB",
    "associativity"      : 8,
    "cache_line_size"    : 64,
    "access_latency_cycles" : 4,
    "mshr_num_entries"   : 12, # Outstanding misses per core
    "maxRequestDelay"    : 10000000,
    #"max_requests_per_cycle" : 2,
    #"request_link_width" : "72B",
    "response_link_width" : "36B",
    "L1"                 : 1,
    "debug"              : debugL1,
    "debug_level"        : debugLev
}

l2_cache_params = {
    "cache_frequency"    : core_clock,
    "coherence_protocol" : coherence_protocol,
    "replacement_policy" : "lru",
    "cache_size"         : "1MiB",
    "associativity"      : 16,
    "cache_line_size"    : 64,
    "access_latency_cycles" : 10,   # Guess, don't know
    "mshr_num_entries"   : 48, # Actually 48 reads and 32 writebacks
    "max_requests_per_cycle" : 2,
    #"request_link_width" : "72B",
    "response_link_width" : "36B",
    "network_bw"         : mesh_link_bw,
    "debug"              : debugL2,
    "debug_level"        : debugLev
}

hbm_simple_mem_params = {
    "clock" : "475 MHz",
    "network_bw" : mesh_link_bw,
    "max_requests_per_cycle" : 1,
    "do_not_back" : 1,
    "backend.access_time" : "50ns"
}

hbm_mem_params = {
    "clock" : "475MHz",
    "network_bw" : mesh_link_bw,
    "max_requests_per_cycle" : 2,   # A little messy, actually 1 per link...
    "do_not_back" : 1,
    "backend" : "memHierarchy.goblinHMCSim",
    "backend.device_count" : 1,
    "backend.link_count" : 4,
    "backend.vault_count" : 32, # HMCSim 3 only supports the latest spec with min 32 vaults
    "backend.queue_depth" : 64,
    "backend.bank_count" : 16,
    "backend.dram_count" : 20,
    "backend.capacity_per_device" : 4, # Ensure hbmCapacity / numHBMs is less than this
    "backend.xbar_depth" : 128,
    "backend.max_req_size" : 64,
    "backend.tag_count" : 512,
    "backend.dram_latency" : 14,    # ~30ns in 475MHz cycles
}

hbm_vault_ctrl_params = {
    "clock" : "475MHz",
    "network_bw" : mesh_link_bw,
    "max_requests_per_cycle" : 4,
    "do_not_back" : 1,
    "backend" : "memHierarchy.vaultsim",
    "backend.access_time" : "1ns", # PHY latency?
}

hbm_vault_logic_params = {
    "clock" : "475MHz",
    "llID" : 0,         #
    "bwlimit" : 8,      # Events per cycle per link
    "terminal" : 1,
    "vaults" : 16,      # 16 vaults
    "LL_MASK" : 0,
}

hbm_vault_params = {
    "clock" : "475MHz",
    "numVaults2" : 4, # 16 vaults/cube = 2^4
    "delay" : "30ns",
}
hbm_dc_params = {
    "coherence_protocol": coherence_protocol,
    "network_bw"        : mesh_link_bw,
    "entry_cache_size"  : 256*1024*1024, #Entry cache size of mem/blocksize
    "clock"             : str(mesh_clock) + "MHz",
    "mshr_num_entries"  : 128,
    "debug"             : debugHBMDC,
    "debug_level"       : debugLev
}

ddr_mem_params = {
    "clock" : "300MHz", 
    "network_bw" : mesh_link_bw,
    "max_requests_per_cycle" : 1,
    "do_not_back" : 1,
    "backend" : "memHierarchy.dramsim",
    "backend.system_ini" : "/home/grvosku/sst-near-mem/project-git/sandbox/grvosku/sst-configs/system.ini",
    #"backend.device_ini" : "/home/grvosku/sst-near-mem/project-git/sandbox/grvosku/sst-configs/DDR3_micron_32M_8B_x4_sg125.ini",
    "backend.device_ini" : "/home/grvosku/sst-near-mem/project-git/sandbox/grvosku/sst-configs/DDR4_gwen.ini",
}

# DDR4-2400
ddr_mem_timing_params = {
    "do_not_back" : 1,
    "backend" : "memHierarchy.timingDRAM",
    "backend.clock" : "1200MHz",
    "backend.id" : 0,
    "backend.addrMapper" : "memHierarchy.simpleAddrMapper",
    "backend.channel.transaction_Q_size" : 32,
    "backend.channel.numRanks" : 2,
    "backend.channel.rank.numBanks" : 16,
    "backend.channel.rank.bank.CL" : 15,
    "backend.channel.rank.bank.CL_WR" : 12,
    "backend.channel.rank.bank.RCD" : 15,
    "backend.channel.rank.bank.TRP" : 15,
    "backend.channel.rank.bank.dataCycles" : 4,
    "backend.channel.rank.bank.pagePolicy" : "memHierarchy.simplePagePolicy",
    "backend.channel.rank.bank.transactionQ" : "memHierarchy.reorderTransactionQ",
    "backend.channel.rank.bank.pagePolicy.close" : 0,
}

# DDR4-2400
ddr_mem_simple_params = {
    "clock" : "300MHz",
    "do_not_back" : 1,
    "backend" : "memHierarchy.reorderSimple",
    "backend.max_requests_per_cycle" : -1,
    "backend.search_window_size" : 16,
    "backend.backend" : "memHierarchy.simpleDRAM",
    "backend.backend.cycle_time" : "1200MHz",
    "backend.backend.tCAS" : 15,
    "backend.backend.tRCD" : 15,
    "backend.backend.tRP" : 15,
    "backend.backend.banks" : 16,
    "backend.backend.row_size" : "8KiB",
    "backend.backend.row_policy" : "open",
    "backend.backend.max_requests_per_cycle" : 1,
}

ddr_dc_params = {
    "coherence_protocol": coherence_protocol,
    "network_bw"        : mesh_link_bw,
    "clock"             : str(mesh_clock) + "MHz",
    "entry_cache_size"  : 256*1024*1024, #Entry cache size of mem/blocksize
    "mshr_num_entries"  : 128,
    "debug"             : debugDDRDC,
    "debug_level"       : debugLev
}

ariel = sst.Component("A0", "ariel.ariel")
ariel.addParams({
        "verbose"             : "0",
        "maxcorequeue"        : "256",
        "maxtranscore"        : "12",
        "maxissuepercycle"    : "2",    # Not sure if this should be 2 or 3. 2 can be issued to MEM RS per cycle but can also drain 2 reads + 1 store per cycle?
        "pipetimeout"         : "0",
        "arielinterceptcalls" : "0",
        "memmgr"              : "ariel.MemoryManagerMalloc",
        "memmgr.memorylevels" : "2",
    	"memmgr.pagecount0"   : ddrNumPages,
        "memmgr.pagesize0"    : ddrPageSize * 1024,
    	"memmgr.pagecount1"   : hbmNumPages,
        "memmgr.pagesize1"    : hbmPageSize * 1024,
        "memmgr.defaultlevel" : 0,
        "corecount"           : corecount,
        "clock"               : str(core_clock)
})

# Get app args from the config file
cp = ConfigParser.ConfigParser()
if not cp.read(cfgFile):
    raise Exception('Could not read file "%s"'%cfgFile)

print "Getting appArgs from %s"%cfgFile

appArgs = dict(cp.items('app'))
ariel.addParams(appArgs);

class DDRBuilder:
    def __init__(self, memCapacity, startAddr, ddrCount):
        self.next_ddr_id = 0
        self.ddr_count = ddrCount
        self.mem_capacity = memCapacity
        self.start_addr = startAddr
    
    def build(self, nodeID, extraKeys):
        print "Creating DDR controller " + str(self.next_ddr_id) + " out of " + str(self.ddr_count) + " on node " + str(nodeID) + "..."
        print " - Capacity: " + str(self.mem_capacity / self.ddr_count) + " per DDR."
        
        mem = sst.Component("ddr_" + str(self.next_ddr_id), "memHierarchy.MemController")
        if backend == "simple":
            mem.addParams(ddr_mem_simple_params)
        elif backend == "dramsim":
            mem.addParams(ddr_mem_params)
        else :
            mem.addParams(ddr_mem_timing_params)
        mem.addParams({
                "backend.mem_size" : str(self.mem_capacity / self.ddr_count) + "B",
            })
        
        memLink = sst.Link("ddr_link_" + str(self.next_ddr_id))
        
        dc = sst.Component("ddr_dc_" + str(self.next_ddr_id), "memHierarchy.DirectoryController")
        dc.addParams(ddr_dc_params)
        dc.addParams({
                "network_address" : nodeID,
                "addr_range_start" : self.start_addr + (64 * self.next_ddr_id),
                "addr_range_end" : self.start_addr + (self.mem_capacity - (64 * self.next_ddr_id)),
                "interleave_step" : str(self.ddr_count * 64) + "B",
                "interleave_size" : "64B",
            })
            
        self.next_ddr_id = self.next_ddr_id + 1
        
        memLink.connect( (mem, "direct_link", mesh_link_latency),
            (dc, "memory", mesh_link_latency))

        return (dc, "network", mesh_link_latency)
            
class HBMBuilder:
    def __init__(self, memCapacity, startAddr, hbmCount):
        self.next_hbm_id = 0    
        self.hbm_count = hbmCount
        self.memCapacity = memCapacity
        self.start_addr = startAddr

    def build(self, nodeID, extraKeys):
        print "Creating HBM controller " + str(self.next_hbm_id) + " out of " + str(self.hbm_count) + " on node " + str(nodeID) + "..."
        print " - Capacity: " + str(self.memCapacity / self.hbm_count) + " per HBM."
        
        mem = sst.Component("hbm_" + str(self.next_hbm_id), "memHierarchy.MemController")
        if hbmBackend == "goblin":
            mem.addParams(hbm_mem_params)
        else:
            mem.addParams(hbm_vault_ctrl_params)
            # Create vaultsim instance
            logic = sst.Component("hbm_logic_" + str(self.next_hbm_id), "VaultSimC.logicLayer")
            logic.addParams(hbm_vault_logic_params)
            logicLink = sst.Link("hbm_logic_link_" + str(self.next_hbm_id))
            logicLink.connect( (mem, "cube_link", "1ns"), (logic, "toCPU", "1ns"))
            for x in range(0, 16):
                vault = sst.Component("hbm_vault_" + str(self.next_hbm_id) + "_" + str(x), "VaultSimC.VaultSimC")
                vault.addParams(hbm_vault_params)
                vault.addParams({ "VaultID" : str(x) })
                vaultLink = sst.Link("hbm_vault_link_" + str(self.next_hbm_id) + "_" + str(x))
                vaultLink.connect( (logic, "bus_" + str(x), "1ns"), (vault, "bus", "1ns") )
             

        mem.addParams({
            "backend.mem_size" : str(self.memCapacity / self.hbm_count) + "B",
        })

        memLink = sst.Link("hbm_link_" + str(self.next_hbm_id))

        dc = sst.Component("hbm_dc_" + str(self.next_hbm_id), "memHierarchy.DirectoryController")
        dc.addParams(hbm_dc_params)
        dc.addParams({
            "network_address" : nodeID,
            "addr_range_start" : self.start_addr + (64 * self.next_hbm_id),
            "addr_range_end" : self.start_addr + (self.memCapacity - (self.next_hbm_id * 64)),
            "interleave_step" : str(self.hbm_count * 64) + "B",
            "interleave_size" : "64B",
        })

        self.next_hbm_id = self.next_hbm_id + 1

        memLink.connect( (mem, "direct_link", mesh_link_latency),
            (dc, "memory", mesh_link_latency))

        return (dc, "network", mesh_link_latency)

class TileBuilder:
    def __init__(self):
        self.next_tile_id = 0
        self.next_core_id = 0

    def build(self, nodeID, extraKeys):
        tileL2cache = sst.Component("l2cache_" + str(self.next_tile_id), "memHierarchy.Cache")
        tileL2cache.addParams(l2_cache_params)
        tileL2cache.addParams({
            "network_address" : nodeID
            })

        l2bus = sst.Component("l2cachebus_" + str(self.next_tile_id), "memHierarchy.Bus")
        l2bus.addParams({
            "bus_frequency" : core_clock,
            "low_network_ports" : 1,
            "high_network_ports" : 2
            })

        l2busLink = sst.Link("l2bus_link_" + str(self.next_tile_id))
        l2busLink.connect( (l2bus, "low_network_0", mesh_link_latency),
            (tileL2cache, "high_network_0", mesh_link_latency))
        l2busLink.setNoCut()

        self.next_tile_id = self.next_tile_id + 1

        tileLeftL1 = sst.Component("l1cache_" + str(self.next_core_id), "memHierarchy.Cache")
        tileLeftL1.addParams(l1_cache_params)

        print "Creating core " + str(self.next_core_id) + " on tile: " + str(self.next_tile_id) + "..."

        leftL1CPUlink = sst.Link("l1cache_cpu_" + str(self.next_core_id))
        leftL1CPUlink.connect( (ariel, "cache_link_" + str(self.next_core_id), mesh_link_latency),
                        (tileLeftL1, "high_network_0", mesh_link_latency) )
        leftL1CPUlink.setNoCut()

        leftL1L2link = sst.Link("l1cache_link_" + str(self.next_core_id))
        leftL1L2link.connect( (l2bus, "high_network_0", mesh_link_latency),
            (tileLeftL1, "low_network_0", mesh_link_latency))
        leftL1L2link.setNoCut()

        self.next_core_id = self.next_core_id + 1

        tileRightL1 = sst.Component("l1cache_" + str(self.next_core_id), "memHierarchy.Cache")
        tileRightL1.addParams(l1_cache_params)

        print "Creating core " + str(self.next_core_id) + " on tile: " + str(self.next_tile_id) + "..."

        rightL1CPUlink = sst.Link("l1cache_cpu_" + str(self.next_core_id))
        rightL1CPUlink.connect( (ariel, "cache_link_" + str(self.next_core_id), mesh_link_latency),
                        (tileRightL1, "high_network_0", mesh_link_latency) )
        rightL1CPUlink.setNoCut()

        rightL1L2link = sst.Link("l1cache_link_" + str(self.next_core_id))
        rightL1L2link.connect( (l2bus, "high_network_1", mesh_link_latency),
                        (tileRightL1, "low_network_0", mesh_link_latency))
        rightL1L2link.setNoCut()

        self.next_core_id = self.next_core_id + 1

        return (tileL2cache, "directory", mesh_link_latency)

class NullBuilder:
    def build(self, nodeID, extraKeys):
        pass

tileBuilder = TileBuilder()
hbmBuilder  = HBMBuilder(hbmCapacity * 1024 * 1024 * 1024, ddrCapacity * 1024 * 1024 * 1024, 8)
ddrBuilder  = DDRBuilder(ddrCapacity * 1024 * 1024 * 1024, 0, 6)

def setNode(nodeId):
    if(nodeId == 16 or nodeId == 24 or nodeId == 32):
        print "Creating DDR on left " + str(nodeId)
        return ddrBuilder
    elif(nodeId == 23 or nodeId == 31 or nodeId == 39):
        print "Creating DDR on right " + str(nodeId)
        return ddrBuilder
    elif((nodeId % (mesh_stops_x + 2) == 0) or ((nodeId+1) % (mesh_stops_x + 2) == 0)):
        # This is on the edge of the mesh
        print "Skipping node on edge of mesh: " + str(nodeId)
        return NullBuilder()
    elif(nodeId == 1 or nodeId == 2 or nodeId == 5 or nodeId == 6):
        print "Creating first HBM row " + str(nodeId)
        return hbmBuilder
    elif(nodeId == 57 or nodeId == 58 or nodeId == 61 or nodeId == 62):
        print "Creating second HBM row " + str(nodeId)
        return hbmBuilder
    elif(nodeId == 3 or nodeId == 4 or nodeId == 59 or nodeId == 60):
        print "Skipping nodeId: " + str(nodeId) + ", not populated by HBM."
        return NullBuilder() 
    else:
        print "Creating node: " + str(nodeId)
        return tileBuilder

print "Building KNL model..."

# Build the KNL mesh using Merlin
topo.setEndPointFunc( setNode )
topo.build()

# Enable SST Statistics Outputs for this simulation
sst.setStatisticLoadLevel(16)
sst.enableAllStatisticsForAllComponents({"type":"sst.AccumulatorStatistic"})

sst.setStatisticOutput("sst.statOutputCSV")
sst.setStatisticOutputOptions( {
        "filepath"  : statFile,
        "separator" : ", "
} )

