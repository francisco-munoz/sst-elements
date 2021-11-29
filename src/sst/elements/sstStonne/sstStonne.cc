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

  bitmapMatrixAFileName = params.find< std::string >("bitmap_matrix_a_init", "");
  bitmapMatrixBFileName = params.find< std::string >("bitmap_matrix_a_init", "");

  rowpointerMatrixAFileName = params.find< std::string >("rowpointer_matrix_a_init", "");
  colpointerMatrixAFileName = params.find< std::string >("colpointer_matrix_a_init", "");


  registerAsPrimaryComponent();
  primaryComponentDoNotEndSim();
  time_converter_ = registerClock(clock_rate, new Clock::Handler<sstStonne>(this,&sstStonne::tic));

  //set up memory interfaces
  mem_interface_ = loadUserSubComponent<SimpleMem>("memory", ComponentInfo::SHARE_NONE, time_converter_,
  new SimpleMem::Handler<sstStonne>(this, &sstStonne::handleEvent));

  if( !mem_interface_ ) {
      std::string interfaceName = params.find<std::string>("memoryinterface", "memHierarchy.memInterface");
      output_->verbose(CALL_INFO, 1, 0, "Memory interface to be loaded is: %s\n", interfaceName.c_str());
      Params interfaceParams = params.find_prefix_params("memoryinterfaceparams.");
      interfaceParams.insert("port", "cache_link");
      mem_interface_ = loadAnonymousSubComponent<SimpleMem>(interfaceName, "memory", 0, ComponentInfo::SHARE_PORTS | ComponentInfo::INSERT_STATS,
      interfaceParams, time_converter_, new SimpleMem::Handler<sstStonne>(this, &sstStonne::handleEvent));

    }
    //Inititating memory parameters
  write_queue_ = new LSQueue();
  read_queue_ = new LSQueue();



}

sstStonne::~sstStonne() {
        
}

