#ifndef _SIMPLE_MEMORY_H_
#define _SIMPLE_MEMORY_H_

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
 * Constructor parameters for a simple_memory object.
 *
 *  \verbatim
   Name                  Type   Description
   ------------------    ----   --------------------------------------------------------
  
   "byte_width"           u32   Memory data interface width in bytes.  Valid values are
                                4, 8, 16, 32, and 64.
  
   "memory_byte_size"     u32   The byte size of this memory.  
  
   "start_byte_address"   u32   The starting byte address of this memory in the 4GB
                                address space.
  
   "response_time"        u32   This is the length of this memory's response time
                                expressed in terms of the SystemC time resolution (from
                                sc_get_time_resolution()).  A value of 0 means one delta
                                cycle. This response time is the time from receiving a
                                request until the response_thread will be woken up to
                                handle it.

   "repeat_time"          u32   This is the length of this memory's repeat time
                                expressed in terms of the SystemC time resolution (from
                                sc_get_time_resolution()).  A value of 0 means one delta
                                cycle. This repeat time is the time between multiple 
                                BLOCK_READ responses and also the time between repeats of
                                the same response when the master reject the first response.

   "fill_byte"            int   The initial value of memory.

    \endverbatim
 *
 * @see simple_memory
 * @see xtsc::xtsc_parms
 */
class simple_memory_parms : public xtsc::xtsc_parms {
public:

  /**
   * Constructor for an simple_memory_parms object.
   *
   * @param width8              Memory data interface width in bytes.
   *
   * @param size8               The byte size of this memory.  
   *
   * @param start_address8      The starting byte address of this memory.
   *
   * @param response_time       The length of this memory's response time expressed
   *                            in terms of the SystemC time resoluton.
   *
   * @param repeat_time         The length of this memory's repeat time expressed
   *                            in terms of the SystemC time resoluton.
   *
   * @param fill_byte           The initial value of memory.
   *
   */
  simple_memory_parms(xtsc::u32 width8,
                      xtsc::u32 size8,
                      xtsc::u32 start_address8  = 0,
                      xtsc::u32 response_time   = 500,
                      xtsc::u32 repeat_time     = 1000,
                      int       fill_byte       = 0)
  {
    add("byte_width",           width8);
    add("start_byte_address",   start_address8);
    add("memory_byte_size",     size8);
    add("fill_byte",            fill_byte);
    add("response_time",        response_time);
    add("repeat_time",          repeat_time);
  }

  /// Our C++ type (the xtsc_parms base class uses this for error messages)
  virtual const char* kind() const { return "simple_memory_parms"; }

};





/**
 * A simple memory device.
 *
 * This simple memory device only accepts one request at a time and only works
 * on little-endian host systems (that is on x86 but not solaris).
 *
 */
class simple_memory : public sc_core::sc_module {
public:

  /// From master to us
  sc_core::sc_export<xtsc::xtsc_request_if>     m_request_export;

  /// From us to master
  sc_core::sc_port  <xtsc::xtsc_respond_if>     m_respond_port;

  /// This SystemC macro inserts some code required for SC_THREAD's to work
  SC_HAS_PROCESS(simple_memory);

  /// Our C++ type (SystemC uses this)
  virtual const char* kind() const { return "simple_memory"; }


  /**
   * Constructor for a simple_memory.
   *
   * @param     module_name     Name of the simple_memory sc_module.
   * @param     memory_parms    The remaining parameters for construction.
   *
   * @see simple_memory_parms
   */
  simple_memory(sc_core::sc_module_name module_name, const simple_memory_parms& memory_parms);

  /// Destructor.
  ~simple_memory(void);


protected:

  /**
   * Helper method which will throw an exception if the specifed address range is not
   * within the memory's aperture.
   * @param     type            A string identifying the method ("nb_peek"/"nb_poke") or
   *                            transaction type ("READ", "BLOCK_READ", etc.) being
   *                            validated.
   * @param     address8        The byte address.
   * @param     size8           The byte size.
   */
  void validate_address_range(const char *type, xtsc::xtsc_address address8, xtsc::u32 size8);

  /**
   * Return true if address8 or size8 are invalid so that the response should be RSP_ERROR_ADDRESS.
   *
   * @param     address8        The byte address
   * @param     size8           The byte size
   * @param     block           True if BLOCK_READ or BLOCK_WRITE, otherwise false.
   * @param     rcw             True if RCW, otherwise false
   */
  bool has_address_error(xtsc::xtsc_address address8, xtsc::u32 size8, bool block, bool rcw);

  /// Send responses to the master
  void response_thread(void);


  /// Implementation of xtsc_request_if.
  class xtsc_request_if_impl : public xtsc::xtsc_request_if, public sc_core::sc_object {
  public:

    /// Constructor
    xtsc_request_if_impl(const char *object_name, simple_memory& memory) :
      sc_object (object_name),
      m_memory  (memory),
      m_p_port  (0)
    {}

    /// The kind of sc_object we are
    const char* kind() const { return "simple_memory::xtsc_request_if_impl"; }

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

    simple_memory&              m_memory;       ///< Our simple_memory object
    sc_core::sc_port_base      *m_p_port;       ///< Port that is bound to us
  };


  /// Send and log a response
  void send_response(xtsc::xtsc_response &response);


  xtsc_request_if_impl                  m_request_impl;         ///< m_request_export binds to this

  log4xtensa::TextLogger&               m_text;                 ///< Used for logging 
  xtsc::xtsc_address                    m_start_address8;       ///< The starting byte address of this memory
  xtsc::u32                             m_size8;                ///< The byte size of this memory
  xtsc::u32                             m_width8;               ///< The byte width of this memories data interface
  xtsc::u8                             *m_mem;                  ///< The memory array
  xtsc::u8                              m_rcw_data[4];          ///< Save RCW buffer from 1st RCW transfer
  xtsc::xtsc_address                    m_end_address8;         ///< The ending byte address of this memory
  int                                   m_fill_byte;            ///< The value with which to fill the peek/read buffer
  xtsc::xtsc_request                    m_active_request;       ///< Our copy of the active (current) request
  bool                                  m_busy;                 ///< We can only accept one request at a time
  sc_core::sc_time                      m_response_time;        ///< How long to take to initially respond
  sc_core::sc_time                      m_repeat_time;          ///< How long to take to repeat the respond and for multiple responses
  sc_core::sc_event                     m_response_event;       ///< Event used to notify response_thread

};



}  // namespace xtsc_component


#endif // _SIMPLE_MEMORY_H_
