#ifndef _TEMPLATE_MEMORY_H_
#define _TEMPLATE_MEMORY_H_

// Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2010 by Tensilica Inc.  ALL RIGHTS RESERVED.
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




namespace xtsc_component {


/**
 * Constructor parameters for a template_memory object.
 *
 *  \verbatim
   Name                 Type    Description
   ------------------   ----    --------------------------------------------------------
  
   "byte_width"           u32   Memory data interface width in bytes.  Valid values are
                                4, 8, 16, 32, and 64.
  
   "fill_byte"            int   The value with which to fill the peek and read buffer.

   "response_time"        u32   This is the length of this memory's response time
                                expressed in terms of the SystemC time resolution (from
                                sc_get_time_resolution()).  A value of 0 means one delta
                                cycle.

    \endverbatim
 *
 * @see template_memory
 * @see xtsc::xtsc_parms
 */
class template_memory_parms : public xtsc::xtsc_parms {
public:

  /**
   * Constructor for an template_memory_parms object.
   *
   * @param width8              Memory data interface width in bytes.
   *
   * @param response_time       The length of this memory's response time.
   *
   */
  template_memory_parms(xtsc::u32 width8, int fill_byte, xtsc::u32 response_time) {
    add("byte_width",    width8);
    add("fill_byte",     fill_byte);
    add("response_time", response_time);
  }

  /// Our C++ type (the xtsc_parms base class uses this for error messages)
  virtual const char* kind() const { return "template_memory_parms"; }

};





/**
 * A null memory device.
 *
 * This null device replies to all peeks and reads with a constant payload and
 * swallows all poke and write payloads without doing anything.  It doesn't do any
 * useful work but it shows the essential code skeleton of a memory device for
 * illustrative purposes and it is suitable as a starting point for developing custom
 * memory devices.
 *
 */
class template_memory : public sc_core::sc_module {
public:

  /// From master to us
  sc_core::sc_export<xtsc::xtsc_request_if>     m_request_export;

  /// From us to master
  sc_core::sc_port  <xtsc::xtsc_respond_if>     m_respond_port;

  /// This SystemC macro inserts some code required for SC_THREAD's to work
  SC_HAS_PROCESS(template_memory);

  /// Our C++ type (SystemC uses this)
  virtual const char* kind() const { return "template_memory"; }


  /**
   * Constructor for a template_memory.
   *
   * @param     module_name     Name of the template_memory sc_module.
   * @param     memory_parms    The remaining parameters for construction.
   *
   * @see template_memory_parms
   */
  template_memory(sc_core::sc_module_name module_name, const template_memory_parms& memory_parms);

  /// Destructor.
  ~template_memory(void);


protected:

  /// Send responses to the master
  void response_thread(void);


  /// Implementation of xtsc_request_if.
  class xtsc_request_if_impl : public xtsc::xtsc_request_if, public sc_core::sc_object {
  public:

    /// Constructor
    xtsc_request_if_impl(const char *object_name, template_memory& memory) :
      sc_object (object_name),
      m_memory  (memory),
      m_p_port  (0)
    {}

    /// The kind of sc_object we are
    const char* kind() const { return "template_memory::xtsc_request_if_impl"; }

    /**
     *  Receive peeks from the master
     *  @see xtsc::xtsc_request_if
     */
    void nb_peek(xtsc::xtsc_address address8, xtsc::u32 size8, xtsc::u8 *buffer);

    /**
     *  Receive pokes from the master
     *  @see xtsc::xtsc_request_if
     */
    void nb_poke(xtsc::xtsc_address address8, xtsc::u32 size8, const xtsc::u8 *buffer);

    /**
     *  Receive fast access requests from the master
     *  @see xtsc::xtsc_request_if
     */
    bool nb_fast_access(xtsc::xtsc_fast_access_request &request);

    /**
     *  Receive requests from the master
     *  @see xtsc::xtsc_request_if
     */
    void nb_request(const xtsc::xtsc_request& request);


  protected:

    /// SystemC callback when something binds to us
    void register_port(sc_core::sc_port_base& port, const char *if_typename);

    template_memory&            m_memory;       ///< Our template_memory object
    sc_core::sc_port_base      *m_p_port;       ///< Port that is bound to us
  };


  /// Send and log a response
  void send_response(xtsc::xtsc_response &response);


  xtsc_request_if_impl                  m_request_impl;         ///< m_request_export binds to this

  log4xtensa::TextLogger&               m_text;                 ///< Used for logging 
  xtsc::u32                             m_width8;               ///< The byte width of this memories data interface
  int                                   m_fill_byte;            ///< The value with which to fill the peek/read buffer
  xtsc::xtsc_request                    m_active_request;       ///< Our copy of the active (current) request
  bool                                  m_busy;                 ///< We can only accept one request at a time
  sc_core::sc_time                      m_response_time;        ///< How long to take to respond
  sc_core::sc_event                     m_response_event;       ///< Event used to notify response_thread

};



}  // namespace xtsc_component


#endif // _TEMPLATE_MEMORY_H_
