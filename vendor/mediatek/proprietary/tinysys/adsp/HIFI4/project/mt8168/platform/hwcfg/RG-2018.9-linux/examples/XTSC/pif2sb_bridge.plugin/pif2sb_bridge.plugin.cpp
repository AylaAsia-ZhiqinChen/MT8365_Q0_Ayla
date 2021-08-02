// Customer ID=13943; Build=0x75f5e; Copyright (c) 2012 by Tensilica Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.


#include "pif2sb_bridge.h"
#include "simple_bus/simple_bus.h"
#include "simple_bus/simple_bus_arbiter.h"
#include "simple_bus/simple_bus_slow_mem.h"
#include "simple_bus/simple_bus_fast_mem.h"

using namespace std;
using namespace sc_core;
using namespace log4xtensa;
using namespace xtsc;


/*
 * This example of an xtsc-run plugin is fairly complex in that it has several models
 * that are being turned into plugins.  For a simpler example which only has a single
 * model please see the neighboring simple_memory.plugin example, especially the code
 * for the simple_memory_plugin_interface class at the end of file:
 *   ../simple_memory.plugin/simple_memory.plugin.cpp   (Linux)
 *   ..\simple_memory.plugin\simple_memory.plugin.cpp   (MS Windows)
 */


/*
 *                                 Overview
 *
 * This file contains 8 classes that are used to make the pif2sb_bridge model and the
 * 4 OSCI simple_bus models available in xtsc-run as a shared library.  The shared
 * library is a dynamic shared object (*.so) on Linux and a dynamic link library (*.dll)
 * on MS Windows.  This file along with the pif2sb_bridge model and the 4 OSCI simple_bus
 * models must be compiled together into a shared library and that shared library must be
 * passed to xtsc-run using the -load_library command in order to make the models available
 * to xtsc-run.
 *
 * The 8 classes are briefly and in-order:
 *
 * 2 classes to provide parameters for those OSCI simple_bus models that need them:
 *   - simple_bus_slow_mem_parms
 *   - simple_bus_fast_mem_parms
 *
 * 5 classes to provide an xtsc_connection_interface for each of the models:
 *   - pif2sb_bridge_connection_interface
 *   - simple_bus_connection_interface
 *   - simple_bus_arbiter_connection_interface
 *   - simple_bus_slow_mem_connection_interface
 *   - simple_bus_fast_mem_connection_interface
 *
 * 1 class to provide an xtsc_plugin_interface that supports all 5 of the models
 *   - pif2sb_bridge_plugin_interface
 *
 * For more information about each class, please see the comments before each class below.
 * For more information about the OSCI simple_bus models, please see simple_bus/README and
 * simple_bus/SLIDES.pdf.
 *
 * This file also contains the xtsc_get_plugin_interface function which must be in the
 * shared library in order for xtsc-run to load the plugin models.  The way this works
 * is briefly:
 *
 * 1) When xtsc-run encounters the --load_library command it passes the shared library
 *    name to the xtsc_load_library() method which will find the shared library in the
 *    file system, load it, find the xtsc_get_plugin_interface symbol in the shared
 *    library, and return a pointer to this symbol (which is a function).
 *
 * 2) When xtsc_load_library() returns, xtsc-run will invoke the xtsc_get_plugin_interface
 *    function to get a pointer to the single instance of pif2sb_bridge_plugin_interface.
 *
 * 3) xtsc-run will then call pif2sb_bridge_plugin_interface::get_plugin_names() to find
 *    out what plugin models the shared library supports.  
 *
 * 4) At this point the plugin models are all available in xtsc-run along with their
 *    associated --show_XXX_parms, --set_XXX_parm, --create_XXX commands.  In addition,
 *    the associated commands --show_libs, --show_plugins, and --help_plugin=<PluginName>
 *    will be available.  Also, once you instantiate an instance of a plugin, you can
 *    then see what ports it has using the --show_ports command.  Finally, because each
 *    model has an associated xtsc_connection_interface they will all be supported by
 *    the generic connection command which can be used to connect plugin instances 
 *    together with each other or with built-in XTSC module instances:
 *       --connect=<InstanceA>,<PortA>,<PortB>,<InstanceB>
 *
 */






/*
 * This class provides parameters needed for constructing a simple_bus_slow_mem model.
 * For more information, please see xtsc_parms in xtsc.h or xtsc_rm.pdf and in
 * xtsc_ug.pdf.
 */
