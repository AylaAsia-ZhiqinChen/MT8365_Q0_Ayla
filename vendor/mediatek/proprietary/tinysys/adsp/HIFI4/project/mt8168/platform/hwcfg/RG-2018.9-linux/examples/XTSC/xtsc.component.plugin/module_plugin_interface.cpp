// Customer ID=13943; Build=0x75f5e; Copyright (c) 2012-2017 by Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Cadence Design Systems, Inc.

// See README.txt

#include MODULE_HEADER

using namespace std;
using namespace xtsc;
using namespace xtsc_component;



class MODULE_PLUGIN_INTERFACE : public xtsc_plugin_interface {
public:

  virtual set<string> get_plugin_names() {
    set<string> plugin_names;
    plugin_names.insert(XTSC_QUOTE(MODULE));
    return plugin_names;
  }


  virtual void help(const string& plugin_name, bool verbose, ostream& os) {
    if (false) {}
    else if (plugin_name == XTSC_QUOTE(MODULE)) {
      os << XTSC_QUOTE(MODULE) << " is an XTSC component module built as a plugin and modified TODO." << endl;
    }
    else {
      ostringstream oss;
      oss << XTSC_QUOTE(MODULE_PLUGIN_INTERFACE) << "::help() called with unknown plugin_name of \"" << plugin_name << "\"";
      throw xtsc_exception(oss.str());
    }
  }


  virtual xtsc_parms *create_parms(const string& plugin_name) {
    if (false) {}
    else if (plugin_name == XTSC_QUOTE(MODULE)) {
      return new MODULE_PARMS;
    }
    else {
      ostringstream oss;
      oss << XTSC_QUOTE(MODULE_PLUGIN_INTERFACE) << "::create_parms() called with unknown plugin_name of \"" << plugin_name << "\"";
      throw xtsc_exception(oss.str());
    }
  }


  virtual sc_core::sc_module& create_module(const string& plugin_name, const string& instance_name, const xtsc_parms *p_parms) {
    if (false) {}
    else if (plugin_name == XTSC_QUOTE(MODULE)) {
      MODULE *p_module = new MODULE(instance_name.c_str(), *(MODULE_PARMS*)p_parms);
      m_instance_map[p_module->name()] = p_module;
      return *p_module;
    }
    else {
      ostringstream oss;
      oss << XTSC_QUOTE(MODULE_PLUGIN_INTERFACE) << "::create_module() called with unknown plugin_name of \"" << plugin_name << "\"";
      throw xtsc_exception(oss.str());
    }
  }


  virtual xtsc_connection_interface *get_connection_interface(const string& hierarchical_name) {
    if (m_instance_map.find(hierarchical_name) == m_instance_map.end()) {
      ostringstream oss;
      oss << XTSC_QUOTE(MODULE_PLUGIN_INTERFACE) << "::get_connection_interface() called with unknown hierarchical_name of \""
          << hierarchical_name << "\"";
      throw xtsc_exception(oss.str());
    }
    return m_instance_map[hierarchical_name];
  }


  map<string, xtsc_connection_interface*> m_instance_map;  ///< Map hierarchical name to connection interface
};



extern "C" 
#if defined(_WIN32)
__declspec(dllexport)
#endif
xtsc_plugin_interface& xtsc_get_plugin_interface() {
  static MODULE_PLUGIN_INTERFACE *p_plugin_interface = 0;
  if (!p_plugin_interface) {
    p_plugin_interface = new MODULE_PLUGIN_INTERFACE;
  }
  return *p_plugin_interface;
}


