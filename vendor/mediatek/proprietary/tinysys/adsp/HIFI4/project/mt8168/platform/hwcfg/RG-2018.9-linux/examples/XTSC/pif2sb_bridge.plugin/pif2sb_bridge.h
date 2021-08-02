#ifndef _PIF2SB_BRIDGE_H_
#define _PIF2SB_BRIDGE_H_

// Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2012 by Tensilica Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.

/**
 * @file 
 */


#include <xtsc/xtsc.h>
#include <xtsc/xtsc_parms.h>
#include <xtsc/xtsc_request_if.h>
#include <xtsc/xtsc_respond_if.h>
#include <xtsc/xtsc_request.h>
#include <xtsc/xtsc_response.h>
#include <xtsc/xtsc_fast_access.h>
#include <vector>
#include <cstring>
#include "simple_bus/simple_bus_direct_if.h"
#include "simple_bus/simple_bus_blocking_if.h"






/**
 * Constructor parameters for a pif2sb_bridge object.
 *
 *  \verbatim
   Name                  Type   Description
   ------------------    ----   -------------------------------------------------------
  
   "pif_byte_width"       u32   PIF width in bytes.  Valid values are
                                4, 8, and 16.
  
   "unique_priority"     u32    The bridge' unique priority on the simple_bus.

   "clock_period"        u32    This is the length of this bridge's clock period
                                expressed in terms of the SystemC time resolution
                                (from sc_get_time_resolution()).  A value of 
                                0xFFFFFFFF means to use the XTSC system clock 
                                period (from xtsc_get_system_clock_period()).  A value
                                of 0 means one delta cycle.
                                Default = 0xFFFFFFFF (i.e. use the system clock 
                                period).

    \endverbatim
 *
 * @see pif2sb_bridge
 * @see xtsc::xtsc_parms
 */
class pif2sb_bridge_parms : public xtsc::xtsc_parms {
public:

  /**
   * Constructor for an pif2sb_bridge_parms object.
   *
   * @param width8              Memory data bus width in bytes.
   *
   * @param unique_priority     The bridge's unique priority on the simple_bus.
   *
   * @param clock_period        The bridge's clock period.
   *
   */
  pif2sb_bridge_parms(xtsc::u32 width8 = 4, xtsc::u32 unique_priority = 0, xtsc::u32 clock_period = 0xFFFFFFFF) {
    add("pif_byte_width",   width8);
    add("unique_priority",  unique_priority);
    add("clock_period",     clock_period);
  }


  /// Return what kind of xtsc_parms this is (our C++ type)
  virtual const char* kind() const { return "pif2sb_bridge_parms"; }

};





/**
 * Example device implementing a PIF-to-simple_bus bridge.
 *
 * For protocol and timing information specific to xtsc_core, see
 * xtsc::xtsc_core::Information_on_memory_interface_protocols.
 *
 *
 * @see pif2sb_bridge_parms
 * @see xtsc::xtsc_request
 * @see xtsc::xtsc_respond
 * @see xtsc::xtsc_request_if
 * @see xtsc::xtsc_respond_if
 * @see xtsc::xtsc_core
 * @see xtsc::xtsc_core::Information_on_memory_interface_protocols.
 */
class pif2sb_bridge : public sc_core::sc_module {
public:

  sc_core::sc_export<xtsc::xtsc_request_if>     m_pif_request_export;   ///<  From PIF master to us
  sc_core::sc_port  <xtsc::xtsc_respond_if>     m_pif_respond_port;     ///<  From us to PIF master
  sc_core::sc_port  <simple_bus_direct_if>      m_sb_direct_port;       ///<  From us to simple_bus (peek/poke)
  sc_core::sc_port  <simple_bus_blocking_if>    m_sb_blocking_port;     ///<  From us to simple_bus (requests)


  // For SystemC
  SC_HAS_PROCESS(pif2sb_bridge);

  /// Our C++ type (SystemC uses this)
  virtual const char* kind() const { return "pif2sb_bridge"; }


  /**
   * Constructor for an pif2sb_bridge.
   * @param     module_name     Name of the pif2sb_bridge sc_module.
   * @param     bridge_parms    The remaining parameters for construction.
   * @see pif2sb_bridge_parms
   */
  pif2sb_bridge(sc_core::sc_module_name module_name, const pif2sb_bridge_parms& bridge_parms);

  // Destructor.
  ~pif2sb_bridge(void);

  /// Get the TextLogger for this component (e.g. to adjust its log level)
  log4xtensa::TextLogger& get_text_logger() { return m_text; }



private:

  /// Implementation of xtsc_request_if.
  class xtsc_request_if_impl : public xtsc::xtsc_request_if, public sc_core::sc_object  {
  public:

    /// Constructor
    xtsc_request_if_impl(const char *object_name, pif2sb_bridge& bridge) :
      sc_object (object_name),
      m_bridge  (bridge),
      m_p_port  (0)
    {}

    /// From PIF master
    /// @see xtsc::xtsc_request_if
    virtual void nb_peek(xtsc::xtsc_address address8, xtsc::u32 size8, xtsc::u8 *buffer);


    /// From PIF master
    /// @see xtsc::xtsc_request_if
    virtual void nb_poke(xtsc::xtsc_address address8, xtsc::u32 size8, const xtsc::u8 *buffer);


    /// From PIF master. Use peek and poke for fast access
    /// @see xtsc::xtsc_request_if
    virtual bool nb_fast_access(xtsc::xtsc_fast_access_request &request);
    

    /// From PIF master
    /// @see xtsc::xtsc_request_if
    void nb_request(const xtsc::xtsc_request& request);


  protected:

    /// SystemC callback when something binds to us
    virtual void register_port(sc_core::sc_port_base& port, const char *if_typename);

    pif2sb_bridge&              m_bridge;       ///< Our pif2sb_bridge object
    sc_core::sc_port_base      *m_p_port;       ///< Port that is bound to us
  };


  virtual void worker_thread(void);                     ///<  Handle incoming requests from single master at the correct time

  void send_response (xtsc::xtsc_response& response);   ///<  Common method for sending a response

  void do_read       (xtsc::xtsc_response& response);   ///<  Handle READ
  void do_block_read (xtsc::xtsc_response& response);   ///<  Handle BLOCK_READ
  void do_rcw        (xtsc::xtsc_response& response);   ///<  Handle RCW
  void do_write      (xtsc::xtsc_response& response);   ///<  Handle WRITE
  void do_block_write(xtsc::xtsc_response& response);   ///<  Handle BLOCK_WRITE


  xtsc_request_if_impl                  m_request_impl;                 ///<  m_pif_request_export binds to this
  xtsc::u32                             m_pif_width8;                   ///<  The PIF width in bytes
  xtsc::u32                             m_priority;                     ///<  This bridge's unique priority on the simple_bus
  sc_core::sc_time                      m_clock_period;                 ///<  This bridge's clock period
  sc_core::sc_event                     m_worker_thread_event;          ///<  To notify worker_thread when a request is accepted
  xtsc::xtsc_request                    m_request;                      ///<  Our copy of current request
  bool                                  m_busy;                         ///<  True when we have a current request
  bool                                  m_rcw_have_first_transfer;      ///<  True when we've rec'd 1st xfer of RCW but not 2nd
  xtsc::u8                              m_rcw_compare_data[4];          ///<  Comparison data from RCW request
  log4xtensa::TextLogger&               m_text;                         ///<  Text logger

};



#endif  // _PIF2SB_BRIDGE_H_
