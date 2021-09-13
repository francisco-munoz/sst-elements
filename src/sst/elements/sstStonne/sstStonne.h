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
       // { "fp_mul_latency", "Number of clock ticks for FP MUL operations", "4" },
       // { "fp_div_latency", "Number of clock ticks for FP DIV operations", "4" }
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
    void Setup();
    void Finish();
    //void Status();
    bool tic(Cycle_t);

private:
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

    //Data 
    float* ifmap;
    float* filters;
    float* ofmap;

    //Aux 
    float EPSILON=0.05;
    unsigned int MAX_RANDOM=10; //Variable used to generate the random values



};
}
}

#endif