void sstSTONNE::init( uint32_t phase )
{
    mem_interface_->init( phase );

    if( 0 == phase ) {
	/*
        std::vector< uint64_t >* initVector;

        //Check to see if there is any memory being initialized
        if( memFileName_ != "" ) {
            initVector = constructMemory(memFileName_);
        } else {
            initVector = new std::vector< uint64_t > {16, 64, 32, 0 , 16382, 0, 0};
        }

        std::vector<uint8_t> memInit;
        constexpr auto buff_size = sizeof(uint64_t);
        uint8_t buffer[buff_size] = {};
        for( auto it = initVector->begin(); it != initVector->end(); ++it ) {
            std::memcpy(buffer, std::addressof(*it), buff_size);
            for( uint32_t i = 0; i < buff_size; ++i ){
                memInit.push_back(buffer[i]);
            }
        }

        output_->verbose(CALL_INFO, 2, 0, ">> Writing memory contents (%" PRIu64 " bytes at index 0)\n",
                        (uint64_t) memInit.size());
//         for( std::vector< uint8_t >::iterator it = memInit.begin() ; it != memInit.end(); ++it ) {
//             std::cout << uint32_t(*it) << ' ';
//         }
//
//         std::cout << "\n";

        SimpleMem::Request* initMemory = new SimpleMem::Request(SimpleMem::Request::Write, 0, memInit.size(), memInit);
        output_->verbose(CALL_INFO, 1, 0, "Sending initialization data to memory...\n");
        mem_interface_->sendInitData(initMemory);
        output_->verbose(CALL_INFO, 1, 0, "Initialization data sent.\n");
	*/
    }
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
  if((kernelOperation==CONV) || (kernelOperation==GEMM)) { //Initializing dense operation
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

  } //End initializing dense operation

  else { //Initializing sparse operation
    if(kernelOperation==bitmapSpMSpM) {
      if(bitmapMatrixAFileName=="") {
        output_->fatal(CALL_INFO, -1, "bitmap_matrix_a_init parameter is not introduced\n");
      }
      if(bitmapMatrixBFileName=="") {
        output_->fatal(CALL_INFO, -1, "bitmap_matrix_b_init parameter is not introduced\n");
      }

      matrixA_size=GEMM_M*GEMM_K;
      matrixB_size=GEMM_N*GEMM_K;
      matrixC_size=GEMM_M*GEMM_N;
      bitmapMatrixA=new unsigned int[matrixA_size]; 
      bitmapMatrixB=new unsigned int[matrixB_size];
      bitmapMatrixC=new unsigned int[matrixC_size];
      unsigned int nActiveValuesA=constructBitmap(bitmapMatrixAFileName, bitmapMatrixA, matrixA_size);
      unsigned int nActiveValuesB=constructBitmap(bitmapMatrixBFileName, bitmapMatrixB, matrixB_size);
      matrixA=new float[matrixA_size]; //TODO fix this
      matrixB=new float[matrixB_size];
      matrixC=new float[matrixC_size];

      // Data is not mandatory
      if(memMatrixAFileName=="") {
        for(int i=0; i<nActiveValuesA; i++) {
          matrixA[i]=rand()%MAX_RANDOM;
        }

      }

      else {
        constructMemory(memMatrixAFileName, matrixA, nActiveValuesA);
      }


      if(memMatrixBFileName=="") {
        for(int i=0; i<matrixB_size; i++) {
            matrixB[i]=rand()%MAX_RANDOM;
        }

      }

      else {
        constructMemory(memMatrixBFileName, matrixB, nActiveValuesB);
      }

    } //End bitmapSpMSpM operation

    else if(kernelOperation==csrSpMM) {
      if(rowpointerMatrixAFileName=="") {
        output_->fatal(CALL_INFO, -1, "rowpointer_matrix_a_init parameter is not introduced\n");
      }
      if(colpointerMatrixAFileName=="") {
        output_->fatal(CALL_INFO, -1, "colpointer_matrix_a_init parameter is not introduced\n");
      }

      matrixA_size=GEMM_M*GEMM_K;
      matrixB_size=GEMM_N*GEMM_K;
      matrixC_size=GEMM_M*GEMM_N;

      rowpointerMatrixA=new unsigned int[matrixA_size]; //Change to the minimum using vector class
      colpointerMatrixA=new unsigned int[matrixA_size];
      unsigned int nValuesRowPointer=constructCSRStructure(rowpointerMatrixAFileName,rowpointerMatrixA);
      unsigned int nValuesColPointer=constructCSRStructure(colpointerMatrixAFileName, colpointerMatrixA);
      matrixA=new float[matrixA_size]; //TODO fix this
      matrixB=new float[matrixB_size];
      matrixC=new float[matrixC_size];
      // Data is not mandatory
      if(memMatrixAFileName=="") {
      for(int i=0; i<nValuesColPointer; i++) {
          matrixA[i]=rand()%MAX_RANDOM;
      }

      }

      else {
        constructMemory(memMatrixAFileName, matrixA, nValuesColPointer);
      }


      if(memMatrixBFileName=="") {
        for(int i=0; i<matrixB_size; i++) {
            matrixB[i]=rand()%MAX_RANDOM;
        }

      }

      else {
        constructMemory(memMatrixBFileName, matrixB, matrixB_size);
      }


    }

    else {
      output_->fatal(CALL_INFO, -1, "Error: Operation unknown\n");
    }
  }

  //Updating hardware parameters
  stonne_instance = new Stonne(stonne_cfg, load_queue_, write_queue_, mem_interface_);

  switch(kernelOperation) {
      case CONV:
          stonne_instance->loadDNNLayer(CONV, layer_name, R, S, C, K, G, N, X, Y, strides, matrixA, matrixB, matrixC, CNN_DATAFLOW); //Loading the layer
          stonne_instance->loadTile(T_R, T_S, T_C, T_K, T_G, T_N, T_X_, T_Y_);
	  break;
      case GEMM:
          stonne_instance->loadDenseGEMM(layer_name, GEMM_N, GEMM_K, GEMM_M, matrixA, matrixB, matrixC, CNN_DATAFLOW);
          stonne_instance->loadGEMMTile(GEMM_T_N, GEMM_T_K, GEMM_T_M);
	  break;
      case bitmapSpMSpM:
          stonne_instance->loadGEMM(layer_name, GEMM_N, GEMM_K, GEMM_M, matrixA, matrixB, bitmapMatrixA, bitmapMatrixB, matrixC, bitmapMatrixC, MK_STR_KN_STA );
	  break;
      case csrSpMM: 
	  stonne_instance->loadSparseDense(layer_name, GEMM_N, GEMM_K, GEMM_M, matrixA, matrixB, colpointerMatrixA, rowpointerMatrixA, matrixC, GEMM_T_N, GEMM_T_K);
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


        inputStream.close();
    } else {
        output_->fatal(CALL_INFO, -1, "Error: Unable to open file\n");
        exit(0);
    }
 
}

