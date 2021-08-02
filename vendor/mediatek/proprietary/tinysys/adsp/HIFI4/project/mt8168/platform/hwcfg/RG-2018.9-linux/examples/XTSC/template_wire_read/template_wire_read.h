#ifndef _TEMPLATE_WIRE_WRITE_H_
#define _TEMPLATE_WIRE_WRITE_H_

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
#include <xtsc/xtsc_wire_read_if.h>



namespace xtsc_component {


/**
 * Constructor parameters for a template_wire_read object.
 *
 * This class contains the constructor parameters for a template_wire_read object.
 * \verbatim
   Name                 Type    Description
   ------------------   ----    --------------------------------------------------
  
   "bit_width"          u32     Width of wire in bits.

   \endverbatim
 *
 *
 * @see template_wire_read
 * @see xtsc::xtsc_parms
 */
class template_wire_read_parms : public xtsc::xtsc_parms {
public:

  /**
   * Constructor for an template_wire_read_parms object.
   *
   * @param     width1          Width of the wire in bits.
   *
   */
  template_wire_read_parms(xtsc::u32 width1 = 32) {
    add("bit_width",            width1);
  }

  /// Return what kind of xtsc_parms this is (our C++ type)
  virtual const char* kind() const { return "template_wire_read_parms"; }

};






/**
 * A null xtsc_wire_read_if implementation.
 *
 * Template XTSC class that implements xtsc::xtsc_wire_read_if.  This class always
 * returns a wire value of 0.
 *
 * To use an instance of this class, bind an sc_port<xtsc_wire_read_if> object
 * to the template_wire_read instance.
 *
 * @see template_wire_read_parms
 * @see xtsc::xtsc_wire_read_if
 */
class template_wire_read : public sc_core::sc_module, virtual public xtsc::xtsc_wire_read_if {
public:


  /// Our C++ type (SystemC uses this)
  virtual const char* kind() const { return "template_wire_read"; }


  /**
   * Constructor for an template_wire_read.
   *
   * @param     module_name     Name of the template_wire_read sc_module.
   *
   * @param     wire_parms      The remaining parameters for construction.
   *
   * @see template_wire_read_parms
   */
  template_wire_read(sc_core::sc_module_name module_name, const template_wire_read_parms& wire_parms);


  /// Destructor.
  ~template_wire_read();


  /**
   * This method is used to read the value of this object.
   * @see xtsc::xtsc_wire_read_if
   */
  sc_dt::sc_unsigned nb_read();


  /**
   * Returns the bit width of this xtsc_wire_read_if implementation.
   * @see xtsc::xtsc_wire_read_if
   */
  xtsc::u32 nb_get_bit_width() { return m_width1; }


protected:

  xtsc::u32                             m_width1;                  ///<  Bit width of wire
  sc_dt::sc_unsigned                    m_value;                   ///<  Our value
  log4xtensa::TextLogger&               m_text;                    ///<  TextLogger

};



}  // namespace xtsc_component

#endif  // _TEMPLATE_WIRE_WRITE_H_