class simple_bus_slow_mem_parms : public xtsc_parms {
public:
  simple_bus_slow_mem_parms() {
    add("start_address",  0x0000000);
    add("end_address",    0x0000FFF);
    add("nr_wait_states", 1);
  }
  virtual const char *kind() const { return "simple_bus_slow_mem_parms"; }
};



/*
 * This class provides parameters needed for constructing a simple_bus_fast_mem model.
 * For more information, please see xtsc_parms in xtsc.h or xtsc_rm.pdf and in
 * xtsc_ug.pdf.
 */
class simple_bus_fast_mem_parms : public xtsc_parms {
public:
  simple_bus_fast_mem_parms() {
    add("start_address",  0x0000000);
    add("end_address",    0x0000FFF);
  }
  virtual const char *kind() const { return "simple_bus_fast_mem_parms"; }
};




/*
 * This class adds an xtsc_connection_interface implementation to the pif2sb_bridge model.
 * For the requirements of the methods of this interface, please see xtsc_connection_interface
 * in xtsc.h or xtsc_rm.pdf and in xtsc_ug.pdf.
 */
class pif2sb_bridge_connection_interface : public pif2sb_bridge, public xtsc_connection_interface {
public:

  pif2sb_bridge_connection_interface(sc_module_name module_name, pif2sb_bridge_parms parms) :
    pif2sb_bridge               (module_name, parms),
    xtsc_connection_interface   (*(sc_object*)this),
    m_bit_width                 (parms.get_u32("pif_byte_width")*8)
  {
    m_port_types["m_pif_request_export"] = REQUEST_EXPORT;
    m_port_types["m_pif_respond_port"  ] = RESPOND_PORT;
    m_port_types["m_sb_direct_port"    ] = USER_DEFINED_PORT;
    m_port_types["m_sb_blocking_port"  ] = USER_DEFINED_PORT;
    m_port_types["pif"                 ] = PORT_TABLE;
    m_port_types["direct"              ] = PORT_TABLE;
    m_port_types["blocking"            ] = PORT_TABLE;
  }


  u32 get_bit_width(const string& port_name, u32 interface_num = 0) const { return m_bit_width; }


  sc_object *get_port(const string& port_name) {
    if (port_name == "m_pif_request_export") return &m_pif_request_export;
    if (port_name == "m_pif_respond_port"  ) return &m_pif_respond_port;
    if (port_name == "m_sb_direct_port"    ) return &m_sb_direct_port;
    if (port_name == "m_sb_blocking_port"  ) return &m_sb_blocking_port;
    ostringstream oss;
    oss << "pif2sb_bridge_connection_interface::get_port() called with unknown port_name of \"" << port_name << "\"";
    throw xtsc_exception(oss.str());
  }


  xtsc_port_table get_port_table(const string& port_table_name) const {
    xtsc_port_table table;

    if (port_table_name == "pif") {
      table.push_back("m_pif_request_export");
      table.push_back("m_pif_respond_port"  );
    }
    else if (port_table_name == "direct") {
      table.push_back("m_sb_direct_port");
    }
    else if (port_table_name == "blocking") {
      table.push_back("m_sb_blocking_port");
    }
    else {
      ostringstream oss;
      oss << "pif2sb_bridge_connection_interface::get_port_table() called with unknown port_table_name of \"" << port_table_name << "\"";
      throw xtsc_exception(oss.str());
    }

    return table;
  }


  string get_user_defined_port_type(const string& port_name) const {
    if (port_name == "m_sb_direct_port") {
      return "simple_bus_direct_if";
    }
    else if (port_name == "m_sb_blocking_port") {
      return "simple_bus_blocking_if";
    }
    else {
      ostringstream oss;
      oss << "pif2sb_bridge_connection_interface::get_user_defined_port_type() called with unknown port_name of \"" << port_name << "\"";
      throw xtsc_exception(oss.str());
    }
  }


  void connect_user_defined_port_type(const string& port_name, sc_object *p_object, const string& signal_name) {
    if (port_name == "m_sb_direct_port") {
      m_sb_direct_port(*dynamic_cast<sc_export<simple_bus_direct_if>*>(p_object));
    }
    else if (port_name == "m_sb_blocking_port") {
      m_sb_blocking_port(*dynamic_cast<sc_export<simple_bus_blocking_if>*>(p_object));
    }
    else {
      ostringstream oss;
      oss << "pif2sb_bridge_connection_interface::connect_user_defined_port_type() called with unknown port_name of \"" << port_name
          << "\"";
      throw xtsc_exception(oss.str());
    }
  }

