#ifndef _TEMPLATE_LOOKUP_H_
#define _TEMPLATE_LOOKUP_H_

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
#include <xtsc/xtsc_lookup_if.h>



namespace xtsc_component {



/**
 * Constructor parameters for a template_lookup object.
 *
 * This class contains the constructor parameters for a template_lookup object.
 *  \verbatim
   Name                 Type    Description
   ------------------   ----    -------------------------------------------------------
  
   "address_bit_width"  u32     The width of the address in bits.  Maximum is 1024.

   "data_bit_width"     u32     The width of the data in bits.  Maximum is 1024.

   "has_ready"          bool    Specifies whether the template_lookup has a ready signal.

    \endverbatim
 *
 * @see template_lookup
 * @see xtsc::xtsc_parms
 */
class template_lookup_parms : public xtsc::xtsc_parms {
public:

  /**
   * Constructor for an template_lookup_parms object.
   *
   * @param     address_bit_width       The width of the address in bits.
   *
   * @param     data_bit_width          The width of the data in bits.
   *
   * @param     has_ready               Specifies whether or not the template_lookup has a
   *                                    ready signal (corresponds to the rdy keyword in
   *                                    the user's TIE code for the lookup).
   *
   */
  template_lookup_parms(xtsc::u32   address_bit_width,
                        xtsc::u32   data_bit_width,
                        bool        has_ready)
  {
    add("address_bit_width",    address_bit_width);
    add("data_bit_width",       data_bit_width);
    add("has_ready",            has_ready);
  }


  /// Return what kind of xtsc_parms this is (our C++ type)
  virtual const char* kind() const { return "template_lookup_parms"; }

};





/**
 * An null TIE lookup implementation that connects using TLM-level ports.
 *
 * Example null device implementing the xtsc::xtsc_lookup_if interface which
 * always returns data of 0 regardless of the address.
 *
 * @see template_lookup_parms
 * @see xtsc::xtsc_lookup_if
 */
class template_lookup : public sc_core::sc_module {
public:

  sc_core::sc_export<xtsc::xtsc_lookup_if>      m_lookup;       ///<  Driver binds to this


  /// Our C++ type (SystemC uses this)
  virtual const char* kind() const { return "template_lookup"; }


  /**
   * Constructor for an template_lookup.
   * @param     module_name     Name of the template_lookup sc_module.
   * @param     lookup_parms    The remaining parameters for construction.
   * @see template_lookup_parms
   */
  template_lookup(sc_core::sc_module_name module_name, const template_lookup_parms& lookup_parms);


  // Destructor.
  ~template_lookup(void);


protected:


  /// Implementation of xtsc_lookup_if.
  class xtsc_lookup_if_impl : public xtsc::xtsc_lookup_if, public sc_core::sc_object  {
  public:

    /// Constructor
    xtsc_lookup_if_impl(const char *object_name, template_lookup& lookup) :
      sc_object (object_name),
      m_lookup  (lookup),
      m_p_port  (0)
    {}

    /// @see xtsc::xtsc_lookup_if
    void nb_send_address(const sc_dt::sc_unsigned& address);

    /// @see xtsc::xtsc_lookup_if
    bool nb_is_ready();

    /// @see xtsc::xtsc_lookup_if
    sc_dt::sc_unsigned nb_get_data();

    /// @see xtsc::xtsc_lookup_if
    xtsc::u32 nb_get_address_bit_width() { return m_lookup.m_address_bit_width; }

    /// @see xtsc::xtsc_lookup_if
    xtsc::u32 nb_get_data_bit_width() {return m_lookup.m_data_bit_width; }

    /**
     * Get the event that will be notified when the lookup data is available.
     *
     * @see xtsc::xtsc_lookup_if::default_event()
     */
    virtual const sc_core::sc_event& default_event() const { return m_lookup.m_lookup_ready_event; }

  protected:

    /// SystemC callback when something binds to us
    virtual void register_port(sc_core::sc_port_base& port, const char *if_typename);

    template_lookup&            m_lookup;       ///< Our template_lookup object
    sc_core::sc_port_base      *m_p_port;       ///< Port that is bound to us
  };



  xtsc_lookup_if_impl                   m_lookup_impl;          ///<  m_lookup binds to this

  xtsc::u32                             m_address_bit_width;    ///<  The lookup address bit width
  xtsc::u32                             m_data_bit_width;       ///<  The lookup data bit width
  bool                                  m_has_ready;            ///<  True if lookup has a ready signal
  sc_core::sc_event                     m_lookup_ready_event;   ///<  Notified when lookup might be ready (i.e. "ask me again")
  sc_dt::sc_unsigned                    m_zero;                 ///<  Constant 0

  log4xtensa::TextLogger&               m_text;                 ///<  The logger for text messages


};



}  // namespace xtsc_component




#endif  // _TEMPLATE_LOOKUP_H_
