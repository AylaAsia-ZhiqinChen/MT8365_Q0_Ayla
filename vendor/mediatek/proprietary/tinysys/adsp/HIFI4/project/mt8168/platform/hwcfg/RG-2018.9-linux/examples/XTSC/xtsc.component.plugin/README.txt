Overview:

This project makes it easy to copy an XTSC module from the XTSC component library and
then rename, modify, and re-build it as an XTSC plugin.  This might be done to make a
small functionality change to an existing XTSC module or as a first step in developing a
custom module.  See Chapter 31, "Custom Module Development" in the XTSC User's Guide
(xtsc_ug.pdf).



Instructions:

Note: In this file, MODULE refers to the original XTSC component library module name and
      MODULE_ext refers to your local renamed version.  In other files in this directory,
      MODULE refers to the local renamed version.


Part I:  Copy, rename, and do a test rebuild of the XTSC component library module as a
         plugin without adding your custom functionality yet.

1. Set up your environment for XtensaTools and for a supported C++ compiler.  See sc_main
   in ../README.txt and ../Makefile.include.

2. Copy the MODULE.cpp and MODULE.h files for the desired MODULE from their installed
   location to the local directory (the directory containing this README.txt file).  The
   XTSC component modules sources are installed at:
        $XTENSA_SW_TOOLS/src/xtsc/MODULE.cpp
        $XTENSA_SW_TOOLS/src/xtsc/xtsc/MODULE.h

3. Rename the local copies as desired (for example, add "_ext" to the module name):
   Linux:
        mv  MODULE.cpp MODULE_ext.cpp
        mv  MODULE.h   MODULE_ext.h
   MS Windows:
        ren MODULE.cpp MODULE_ext.cpp
        ren MODULE.h   MODULE_ext.h

4. Edit the local copies of MODULE_ext.cpp and MODULE_ext.h as needed to make them build
   as a plugin module that does not conflict with the original XTSC component library
   module:
   A) In MODULE_ext.h:
      1) Do a global search and conditional replace of MODULE with MODULE_ext.  The two
         whole words you need to do a replace in are MODULE and MODULE_parms.
         Caution:  The replace is conditional because in some modules there will be 
         occurrances of the MODULE text string that should not be replaced.  For example,
         in the xtsc_wire module you should replace the whole word xtsc_wire with
         xtsc_wire_ext and you should replace xtsc_wire_parms with xtsc_wire_ext_parms,
         but you should not replace the string "xtsc_wire" in the words xtsc_wire_source,
         xtsc_wire_logic, xtsc_wire_read_if, and xtsc_wire_write_if.  Other XTSC modules
         that must take care to do a conditional replace include xtsc_lookup,
         xtsc_memory, and xtsc_queue,
      2) Fix the include guard lines (1st two lines and the last line in the file) to
         reflect the new name:
         Before:
           #ifndef _MODULE_H_
           #define _MODULE_H_
           ...
           #endif  // _MODULE_H_
         After:
           #ifndef _MODULE_EXT_H_
           #define _MODULE_EXT_H_
           ...
           #endif  // _MODULE_EXT_H_
      3) Add the following 2 lines just after the last #include line (this is okay to do
         on Linux but it is actually only necessary on MS Windows):
           #undef  XTSC_COMP_API
           #define XTSC_COMP_API
   B) In MODULE_ext.cpp:
      1) Do a global search and conditional replace of MODULE with MODULE_ext.
         See Caution in 4-A-1 above.
      2) Change the #include line for MODULE_ext.h to not have the xtsc subdirectory:
         Before:
           #include <xtsc/MODULE_ext.h>
         After:
           #include "MODULE_ext.h"

5. Edit the "export MODULE" line in Makefile to specify the desired renamed MODULE_ext.

6. Edit the help output in module_plugin_interface.cpp (look for the TODO line).

7. Do a test build of the plugin shared library (*.so on Linux, *.dll on MS Windows):
   Linux:
        make clean library
   MS Windows:
        xt-make clean library

8. Do a simple run-time test using xtsc-run:
   Note:  A successful test is indicated by the module parameters and their values being
          listed.  In many cases, the parameter value listing will be followed by an
          error message and/or an exception regarding an illegal parameter value or
          "complete binding failed".  These kinds of errors are to be expected here
          because system.inc does not set any module parameters and in some cases the
          default value is illegal nor does system.inc have any connect commands in it
          to do port binding.  See step 9 for a way to do a more complete test.
   Linux:
        make xtsc-run
   MS Windows:
        xt-make xtsc-run

9. If desired do a full test that the renamed plugin can be used in lieu of original
   XTSC component library module by editing an XTSC example that uses that module to
   instead use the functionally identical renamed plugin version.  Typically, this can
   be done by editing the MODULE.inc file in the chosen XTSC example direcory as follows:
   A) Add a -load_library command at the beginning to load the plugin library built in
      step 6 above.
   B) Replace the -set_XXX_parm commands with -set_MODULE_ext_parm (where XXX is the
      xtsc-run name of MODULE).
   C) Replace the -create_XXX commands with -create_MODULE_ext.


Part II:  Apply your custom functionality, rebuild, and test.

10. Edit the local copies of MODULE_ext.cpp and MODULE_ext.h as desired for your custom
    functionality.

11. Repeat steps 6 through 10 as needed to test and debug your custom functionality.
    Depending on your custom functionality you may need to modify the example system in
    step 9.

