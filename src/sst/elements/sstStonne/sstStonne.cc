//sstStonne.cc
#include "sstStonne.h"
#include <math.h>
#include <iostream>
#include "include/utility.h"
using namespace SST;
using namespace SST::SST_STONNE;

//Constructor
sstStonne::sstStonne(SST::ComponentId_t id, SST::Params& params) : Component(id) {

  //Load general parameters
  const std::string clock_rate = params.find< std::string >("clock", "1.0GHz");	
  std::cout << "Clock is configured for: " << clock_rate << std::endl;

  //Load operation parameters
  std::string kernelOperationString = params.find<std::string>("kernelOperation", "CONV");
  kernelOperation=get_type_layer_t(kernelOperationString);
  //Convolution parameters
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
  
  //Gemm Parameters
  GEMM_K=params.find<unsigned>("GEMM_K", 3);
  GEMM_M=params.find<unsigned>("GEMM_M", 3);
  GEMM_N=params.find<unsigned>("GEMM_N", 3);
  GEMM_T_K=params.find<unsigned>("GEMM_T_K", 3);
  GEMM_T_M=params.find<unsigned>("GEMM_T_M", 3);
  GEMM_T_N=params.find<unsigned>("GEMM_T_N", 3);


  //Load hardware parameters via configuration file
  std::string hwFileName = params.find< std::string >("hardware_configuration", "hardware.cfg");
  std::cout << "Reading stonne hardware configuration file " << hwFileName << std::endl; 
  stonne_cfg.loadFile(hwFileName); 

  //Load memory file 
  memMatrixAFileName = params.find< std::string >("mem_matrix_a_init", "");
  memMatrixBFileName = params.find<std::string>("mem_matrix_b_init", "");
  memMatrixCFileName = params.find<std::string>("mem_matrix_c_init", "");

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
  switch(kernelOperation) {
      case CONV:
        matrixA_size=N*X*Y*C; //ifmap
        matrixB_size=R*S*(C/G)*K;
        matrixC_size=N*X_*Y_*K;
	break;
      case GEMM:
	matrixA_size=GEMM_M*GEMM_K;
	matrixB_size=GEMM_N*GEMM_K;
	matrixC_size=GEMM_M*GEMM_N;
	break;
      default:
	output_->fatal(CALL_INFO, -1, "Error: Operation unknown\n");

  };
  matrixA = new float[matrixA_size];
  matrixB = new float[matrixB_size];
  matrixC = new float[matrixC_size];

  if(memMatrixAFileName=="") {
    for(int i=0; i<matrixA_size; i++) {
          matrixA[i]=rand()%MAX_RANDOM;
    }

  }

  else {
    constructMemory(memMatrixAFileName, matrixA, matrixA_size);
  }

  if(memMatrixBFileName=="") {

    for(int i=0;i<matrixB_size; i++) {
      matrixB[i]=rand()%MAX_RANDOM;
    }
  }

  else {
    constructMemory(memMatrixBFileName, matrixB, matrixB_size);
  }

  //Updating hardware parameters
  stonne_instance = new Stonne(stonne_cfg);

  switch(kernelOperation) {
      case CONV:
          stonne_instance->loadDNNLayer(CONV, layer_name, R, S, C, K, G, N, X, Y, strides, matrixA, matrixB, matrixC, CNN_DATAFLOW); //Loading the layer
          stonne_instance->loadTile(T_R, T_S, T_C, T_K, T_G, T_N, T_X_, T_Y_);
	  break;
      case GEMM:
          stonne_instance->loadDenseGEMM(layer_name, GEMM_N, GEMM_K, GEMM_M, matrixA, matrixB, matrixC, CNN_DATAFLOW);
          stonne_instance->loadGEMMTile(GEMM_T_N, GEMM_T_K, GEMM_T_M);
	  break;
      default:
	  output_->fatal(CALL_INFO, -1, "Error: Operation unknown\n");


  };
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

void sstStonne::finish() {
    //This code should have the logic to write the output memory into a certain file passed by parameter. TODO
    std::cout << "The execution of STONNE has finished" << std::endl;
    dumpMemoryToFile(memMatrixCFileName, matrixC, matrixC_size);
    delete stonne_instance;
    delete[] matrixA;
    delete[] matrixB;
    delete[] matrixC; 
}

void sstStonne::dumpMemoryToFile(std::string fileName, float* array, unsigned int size) {
  if(fileName != "") {
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


}

