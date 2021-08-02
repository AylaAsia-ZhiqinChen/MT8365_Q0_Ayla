IMPORTANT:  When linking an XTSC plugin, be sure to use the shared library version of
            SystemC to avoid bizarre run-time behavior when your plugin is using one
            copy of the SystemC kernel while the rest of the simulation is using another.
            The shared libary version has a "_sh" or "_shd" suffix:
                MS Windows Release:  systemc_sh.lib   (NOT systemc.lib)
                MS Windows Debug:    systemc_shd.lib  (NOT systemcd.lib)
                Linux:               -l systemc_sh    (NOT -l systemc)

This example is a follow-on to the simple_memory example.  That example is meant to
illustrate Xtensa TLM module development.  This example goes farther and shows how to
add the model to xtsc-run using the generic connection and plugin capability of XTSC and
xtsc-run.

The differences between the two examples can be easily seen by doing a diff on the header files
and on the cpp files.  For example (if desired, please substitute your favorite differencing
tool in place of gvim):
    $ cd <xtsc_examples_root>
    $ gvim -d simple_memory/simple_memory.h   simple_memory.plugin/simple_memory.h
    $ gvim -d simple_memory/simple_memory.cpp simple_memory.plugin/simple_memory.plugin.cpp

For more information about xtsc-run plugins, please consult the XTSC User's Guide
chapters on xtsc-run and module development (xtsc_ug.pdf) and the xtsc-run reference
manual available using the "xtsc-run --manual" command (search the output for
"load_library" and "plugin").  Also see the xtsc_connection_interface and
xtsc_plugin_interface class documentation in xtsc_ug.pdf and in the XTSC Reference Manual
(xtsc_rm.pdf) or in xtsc.h.

Note: Another example that shows how to add models to xtsc-run is the pif2sb_bridge.plugin
example.  That example goes farther then this one in that it shows how to add models
that use non-Xtensa TLM interfaces (that is, user-defined interfaces) that are unknown
to XTSC and xtsc-run.

Note:  If you are modifying an XTSC component library module, please see
	../xtsc.component.plugin/README.txt

Note For Microsoft Visual Studio users:
When you open the solution file (*.sln) in MSVC, you will see two projects in the Solution
Explorer frame:
  simple_memory
  simple_memory.plugin
For proper build order, it is recommended to select simple_memory as the start-up project.
This can be done by selecting simple_memory in the Solution Explorer frame and then
following the drop-down menu sequence:   Project>Set as StartUp Project
