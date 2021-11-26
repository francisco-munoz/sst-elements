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
#incldue "include/DataPackage.h"

#include "llyrTypes.h"

using namespace SST::Interfaces;

namespace SST {
namespace Llyr {


class LSEntry
{
public:
    LSEntry(const SimpleMem::Request::id_t reqId, DataPackage* data_package , uint32_t write_) :
            req_id_(reqId), data_package_(data_package), write_(write), ready_(0) {}
    ~LSEntry() {}

    SimpleMem::Request::id_t getReqId() const { return req_id_; }
    DataPackage* getDataPackage() const {return data_package_;}
    
    void setDataPackage( DataPackage* data_package ) { data_package_ = data_package; }
    LlyrData getData() const{ return data_; }

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
    }
    ~LSQueue() {}

    uint32_t getNumEntries() const { return memory_queue_.size(); }
    SimpleMem::Request::id_t getNextEntry() const { return memory_queue_.front(); }

    void addEntry( LSEntry* entry )
    {
        memory_queue_.push( entry->getReqId() );
        pending_.emplace( entry->getReqId(), entry );
    }

    std::pair< uint32_t, uint32_t > lookupEntry( SimpleMem::Request::id_t id )
    {
        auto entry = pending_.find( id );
        if( entry == pending_.end() )
        {
            output_->verbose(CALL_INFO, 0, 0, "Error: response from memory could not be found.\n");
            exit(-1);
        }

        return std::make_pair( entry->second->getSourcePe(), entry->second->getTargetPe() );
    }

    void removeEntry( SimpleMem::Request::id_t id )
    {
        memory_queue_.pop();
        auto entry = pending_.find( id );
        if( entry != pending_.end() )
        {
            pending_.erase(entry);
        }
    }

    LlyrData getEntryData( SimpleMem::Request::id_t id ) const
    {
        auto entry = pending_.find( id );
        if( entry != pending_.end() )
        {
            return entry->second->getData();
        }

        return 0;
    }

    void setEntryData( SimpleMem::Request::id_t id, LlyrData data )
    {
        auto entry = pending_.find( id );
        if( entry != pending_.end() )
        {
            entry->second->setData(data);
        }
    }

    uint32_t getEntryReady( SimpleMem::Request::id_t id ) const
    {
        auto entry = pending_.find( id );
        if( entry != pending_.end() )
        {
            return entry->second->getReady();
        }

        return 0;
    }

    void setEntryReady( SimpleMem::Request::id_t id, uint32_t ready )
    {
        auto entry = pending_.find( id );
        if( entry != pending_.end() )
        {
            entry->second->setReady(ready);
        }
    }


protected:


private:
    SST::Output* output_;

    std::queue< SimpleMem::Request::id_t > memory_queue_;
    std::map< SimpleMem::Request::id_t, LSEntry* > pending_;

};


}
}

#endif
