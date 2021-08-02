// Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2011 by Tensilica Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.


#include <iostream>
#include <xtsc/xtsc_response.h>
#include <xtsc/xtsc_fast_access.h>
#include "simple_memory.h"

using namespace std;
using namespace sc_core;
using namespace log4xtensa;
using namespace xtsc;



xtsc_component::simple_memory::simple_memory(sc_module_name module_name, const simple_memory_parms& memory_parms) :
  sc_module             (module_name),
  m_request_export      ("m_request_export"),
  m_respond_port        ("m_respond_port"),
  m_request_impl        ("m_request_impl", *this),
  m_text                (TextLogger::getInstance(name()))
{

  // Initialize remaining members
  m_busy                = false;
  m_start_address8      = memory_parms.get_u32("start_byte_address");
  m_size8               = memory_parms.get_u32("memory_byte_size");
  m_width8              = memory_parms.get_u32("byte_width");
  m_fill_byte           = memory_parms.get_int("fill_byte");
  m_end_address8        = m_start_address8 + m_size8 - 1;
  m_mem                 = new u8[m_size8];

  for (u32 i=0; i<m_size8; i++) {
    m_mem[i] = static_cast<u8>(m_fill_byte && 0xFF);
  }

  u32 bad_data = 0xbad1bad1;
  memcpy(m_rcw_data, &bad_data, 4);

  // Get the response time as a u32 and save it as an sc_time
  u32 response_time = memory_parms.get_u32("response_time");
  m_response_time = response_time * sc_get_time_resolution();

  // Get the repeat time as a u32 and save it as an sc_time
  u32 repeat_time = memory_parms.get_u32("repeat_time");
  m_repeat_time = repeat_time * sc_get_time_resolution();

  // Tell SystemC to run this funtion in a SystemC thread process
  SC_THREAD(response_thread);

  // Bind the export to the implementation
  m_request_export(m_request_impl);

  // Log our construction
  XTSC_INFO(m_text, "Constructed simple_memory '" << name() << "':");
  XTSC_INFO(m_text, " byte_width         =   " << m_width8);
  XTSC_INFO(m_text, " start_byte_address = 0x" << hex << m_start_address8);
  XTSC_INFO(m_text, " memory_byte_size   = 0x" << hex << m_size8);
  XTSC_INFO(m_text, " fill_byte          =   " << m_fill_byte);
  XTSC_INFO(m_text, " response_time      =   " << response_time << " (" << m_response_time << ")");
  XTSC_INFO(m_text, " repeat_time        =   " << repeat_time << " (" << m_repeat_time << ")");
}



xtsc_component::simple_memory::~simple_memory(void) {
  // Do any required clean-up here
  delete [] m_mem;
}



void xtsc_component::simple_memory::validate_address_range(const char *type, xtsc_address address8, u32 size8) {
  xtsc_address end_address8 = address8 + size8 - 1;
  if ((address8 < m_start_address8) || (end_address8 > m_end_address8) || (end_address8 < m_start_address8)) {
    ostringstream oss;
    oss << "simple_memory '" << name() << "' received out-of-range " << type << " address." << endl;
    oss << " address=0x" << hex << address8 << " size=0x" << size8 << endl;
    oss << " Configured \"start_byte_address\"=0x" << m_start_address8 << " \"memory_byte_size\"=0x" << m_size8;
    throw xtsc_exception(oss.str());
  }
}



bool xtsc_component::simple_memory::has_address_error(xtsc_address address8, u32 size8, bool block, bool rcw) {
  // address must be size-aligned for all types
  if ((address8 % size8) != 0) return true;
  if (block) {
    // size must match bus width for BLOCK_WRITE and BLOCK_READ
    if (size8 != m_width8) return true;
  }
  else if (rcw) {
    // size must be 4 for RCW
    if (size8 != 4) return true;
  }
  else {
    // size must be a power of two for READ and WRITE
    if ((size8 != 1) && (size8 != 2) && (size8 != 4) && (size8 != 8) && (size8 != 16) && (size8 != 32) && (size8 != 64)) return true;
    // size may not exceed bus width 
    if (size8 > m_width8) return true;
  }
  return false;
}



