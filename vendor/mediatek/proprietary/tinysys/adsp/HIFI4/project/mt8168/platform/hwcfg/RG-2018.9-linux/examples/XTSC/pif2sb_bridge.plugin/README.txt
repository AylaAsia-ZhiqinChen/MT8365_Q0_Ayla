IMPORTANT:  When linking an XTSC plugin, be sure to use the shared library version of
            SystemC to avoid bizarre run-time behavior when your plugin is using one
            copy of the SystemC kernel while the rest of the simulation is using another.
            The shared libary version has a "_sh" or "_shd" suffix:
                MS Windows Release:  systemc_sh.lib   (NOT systemc.lib)
                MS Windows Debug:    systemc_shd.lib  (NOT systemcd.lib)
                Linux:               -l systemc_sh    (NOT -l systemc)

This example is a follow-on to the pif2sb_bridge example.  That example is meant to
illustrate module development of components that can bridge between an Xtensa TLM
interface and some other interface.

This example goes farther and shows how to add models to xtsc-run without having to
modify the xtsc-run source code.  This uses the plugin capability of XTSC and xtsc-run.

Another example that shows how to add models to xtsc-run is the simple_memory.plugin
example.  This example goes farther then that one in that this example also shows how
to add models that use custom interfaces (user-defined interfaces) that are unknown
to XTSC and xtsc-run.

The main addition of this example is the pif2sb_bridge.plugin.cpp file.  In that file
each of the models is given its own implementation of the xtsc_connection_interface so
that their port binding can be handled using the generic connection mechanism of XTSC
and xtsc-run.  In addition, the xtsc_plugin_interface is implemented once and that
implementation exposes all the models to xtsc-run.

For more information about xtsc-run plugins, please consult the pif2sb_bridge.plugin.cpp
file, the XTSC User's Guide chapters on xtsc-run and module development (xtsc_ug.pdf),
and the xtsc-run reference manual available using the "xtsc-run --manual" command
(search the output for "load_library" and "plugin").  Also see the xtsc_connection_interface
and xtsc_plugin_interface class documentation in xtsc_ug.pdf and in the XTSC Reference Manual
(xtsc_rm.pdf) or in xtsc.h.

Note:  If you are modifying an XTSC component library module, please see
	../xtsc.component.plugin/README.txt

Note For Microsoft Visual Studio users:
When you open the solution file (*.sln) in MSVC, you will see two projects in the Solution
Explorer frame:
  pif2sb_bridge
  pif2sb_bridge.plugin
For proper build order, it is recommended to select pif2sb_bridge as the start-up project.
This can be done by selecting pif2sb_bridge in the Solution Explorer frame and then
following the drop-down menu sequence:   Project>Set as StartUp Project
