// Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2010 by Tensilica Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.

#include <sstream>
#include "template_wire_read.h"


using namespace std;
using namespace sc_core;
using namespace sc_dt;
using namespace log4xtensa;
using namespace xtsc;


xtsc_component::template_wire_read::template_wire_read(sc_module_name module_name, const template_wire_read_parms& wire_parms) :
  sc_module             (module_name),
  m_width1              (wire_parms.get_non_zero_u32("bit_width")),
  m_value               (m_width1),
  m_text                (TextLogger::getInstance(name()))
{
  m_value = "0x0";

  LogLevel ll = xtsc_get_constructor_log_level();
  XTSC_LOG(m_text, ll, "Constructed template_wire_read '" << name() << "':");
  XTSC_LOG(m_text, ll, " bit_width               = "   << m_width1);

}



xtsc_component::template_wire_read::~template_wire_read() {
}



sc_unsigned xtsc_component::template_wire_read::nb_read() {
  XTSC_INFO(m_text, "Read import wire (TLM) " << m_value.to_string(SC_HEX));
  return m_value;
}



