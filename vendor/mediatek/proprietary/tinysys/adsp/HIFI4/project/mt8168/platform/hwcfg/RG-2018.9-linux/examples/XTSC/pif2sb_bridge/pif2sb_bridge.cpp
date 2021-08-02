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
#include "pif2sb_bridge.h"

using namespace std;
using namespace sc_core;
using namespace log4xtensa;
using namespace xtsc;



pif2sb_bridge::pif2sb_bridge(sc_module_name module_name, const pif2sb_bridge_parms& bridge_parms) :
  sc_module             (module_name),
  m_pif_request_export  ("m_pif_request_export"),
  m_pif_respond_port    ("m_pif_respond_port"),
  m_sb_direct_port      ("m_sb_direct_port"),
  m_sb_blocking_port    ("m_sb_blocking_port"),
  m_request_impl        ("m_request_impl", *this),
  m_text                (TextLogger::getInstance(name()))
{

  m_pif_width8  = bridge_parms.get_u32("pif_byte_width");
  if ((m_pif_width8 != 4) && (m_pif_width8 != 8) && (m_pif_width8 != 16)) {
    ostringstream oss;
    oss << name() << ":p2f2sb_bridge: \"pif_byte_width\"=" << m_pif_width8 << " (expected 4|8|16).";
    throw xtsc_exception(oss.str());
  }
  m_priority    = bridge_parms.get_u32("unique_priority");

  m_busy                        = false;
  m_rcw_have_first_transfer     = false;

  // Get clock period 
  u32 clock_period = bridge_parms.get_u32("clock_period");
  if (clock_period == 0xFFFFFFFF) {
    m_clock_period = xtsc_get_system_clock_period();
  }
  else {
    m_clock_period = sc_get_time_resolution() * clock_period;
  }

  m_pif_request_export(m_request_impl);

  SC_THREAD(worker_thread);

  LogLevel ll = xtsc_get_constructor_log_level();
  XTSC_LOG(m_text, ll,        "Constructed pif2sb_bridge '" << name() << "':");
  XTSC_LOG(m_text, ll,        " pif_byte_width          =   " << m_pif_width8);
  XTSC_LOG(m_text, ll,        " unique_priority         =   " << m_priority);
  if (clock_period == 0xFFFFFFFF) {
  XTSC_LOG(m_text, ll, hex << " clock_period            = 0x" << clock_period << " (" << m_clock_period << ")");
  } else {
  XTSC_LOG(m_text, ll,        " clock_period            = "   << clock_period << " (" << m_clock_period << ")");
  }
}



pif2sb_bridge::~pif2sb_bridge(void) {
}



void pif2sb_bridge::worker_thread(void) {

  try {

    while (true) {
      m_busy = false;
      XTSC_TRACE(m_text, "worker_thread going to sleep.");
      wait(m_worker_thread_event);
      XTSC_TRACE(m_text, "worker_thread woke up.");
      xtsc_response response(m_request, xtsc_response::RSP_OK, true);
      switch (m_request.get_type()) {
        case xtsc_request::READ:          { do_read       (response); break; }
        case xtsc_request::BLOCK_READ:    { do_block_read (response); break; }
        case xtsc_request::RCW:           { do_rcw        (response); break; }
        case xtsc_request::WRITE:         { do_write      (response); break; }
        case xtsc_request::BLOCK_WRITE:   { do_block_write(response); break; }
        default: {
          throw xtsc_exception("Unrecognized request type in pif2sb_bridge::worker_thread()");
        }
      }
    }

  }
  catch (const exception& error) {
    ostringstream oss;
    oss << "std::exception caught in SC_THREAD of " << kind() << " '" << name() << "'." << endl;
    oss << "what(): " << error.what() << endl;
    xtsc_log_multiline(m_text, FATAL_LOG_LEVEL, oss.str(), 2);
    throw;
  }

}



void pif2sb_bridge::send_response(xtsc_response& response) {
  XTSC_INFO(m_text, response);
  // Need to check return value because master might be busy
  while (!m_pif_respond_port->nb_respond(response)) {
    XTSC_VERBOSE(m_text, "nb_respond() returned false; waiting one clock period to try again ");
    wait(m_clock_period);
  }
}



