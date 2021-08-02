// Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2010 by Tensilica Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.


#include <cerrno>
#include <algorithm>
#include <ostream>
#include <string>
#include <xtsc/xtsc_logging.h>
#include "template_lookup.h"


using namespace std;
using namespace sc_core;
using namespace sc_dt;
using namespace log4xtensa;
using namespace xtsc;


xtsc_component::template_lookup::template_lookup(sc_module_name module_name, const template_lookup_parms& lookup_parms) :
  sc_module             (module_name),
  m_lookup              ("m_lookup"),
  m_lookup_impl         ("m_lookup_impl", *this),
  m_address_bit_width   (lookup_parms.get_non_zero_u32("address_bit_width")),
  m_data_bit_width      (lookup_parms.get_non_zero_u32("data_bit_width")),
  m_zero                (1),
  m_text                (TextLogger::getInstance(name()))
{

  // Bind out sc_export to our implementation
  m_lookup(m_lookup_impl);

  m_has_ready           = lookup_parms.get_bool("has_ready");
  m_zero                = 0;

  LogLevel ll = xtsc_get_constructor_log_level();
  XTSC_LOG(m_text, ll,        "Constructed template_lookup '" << name() << "':");
  XTSC_LOG(m_text, ll,        " address_bit_width           = "   << m_address_bit_width);
  XTSC_LOG(m_text, ll,        " data_bit_width         = "   << m_data_bit_width);
  XTSC_LOG(m_text, ll,        " has_ready               = "   << (m_has_ready ? "true" : "false"));
}



xtsc_component::template_lookup::~template_lookup(void) {
}



void xtsc_component::template_lookup::xtsc_lookup_if_impl::nb_send_address(const sc_unsigned& address) {
  XTSC_DEBUG(m_lookup.m_text, "nb_send_address(" << address.to_string(SC_HEX) << ")");
}



bool xtsc_component::template_lookup::xtsc_lookup_if_impl::nb_is_ready() {
  if (!m_lookup.m_has_ready) {
    ostringstream oss;
    oss << "template_lookup '" << m_lookup.name() << "': Illegal call to nb_is_ready() (\"has_ready\" is false)";
    throw xtsc_exception(oss.str());
  }
  XTSC_DEBUG(m_lookup.m_text, "nb_is_ready()");
  return true;
}



sc_unsigned xtsc_component::template_lookup::xtsc_lookup_if_impl::nb_get_data() {
  XTSC_VERBOSE(m_lookup.m_text, "nb_get_data()=" << m_lookup.m_zero.to_string(SC_HEX));
  return m_lookup.m_zero;
}



void xtsc_component::template_lookup::xtsc_lookup_if_impl::register_port(sc_port_base& port, const char *if_typename) {
  if (m_p_port) {
    ostringstream oss;
    oss << "Illegal multiple binding detected to template_lookup '" << m_lookup.name() << "' m_lookup export: " << endl;
    oss << "  " << port.name() << endl;
    oss << "  " << m_p_port->name();
    throw xtsc_exception(oss.str());
  }
  XTSC_INFO(m_lookup.m_text, "Binding '" << port.name() << "' to template_lookup::m_lookup");
  m_p_port = &port;
}



