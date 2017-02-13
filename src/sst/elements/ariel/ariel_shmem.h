// Copyright 2009-2017 Sandia Corporation. Under the terms
// of Contract DE-NA0003525 with Sandia Corporation, the U.S.
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

#ifndef SST_ARIEL_SHMEM_H
#define SST_ARIEL_SHMEM_H 1

#include <atomic>
#include <inttypes.h>
#include <boost/concept_check.hpp>

#include <sst/core/interprocess/ipctunnel.h>
#include "ariel_inst_class.h"

#define MAX_PROCS 2048

namespace SST {
namespace ArielComponent {

enum ArielShmemCmd_t {
    ARIEL_PERFORM_EXIT = 1,
    ARIEL_PERFORM_READ = 2,
    ARIEL_PERFORM_WRITE = 4,
    ARIEL_START_DMA = 8,
    ARIEL_WAIT_DMA = 16,
    ARIEL_START_INSTRUCTION = 32,
    ARIEL_END_INSTRUCTION = 64,
    ARIEL_ISSUE_TLM_MAP = 80,
    ARIEL_ISSUE_TLM_FREE = 100,
    ARIEL_SWITCH_POOL = 110,
    ARIEL_NOOP = 128,
    ARIEL_OUTPUT_STATS = 140,
    ARIEL_START_TRANSACTION = 312,
    ARIEL_END_TRANSACTION = 314,
    ARIEL_ABORT_TRANSACTION = 316,
    ARIEL_COMMIT_TRANSACTION = 318
};

enum TransactionState_t {
    TX_RUN,
    TX_ABORT,
    TX_COMMIT
};

struct ArielCommand {
    ArielShmemCmd_t command;
    uint64_t instPtr;
    union {
        struct {
            uint32_t size;
            uint64_t addr;
            uint32_t instClass;
	    uint32_t simdElemCount;
        } inst;
        struct {
            uint64_t vaddr;
            uint64_t alloc_len;
            uint32_t alloc_level;
        } mlm_map;
        struct {
            uint64_t vaddr;
        } mlm_free;
        struct {
            uint32_t pool;
        } switchPool;
        struct {
            uint64_t src;
            uint64_t dest;
            uint32_t len;
        } dma_start;
    };
};


struct ArielSharedData {
    std::atomic<size_t> numCores;
    std::atomic<uint64_t> simTime;
    std::atomic<uint64_t> cycles;
    std::atomic<TransactionState_t> txState[MAX_PROCS];
    volatile std::atomic<uint32_t> child_attached;
    uint8_t __pad[ 256 - sizeof(uint32_t) - sizeof(size_t) - sizeof(uint64_t) - sizeof(uint64_t) - (sizeof(txState) % 256) ];
};




class ArielTunnel : public SST::Core::Interprocess::IPCTunnel<ArielSharedData, ArielCommand>
{
public:
    /**
     * Create a new Ariel Tunnel
     */
    ArielTunnel(uint32_t comp_id, size_t numCores, size_t bufferSize) :
        SST::Core::Interprocess::IPCTunnel<ArielSharedData, ArielCommand>(comp_id, numCores, bufferSize)
    {
        sharedData->numCores = numCores;
        sharedData->simTime = 0;
        sharedData->cycles = 0;
        sharedData->child_attached = 0;

        for(uint32_t i = 0; i < MAX_PROCS; i++) {
           sharedData->txState[i] = TX_RUN;
        }

    }

    /**
     * Attach to an existing Ariel Tunnel (Created in another process
     */
    ArielTunnel(const std::string &region_name) :
        SST::Core::Interprocess::IPCTunnel<ArielSharedData, ArielCommand>(region_name)
    {
        /* Ideally, this would be done atomically, but we'll only have 1 child */
        sharedData->child_attached++;
    }

    void waitForChild(void)
    {
        while ( sharedData->child_attached == 0 ) ;
    }

    /** Update the current simulation cycle count in the SharedData region */
    void updateTime(uint64_t newTime)
    {
        sharedData->simTime = newTime;
    }

    /** Increment current cycle count */
    void incrementCycles()
    {
        sharedData->cycles++;
    }

    uint64_t getCycles() const
    {
        return sharedData->cycles.load();
    }

    void updateTransactionState(uint32_t coreID, TransactionState_t stateIn)
    {
       sharedData->txState[coreID] = stateIn;
    }

    TransactionState_t getTransactionState(uint32_t coreID) const
    {
       return sharedData->txState[coreID].load();
    }

    /** Return the current time (in seconds) of the simulation */
    void getTime(struct timeval *tp)
    {
        uint64_t cTime = sharedData->simTime.load();
        tp->tv_sec = cTime / 1e9;
        tp->tv_usec = (cTime - (tp->tv_sec * 1e9)) / 1e3;
    }

    /** Return the current time in nanoseconds of the simulation */
    void getTimeNs(struct timespec *tp)
    {
        uint64_t cTime = sharedData->simTime.load();
        tp->tv_sec = cTime / 1e9;
        tp->tv_nsec = cTime - (tp->tv_sec * 1e9);
    }

};


}
}

#endif
