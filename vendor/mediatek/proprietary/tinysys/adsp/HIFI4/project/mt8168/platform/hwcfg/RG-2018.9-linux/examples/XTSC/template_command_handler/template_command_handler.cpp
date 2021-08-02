// Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2014 by Tensilica Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.

#include <sstream>
#include <xtsc/xtsc_core.h>
#include "template_command_handler.h"


using namespace std;
using namespace sc_core;
using namespace log4xtensa;
using namespace xtsc;


xtsc_component::template_command_handler::template_command_handler(sc_module_name module_name) :
  sc_module             (module_name),
  m_text                (TextLogger::getInstance(name()))
{

  xtsc_register_command(*this, *this, "douglas_adams", 0, 0,
      "douglas_adams", 
      "Return the ultimate answer."
  );

  xtsc_register_command(*this, *this, "dump_core_info", 0, 1,
      "dump_core_info [<CoreName>]", 
      "Return the ostream buffer from calling dump_core_info(<CoreName>)."
  );

  LogLevel ll = xtsc_get_constructor_log_level();
  XTSC_LOG(m_text, ll,        "Constructed template_command_handler '" << name() << "':");

}



xtsc_component::template_command_handler::~template_command_handler() {
}



void xtsc_component::template_command_handler::man(ostream& os) {
  os << " [General help/man information for template_command_handler should go here]." << endl;
}




void xtsc_component::template_command_handler::execute(const string&                 cmd_line, 
                                                       const vector<string>&         words,
                                                       const vector<string>&         words_lc,
                                                       ostream&                      result)
{
  ostringstream res;

  if (false) {
  }
  else if (words[0] == "douglas_adams") {
    res << 42;
  }
  else if (words[0] == "dump_core_info") {
    string core_name = (words.size() > 1 ? words[1] : "");
    dump_core_info(core_name, res);
  }
  else {
    ostringstream oss;
    oss << name() << "::" << __FUNCTION__ << "() called for unknown command '" << cmd_line << "'.";
    throw xtsc_exception(oss.str());
  }

  result << res.str();
}



void xtsc_component::template_command_handler::dump_core_info(const std::string& core_name, std::ostream& os) const {
  u32 count = 0;
  vector<xtsc_core*> all_cores = xtsc_core::get_all_cores();
  for (vector<xtsc_core*>::const_iterator i = all_cores.begin(); i != all_cores.end(); ++i) {
    if ((core_name == "") || (core_name == (*i)->name())) {
      count += 1;
      os << (*i)->name() << endl;
      os << " RunStall: "   << ((*i)->get_stall() ? "Stalled" : "Running") << endl;
      os << " Clock:    "   << ((*i)->is_clock_enabled() ? "Enabled" : "Disabled") << endl;
      os << " PC:       0x" << hex << setw(8) << (*i)->get_pc() << dec << endl;
    }
  }
  if ((core_name != "") && (count == 0)) {
    ostringstream oss;
    oss << "No core named \"" << core_name << "\" exists in the simulation.";
    throw xtsc_exception(oss.str());
  }
}



