#ifndef _SST_STONNE_H
#define _SST_STONNE_H

//sst_stonne.h
#include "include/STONNEModel.h"
#include "include/types.h"
#include <sst/core/component.h>
#include <sst/core/sst_config.h>


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
        {"mem_ifmap_init","ifmap elements to compute separated by commas", "" },
	{"mem_filter_init","filter elements to compute separated by commas", "" },
	{"mem_output_init","file where the output elements will be dumped", "" },
    )

    ///TODO
    SST_ELI_DOCUMENT_STATISTICS(
    )

    SST_ELI_DOCUMENT_PORTS(
       // { "cache_link",     "Link to Memory Controller", { "memHierarchy.memEvent" , "" } }
    )

    SST_ELI_DOCUMENT_SUBCOMPONENT_SLOTS(
        //{ "memory",         "The memory interface to use (e.g., interface to caches)", "Experimental::Interfaces::SST::StandardMem" }
    )
    sstStonne(SST::ComponentId_t id, SST::Params& params);
    sstStonne();
    ~sstStonne();

    // Override SST::Component Virtual Methods
    void setup();
    void Finish();
    //void Status();
    bool tic(Cycle_t);

private:
    //SST Variables
    SST::Output* output_;

    Stonne* stonne_instance;


    //Input parameters

    //Layer parameters (See MAERI paper to find out the taxonomy meaning)
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

    unsigned int ifmap_size;
    unsigned int filter_size;
    unsigned int ofmap_size;

    //Tile parameters (See MAERI paper to find out the taxonomy meaning)
    unsigned int T_R;                                // T_R
    unsigned int T_S;                                // T_S
    unsigned int T_C;                                // T_C
    unsigned int T_K;                                // T_K
    unsigned int T_G;                                // T_G
    unsigned int T_N;                                // T_N
    unsigned int T_X_;                               // T_X
    unsigned int T_Y_;                               // T_Y   


    //Hardware parameters
    Config stonne_cfg; 
    std::string memIfmapFileName;
    std::string memFilterFileName;
    std::string memOutputFileName;

    //Data 
    float* ifmap;
    float* filters;
    float* ofmap;

    //Aux 
    float EPSILON=0.05;
    unsigned int MAX_RANDOM=10; //Variable used to generate the random values

    //Private functions
    void constructMemory(std::string fileName, float* array, unsigned int size);
    void dumpMemoryToFile(std::string fileName, float* array, unsigned int size);



};
}
}

#endif
