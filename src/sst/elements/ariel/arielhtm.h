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


#ifndef _H_SST_ARIEL_HTM_EVENT
#define _H_SST_ARIEL_HTM_EVENT

#include "arielevent.h"

using namespace SST;

namespace SST {
namespace ArielComponent {

class ArielHTMEvent : public ArielEvent {

public:
   ArielHTMEvent(ArielEventType eventType) :
      txEventType(eventType) {}
   ~ArielHTMEvent() {}

   ArielEventType getEventType() const
   {
      return txEventType;
   }

private:
   ArielEventType txEventType;

};

}
}

#endif
