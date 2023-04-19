#ifndef _SST_STONNE_H
#define _SST_STONNE_H

//sst_stonne.h
#include "include/STONNEModel.h"
#include "include/types.h"
#include <sst/core/component.h>
#include <sst/core/sst_config.h>
#include <sst/core/interfaces/simpleMem.h>

#include "lsQueue.h"


namespace SST {
namespace SST_STONNE {
class sstStonne : public SST::Component {
public:
     // REGISTER THIS COMPONENT INTO THE ELEMENT LIBRARY
    SST_ELI_REGISTER_COMPONENT(
        sstStonne,
        "sstStonne",
        "MAERI",
        SST_ELI_ELEMENT_VERSION(1,0,0),
        "Configurable Dataflow Component",
        COMPONENT_CATEGORY_PROCESSOR
    )

    SST_ELI_DOCUMENT_PARAMS(
        {"clock","Clock frequency", "1GHz" },
	{"hardware_configuration","stonne hardware configuration file", "hardware.cfg"},
	{ "kernelOperation", "Type of the kernel to execute (CONV,GEMM,bitmapSpMSpM,csrSpMM,innerProductGEMM,outerProductGEMM,gustavsonsGEMM)", "CONV"},
        { "R", "Number of filter rows in a CONV operation", "3"},
	{ "S", "Number of filter cols in a CONV operation", "3"},
	{ "C", "Number of input channels in a CONV operation", "1"},
	{ "K", "Number of output channels in a CONV operation", "1"},
	{ "G", "Number of groups in a CONV operation", "1"},
	{ "N", "Batch size in a CONV operation", "1"},
	{ "X", "Number of ifmap rows in a CONV operation", "9"},
	{ "Y", "Number of ifmap cols in a CONV operation", "9"},
	{ "strides", "Stride to use in a CONV operation", "1"},
	{ "kernelName", "Name of the kernel to execute", "DefaultKernelName"},
	{ "T_R", "Number of mapped filter rows in a CONV operation", "3"},
	{ "T_S", "Number of mapped filter cols in a CONV operation", "3"},
	{ "T_C", "Number of mapped channels in a CONV operation", "1"},
	{ "T_K", "Number of mapped output channels in a CONV operation", "1"},
	{ "T_G", "Number of mapped filter groups in a CONV operation", "1"},
	{ "T_N", "Number of mapped batches in a CONV operation", "1"},
	{ "T_X_", "Number of mapped output rows in a CONV operation", "1"},
	{ "T_Y_", "Number of mapped output cols in a CONV operation", "1"},
	{ "GEMM_K", "Number of cols and rows in matrices MK and KN, respectively in a GEMM operation", "3"},
	{ "GEMM_M", "Number of rows in matrix in a GEMM operation", "3"},
	{ "GEMM_N", "Number of cols and rows in matrix KN in a GEMM operation", "3"},
	{ "GEMM_T_K", "Mapped K in a GEMM operation", "3"},
	{ "GEMM_T_M", "Mapped M in a GEMM operation", "3"},
	{ "GEMM_T_N", "Mapped N in a GEMM operation", "3"},
        {"mem_init","DRAM initialization values", "" },
	{"matrix_a_dram_address", "DRAM Address where the matrix a starts", "0"},
	{"matrix_b_dram_address", "DRAM Address where the matrix b starts", "10000"},
	{"matrix_c_dram_address", "DRAM Address where the matrix c will be stored", "20000"},
	{"mem_matrix_c_file_name", "File where the matrix C will be written", ""},
	{"bitmap_matrix_a_init", "MK bitmap used for bitmapSpMSpM operation", ""},
	{"bitmap_matrix_b_init", "MK bitmap used for bitmapSpMSpM operation", ""},
	{"rowpointer_matrix_a_init","MK row pointer for csrSpMM operation",""},
	{"colpointer_matrix_a_init","MK col pointer for csrSpMM operation",""},
	{"rowpointer_matrix_b_init","KN row pointer for csrSpMM operation",""},
        {"colpointer_matrix_b_init","KN col pointer for csrSpMM operation",""},

    )

    ///TODO
    SST_ELI_DOCUMENT_STATISTICS(
    )

    SST_ELI_DOCUMENT_PORTS(
        { "cache_link",     "Link to Memory Controller", { "memHierarchy.memEvent" , "" } }
    )

    SST_ELI_DOCUMENT_SUBCOMPONENT_SLOTS(
        { "memory",         "The memory interface to use (e.g., interface to caches)", "Experimental::Interfaces::SST::StandardMem" }
    )
    sstStonne(SST::ComponentId_t id, SST::Params& params);
    sstStonne();
    ~sstStonne();

    // Override SST::Component Virtual Methods
    void setup();
    void finish();
    void init( uint32_t phase );
    void handleEvent( SimpleMem::Request* ev );
    //void Status();
    bool tic(Cycle_t);

private:
    //SST Variables
    SST::Output* output_;
    SST::TimeConverter*     time_converter_;
    SimpleMem*  mem_interface_;
    SST::Link**  links_;
    SST::Link*   clockLink_;