  u32   m_bit_width;
};




/*
 * This class adds an xtsc_connection_interface implementation to the simple_bus model.
 * For the requirements of the methods of this interface, please see xtsc_connection_interface
 * in xtsc.h or xtsc_rm.pdf and in xtsc_ug.pdf.
 */
class simple_bus_connection_interface : public simple_bus, public xtsc_connection_interface {
public:

  simple_bus_connection_interface(sc_module_name module_name) :
    simple_bus                  (module_name),
    xtsc_connection_interface   (*(sc_object*)this),
    m_sb_direct_export          ("m_sb_direct_export"),
    m_sb_blocking_export        ("m_sb_blocking_export")
  {
    m_port_types["clock"               ] = BOOL_INPUT;
    m_port_types["arbiter_port"        ] = USER_DEFINED_PORT;
    m_port_types["slave_port"          ] = USER_DEFINED_PORT;
    m_port_types["m_sb_direct_export"  ] = USER_DEFINED_EXPORT;
    m_port_types["m_sb_blocking_export"] = USER_DEFINED_EXPORT;
    m_port_types["arbiter"             ] = PORT_TABLE;
    m_port_types["slave"               ] = PORT_TABLE;
    m_port_types["direct"              ] = PORT_TABLE;
    m_port_types["blocking"            ] = PORT_TABLE;
    m_sb_direct_export  (*this);
    m_sb_blocking_export(*this);
  }


  u32 get_bit_width(const string& port_name, u32 interface_num = 0) const {
    return ((port_name == "clock") ? 1 : 0);
  }


  sc_object *get_port(const string& port_name) {
    if (port_name == "clock"               ) return &clock;
    if (port_name == "arbiter_port"        ) return &arbiter_port;
    if (port_name == "slave_port"          ) return &slave_port;
    if (port_name == "m_sb_direct_export"  ) return &m_sb_direct_export;
    if (port_name == "m_sb_blocking_export") return &m_sb_blocking_export;
    ostringstream oss;
    oss << "simple_bus_connection_interface::get_port() called with unknown port_name of \"" << port_name << "\"";
    throw xtsc_exception(oss.str());
  }


  xtsc_port_table get_port_table(const string& port_table_name) const {
    xtsc_port_table table;

    if (port_table_name == "arbiter") {
      table.push_back("arbiter_port");
    }
    else if (port_table_name == "slave") {
      table.push_back("slave_port");
    }
    else if (port_table_name == "direct") {
      table.push_back("m_sb_direct_export");
    }
    else if (port_table_name == "blocking") {
      table.push_back("m_sb_blocking_export");
    }
    else {
      ostringstream oss;
      oss << "simple_bus_connection_interface::get_port_table() called with unknown port_table_name of \"" << port_table_name << "\"";
      throw xtsc_exception(oss.str());
    }

    return table;
  }


  string get_user_defined_port_type(const string& port_name) const {
    if (port_name == "arbiter_port") {
      return "simple_bus_arbiter_if";
    }
    else if (port_name == "slave_port") {
      return "simple_bus_slave_if";
    }
    else if (port_name == "m_sb_direct_export") {
      return "simple_bus_direct_if";
    }
    else if (port_name == "m_sb_blocking_export") {
      return "simple_bus_blocking_if";
    }
    else {
      ostringstream oss;
      oss << "simple_bus_connection_interface::get_user_defined_port_type() called with unknown port_name of \"" << port_name << "\"";
      throw xtsc_exception(oss.str());
    }
  }


  void connect_user_defined_port_type(const string& port_name, sc_object *p_object, const string& signal_name) {
    if (port_name == "arbiter_port") {
      arbiter_port(*dynamic_cast<sc_export<simple_bus_arbiter_if>*>(p_object));
    }
    else if (port_name == "slave_port") {
      slave_port(*dynamic_cast<sc_export<simple_bus_slave_if>*>(p_object));
    }
    else {
      ostringstream oss;
      oss << "simple_bus_connection_interface::connect_user_defined_port_type() called with unknown port_name of \"" << port_name
          << "\"";
      throw xtsc_exception(oss.str());
    }
  }


  sc_export<simple_bus_direct_if>       m_sb_direct_export;
  sc_export<simple_bus_blocking_if>     m_sb_blocking_export;

};




/*
 * This class adds an xtsc_connection_interface implementation to the simple_bus_arbiter model.
 * For the requirements of the methods of this interface, please see xtsc_connection_interface
 * in xtsc.h or xtsc_rm.pdf and in xtsc_ug.pdf.
 */
