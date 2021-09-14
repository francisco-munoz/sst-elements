//sstStonne.cc
#include "sstStonne.h"
#include <math.h>
#include <iostream>

using namespace SST;
using namespace SST::SST_STONNE;

//Constructor
sstStonne::sstStonne(SST::ComponentId_t id, SST::Params& params) : Component(id) {

  //Load general parameters
  const std::string clock_rate = params.find< std::string >("clock", "1.0GHz");	
  std::cout << "Clock is configured for: " << clock_rate << std::endl;

  //Load operation parameters
  
  R=params.find<unsigned>("R", 3);
  S=params.find<unsigned>("S", 3);
  C=params.find<unsigned>("C", 1);
  K=params.find<unsigned>("K", 1);
  G=params.find<unsigned>("G", 1);
  N=params.find<unsigned>("N", 1);
  X=params.find<unsigned>("X", 9);
  Y=params.find<unsigned>("Y", 9);
  strides=params.find<unsigned>("strides", 1);
  layer_name=params.find<std::string>("kernelName", "DefaultKernelName");
  T_R=params.find<unsigned>("T_R", 3);
  T_S=params.find<unsigned>("T_S", 3);
  T_C=params.find<unsigned>("T_C", 1);
  T_K=params.find<unsigned>("T_K", 1);
  T_G=params.find<unsigned>("T_G", 1);
  T_N=params.find<unsigned>("T_N", 1);
  T_X_=params.find<unsigned>("T_X_", 1);;
  T_Y_=params.find<unsigned>("T_Y_", 1);
  X_= (X - R + strides) / strides;      // X_
  Y_=(Y - S + strides) / strides;       // Y_

  //Load hardware parameters via configuration file
  std::string hwFileName = params.find< std::string >("hardware_configuration", "hardware.cfg");
  std::cout << "Reading stonne hardware configuration file " << hwFileName << std::endl; 
  stonne_cfg.loadFile(hwFileName); 

  //Load memory file 
  memIfmapFileName = params.find< std::string >("mem_ifmap_init", "");
  memFilterFileName = params.find<std::string>("mem_filter_init", "");
  memOutputFileName = params.find<std::string>("mem_output_init", "");

  registerAsPrimaryComponent();
   primaryComponentDoNotEndSim();
  registerClock(clock_rate, new Clock::Handler<sstStonne>(this,&sstStonne::tic));

}

sstStonne::~sstStonne() {
        
}

bool sstStonne::tic(Cycle_t) {
    stonne_instance->cycle();
    bool work_done = stonne_instance->isExecutionFinished();
    if(work_done) {
        primaryComponentOKToEndSim();
    }
    return work_done;
}

void sstStonne::setup() {
  //Creating arrays for this version of the integration
  ifmap_size=N*X*Y*C;
  filter_size=R*S*(C/G)*K;
  ofmap_size=N*X_*Y_*K;
  ifmap = new float[ifmap_size];
  filters = new float[filter_size];
  ofmap = new float[ofmap_size];

  if(memIfmapFileName=="") {
    //Filling the arrays with random values. Later this will come in files
    for(int i=0; i<ifmap_size; i++) {
          ifmap[i]=rand()%MAX_RANDOM;
    }

  }

  else {
    constructMemory(memIfmapFileName, ifmap, ifmap_size);
  }

  if(memFilterFileName=="") {

    for(int i=0;i<filter_size; i++) {
      filters[i]=rand()%MAX_RANDOM;
    }
  }

  else {
    constructMemory(memFilterFileName, filters, filter_size);
  }

  //Updating hardware parameters
  stonne_instance = new Stonne(stonne_cfg);
  stonne_instance->loadDNNLayer(CONV, layer_name, R, S, C, K, G, N, X, Y, strides, ifmap, filters, ofmap, CNN_DATAFLOW); //Loading the layer
  stonne_instance->loadTile(T_R, T_S, T_C, T_K, T_G, T_N, T_X_, T_Y_);
}

void sstStonne::constructMemory(std::string fileName, float* array, unsigned int size) { //In the future version this will be directly simulated memory
  std::ifstream inputStream(fileName, std::ios::in);
  unsigned int currentIndex=0;
  if( inputStream.is_open() ) {

        std::string thisLine;
        while( std::getline( inputStream, thisLine ) )
        {
            std::string value;
            std::stringstream stringIn(thisLine);
            while( std::getline(stringIn, value, ',') ) {
	        array[currentIndex]=stof(value);
		currentIndex++;
            }
        }

//         std::cout << "Init Vector(" << tempVector->size() << "):  ";
//         for( auto it = tempVector->begin(); it != tempVector->end(); ++it ) {
//             std::cout << *it;
//             std::cout << " ";
//         }
//         std::cout << std::endl;

        inputStream.close();
    } else {
        output_->fatal(CALL_INFO, -1, "Error: Unable to open file\n");
        exit(0);
    }
 

}

void sstStonne::Finish() {
    //This code should have the logic to write the output memory into a certain file passed by parameter. TODO
    std::cout << "The execution of STONNE has finished" << std::endl;
    dumpMemoryToFile(memOutputFileName, ofmap, ofmap_size);
    delete stonne_instance;
    delete[] ifmap;
    delete[] filters;
    delete[] ofmap; 
}

void sstStonne::dumpMemoryToFile(std::string fileName, float* array, unsigned int size) {
  std::ofstream outputStream (fileName, std::ios::out);
  if( outputStream.is_open()) {
    for(unsigned i=0; i<size; i++) {
       float value = array[i];
       outputStream << value << ","; 
    }

    outputStream.close();
  }

  else {
    output_->fatal(CALL_INFO, -1, "Error: Unable to open file\n");
    exit(0);
  }
}

