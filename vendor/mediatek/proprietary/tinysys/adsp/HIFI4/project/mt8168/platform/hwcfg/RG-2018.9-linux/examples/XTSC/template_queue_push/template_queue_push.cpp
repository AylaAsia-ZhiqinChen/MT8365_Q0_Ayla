// Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2010 by Tensilica Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.

#include "template_queue_push.h"


using namespace std;
using namespace sc_core;
using namespace sc_dt;
using namespace log4xtensa;
using namespace xtsc;


xtsc_component::template_queue_push::template_queue_push(sc_module_name module_name, const template_queue_push_parms& queue_parms) :
  sc_module     (module_name),
  m_producer    ("m_producer"),
  m_push_impl   ("m_push_impl", *this),
  m_text        (TextLogger::getInstance(name()))
{

  m_width1 = queue_parms.get_non_zero_u32("bit_width");

  // Bind export to implementation
  m_producer(m_push_impl);

  XTSC_INFO(m_text, "Constructed template_queue_push '" << name() << "':");
  XTSC_INFO(m_text, " bit_width     = "   << m_width1);

}



xtsc_component::template_queue_push::~template_queue_push() {
}



bool xtsc_component::template_queue_push::xtsc_queue_push_if_impl::nb_can_push() {
  return true;
}



bool xtsc_component::template_queue_push::xtsc_queue_push_if_impl::nb_push(const sc_unsigned& element, u64& ticket) {
  // If the queue were previously empty, here is where a normal queue should notify the no-longer-empty event
  ticket = xtsc_create_queue_ticket();
  XTSC_INFO(m_queue.m_text, "Pushed (ticket=" << ticket << "): " << element.to_string(SC_HEX));
  return true;
}



void xtsc_component::template_queue_push::xtsc_queue_push_if_impl::register_port(sc_port_base& port, const char *if_typename) {
  if (m_p_port) {
    ostringstream oss;
    oss << "Illegal multiple binding detected to template_queue_push '" << m_queue.name() << "' m_producer export: " << endl;
    oss << "  " << port.name() << endl;
    oss << "  " << m_p_port->name();
    throw xtsc_exception(oss.str());
  }
  XTSC_INFO(m_queue.m_text, "Binding '" << port.name() << "' to template_queue_push::m_producer");
  m_p_port = &port;
}