//Return number of active elements to build later the data array. This is necessary because we do not know the number of elements.
unsigned int sstStonne::constructBitmap(std::string fileName, unsigned int * array, unsigned int size) { //In the future version this will be directly simulated memory
  std::ifstream inputStream(fileName, std::ios::in);
  unsigned int currentIndex=0;
  unsigned int nActiveValues=0;
  if( inputStream.is_open() ) {

        std::string thisLine;
        while( std::getline( inputStream, thisLine ) )
        {
            std::string value;
            std::stringstream stringIn(thisLine);
            while( std::getline(stringIn, value, ',') ) {
                array[currentIndex]=stoi(value);
                currentIndex++;
		if(stoi(value)==1) {
                    nActiveValues++;
		}
            }
        }


        inputStream.close();
    } else {
        output_->fatal(CALL_INFO, -1, "Error: Unable to open file\n");
        exit(0);
    }

   return nActiveValues;

}

//Return number of active elements to build later the data array. This is necessary because we do not know the number of elements.
unsigned int sstStonne::constructCSRStructure(std::string fileName, unsigned int * array) { //In the future version this will be directly simulated memory
  std::ifstream inputStream(fileName, std::ios::in);
  unsigned int currentIndex=0;
  if( inputStream.is_open() ) {

        std::string thisLine;
        while( std::getline( inputStream, thisLine ) )
        {
            std::string value;
            std::stringstream stringIn(thisLine);
            while( std::getline(stringIn, value, ',') ) {
                array[currentIndex]=stoi(value);
                currentIndex++;
            }
        }


        inputStream.close();
    } else {
        output_->fatal(CALL_INFO, -1, "Error: Unable to open file\n");
        exit(0);
    }

   return currentIndex;

}


void sstStonne::finish() {
    //This code should have the logic to write the output memory into a certain file passed by parameter. TODO
    std::cout << "The execution of STONNE has finished" << std::endl;
    dumpMemoryToFile(memMatrixCFileName, matrixC, matrixC_size);
    delete stonne_instance;
    delete[] matrixA;
    delete[] matrixB;
    delete[] matrixC; 
    if(kernelOperation==bitmapSpMSpM) {
     delete[] bitmapMatrixA;
      delete[] bitmapMatrixB;
    }

    else if(kernelOperation==csrSpMM) {
      delete[] rowpointerMatrixA;
      delete[] colpointerMatrixA;
    }
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

void sstStonne::handleEvent( SimpleMem::Request* ev ) {
    output_->verbose(CALL_INFO, 4, 0, "Recv response from cache\n");

    for( auto &it : ev->data ) {
        std::cout << unsigned(it) << " ";
    }
    std::cout << std::endl;

    if( ev->cmd == SimpleMem::Request::Command::ReadResp ) {
        // Read request needs some special handling
        uint64_t addr = ev->addr;
        data_t memValue = 0.0;

        std::memcpy( std::addressof(memValue), std::addressof(ev->data[0]), sizeof(memValue) );

        //output_->verbose(CALL_INFO, 8, 0, "Response to a read, payload=%" PRIu64 ", for addr: %" PRIu64
          //               " to PE %" PRIu32 "\n", memValue, addr, ls_queue_->lookupEntry( ev->id ).second );

        load_queue_->setEntryData( ev->id, memValue);
        load_queue_->setEntryReady( ev->id, 1 );
    } else {
        //output_->verbose(CALL_INFO, 8, 0, "Response to a write for addr: %" PRIu64 " to PE %" PRIu32 "\n",
        //                 ev->addr, ls_queue_->lookupEntry( ev->id ).second );
        write_queue_->setEntryReady( ev->id, 1 );
    }

    // Need to clean up the events coming back from the cache
    delete ev;
    output_->verbose(CALL_INFO, 4, 0, "Complete cache response handling.\n");
}