void pif2sb_bridge::do_read(xtsc_response& response) {
  u8  *buffer           = response.get_buffer();
  u32  address8         = m_request.get_byte_address();
  u32  size8            = m_request.get_byte_size();
  u32  length           = size8/4;
  XTSC_VERBOSE(m_text, "do_read() address=0x" << hex << address8 << " size=" << dec << size8);
  if (((size8 != 1) && (size8 != 2) && (size8 != 4) && (size8 != 8) && (size8 != 16)) ||        // not a power of 2   .OR.
      (size8 > m_pif_width8) ||                                                                 // exceeds pif width  .OR.
      ((address8 % size8) != 0))                                                                // not size8-aligned
  {
    XTSC_INFO(m_text, "do_read() returning RSP_ADDRESS_ERROR: address=0x" << hex << address8 <<
                         " size=" << dec << size8 << " m_pif_width8=" << m_pif_width8);
    response.set_status(xtsc_response::RSP_ADDRESS_ERROR);
  }
  else {
    if (size8 < 4) {
      u32 addr8         = address8 & (-4);  // align addr8 to word32 boundary
      u32 offset        = address8 & 0x3;
      u32 data;
      simple_bus_status status = m_sb_blocking_port->burst_read(m_priority, (int*)&data, addr8, 1, false);
      if (status == SIMPLE_BUS_ERROR) {
        response.set_status(xtsc_response::RSP_ADDRESS_ERROR);
      }
      else if (status != SIMPLE_BUS_OK) {
        ostringstream oss;
        oss << name() << " - pif2sb_bridge::do_read() got simple_bus_status=" << status << " from call to burst_read()";
        throw xtsc_exception(oss.str());
      }
      else {
        u8 *p_d = reinterpret_cast<u8*>(&data);
        p_d += offset;
        memcpy(buffer, p_d, size8);
      }
    }
    else {
      simple_bus_status status = m_sb_blocking_port->burst_read(m_priority, (int *)buffer, address8, length, false);
      if (status == SIMPLE_BUS_ERROR) {
        response.set_status(xtsc_response::RSP_ADDRESS_ERROR);
      }
      else if (status != SIMPLE_BUS_OK) {
        ostringstream oss;
        oss << name() << " - pif2sb_bridge::do_read() got simple_bus_status=" << status << " from call to burst_read()";
        throw xtsc_exception(oss.str());
      }
    }
  }
  send_response(response);
}



void pif2sb_bridge::do_block_read(xtsc_response& response) {
  int *data             = (int *) response.get_buffer();
  u32  address8         = m_request.get_byte_address();
  u32  size8            = m_request.get_byte_size();
  u32  length           = size8/4;
  u32  num_transfers    = m_request.get_num_transfers();
  u32  total_size8      = size8 * num_transfers;
  XTSC_VERBOSE(m_text, "do_block_read() address=0x" << hex << address8 << " num_transfers=" << dec << num_transfers);
  if ((size8 != m_pif_width8) || ((address8 % m_pif_width8) != 0)) {
    XTSC_INFO(m_text, "do_block_read() returning RSP_ADDRESS_ERROR: address=0x" << hex << address8 <<
                         " size=" << dec << size8 << " m_pif_width8=" << m_pif_width8);
    response.set_status(xtsc_response::RSP_ADDRESS_ERROR);
    send_response(response);
    return;
  }
  response.set_last_transfer(false);
  for (u32 i=1; i <= num_transfers; ++i) {
    // Adjust address for this transfer.  Include wrap-around for Critical Word First support.
    xtsc_address address8 = m_request.get_byte_address();
    address8 =  (address8 & ~(total_size8-1)) | (((address8 & (total_size8-1)) + (i-1) * size8) % total_size8);
    simple_bus_status status = m_sb_blocking_port->burst_read(m_priority, data, address8, length, false);
    if (status == SIMPLE_BUS_ERROR) {
      i = num_transfers; // Do only 1 transfer
      response.set_status(xtsc_response::RSP_ADDRESS_ERROR);
    }
    else if (status != SIMPLE_BUS_OK) {
      ostringstream oss;
      oss << name() << " - pif2sb_bridge::do_block_read() got simple_bus_status=" << status << " from call to burst_read()";
      throw xtsc_exception(oss.str());
    }
    if (i == num_transfers) response.set_last_transfer(true);
    send_response(response);
  }
}



