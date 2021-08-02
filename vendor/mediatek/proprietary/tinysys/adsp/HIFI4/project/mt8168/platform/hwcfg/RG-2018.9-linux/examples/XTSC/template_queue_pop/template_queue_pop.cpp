// Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2010 by Tensilica Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.

#include "template_queue_pop.h"


using namespace std;
using namespace sc_core;
using namespace sc_dt;
using namespace log4xtensa;
using namespace xtsc;


xtsc_component::template_queue_pop::template_queue_pop(sc_module_name module_name, const template_queue_pop_parms& queue_parms) :
  sc_module     (module_name),
  m_consumer    ("m_consumer"),
  m_pop_impl    ("m_pop_impl", *this),
  m_text        (TextLogger::getInstance(name())),
  m_width1      (queue_parms.get_non_zero_u32("bit_width"))
{

  m_consumer(m_pop_impl);

  XTSC_INFO(m_text, "Constructed template_queue_pop '" << name() << "':");
  XTSC_INFO(m_text, " bit_width     = "   << m_width1);

}



xtsc_component::template_queue_pop::~template_queue_pop() {
}



bool xtsc_component::template_queue_pop::xtsc_queue_pop_if_impl::nb_can_pop() {
  return true;
}



bool xtsc_component::template_queue_pop::xtsc_queue_pop_if_impl::nb_pop(sc_unsigned& element, u64& ticket) {
  element = "0x0";
  ticket = 0ULL;
  // If the queue were previously full, here is where a normal queue should notify the no-longer-full event
  return true;
}



void xtsc_component::template_queue_pop::xtsc_queue_pop_if_impl::register_port(sc_port_base& port, const char *if_typename) {
  if (m_p_port) {
    ostringstream oss;
    oss << "Illegal multiple binding detected to template_queue_pop '" << m_queue.name() << "' m_consumer export: " << endl;
    oss << "  " << port.name() << endl;
    oss << "  " << m_p_port->name();
    throw xtsc_exception(oss.str());
  }
  XTSC_INFO(m_queue.m_text, "Binding '" << port.name() << "' to template_queue_pop::m_consumer");
  m_p_port = &port;
}



