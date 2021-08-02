// Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2010 by Tensilica Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of Tensilica Inc.
// They may not be modified, copied, reproduced, distributed, or disclosed to
// third parties in any manner, medium, or form, in whole or in part, without
// the prior written consent of Tensilica Inc.


#include "q2f_adapter.h"


using namespace std;
using namespace sc_core;
using namespace sc_dt;
using namespace xtsc;




q2f_adapter::q2f_adapter(sc_module_name module_name, const q2f_adapter_parms &parms) :
  sc_module     (module_name),
  m_producer    ("m_producer"),
  m_push_impl   ("m_push_impl", *this),
  m_fifo        (get_or_create_fifo(parms, m_our_fifo)),
  m_width1      (parms.get_non_zero_u32("bit_width")),
  m_text        (log4xtensa::TextLogger::getInstance(name()))
{
  m_producer(m_push_impl);

  log4xtensa::LogLevel ll = xtsc_get_constructor_log_level();
  XTSC_LOG(m_text, ll,        "Constructed q2f_adapter '" << name() << "':");
  XTSC_LOG(m_text, ll,        " p_fifo                  = "   << parms.get_void_pointer("p_fifo"));
  XTSC_LOG(m_text, ll,        " depth                   = "   << m_fifo.num_free());
  XTSC_LOG(m_text, ll,        " bit_width               = "   << m_width1);

}



q2f_adapter::~q2f_adapter() {
  if (m_our_fifo) {
    delete &m_fifo;
  }
}



// static method
sc_fifo<sc_unsigned>& q2f_adapter::get_or_create_fifo(const q2f_adapter_parms &parms, bool& our_fifo) {
  void *pvoid = const_cast<void*>(parms.get_void_pointer("p_fifo"));
  if (pvoid == NULL) {
    sc_length_param length_param(parms.get_u32("bit_width"));
    sc_length_context length_context(length_param, SC_NOW);
    sc_fifo<sc_unsigned> *p_fifo = new sc_fifo<sc_unsigned>("m_fifo", parms.get_non_zero_u32("depth"));
    length_context.end();
    our_fifo = true;
    return *p_fifo;
  }
  else {
    sc_fifo<sc_unsigned> *p_fifo = static_cast<sc_fifo<sc_unsigned>*>(pvoid);
    our_fifo = false;
    return *p_fifo;
  }
}



bool q2f_adapter::xtsc_queue_push_if_impl::nb_push(const sc_unsigned& element, u64& ticket) {
  if (static_cast<u32>(element.length()) != m_adapter.m_width1) {
    ostringstream oss;
    oss << "ERROR: Element of width=" << element.length() << " bits added to q2f_adapter '" << m_adapter.name()
        << "' of width=" << m_adapter.m_width1;
    throw xtsc_exception(oss.str());
  }
  if (!m_adapter.m_fifo.nb_write(element)) {
    return false;
  }
  XTSC_INFO(m_adapter.m_text, "Pushed (cnt=" << m_adapter.m_fifo.num_available()+1 << "): " << element.to_string(SC_HEX));
  ticket = 0;
  return true;
}



void q2f_adapter::xtsc_queue_push_if_impl::register_port(sc_port_base& port, const char *if_typename) {
  if (m_p_port) {
    ostringstream oss;
    oss << "Illegal multiple binding detected to q2f_adapter '" << m_adapter.name() << "' m_producer export: " << endl;
    oss << "  " << port.name() << endl;
    oss << "  " << m_p_port->name();
    throw xtsc_exception(oss.str());
  }
  XTSC_INFO(m_adapter.m_text, "Binding '" << port.name() << "' to q2f_adapter::m_producer");
  m_p_port = &port;
}