void pif2sb_bridge::do_rcw(xtsc_response& response) {
  u8  *buffer           = m_request.get_buffer();
  u32  address8         = m_request.get_byte_address();
  u32  size8            = m_request.get_byte_size();
  bool last_transfer    = m_request.get_last_transfer();
  bool respond          = last_transfer;
  // size8 must be 4 and address8 must be 4-byte aligned
  if ((size8 != 4) || ((address8 % 4) != 0)) {
    XTSC_INFO(m_text, "do_rcw() returning RSP_ADDRESS_ERROR: address=0x" << hex << address8 <<
                         " size=" << dec << size8 << " m_pif_width8=" << m_pif_width8);
    response.set_status(xtsc_response::RSP_ADDRESS_ERROR);
  }
  else {
    if (!m_rcw_have_first_transfer && last_transfer) {
      ostringstream oss;
      oss << name() << " - pif2sb_bridge::do_rcw() received a last-transfer RCW without receiving a non-last-transfer RCW";
      throw xtsc_exception(oss.str());
    }
    if (m_rcw_have_first_transfer && !last_transfer) {
      ostringstream oss;
      oss << name() << " - pif2sb_bridge::do_rcw() received two non-last-transfer RCW's";
      throw xtsc_exception(oss.str());
    }
    if (!m_rcw_have_first_transfer) {
      m_rcw_have_first_transfer = true;
      memcpy(m_rcw_compare_data, buffer, 4);
    }
    else {
      u32 data;
      // We want to lock between read and write, but the simple_bus doesn't support this (only locking during
      // a burst).
      simple_bus_status status = m_sb_blocking_port->burst_read(m_priority, (int *)&data, address8, 1, false);
      if (status == SIMPLE_BUS_ERROR) {
        response.set_status(xtsc_response::RSP_ADDRESS_ERROR);
        respond = true;
      }
      else if (status != SIMPLE_BUS_OK) {
        ostringstream oss;
        oss << name() << " - pif2sb_bridge::do_rcw() got simple_bus_status=" << status << " from call to burst_read()";
        throw xtsc_exception(oss.str());
      }
      u8 *response_buffer = response.get_buffer();
      u8 *p_data = (u8*) &data;
      memcpy(response_buffer, &data, 4);
      bool compare_data_matches = true;
      for (u32 i = 0; i<4; i++) {
        if (m_rcw_compare_data[i] != p_data[i]) {
          compare_data_matches = false;
          break;
        }
      }
      if (compare_data_matches) {
        // Test if address8 data has change since we read it.  This artificial check is done because 
        // simple_bus doesn't support locking the bus from the RCW read until the RCW write.
        u32 data2;
        m_sb_direct_port->direct_read((int *)&data2, address8);
        if (data2 != data) {
          XTSC_WARN(m_text, "RCW: data at address=0x" << hex << address8 << " has changed between read and write:");
          XTSC_WARN(m_text, "  (The simple_bus does not support bus locking between read and write)");
        }
        memcpy(&data, buffer, 4);
        simple_bus_status status = m_sb_blocking_port->burst_write(m_priority, (int*)&data, address8, 1, false);
        if (status == SIMPLE_BUS_ERROR) {
          response.set_status(xtsc_response::RSP_ADDRESS_ERROR);
          respond = true;
        }
        else if (status != SIMPLE_BUS_OK) {
          ostringstream oss;
          oss << name() << " - pif2sb_bridge::do_rcw() got simple_bus_status=" << status << " from call to burst_write()";
          throw xtsc_exception(oss.str());
        }
      }
      m_rcw_have_first_transfer = false;
    }
  }
  if (respond) {
    send_response(response);
  }
}



