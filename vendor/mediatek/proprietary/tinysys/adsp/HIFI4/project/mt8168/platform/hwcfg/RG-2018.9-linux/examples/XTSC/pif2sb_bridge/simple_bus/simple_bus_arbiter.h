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
 
  simple_bus_arbiter.h : The arbitration unit.
 
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

#ifndef __simple_bus_arbiter_h
#define __simple_bus_arbiter_h

#include <systemc.h>

#include "simple_bus_types.h"
#include "simple_bus_request.h"
#include "simple_bus_arbiter_if.h"
#include <log4xtensa/log4xtensa.h>


class simple_bus_arbiter
  : public simple_bus_arbiter_if
  , public sc_module
{
public:
  // constructor
  simple_bus_arbiter(sc_module_name name_
                     , bool verbose = false)
    : sc_module(name_)
    , m_verbose(verbose)
    , m_text(log4xtensa::TextLogger::getInstance(name()))
  {}

  simple_bus_request *arbitrate(const simple_bus_request_vec &requests);

private:
  bool m_verbose;
  log4xtensa::TextLogger& m_text;

}; // end class simple_bus_arbiter

#endif
