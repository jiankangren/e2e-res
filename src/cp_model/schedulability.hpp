/**
 * Copyright (c) 2013-2016, Nima Khalilzad   <nkhal@kth.se>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once
#include <gecode/int.hh>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <chrono>
#include <sstream>
#include <fstream>
#include "../application/application.hpp"

using namespace Gecode;
using namespace Int;
using namespace std;

class Schedulability : public Propagator {

protected:
  ViewArray<IntView> theta_array; /*!< current budgets. */
  ViewArray<IntView> pi_array; /*!< current periods. */
  vector<Base_Transaction*> base_transactions;
public:
  Schedulability( Space& home, 
                  ViewArray<IntView> _theta_array,
                  ViewArray<IntView> _pi_array,
                  vector<Base_Transaction*> _base_transactions);

static ExecStatus post(Space& home, 
					   ViewArray<IntView> _theta_array,
					   ViewArray<IntView> _pi_array,
					   vector<Base_Transaction*> _base_transactions){
    (void) new (home) Schedulability(home, _theta_array, _pi_array, _base_transactions);
    return ES_OK;
  }

  virtual size_t dispose(Space& home);

  Schedulability(Space& home, bool share, Schedulability& p);

  virtual Propagator* copy(Space& home, bool share);

  virtual PropCost cost(const Space& home, const ModEventDelta& med) const;

  virtual ExecStatus propagate(Space& home, const ModEventDelta&);
        
};

extern void  Schedulability( Space& home, 
                             const IntVarArgs& _theta_args,
                             const IntVarArgs& _pi_args,
                             vector<Base_Transaction*> _base_transactions
                             );
extern void Schedulability();
