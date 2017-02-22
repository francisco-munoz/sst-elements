// Copyright 2009-2016 Sandia Corporation. Under the terms
// of Contract DE-AC04-94AL85000 with Sandia Corporation, the U.S.
// Government retains certain rights in this software.
//
// Copyright (c) 2009-2017, Sandia Corporation
// All rights reserved.
//
// Portions are copyright of other developers:
// See the file CONTRIBUTORS.TXT in the top level directory
// the distribution for more information.
//
// This file is part of the SST software package. For license
// information, see the LICENSE file in the top level directory of the
// distribution.


#ifndef _HTM_H_
#define _HTM_H_

#include <map>
#include <queue>

#include <sst/core/event.h>
#include <sst/core/sst_types.h>
#include <sst/core/component.h>
#include <sst/core/link.h>
#include <sst/core/timeConverter.h>
#include <sst/core/output.h>

#include "cacheArray.h"
#include "memEvent.h"
#include "util.h"



namespace SST { namespace MemHierarchy {

using namespace std;

class HTM : public SST::Component {
public:

    typedef CacheArray::CacheLine CacheLine;
//     typedef unsigned int uint;
//     typedef uint64_t uint64;
//
//     virtual void init(unsigned int);
//     virtual void finish(void);
//

    /** Constructor for HTM Component */
    HTM(ComponentId_t id, Params &params);

    /** Destructor for HTM Component */
    ~HTM()
    {
//         delete cacheArray_;
    }


    /** Computes the 'Base Address' of the requests.  The base address points to the first address of the cache line */
    Addr getBaseAddr(Addr addr)
    {
        Addr baseAddr = (addr) & ~(cacheArray_->getLineSize() - 1);  //Remove the block offset bits
        return baseAddr;
    }

private:


    /** Handler for incoming link events.  */
    void processRequest(SST::Event* event);

    void processResponse(SST::Event* event);

//     /** Handler for incoming allocation events.  */
//     void processAllocEvent(SST::Event* event);
//
//     /** output and clear stats to file  */
//     void outputStats(int marker);
//     bool resetStatsOnOutput;
    Output*             output_;

    uint32_t            core_count_;
    CacheArray*         cacheArray_;

//     vector<SST::Link*>  cpuLinks_;
//     vector<SST::Link*>  cacheLinks_;
    SST::Link*  cpuLink_;
    SST::Link*  cacheLink_;

    /* Statistics */
    Statistic<uint64_t>* statReadSetSize;
    Statistic<uint64_t>* statWriteSetSize;
    Statistic<uint64_t>* statAborts;
    Statistic<uint64_t>* statCommits;

};


}
}

#endif
