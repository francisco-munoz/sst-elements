//sst_STONNE.cc
#include "sst_STONNE.h"
#include <math.h>
#include <iostream>

using namespace SST;
using namespace SST::SST_STONNE;

//Constructor
sst_STONNE::sst_STONNE(SST::ComponentId_t id, SST::Params& params) : Component(id) {
  //TODO
  //This code will be replaced by reading parameters from SST.
  //    Option 1: get a file and read from there
  //    Option 2: Introduce each parameter manually 
  //    Option 3: Introduce the parameters with the memory file
 
  //Temporal code to introduce the parameters by hand
  R=1;
  S=3;
  C=1;
  K=1;
  G=1;
  N=1;
  X=1;
  Y=3;
  strides=1;
  layer_name="TestLayer";
  T_R=1;
  T_S=3;
  T_C=1;
  T_K=1;
  T_G=1;
  T_N=1;
  X_= (X - R + strides) / strides;      // X_
  Y_=(Y - S + strides) / strides;       // Y_


  unsigned int ifmap_size=N*X*Y*C;
  unsigned int filter_size=R*S*(C/G)*K;
  unsigned int ofmap_size=N*X_*Y_*K;
  ifmap = new float[ifmap_size];
  filters = new float[filter_size];
  ofmap = new float[ofmap_size];

  //Filling the arrays with random values. Later this will come in files
  for(int i=0; i<ifmap_size; i++) {
        ifmap[i]=rand()%MAX_RANDOM;
  }

  for(int i=0;i<filter_size; i++) {
       filters[i]=rand()%MAX_RANDOM;
  }

  //Updating hardware parameters
  stonne_cfg.m_MSNetworkCfg.ms_size=64;
  stonne_cfg.m_SDMemoryCfg.n_read_ports=64;
  stonne_cfg.m_SDMemoryCfg.n_write_ports=64;
  stonne_cfg.m_ASNetworkCfg.accumulation_buffer_enabled=1;
  stonne_cfg.print_stats_enabled=1;
  stonne_cfg.m_ASNetworkCfg.reduce_network_type=ASNETWORK;
  stonne_cfg.m_SDMemoryCfg.mem_controller_type=MAERI_DENSE_WORKLOAD;
  stonne_cfg.m_MSNetworkCfg.multiplier_network_type=LINEAR;

  stonne_instance = new Stonne(stonne_cfg);
  stonne_instance->loadDNNLayer(CONV, layer_name, R, S, C, K, G, N, X, Y, strides, ifmap, filters, ofmap, CNN_DATAFLOW); //Loading the layer

  registerAsPrimaryComponent();
   primaryComponentDoNotEndSim();
  registerClock("1 GHz", new Clock::Handler<sst_STONNE>(this,&sst_STONNE::tic));

}

bool sst_STONNE::tic(Cycle_t) {
    stonne_instance->cycle();
    bool work_done = stonne_instance->isExecutionFinished();
    if(work_done) {
        primaryComponentOKToEndSim();
    }
    return work_done;
}

void sst_STONNE::Setup() {
    //The code to initialize parameters could be here TODO
    std::cout << "Initiating the simulation with SST" << std::endl;

}

void sst_STONNE::Finish() {
    //This code should have the logic to write the output memory into a certain file passed by parameter. TODO
    std::cout << "The execution of STONNE has finished" << std::endl;
    delete stonne_instance;
    delete[] ifmap;
    delete[] filters;
    delete[] ofmap; 
}