void xtsc_component::simple_memory::xtsc_request_if_impl::nb_peek(xtsc_address address8, u32 size8, u8 *buffer) {
  // Log the peek
  XTSC_VERBOSE(m_memory.m_text, "nb_peek: address=0x" << hex << address8 << " size=" << dec << size8);

  m_memory.validate_address_range("nb_peek", address8, size8);
  u8 *pmem = &m_memory.m_mem[address8 - m_memory.m_start_address8];
  memcpy(buffer, pmem, size8);
}



void xtsc_component::simple_memory::xtsc_request_if_impl::nb_poke(xtsc_address address8, u32 size8, const u8 *buffer) {
  // Log the call
  XTSC_VERBOSE(m_memory.m_text, "nb_poke: address=0x" << hex << address8 << " size=" << dec << size8);

  m_memory.validate_address_range("nb_poke", address8, size8);
  u8 *pmem = &m_memory.m_mem[address8 - m_memory.m_start_address8];
  memcpy(pmem, buffer, size8);
}



bool xtsc_component::simple_memory::xtsc_request_if_impl::nb_fast_access(xtsc_fast_access_request &request) {
  // Log the call
  XTSC_VERBOSE(m_memory.m_text, "nb_fast_access: using peek/poke");

  request.allow_peek_poke_access();
  return true;
}



void xtsc_component::simple_memory::xtsc_request_if_impl::nb_request(const xtsc_request& request) {
  // Log the request
  XTSC_INFO(m_memory.m_text, request);

  // Can we accept the request at this time?
  if (m_memory.m_busy) {
    // No. We're already busy.  Create an RSP_NACC response.
    xtsc_response response(request, xtsc_response::RSP_NACC, true);
    // Log the response
    XTSC_INFO(m_memory.m_text, response);
    // Send the response
    m_memory.m_respond_port->nb_respond(response);
  }
  else {
    // Yes.  We accept this request, so now we're busy.
    m_memory.m_busy = true;
    // Create our copy of the request
    m_memory.m_active_request = request;
    // Notify response_thread
    m_memory.m_response_event.notify(m_memory.m_response_time);
  }
}



void xtsc_component::simple_memory::xtsc_request_if_impl::register_port(sc_port_base& port, const char *if_typename) {
  if (m_p_port) {
    ostringstream oss;
    oss << "Illegal multiple binding detected to simple_memory '" << m_memory.name() << "' m_request_export: " << endl;
    oss << "  " << port.name() << endl;
    oss << "  " << m_p_port->name();
    throw xtsc_exception(oss.str());
  }
  XTSC_INFO(m_memory.m_text, "Binding '" << port.name() << "' to '" << m_memory.name() << ".m_request_export'");
  m_p_port = &port;
}



