#ifndef _Q2F_ADAPTER_H_
#define _Q2F_ADAPTER_H_

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



/**
 * Constructor parameters for a q2f_adapter object.
 *
 *  \verbatim
   Name                 Type    Description
   ------------------   ----    --------------------------------------------------------

   "p_fifo"             void*   An optional pointer to a sc_fifo object. If NULL, a
                                sc_fifo will be created using the following parameters.
                
   "bit_width"          u32     Width of each sc_fifo element in bits.

   "depth"              u32     Number of elements the sc_fifo can hold.  Ignored if
                                p_fifo is not NULL.
  
    \endverbatim
 *
 * @see xtsc_parms
 */
class q2f_adapter_parms : public xtsc::xtsc_parms {
public:

  /**
   * Constructor for a q2f_adapter_parms object. 
   *
   * @param     p_fifo          An optional pointer to a sc_fifo object. If NULL, a
   *                            sc_fifo will be created using the following parameters.
   * 
   * @param     bit_width       Width of each sc_fifo element in bits.
   *
   * @param     depth           Number of elements the sc_fifo can hold.  Ignored if 
   *                            p_fifo is not NULL.
   *                    
   */
  q2f_adapter_parms(sc_core::sc_fifo<sc_dt::sc_unsigned> *p_fifo = NULL,
                    xtsc::u32                             bit_width = 32,
                    xtsc::u32                             depth  = 16)
  {
    add("p_fifo",               (void*) p_fifo);
    add("bit_width",            bit_width);
    add("depth",                depth);
  }

  /// Return what kind of xtsc_parms this is (our C++ type)
  const char *kind() const { return "q2f_adapter_parms"; }
};




/**
 * This module provides an adapter from the xtsc_queue_push_if interface to a sc_fifo.
 * The sc_fifo can already exist and be passed in to this module via the
 * q2f_adapter_parms object, or, alternatively, this module will create the sc_fifo
 * and make it available via the get_fifo() method.
 *
 * One possible usage for this module is to allow a TIE output queue of xtsc::xtsc_core
 * to write to (push) a sc_fifo.
 */
class q2f_adapter : public sc_core::sc_module {
public:

  sc_core::sc_export<xtsc::xtsc_queue_push_if>  m_producer;     ///<  Producer binds to this


  /// Our C++ type (SystemC uses this)
  virtual const char* kind() const { return "q2f_adapter"; }


  /**
   * Constructor for a q2f_adapter.
   *
   * @param     module_name     Name of the q2f_adapter sc_module.
   *
   * @param     parms           The remaining parameters for construction.
   *
   * @see q2f_adapter_parms
   */
  q2f_adapter(sc_core::sc_module_name module_name, const q2f_adapter_parms &parms);

  /// Destructor
  ~q2f_adapter();


  /// Get a reference to the sc_fifo that this adapter is using
  sc_core::sc_fifo<sc_dt::sc_unsigned>& get_fifo() { return m_fifo; }

protected:


  /**
   * Static method to either get the sc_fifo passed in via the q2f_adapter_parms
   * object or to create one.  If "p_fifo" in parms is not NULL then the sc_fifo that it
   * points to will be returned.  Otherwise, a sc_fifo will be created and returned.
   * The our_fifo flag will be set to true if this method created the sc_fifo.  
   * Otherwise, our_fifo will be set to false.
   *
   * @param     parms           The construction parameters.
   *
   * @param     our_fifo        Set to false if "p_fifo" in parms is not NULL.
   *                            Otherwise, set to true.
   */
  static sc_core::sc_fifo<sc_dt::sc_unsigned>& get_or_create_fifo(const q2f_adapter_parms &parms, bool& our_fifo);


  /// Implementation of xtsc_queue_push_if.
  class xtsc_queue_push_if_impl : public xtsc::xtsc_queue_push_if, public sc_core::sc_object  {
  public:

    /// Constructor
    xtsc_queue_push_if_impl(const char *object_name, q2f_adapter& adapter) :
      sc_object         (object_name),
      m_adapter         (adapter),
      m_p_port          (0)
    {}

    /// @see xtsc::xtsc_queue_push_if
    bool nb_can_push() { return (m_adapter.m_fifo.num_free() != 0); }

    /// @see xtsc::xtsc_queue_push_if
    bool nb_push(const sc_dt::sc_unsigned& element, xtsc::u64& ticket = push_ticket);

    /// @see xtsc::xtsc_queue_push_if
    xtsc::u32 nb_get_bit_width() { return m_adapter.m_width1; }

    /**
     * Get the event that will be notified when the sc_fifo transitions from full
     * to not full.
     */
    virtual const sc_core::sc_event& default_event() const { return m_adapter.m_fifo.data_read_event(); }

  private:

    /// SystemC callback when something binds to us
    virtual void register_port(sc_core::sc_port_base& port, const char *if_typename);

    q2f_adapter&                m_adapter;              ///< Our q2f_adapter object
    sc_core::sc_port_base      *m_p_port;               ///< Port that is bound to us
  };




  xtsc_queue_push_if_impl               m_push_impl;    ///<  m_producer binds to this
  sc_core::sc_fifo<sc_dt::sc_unsigned>& m_fifo;         ///<  The sc_fifo
  bool                                  m_our_fifo;     ///<  True if we created m_fifo
  xtsc::u32                             m_width1;       ///<  The bit width of elements in m_fifo
  log4xtensa::TextLogger&               m_text;         ///<  Text logger
};

#endif  // _Q2F_ADAPTER_H_