void pif2sb_bridge::do_write(xtsc_response& response) {
  u8  *buffer           = m_request.get_buffer();
  u32  address8         = m_request.get_byte_address();
  u32  size8            = m_request.get_byte_size();
  u32  length           = size8/4;
  XTSC_VERBOSE(m_text, "do_write() address=0x" << hex << address8 << " size8=" << dec << size8);
  if (((size8 != 1) && (size8 != 2) && (size8 != 4) && (size8 != 8) && (size8 != 16)) ||        // not a power of 2   .OR.
      (size8 > m_pif_width8) ||                                                                 // exceeds pif width  .OR.
      ((address8 % size8) != 0))                                                                // not size8-aligned
  {
    XTSC_INFO(m_text, "do_write() returning RSP_ADDRESS_ERROR: address=0x" << hex << address8 <<
                         " size=" << dec << size8 << " m_pif_width8=" << m_pif_width8);
    response.set_status(xtsc_response::RSP_ADDRESS_ERROR);
  }
  else {
    if (size8 < 4) {
      u32 addr8         = address8 & (-4);  // align addr8 to word32 boundary
      u32 offset        = address8 & 0x3;
      u32 data;
      simple_bus_status status = m_sb_blocking_port->burst_read(m_priority, (int*)&data, addr8, 1, false);
      if (status == SIMPLE_BUS_ERROR) {
        response.set_status(xtsc_response::RSP_ADDRESS_ERROR);
      }
      else if (status != SIMPLE_BUS_OK) {
        ostringstream oss;
        oss << name() << " - pif2sb_bridge::do_write() got simple_bus_status=" << status << " from call to burst_read()";
        throw xtsc_exception(oss.str());
      }
      else {
        u8 *p_d = reinterpret_cast<u8*>(&data);
        p_d += offset;
        memcpy(p_d, buffer, size8);
        simple_bus_status status = m_sb_blocking_port->burst_write(m_priority, (int*)&data, addr8, 1, false);
        if (status == SIMPLE_BUS_ERROR) {
          response.set_status(xtsc_response::RSP_ADDRESS_ERROR);
        }
        else if (status != SIMPLE_BUS_OK) {
          ostringstream oss;
          oss << name() << " - pif2sb_bridge::do_write() got simple_bus_status=" << status << " from call to burst_write()";
          throw xtsc_exception(oss.str());
        }
      }
    }
    else {
      int *buf = const_cast<int *>(reinterpret_cast<int*>(buffer));
      simple_bus_status status = m_sb_blocking_port->burst_write(m_priority, buf, address8, length, false);
      if (status == SIMPLE_BUS_ERROR) {
        response.set_status(xtsc_response::RSP_ADDRESS_ERROR);
      }
      else if (status != SIMPLE_BUS_OK) {
        ostringstream oss;
        oss << name() << " - pif2sb_bridge::do_write() got simple_bus_status=" << status << " from call to burst_write()";
        throw xtsc_exception(oss.str());
      }
    }
  }
  send_response(response);
}



void pif2sb_bridge::do_block_write(xtsc_response& response) {
  u8  *buffer           = m_request.get_buffer();
  u32  address8         = m_request.get_byte_address();
  u32  size8            = m_request.get_byte_size();
  u32  length           = size8/4;
  bool respond          = m_request.get_last_transfer();
  XTSC_VERBOSE(m_text, "do_block_write() address=0x" << hex << address8 << " size8=" << dec << size8);
  // size must equal PIF width and address must be PIF width aligned
  if ((size8 != m_pif_width8) || ((address8 % m_pif_width8) != 0)) {
    response.set_status(xtsc_response::RSP_ADDRESS_ERROR);
    respond = true;
  }
  else {
    int *buf = const_cast<int *>(reinterpret_cast<int*>(buffer));
    simple_bus_status status = m_sb_blocking_port->burst_write(m_priority, buf, address8, length, false);
    if (status == SIMPLE_BUS_ERROR) {
      response.set_status(xtsc_response::RSP_ADDRESS_ERROR);
      respond = true;
    }
    else if (status != SIMPLE_BUS_OK) {
      ostringstream oss;
      oss << name() << " - pif2sb_bridge::do_block_write() got simple_bus_status=" << status << " from call to burst_write()";
      throw xtsc_exception(oss.str());
    }
  }
  if (respond) {
    send_response(response);
  }
}