void xtsc_component::simple_memory::response_thread(void) {

  // A try/catch block in sc_main will not catch an exception thrown from
  // an SC_THREAD, so we'll catch them here, log them, then rethrow them.
  try {

    // Loop forever
    while (true) {

      // Tell nb_request that we're no longer busy
      m_busy = false;

      // Wait for nb_request to tell us there's something to do 
      wait(m_response_event);

      // Create response from request
      xtsc_response response(m_active_request, xtsc_response::RSP_OK);

      // Extract commonly-needed values
      xtsc_address address8 = m_active_request.get_byte_address();
      u32          size8    = m_active_request.get_byte_size();

      // Handle request according to its type
      switch (m_active_request.get_type()) {

        // READ: validate then send response with data 
        case xtsc_request::READ: {
          validate_address_range("READ", address8, size8);
          if (has_address_error(address8, size8, false, false)) {
            response.set_status(xtsc_response::RSP_ADDRESS_ERROR);
          }
          else {
            u8 *pmem = &m_mem[address8 - m_start_address8];
            u8 *pbuf = response.get_buffer();
            memcpy(pbuf, pmem, size8);
          }
          send_response(response);
          break;
        }

        // BLOCK_READ: validate then send response with data for each transfer
        case xtsc_request::BLOCK_READ: {
          u32 num_transfers = m_active_request.get_num_transfers();
          u32 total_size = size8 * num_transfers;
          xtsc_address aligned_address = address8 & ~(((xtsc_address) total_size) - (xtsc_address) 1);
          validate_address_range("BLOCK_READ", aligned_address, total_size);
          if (has_address_error(address8, size8, true, false)) {
            response.set_status(xtsc_response::RSP_ADDRESS_ERROR);
            send_response(response);
          }
          else {
            u32 index = address8 - m_start_address8;
            u32 unalignment = (address8 - aligned_address) / size8;
            for (u32 i=1; i<=num_transfers; ++i, index+=m_width8, unalignment+=1) {
              if (unalignment == num_transfers) {
                index = aligned_address - m_start_address8;
              }
              u8 *pbuf = response.get_buffer();
              u8 *pmem = &m_mem[index];
              memcpy(pbuf, pmem, m_width8);
              bool last = (i == num_transfers);
              response.set_last_transfer(last);
              send_response(response);
              if (!last) {
                wait(m_repeat_time);
              }
            }
          }
          break;
        }

        // RCW: validate. If 1st RCW: save value and don't send a response.
        //                If 2nd RCW: Return original mem data.  If mem
        //                data matches 1st RCW data then replace mem data.
        case xtsc_request::RCW: {
          validate_address_range("RCW", address8, size8);
          if (has_address_error(address8, size8, false, true)) {
            response.set_status(xtsc_response::RSP_ADDRESS_ERROR);
            send_response(response);
          }
          else {
            // Is this 2nd transfer?
            if (m_active_request.get_last_transfer()) {
              // 2nd transfer. Return data currently at that address
              u8 *prspbuf = response.get_buffer();
              memcpy(prspbuf, m_rcw_data, 4);
              // If 1st transfer matched memory, then update memory
              // with data from 2nd transfer 
              u8 *pmem = &m_mem[address8 - m_start_address8];
              if (memcmp(m_rcw_data, pmem, 4) == 0) {
                u8 *preqbuf = m_active_request.get_buffer();
                memcpy(pmem, preqbuf, 4);
              }
              // Always respond on 2nd transfer
              send_response(response);
            }
            else {
              // 1st transfer. Just save the data for later comparison
              u8 *preqbuf = m_active_request.get_buffer();
              memcpy(m_rcw_data, preqbuf, 4);
            }
          }
          break;
        }

        // WRITE: validate, update memory, send response
        case xtsc_request::WRITE: {
          validate_address_range("WRITE", address8, size8);
          if (has_address_error(address8, size8, false, false)) {
            response.set_status(xtsc_response::RSP_ADDRESS_ERROR);
          }
          else {
            u8 *pmem = &m_mem[address8 - m_start_address8];
            const u8 *pbuf = m_active_request.get_buffer();
            xtsc_byte_enables bytes = m_active_request.get_byte_enables();
            u32 mem_offset  = 0;
            u32 buf_offset  = 0;
            for (u32 i = 0; i<size8; ++i) {
              if (bytes & 0x1) {
                pmem[mem_offset] = pbuf[buf_offset];
              }
              bytes >>= 1;
              mem_offset += 1;
              buf_offset += 1;
            }
          }
          send_response(response);
          break;
        }

        // BLOCK_WRITE: validate and update memory. If last transfer send response.
        case xtsc_request::BLOCK_WRITE: {
          validate_address_range("BLOCK_WRITE", address8, size8);
          if (has_address_error(address8, size8, true, false)) {
            response.set_status(xtsc_response::RSP_ADDRESS_ERROR);
            send_response(response);
          }
          else {
            u8 *pmem = &m_mem[address8 - m_start_address8];
            const u8 *pbuf = m_active_request.get_buffer();
            memcpy(pmem, pbuf, size8);
            if (m_active_request.get_last_transfer()) {
              send_response(response);
            }
          }
          break;
        }

        // We covered all the cases, but just in case . . .
        default: {
          ostringstream oss;
          oss << "Unsupported request type=" << m_active_request.get_type_name();
          throw xtsc_exception(oss.str());
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



void xtsc_component::simple_memory::send_response(xtsc_response &response) {
  // Log the response
  XTSC_INFO(m_text, response);
  
  // Send the response until it is accepted
  while (!m_respond_port->nb_respond(response)) {
    XTSC_INFO(m_text, response << " <-- REJECTED");
    wait(m_repeat_time);
  }
}