    Stonne* stonne_instance;
    Layer_t kernelOperation;

    //Input parameters
    /***************************************************************************/
    /*Convolution parameters (See MAERI paper to find out the taxonomy meaning)*/
    /***************************************************************************/
    std::string layer_name;
    unsigned int R;                                  // R
    unsigned int S;                                  // S
    unsigned int C;                                  // C
    unsigned int K;                                  // K
    unsigned int G;                                  // G
    unsigned int N;                                  // N
    unsigned int X;                                  // X //TODO CHECK X=1 and Y=1
    unsigned int Y;                                  // Y
    unsigned int X_;                                 // X_
    unsigned int Y_;                                 // Y_
    unsigned int strides;                            // Strides

    unsigned int matrixA_size;
    unsigned int matrixB_size;
    unsigned int matrixC_size;

    //Convolution Tile parameters (See MAERI paper to find out the taxonomy meaning)
    unsigned int T_R;                                // T_R
    unsigned int T_S;                                // T_S
    unsigned int T_C;                                // T_C
    unsigned int T_K;                                // T_K
    unsigned int T_G;                                // T_G
    unsigned int T_N;                                // T_N
    unsigned int T_X_;                               // T_X
    unsigned int T_Y_;                               // T_Y   


    /******************************************************************************/
    /*  GEMM Parameters */
    /******************************************************************************/
    //Layer parameters
    unsigned int GEMM_K;
    unsigned int GEMM_N;
    unsigned int GEMM_M;

    //Tile parameters
    unsigned int GEMM_T_K;
    unsigned int GEMM_T_N;
    unsigned int GEMM_T_M;

    /**************************************************************************/
    /* Hardware parameters */
    /**************************************************************************/
    Config stonne_cfg; 
    std::string memFileName;
    uint64_t dram_matrixA_address;
    uint64_t dram_matrixB_address;
    uint64_t dram_matrixC_address;

    std::string memMatrixCFileName;

    std::string bitmapMatrixAFileName;
    std::string bitmapMatrixBFileName;

    std::string rowpointerMatrixAFileName;
    std::string colpointerMatrixAFileName;
   
    std::string rowpointerMatrixBFileName;
    std::string colpointerMatrixBFileName;


    /**************************************************************************/
    /* Data pointers */
    /**************************************************************************/

    float* matrixA;  //This is input ifmaps in CONV operation or MK matrix in GEMM operation
    float* matrixB;  //This is filter matrix in CONV operation or KN matrix in GEMM operation
    float* matrixC;  //This is output fmap in CONV operation or resulting MN matrix in GEMM operation

    //These three structures are used to represent the bitmaps in a bitmapSpMSpM operation. The datatype
    //could be smaller to unsigned int (one single bit per element is necessary) but will use this for simplicity 
    //In terms of functionallity the simulation is not affected
    unsigned int* bitmapMatrixA; //This is the bitmap of MK matrix in bitmapSpMSpM operation
    unsigned int* bitmapMatrixB; //This is the bitmap of KN matrix in bitmapSpMSpM operation
    unsigned int* bitmapMatrixC; //This is the bitmap for the resulting matrix in bitmapSpMSpM operation

    unsigned int* rowpointerMatrixA; //This is the row pointer of MK matrix in csrSpMM operation
    unsigned int* colpointerMatrixA; //This is the col pointer of MK matrix in csrSpMM operation

    unsigned int* rowpointerMatrixB;  //This is the pointer of KN matrix in outerProduct operation
    unsigned int* colpointerMatrixB;  //This is the id pointer of KN matrix in outerProduct operation
    
    /**************************************************************************/
    /* Auxiliary variables */
    /**************************************************************************/
    float EPSILON=0.05;
    unsigned int MAX_RANDOM=10; //Variable used to generate the random values

    /**************************************************************************/
    /* Memory Hierarchy structures and variables */
    /**************************************************************************/

    LSQueue* load_queue_;    //This FIFO stores the load requests sent to the memory hirarchy component
    LSQueue* write_queue_;   //This FIFO stores the write requests sent to the memory hierarchy component

    /**************************************************************************/
    /* Private functions */
    /**************************************************************************/
    std::vector< uint32_t >* constructMemory(std::string fileName);
    unsigned int constructBitmap(std::string fileName, unsigned int * array, unsigned int size);
    void dumpMemoryToFile(std::string fileName, float* array, unsigned int size);
    unsigned int constructCSRStructure(std::string fileName, unsigned int * array);
    void transformCSRtoBitmap(unsigned int * rowpointer, unsigned int * colpointer, unsigned int * bitmap, unsigned int nRows, unsigned int nCols);
    void transformCSCtoBitmap(unsigned int * rowpointer, unsigned int * colpointer, unsigned int * bitmap, unsigned int nRows, unsigned int nCols);


};
}
}

#endif