class simple_bus_arbiter_connection_interface : public simple_bus_arbiter, public xtsc_connection_interface {
public:

  simple_bus_arbiter_connection_interface(sc_module_name module_name) :
    simple_bus_arbiter          (module_name),
    xtsc_connection_interface   (*(sc_object*)this),
    m_sb_arbiter_export         ("m_sb_arbiter_export")
  {
    m_port_types["m_sb_arbiter_export"] = USER_DEFINED_EXPORT;
    m_port_types["arbiter"            ] = PORT_TABLE;
    m_sb_arbiter_export(*this);
  }


  u32 get_bit_width(const string& port_name, u32 interface_num = 0) const {
    return 0;
  }


  sc_object *get_port(const string& port_name) {
    if (port_name == "m_sb_arbiter_export") return &m_sb_arbiter_export;
    ostringstream oss;
    oss << "simple_bus_arbiter_connection_interface::get_port() called with unknown port_name of \"" << port_name << "\"";
    throw xtsc_exception(oss.str());
  }


  string get_default_port_name() const { return "arbiter"; }


  xtsc_port_table get_port_table(const string& port_table_name) const {
    xtsc_port_table table;

    if (port_table_name == "arbiter") {
      table.push_back("m_sb_arbiter_export");
    }
    else {
      ostringstream oss;
      oss << "simple_bus_arbiter_connection_interface::get_port_table() called with unknown port_table_name of \"" << port_table_name
          << "\"";
      throw xtsc_exception(oss.str());
    }

    return table;
  }


  string get_user_defined_port_type(const string& port_name) const {
    if (port_name == "m_sb_arbiter_export") {
      return "simple_bus_arbiter_if";
    }
    else {
      ostringstream oss;
      oss << "simple_bus_arbiter_connection_interface::get_user_defined_port_type() called with unknown port_name of \"" << port_name
          << "\"";
      throw xtsc_exception(oss.str());
    }
  }


  sc_export<simple_bus_arbiter_if>     m_sb_arbiter_export;

};



/*
 * This class adds an xtsc_connection_interface implementation to the simple_bus_slow_mem model.
 * For the requirements of the methods of this interface, please see xtsc_connection_interface
 * in xtsc.h or xtsc_rm.pdf and in xtsc_ug.pdf.
 */
class simple_bus_slow_mem_connection_interface : public simple_bus_slow_mem, public xtsc_connection_interface {
public:

  simple_bus_slow_mem_connection_interface(sc_module_name module_name, simple_bus_slow_mem_parms parms) :
    simple_bus_slow_mem      (module_name, parms.get_u32("start_address"), parms.get_u32("end_address"), parms.get_u32("nr_wait_states")),
    xtsc_connection_interface(*(sc_object*)this),
    m_sb_slave_export        ("m_sb_slave_export")
  {
    m_port_types["clock"            ] = BOOL_INPUT;
    m_port_types["m_sb_slave_export"] = USER_DEFINED_EXPORT;
    m_port_types["slave"            ] = PORT_TABLE;
    m_sb_slave_export(*this);
  }


  u32 get_bit_width(const string& port_name, u32 interface_num = 0) const {
    return ((port_name == "clock") ? 1 : 0);
  }


  sc_object *get_port(const string& port_name) {
    if (port_name == "clock"            ) return &clock;
    if (port_name == "m_sb_slave_export") return &m_sb_slave_export;
    ostringstream oss;
    oss << "simple_bus_slow_mem_connection_interface::get_port() called with unknown port_name of \"" << port_name << "\"";
    throw xtsc_exception(oss.str());
  }


  string get_default_port_name() const { return "slave"; }


  xtsc_port_table get_port_table(const string& port_table_name) const {
    xtsc_port_table table;

    if (port_table_name == "slave") {
      table.push_back("m_sb_slave_export");
    }
    else {
      ostringstream oss;
      oss << "simple_bus_slow_mem_connection_interface::get_port_table() called with unknown port_table_name of \"" << port_table_name
          << "\"";
      throw xtsc_exception(oss.str());
    }

    return table;
  }


  string get_user_defined_port_type(const string& port_name) const {
    if (port_name == "m_sb_slave_export") {
      return "simple_bus_slave_if";
    }
    else {
      ostringstream oss;
      oss << "simple_bus_slow_mem_connection_interface::get_user_defined_port_type() called with unknown port_name of \"" << port_name
          << "\"";
      throw xtsc_exception(oss.str());
    }
  }


