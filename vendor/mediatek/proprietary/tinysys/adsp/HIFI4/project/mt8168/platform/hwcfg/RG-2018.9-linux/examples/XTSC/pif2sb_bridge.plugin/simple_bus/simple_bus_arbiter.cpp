/*****************************************************************************

  The following code is derived, directly or indirectly, from the SystemC
  source code Copyright (c) 1996-2004 by all Contributors.
  All Rights reserved.

  The contents of this file are subject to the restrictions and limitations
  set forth in the SystemC Open Source License Version 2.4 (the "License");
  You may not use this file except in compliance with such restrictions and
  limitations. You may obtain instructions on how to receive a copy of the
  License at http://www.systemc.org/. Software distributed by Contributors
  under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
  ANY KIND, either express or implied. See the License for the specific
  language governing rights and limitations under the License.

 *****************************************************************************/

/*****************************************************************************
 
  simple_bus_arbiter.cpp : The arbitration unit.
 
  Original Author: Ric Hilderink, Synopsys, Inc., 2001-10-11
 
 *****************************************************************************/
 
/*****************************************************************************
 
  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.
 
      Name, Affiliation, Date: Ric Howard, Tensilica, Inc, 2006.05.08
  Description of Modification: Use log4xtensa 
 
      Name, Affiliation, Date:
  Description of Modification:
 
 *****************************************************************************/

#include "simple_bus_arbiter.h"
#include <xtsc/xtsc.h>

simple_bus_request *
simple_bus_arbiter::arbitrate(const simple_bus_request_vec &requests)
{
  int i;
  // at least one request is here
  simple_bus_request *best_request = requests[0]; 

  if (m_text.isEnabledFor(log4xtensa::VERBOSE_LOG_LEVEL))
    { // shows the list of pending requests
      XTSC_VERBOSE(m_text, "Pending requests . . .");
      for (i = 0; i < requests.size(); ++i)
	{
	  simple_bus_request *request = requests[i];
          // simple_bus_lock_status encoding
          const char lock_chars[] = { '-', '=', '+' };
          // simple_bus_status encoding
          XTSC_VERBOSE(m_text, "R[" << request->priority << "](" << lock_chars[request->lock] <<
                               simple_bus_status_str[request->status] << "@0x" << hex << request->address);
	}
    }

  // highest priority: status==SIMPLE_BUS_WAIT and lock is set: 
  // locked burst-action
  for (i = 0; i < requests.size(); ++i)
    {
      simple_bus_request *request = requests[i];
      if ((request->status == SIMPLE_BUS_WAIT) &&
	  (request->lock == SIMPLE_BUS_LOCK_SET))
	{
	  // cannot break-in a locked burst
          XTSC_VERBOSE(m_text, "-> R[" << request->priority << "] (rule 1)");
	  return request;
	}
    }

  // second priority: lock is set at previous call, 
  // i.e. SIMPLE_BUS_LOCK_GRANTED
  for (i = 0; i < requests.size(); ++i)
    if (requests[i]->lock == SIMPLE_BUS_LOCK_GRANTED)
      {
        XTSC_VERBOSE(m_text, "-> R[" << requests[i]->priority << "] (rule 2)");
	return requests[i];
      }

  // third priority: priority
  for (i = 1; i < requests.size(); ++i)
    {
      sc_assert(requests[i]->priority != best_request->priority);
      if (requests[i]->priority < best_request->priority)
	best_request = requests[i];
    }

  if (best_request->lock != SIMPLE_BUS_LOCK_NO)
    best_request->lock = SIMPLE_BUS_LOCK_GRANTED;
	
  XTSC_VERBOSE(m_text, "-> R[" << best_request->priority << "] (rule 3)");

  return best_request;
}
