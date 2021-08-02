#ifndef _TEMPLATE_COMMAND_HANDLER_H_
#define _TEMPLATE_COMMAND_HANDLER_H_

// Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2014 by Tensilica Inc.  ALL RIGHTS RESERVED.
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



namespace xtsc_component {


/**
 * A template xtsc_command_handler_interface implementation.
 *
 * This simple command handler supports the commands shown in the doxygen for the
 * execute() method.
 *
 * @see xtsc::xtsc_command_handler_interface
 */
class template_command_handler : public sc_core::sc_module, virtual public xtsc::xtsc_command_handler_interface {
public:


  /// Our C++ type (SystemC uses this)
  virtual const char* kind() const { return "template_command_handler"; }


  /**
   * Constructor for an template_command_handler.
   *
   * @param     module_name     Name of the template_command_handler sc_module.
   */
  template_command_handler(sc_core::sc_module_name module_name);


  /// Destructor.
  ~template_command_handler();


  /// Implementation of the xtsc::xtsc_command_handler_interface.
  virtual void man(std::ostream& os);


  /**
   * Implementation of the xtsc::xtsc_command_handler_interface.
   *
   * This implementation supports the following commands:
   *  \verbatim

        douglas_adams
          Return the ultimate answer.

        dump_core_info [<CoreName>]
          Return the ostream buffer from calling dump_core_info(<CoreName>).
      \endverbatim
   */
  virtual void execute(const std::string&               cmd_line,
                       const std::vector<std::string>&  words,
                       const std::vector<std::string>&  words_lc,
                       std::ostream&                    result);


  /**
   * Method to dump miscellaneous information about the specified xtsc_core instance.
   *
   * @param     core_name       The full hierarchical name of the xtsc_core instance of
   *                            interest.  If empty, then dump information about all
   *                            xtsc_core instances in the simulation.
   *
   * @param     os              The ostream object to dump core information to.
   */
  void dump_core_info(const std::string& core_name = "", std::ostream& os = std::cout) const;


protected:

  log4xtensa::TextLogger&               m_text;                    ///<  TextLogger

};



}  // namespace xtsc_component

#endif  // _TEMPLATE_COMMAND_HANDLER_H_
