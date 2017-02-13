/*
 * Copyright 2009-2017 Sandia Corporation. Under the terms
 * of Contract DE-AC04-94AL85000 with Sandia Corporation, the U.S.
 * Government retains certain rights in this software.
 *
 * Copyright (c) 2009-2017, Sandia Corporation
 * All rights reserved.
 *
 * Portions are copyright of other developers:
 * See the file CONTRIBUTORS.TXT in the top level directory
 * the distribution for more information.
 *
 * This file is part of the SST software package. For license
 * information, see the LICENSE file in the top level directory of the
 * distribution
 */

#ifndef TX_MANAGER_H
#define TX_MANAGER_H

#include <map>
#include <iostream>

#include "memEvent.h"
#include "cacheArray.h"



namespace SST { namespace MemHierarchy {

typedef CacheArray::DataLine            DataLine;

class TxManager
{
public:
   TxManager(uint64_t enableHTM)
   {
      htmEnabled = enableHTM;
      cacheMap = new std::map< Addr, vector<uint8_t>* >;

      txDepth = 0;
   };

   ~TxManager()
   {
      std::map< Addr, vector<uint8_t>* >::iterator iter;
      for(iter =  cacheMap->begin(); iter != cacheMap->end(); iter++)
      {
         delete iter->second;
      }

      delete cacheMap;
   };

   bool addLine(Addr addressIn, vector<uint8_t>* dataIn)
   {
      std::pair< std::map< Addr, vector<uint8_t>*  >::iterator, bool > retVal;
      retVal = cacheMap->insert( std::pair< Addr, vector<uint8_t>* >(addressIn, dataIn) );
      if(retVal.second == false)
      {
         std::cout << "\tAlready Present " << addressIn << "\n" << std::flush;
      }
      else
      {
         std::cout << "\tInserting " << addressIn << "\n" << std::flush;
      }
   };

   bool linePresent(Addr addressIn)
   {
      std::map< Addr, vector<uint8_t>*  >::iterator iter = cacheMap->find(addressIn);
      if(iter != cacheMap->end())
         return 1;
      else
         return 0;
   };

   uint64_t get_htmEnabled(void) { return htmEnabled; };

   uint64_t get_txDepth(void) { return txDepth; };

   void inc_txDepth(void) { txDepth++; };

   void dec_txDepth(void) { txDepth--; };

   uint32_t beginTransaction(void)
   {
      std::cout << "-------------    TX Begin    -------------\n" << std::flush;

      inc_txDepth();

      return get_txDepth();
   }

   uint32_t commitTransaction(void)
   {
      std::cout << "-------------    TX End    -------------\n" << std::flush;

      dec_txDepth();

      return get_txDepth();
   }


private:
   std::map< Addr, vector<uint8_t>*  >* cacheMap;
   uint64_t                             htmEnabled;
   uint64_t                             txDepth;


};



}} //END namespaces

#endif // TX_MANAGER_H