void pif2sb_bridge::xtsc_request_if_impl::nb_request(const xtsc_request& request) {
  XTSC_INFO(m_bridge.m_text, request);
  // Check if we've already got a request
  if (m_bridge.m_busy) {
    xtsc_response response(request, xtsc_response::RSP_NACC, true);
    XTSC_INFO(m_bridge.m_text, response << " (busy)");
    // No need to check return value; nb_respond with RSP_NACC cannot return false
    m_bridge.m_pif_respond_port->nb_respond(response);
    return;
  }
  m_bridge.m_busy = true;
  m_bridge.m_request = request;
  m_bridge.m_worker_thread_event.notify(SC_ZERO_TIME);
}



/*
 * Arbitrary size and alignment are handled by breaking each peek/poke into three parts:
 *   - header
 *   - body
 *   - trailer
 *
 * The following table shows the size of these 3 parts as a function of the 2 least-
 * significant bits of address (Adr) and size.
 *
 * Table entry format is:   header_bytes:body_bytes:trailer_bytes
 * 
 *        Adr    Adr    Adr    Adr
 * size   0x0    0x1    0x2    0x3
 * ----  -----  -----  -----  -----
 * 1     1:0:0  1:0:0  1:0:0  1:0:0
 * 2     2:0:0  2:0:0  2:0:0  1:0:1
 * 3     3:0:0  3:0:0  2:0:1  1:0:2
 * 4     0:4:0  3:0:1  2:0:2  1:0:3
 * 5     0:4:1  3:0:2  2:0:3  1:4:0
 * 6     0:4:2  3:0:3  2:4:0  1:4:1
 * 7     0:4:3  3:4:0  2:4:1  1:4:2
 */
void pif2sb_bridge::xtsc_request_if_impl::nb_peek(xtsc_address address8, u32 size8, u8 *buffer) {
  XTSC_VERBOSE(m_bridge.m_text, "nb_peek addr=0x" << hex << address8 << " size8=0x" << size8);

  u32 header_bytes  = ((address8 & 0x3) ? min(size8, (4 - (address8 & 0x3))) : ((size8 < 4) ? size8 : 0));
  u32 trailer_bytes = (size8 - header_bytes) & 0x3;
  u32 body_bytes    = size8 - header_bytes - trailer_bytes;

  u32 data;
  u32 addr8 = address8;

  if (header_bytes) {
    u32 offset = addr8 & 0x3;
    addr8 = addr8 & (-4);  // align addr8 to word32 boundary
    if (!m_bridge.m_sb_direct_port->direct_read((int *)&data, addr8)) {
      ostringstream oss;
      oss << m_bridge.name() << " (in pif2sb_bridge::nb_peek header_bytes block): direct_read() failed:  address8=0x"
          << hex << address8 << " size8=0x" << size8;
      throw xtsc_exception(oss.str());
    }
    u8 *p_d = reinterpret_cast<u8*>(&data);
    p_d += offset;
    memcpy(buffer, p_d, header_bytes);
    buffer += header_bytes;
    addr8 += 4;
  }

  while (body_bytes) {
    if (!m_bridge.m_sb_direct_port->direct_read((int *)&data, addr8)) {
      ostringstream oss;
      oss << m_bridge.name() << " (in pif2sb_bridge::nb_peek body_bytes block): direct_read() failed:  address8=0x"
          << hex << address8 << " size8=0x" << size8;
      throw xtsc_exception(oss.str());
    }
    memcpy(buffer, &data, 4);
    buffer += 4;
    addr8 += 4;
    body_bytes -= 4;
  }

  if (trailer_bytes) {
    if (!m_bridge.m_sb_direct_port->direct_read((int *)&data, addr8)) {
      ostringstream oss;
      oss << m_bridge.name() << " (in pif2sb_bridge::nb_peek trailer_bytes block): direct_read() failed:  address8=0x"
          << hex << address8 << " size8=0x" << size8;
      throw xtsc_exception(oss.str());
    }
    memcpy(buffer, &data, trailer_bytes);
    buffer += trailer_bytes;
    addr8 += 4;
  }

}



