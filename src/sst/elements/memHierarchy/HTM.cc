// Copyright 2009-2016 Sandia Corporation. Under the terms
// of Contract DE-AC04-94AL85000 with Sandia Corporation, the U.S.
// Government retains certain rights in this software.
//
// Copyright (c) 2009-2016, Sandia Corporation
// All rights reserved.
//
// Portions are copyright of other developers:
// See the file CONTRIBUTORS.TXT in the top level directory
// the distribution for more information.
//
// This file is part of the SST software package. For license
// information, see the LICENSE file in the top level directory of the
// distribution.


#include <sst_config.h>
#include <sst/core/interfaces/stringEvent.h>

#include "HTM.h"
#include "memEvent.h"

SST::MemHierarchy::HTM::HTM(ComponentId_t id, Params &params) : Component(id)
{
    /* --------------- Output Class --------------- */
    output_ = new Output();
    int debugLevel = params.find<int>("debug_level", 0);

    output_->init("--->  ", debugLevel, 0,(Output::output_location_t)params.find<int>("debug", 0));
    if(debugLevel < 0 || debugLevel > 10)     output_->fatal(CALL_INFO, -1, "Debugging level must be between 0 and 10. \n");
    output_->debug(_INFO_,"--------------------------- Initializing [HTM]: %s... \n", this->Component::getName().c_str());

    /* --------------- Get Parameters --------------- */
    core_count_ = (uint32_t) params.find<uint32_t>("corecount", 1);

    // LRU - default replacement policy
    int associativity           = params.find<int>("associativity", -1);
    string sizeStr              = params.find<std::string>("cache_size", "");         //Bytes
    int lineSize                = params.find<int>("cache_line_size", -1);            //Bytes

    /* Check user specified all required fields */
    if(-1 >= associativity)         output_->fatal(CALL_INFO, -1, "Param not specified: associativity\n");
    if(sizeStr.empty())             output_->fatal(CALL_INFO, -1, "Param not specified: cache_size\n");
    if(-1 == lineSize)              output_->fatal(CALL_INFO, -1, "Param not specified: cache_line_size - number of bytes in a cacheline (block size)\n");

    fixByteUnits(sizeStr);
    UnitAlgebra ua(sizeStr);
    if (!ua.hasUnits("B")) {
        output_->fatal(CALL_INFO, -1, "Invalid param: cache_size - must have units of bytes (e.g., B, KB,etc.)\n");
    }
    uint64_t cacheSize = ua.getRoundedValue();
    uint numLines = cacheSize/lineSize;

    /* ---------------- Initialization ----------------- */
    HashFunction* ht = new PureIdHashFunction;
    ReplacementMgr* replManager = new LRUReplacementMgr(output_, numLines, associativity, true);
    CacheArray* cacheArray = new SetAssociativeArray(output_, numLines, lineSize, associativity, replManager, ht, false);

    /* --------------- Setup links --------------- */
    if (isPortConnected("cpu_htm_link"))
    {
        cpuLink_ = configureLink("cpu_htm_link", "50ps", new Event::Handler<HTM>(this, &HTM::processRequest));
        output_->debug(_INFO_, "High Network Link ID: %u\n", (uint)cpuLink_->getId());
    } else
    {
        output_->fatal(CALL_INFO, -1, "%s, Error: no connected cache port. Please connect a cache to port 'cache'\n", getName().c_str());
    }

    if (isPortConnected("htm_cache_link"))
    {
        cacheLink_ = configureLink("htm_cache_link", "50ps", new Event::Handler<HTM>(this, &HTM::processResponse));
        output_->debug(_INFO_, "Low Network Link ID: %u\n", (uint)cacheLink_->getId());
    } else
    {
        output_->fatal(CALL_INFO, -1, "%s, Error: no connected cache port. Please connect a cache to port 'cache'\n", getName().c_str());
    }


    /* Register statistics */
    statReadSetSize    = registerStatistic<uint64_t>("ReadSetSize");
    statWriteSetSize  = registerStatistic<uint64_t>("WriteSetSize");
    statAborts   = registerStatistic<uint64_t>("NumAborts");
    statCommits = registerStatistic<uint64_t>("NumCommits");
}


void SST::MemHierarchy::HTM::processRequest(SST::Event* ev) {
    MemEvent* event = static_cast<MemEvent*>(ev);
    Command cmd     = event->getCmd();

    if(cmd == SST::MemHierarchy::BeginTx)
    {
        std::cout << "HTM-" << " Transaction Starting\n" << std::flush;
    }
    else if(cmd == SST::MemHierarchy::EndTx)
    {
        std::cout << "HTM-" << " Transaction Ending\n" << std::flush;
    }
//     else
    {
        cacheLink_->send(ev);
//         SST::Link * link = event->getDeliveryLink();
//         link->send(ev);
    }

//     delete ev;
}

void SST::MemHierarchy::HTM::processResponse(SST::Event* ev) {
//     MemEvent* event = static_cast<MemEvent*>(ev);
//     Command cmd     = event->getCmd();

    cpuLink_->send(ev);

//     if(cmd == SST::MemHierarchy::BeginTx)
//     {
//         std::cout << "HTM-" << " Transaction Starting\n" << std::flush;
//     }
//     else if(cmd == SST::MemHierarchy::EndTx)
//     {
//         std::cout << "HTM-" << " Transaction Starting\n" << std::flush;
//     }
//     else
//     {
//         SST::Link * link = event->getDeliveryLink();
//         link->send(ev);
//     }

//     delete ev;
}