  sc_export<simple_bus_slave_if>     m_sb_slave_export;

};




/*
 * This class adds an xtsc_connection_interface implementation to the simple_bus_fast_mem model.
 * For the requirements of the methods of this interface, please see xtsc_connection_interface
 * in xtsc.h or xtsc_rm.pdf and in xtsc_ug.pdf.
 */
class simple_bus_fast_mem_connection_interface : public simple_bus_fast_mem, public xtsc_connection_interface {
public:

  simple_bus_fast_mem_connection_interface(sc_module_name module_name, simple_bus_fast_mem_parms parms) :
    simple_bus_fast_mem         (module_name, parms.get_u32("start_address"), parms.get_u32("end_address")),
    xtsc_connection_interface   (*(sc_object*)this),
    m_sb_slave_export           ("m_sb_slave_export")
  {
    m_port_types["m_sb_slave_export"] = USER_DEFINED_EXPORT;
    m_port_types["slave"            ] = PORT_TABLE;
    m_sb_slave_export(*this);
  }


  u32 get_bit_width(const string& port_name, u32 interface_num = 0) const {
    return 0;
  }


  sc_object *get_port(const string& port_name) {
    if (port_name == "m_sb_slave_export") return &m_sb_slave_export;
    ostringstream oss;
    oss << "simple_bus_fast_mem_connection_interface::get_port() called with unknown port_name of \"" << port_name << "\"";
    throw xtsc_exception(oss.str());
  }


  string get_default_port_name() const { return "slave"; }


  xtsc_port_table get_port_table(const string& port_table_name) const {
    xtsc_port_table table;

    if (port_table_name == "slave") {
      table.push_back("m_sb_slave_export");
    }
    else {
      ostringstream oss;
      oss << "simple_bus_fast_mem_connection_interface::get_port_table() called with unknown port_table_name of \"" << port_table_name
          << "\"";
      throw xtsc_exception(oss.str());
    }

    return table;
  }


  string get_user_defined_port_type(const string& port_name) const {
    if (port_name == "m_sb_slave_export") {
      return "simple_bus_slave_if";
    }
    else {
      ostringstream oss;
      oss << "simple_bus_fast_mem_connection_interface::get_user_defined_port_type() called with unknown port_name of \"" << port_name
          << "\"";
      throw xtsc_exception(oss.str());
    }
  }


  sc_export<simple_bus_slave_if>     m_sb_slave_export;

};



/*
 * This class adds an xtsc_plugin_interface implementation which supports the
 * pif2sb_bridge and all the OSCI simple_bus models.
 *
 * For the requirements of the methods of this interface, please see xtsc_plugin_interface
 * in xtsc.h or xtsc_rm.pdf and in xtsc_ug.pdf.
 */
class pif2sb_bridge_plugin_interface : public xtsc_plugin_interface {
public:


  virtual set<string> get_plugin_names() {
    set<string> plugin_names;
    plugin_names.insert("pif2sb");
    plugin_names.insert("sb");
    plugin_names.insert("sb_arbiter");
    plugin_names.insert("sb_slow_mem");
    plugin_names.insert("sb_fast_mem");
    return plugin_names;
  }


  virtual void help(const string& plugin_name, bool verbose, ostream& os) {
    if (plugin_name == "pif2sb") {
      if (verbose) {
        os << "pif2sb is a PIF-to-OSCI-simple_bus bridge with a configurable PIF byte width (default 4)," << endl;
        os << "priority on the simple_bus (default 0), and clock period." << endl;
        os << "The pif2sb_bridge model uses the simple_bus_direct_if for nb_peek() and nb_poke() and" << endl;
        os << "the simple_bus_blocking_if for nb_request()." << endl;
      }
      else {
        os << "pif2sb is a PIF-to-OSCI-simple_bus bridge model." << endl;
      }
    }
    else if (plugin_name == "sb") {
      os << "sb is the simple_bus model from the OSCI simple_bus example." << endl;
    }
    else if (plugin_name == "sb_arbiter") {
      os << "sb_arbiter is the simple_bus_arbiter model from the OSCI simple_bus example." << endl;
    }
    else if (plugin_name == "sb_slow_mem") {
      os << "sb_slow_mem is the simple_bus_slow_mem model from the OSCI simple_bus example." << endl;
    }
    else if (plugin_name == "sb_fast_mem") {
      os << "sb_fast_mem is the simple_bus_fast_mem model from the OSCI simple_bus example." << endl;
    }
    else {
      ostringstream oss;
      oss << "pif2sb_bridge_plugin_interface::help() called with unknown plugin_name of \"" << plugin_name << "\"";
      throw xtsc_exception(oss.str());
    }
    if (verbose) {
      os << "For more information on the OSCI simple_bus example please see simple_bus/README" << endl;
      os << "and simple_bus/SLIDES.pdf." << endl;
    }
  }


