// Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2010 by Tensilica Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.

#include <sstream>
#include "template_wire_write.h"


using namespace std;
using namespace sc_core;
using namespace sc_dt;
using namespace log4xtensa;
using namespace xtsc;


xtsc_component::template_wire_write::template_wire_write(sc_module_name module_name, const template_wire_write_parms& wire_parms) :
  sc_module             (module_name),
  m_width1              (wire_parms.get_non_zero_u32("bit_width")),
  m_text                (TextLogger::getInstance(name()))
{

  LogLevel ll = xtsc_get_constructor_log_level();
  XTSC_LOG(m_text, ll,        "Constructed template_wire_write '" << name() << "':");
  XTSC_LOG(m_text, ll,        " bit_width               = "   << m_width1);

}



xtsc_component::template_wire_write::~template_wire_write() {
}



void xtsc_component::template_wire_write::nb_write(const sc_unsigned& value) {
  if (static_cast<u32>(value.length()) != m_width1) {
    ostringstream oss;
    oss << "ERROR: Value of width=" << value.length() << " bits written to template_wire_write '" << name() << "' of width="
        << m_width1;
    throw xtsc_exception(oss.str());
  }
  XTSC_INFO(m_text, "template_wire_write::nb_write: " << value.to_string(SC_HEX));
}



