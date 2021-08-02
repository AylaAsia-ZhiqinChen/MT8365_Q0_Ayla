#ifndef _TEMPLATE_QUEUE_PUSH_H_
#define _TEMPLATE_QUEUE_PUSH_H_

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
#include <xtsc/xtsc_queue_push_if.h>


namespace xtsc_component {



/**
 * Constructor parameters for an template_queue_push object.
 *
 * This class contains the constructor parameters for an template_queue_push object.
 * \verbatim
   Name                 Type    Description
   ------------------   ----    --------------------------------------------------
  
   "bit_width"          u32     Width of each queue element in bits.

   \endverbatim
 *
 *
 * @see template_queue_push
 * @see xtsc::xtsc_parms
 */
class template_queue_push_parms : public xtsc::xtsc_parms {
public:

  /**
   * Constructor for an template_queue_push_parms object.
   *
   * @param     width1          Width of each queue element in bits.
   *
   */
  template_queue_push_parms(xtsc::u32 width1) {
    add("bit_width",            width1);
  }

  /// Our C++ type (the xtsc_parms base class uses this for error messages)
  virtual const char* kind() const { return "template_queue_push_parms"; }
};








/**
 * A null xtsc_queue_push_if implementation.
 *
 * This null device can be connected to a TIE output queue interface.  It accepts all
 * nb_push attempts and simply logs and discards the data.
 *
 * @see template_queue_push_parms
 * @see xtsc::xtsc_queue_push_if
 */
class template_queue_push : public sc_core::sc_module {
public:

  sc_core::sc_export<xtsc::xtsc_queue_push_if>  m_producer;     ///<  Producer binds to this


  /// Our C++ type (SystemC uses this)
  virtual const char* kind() const { return "template_queue_push"; }


  /**
   * Constructor for an template_queue_push.
   *
   * @param     module_name     Name of the template_queue_push sc_module.
   *
   * @param     queue_parms     The remaining parameters for construction.
   *
   * @see template_queue_push_parms
   */
  template_queue_push(sc_core::sc_module_name module_name, const template_queue_push_parms& queue_parms);


  /// Destructor.
  ~template_queue_push();


protected:

  /// Implementation of xtsc_queue_push_if.
  class xtsc_queue_push_if_impl : public xtsc::xtsc_queue_push_if, public sc_core::sc_object  {
  public:

    /// Constructor
    xtsc_queue_push_if_impl(const char *object_name, template_queue_push& queue) :
      sc_object (object_name),
      m_queue   (queue),
      m_p_port  (0)
    {}

    /**
     * This method is used to determine if the queue can accept another element.
     * @see xtsc::xtsc_queue_push_if
     */
    bool nb_can_push();

    /**
     * This method is used to add an element to the queue.  
     * @see xtsc::xtsc_queue_push_if
     */
    bool nb_push(const sc_dt::sc_unsigned& element, xtsc::u64& ticket = push_ticket);

    /**
     * Returns the width (in units of bits) of the elements that this device
     * will accept.
     * @see xtsc::xtsc_queue_push_if
     */
    xtsc::u32 nb_get_bit_width() { return m_queue.m_width1; }

   /**
     * Get the event that will be notified when the queue transitions from full
     * to not full.
     */
    virtual const sc_core::sc_event& default_event() const { return m_nonfull_event; }

  private:

    /// SystemC callback when something binds to us
    virtual void register_port(sc_core::sc_port_base& port, const char *if_typename);

    template_queue_push&        m_queue;                ///< Our template_queue_push object
    sc_core::sc_port_base      *m_p_port;               ///< Port that is bound to us
    sc_core::sc_event           m_nonfull_event;        ///< Our no-longer-full event
  };


  xtsc_queue_push_if_impl               m_push_impl;            ///<  m_producer binds to this

  log4xtensa::TextLogger&               m_text;                 ///< Used for logging 
  xtsc::u32                             m_width1;               ///< Bit width of each element


};



}  // namespace xtsc_component

#endif // _TEMPLATE_QUEUE_PUSH_H_