  virtual xtsc_parms *create_parms(const string& plugin_name) {
    if (plugin_name == "pif2sb") {
      return new pif2sb_bridge_parms();
    }
    else if (plugin_name == "sb") {
      return new xtsc_parms();  // no parameters
    }
    else if (plugin_name == "sb_arbiter") {
      return new xtsc_parms();  // no parameters
    }
    else if (plugin_name == "sb_slow_mem") {
      return new simple_bus_slow_mem_parms();
    }
    else if (plugin_name == "sb_fast_mem") {
      return new simple_bus_fast_mem_parms();
    }
    else {
      ostringstream oss;
      oss << "pif2sb_bridge_plugin_interface::create_parms() called with unknown plugin_name of \"" << plugin_name << "\"";
      throw xtsc_exception(oss.str());
    }
  }


  virtual sc_module& create_module(const string& plugin_name, const string& instance_name, const xtsc_parms *p_parms) {
    if (plugin_name == "pif2sb") {
      pif2sb_bridge_parms &parms = *(pif2sb_bridge_parms*)p_parms;
      pif2sb_bridge_connection_interface *p_ci = new pif2sb_bridge_connection_interface(instance_name.c_str(), parms);
      m_instance_map[p_ci->name()] = p_ci;
      return *p_ci;
    }
    else if (plugin_name == "sb") {
      simple_bus_connection_interface *p_ci = new simple_bus_connection_interface(instance_name.c_str());
      m_instance_map[p_ci->name()] = p_ci;
      return *p_ci;
    }
    else if (plugin_name == "sb_arbiter") {
      simple_bus_arbiter_connection_interface *p_ci = new simple_bus_arbiter_connection_interface(instance_name.c_str());
      m_instance_map[p_ci->name()] = p_ci;
      return *p_ci;
    }
    else if (plugin_name == "sb_slow_mem") {
      simple_bus_slow_mem_parms &parms = *(simple_bus_slow_mem_parms*)p_parms;
      simple_bus_slow_mem_connection_interface *p_ci = new simple_bus_slow_mem_connection_interface(instance_name.c_str(), parms);
      m_instance_map[p_ci->name()] = p_ci;
      return *p_ci;
    }
    else if (plugin_name == "sb_fast_mem") {
      simple_bus_fast_mem_parms &parms = *(simple_bus_fast_mem_parms*)p_parms;
      simple_bus_fast_mem_connection_interface *p_ci = new simple_bus_fast_mem_connection_interface(instance_name.c_str(), parms);
      m_instance_map[p_ci->name()] = p_ci;
      return *p_ci;
    }
    else {
      ostringstream oss;
      oss << "pif2sb_bridge_plugin_interface::create_module() called with unknown plugin_name of \"" << plugin_name << "\"";
      throw xtsc_exception(oss.str());
    }
  }


  virtual xtsc_connection_interface *get_connection_interface(const string& hierarchical_name) {
    if (m_instance_map.find(hierarchical_name) == m_instance_map.end()) {
      ostringstream oss;
      oss << "pif2sb_bridge_plugin_interface::get_connection_interface() called with unknown hierarchical_name of \""
          << hierarchical_name << "\"";
      throw xtsc_exception(oss.str());
    }
    return m_instance_map[hierarchical_name];
  }


  map<string, xtsc_connection_interface*> m_instance_map;  ///< Map hierarchical name to connection interface
};




/*
 * Create the known xtsc_get_plugin_interface symbol and make it visible.
 * See xtsc_get_plugin_interface_t in xtsc.h or xtsc_rm.pdf.
 */
extern "C" 
#if defined(_WIN32)
__declspec(dllexport)
#endif
xtsc_plugin_interface& xtsc_get_plugin_interface() {
  static pif2sb_bridge_plugin_interface *p_pif2sb_bridge_plugin = 0;
  if (!p_pif2sb_bridge_plugin) {
    p_pif2sb_bridge_plugin = new pif2sb_bridge_plugin_interface;
  }
  return *p_pif2sb_bridge_plugin;
}


