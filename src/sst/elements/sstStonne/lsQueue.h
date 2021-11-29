// Copyright 2009-2020 NTESS. Under the terms
// of Contract DE-NA0003525 with NTESS, the U.S.
// Government retains certain rights in this software.
//
// Copyright (c) 2009-2020, NTESS
// All rights reserved.
//
// Portions are copyright of other developers:
// See the file CONTRIBUTORS.TXT in the top level directory
// the distribution for more information.
//
// This file is part of the SST software package. For license
// information, see the LICENSE file in the top level directory of the
// distribution.


#ifndef _LLYR_LSQ
#define _LLYR_LSQ

#include <sst/core/output.h>
#include <sst/core/interfaces/simpleMem.h>

#include <map>
#include <queue>
#include <bitset>
#include <utility>
#include <cstdint>
#include "include/types.h"
#include "include/DataPackage.h"


using namespace SST::Interfaces;

namespace SST {
namespace SST_STONNE {


class LSEntry
{
public:
    LSEntry(const SimpleMem::Request::id_t reqId, DataPackage* data_package , uint32_t write_) :
            req_id_(reqId), data_package_(data_package), write_(write), ready_(0) {}
    ~LSEntry() {}

    SimpleMem::Request::id_t getReqId() const { return req_id_; }
    DataPackage* getDataPackage() const {return data_package_;}
    
    void setDataPackage( DataPackage* data_package ) { data_package_ = data_package; }
    data_t getData() const{ return data_package_->get_data(); }
    void setData(data_t data) const { data_package_->setData(data);  }

    void setWrite( uint32_t write ) { write_ = write; }
    uint32_t getWrite() const{return write_;}

    void setReady( uint32_t ready ) { ready_ = ready; }
    uint32_t getReady() const{ return ready_; }

protected:
    SimpleMem::Request::id_t req_id_;
    DataPackage*  data_package_; //Associated data package 
    uint32_t write_;
    uint32_t ready_;

private:

};


/* The idea is to have the requests in this QUEUE and as long as the requests com through from the memory hierachy
 * being able to remove the entry and send the package out to the distribution networrk */

class LSQueue
{
public:
    LSQueue()
    {
        //setup up i/o for messages
        char prefix[256];
        sprintf(prefix, "[t=@t][LSQueue]: ");
        output_ = new SST::Output(prefix, 0, 0, Output::STDOUT);
    }
    LSQueue(const LSQueue &copy)
    {
        output_ = copy.output_;
        memory_queue_ = copy.memory_queue_;
        pending_ = copy.pending_;
	completed_ = copy.completed_;
    }
    ~LSQueue() {}

    uint32_t getNumCompletedEntries() const { return completed_memory_queue_.size(); }
    uint32_t getNumPendingEntries() const { return pending_.size(); }
    SimpleMem::Request::id_t getNextCompletedEntry() const { return completed_memory_queue_.front(); }


    void addEntry( LSEntry* entry )
    {
        pending_memory_queue_.push( entry->getReqId() );
        pending_.emplace( entry->getReqId(), entry );
    }


    void removeEntry( SimpleMem::Request::id_t id )
    {
        completed_memory_queue_.pop(); //Always call first to getNextCompletedEntry
        auto entry = completed_.find( id );

	if(entry != completed_.end()) {
	    completed_.erase(entry);
	}
    }

    DataPackage* getEntryPackage( SimpleMem::Request::id_t id)
    {
        auto entry = completed_.find( id );
        if( entry != completed_.end() )
        {
            return entry->second->getDataPackage();
        }

	else {
 	    output_->verbose(CALL_INFO, 0, 0, "Error: request from memory could not be found.\n");
            exit(-1);

	}

    }

    void setEntryPackage( SimpleMem::Request::id_t id, DataPackage* data_package )
    {
        auto entry = pending_.find( id );
        if( entry != pending_.end() )
        {
            entry->second->setDataPackage(data_package);
        }
    }

    void setEntryData( SimpleMem::Request::id_t id, data_t data ) {
        auto entry = pending_.find(id);
	if(entry != pending_.end()) {
            entry->second->setData(data);
	}

	else {
            output_->verbose(CALL_INFO, 0, 0, "Error: request from memory could not be found.\n");
            exit(-1);
	}
    }


    void setEntryReady( SimpleMem::Request::id_t id, uint32_t ready )
    {
        auto entry = pending_.find( id );
        if( entry != pending_.end() )
        {
            entry->second->setReady(ready);
	    completed_.emplace(entry->second->getReqId(), entry->second);
	    pending_.erase(entry);
        }

	else {
            output_->verbose(CALL_INFO, 0, 0, "Error: request from memory could not be found.\n");
            exit(-1);
        }

    }


protected:


private:
    SST::Output* output_;

    std::queue< SimpleMem::Request::id_t> completed_memory_queue_;
    std::map< SimpleMem::Request::id_t, LSEntry* > pending_;
    std::map< SimpleMem::Request::id_t, LSEntry*> completed_;

};


}
}

#endif