// See comments above the nb_peek method
void pif2sb_bridge::xtsc_request_if_impl::nb_poke(xtsc_address address8, u32 size8, const u8 *buffer) {
  XTSC_VERBOSE(m_bridge.m_text, "nb_poke addr=0x" << hex << address8 << " size8=0x" << size8);

  u32 header_bytes  = ((address8 & 0x3) ? min(size8, (4 - (address8 & 0x3))) : ((size8 < 4) ? size8 : 0));
  u32 trailer_bytes = (size8 - header_bytes) & 0x3;
  u32 body_bytes    = size8 - header_bytes - trailer_bytes;

  u32 data;
  u32 addr8 = address8;

  if (header_bytes) {
    u32 offset = addr8 & 0x3;
    addr8 = addr8 & (-4);  // align addr8 to word32 boundary
    nb_peek(addr8, 4, (u8*)&data);
    u8 *p_d = reinterpret_cast<u8*>(&data);
    p_d += offset;
    memcpy(p_d, buffer, header_bytes);
    if (!m_bridge.m_sb_direct_port->direct_write((int *)&data, addr8)) {
      ostringstream oss;
      oss << m_bridge.name() << " (in pif2sb_bridge::nb_poke header_bytes block): direct_write() failed:  address8=0x"
          << hex << address8 << " size8=0x" << size8;
      throw xtsc_exception(oss.str());
    }
    buffer += header_bytes;
    addr8 += 4;
  }

  while (body_bytes) {
    memcpy(&data, buffer, 4);
    if (!m_bridge.m_sb_direct_port->direct_write((int *)&data, addr8)) {
      ostringstream oss;
      oss << m_bridge.name() << " (in pif2sb_bridge::nb_poke body_bytes block): direct_write() failed:  address8=0x"
          << hex << address8 << " size8=0x" << size8;
      throw xtsc_exception(oss.str());
    }
    buffer += 4;
    addr8 += 4;
    body_bytes -= 4;
  }

  if (trailer_bytes) {
    nb_peek(addr8, 4, (u8*)&data);
    memcpy(&data, buffer, trailer_bytes);
    if (!m_bridge.m_sb_direct_port->direct_write((int *)&data, addr8)) {
      ostringstream oss;
      oss << m_bridge.name() << " (in pif2sb_bridge::nb_poke trailer_bytes block): direct_write() failed:  address8=0x"
          << hex << address8 << " size8=0x" << size8;
      throw xtsc_exception(oss.str());
    }
    buffer += trailer_bytes;
    addr8 += 4;
  }


}

/* implement fast access data transfers with peeks and pokes */
bool pif2sb_bridge::xtsc_request_if_impl::nb_fast_access(xtsc_fast_access_request &request) {
  xtsc_address address8 = request.get_translated_request_address();
  XTSC_VERBOSE(m_bridge.m_text, "nb_fast_access addr=0x" << hex << address8);
  request.allow_peek_poke_access();
  return true;
}



void pif2sb_bridge::xtsc_request_if_impl::register_port(sc_port_base& port, const char *if_typename) {
  if (m_p_port) {
    ostringstream oss;
    oss << "Illegal multiple binding detected to pif2sb_bridge '" << m_bridge.name() << "' m_pif_request_export: " << endl;
    oss << "  " << port.name() << endl;
    oss << "  " << m_p_port->name();
    throw xtsc_exception(oss.str());
  }
  XTSC_INFO(m_bridge.m_text, "Binding '" << port.name() << "' to pif2sb_bridge::m_pif_request_export");
  m_p_port = &port;
}



