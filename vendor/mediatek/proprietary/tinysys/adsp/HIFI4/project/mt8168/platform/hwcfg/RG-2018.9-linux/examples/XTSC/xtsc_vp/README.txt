Support Matrix Information:

Note: Support for static libraries has been removed from XTSC_VP.

Note: XTENSA_SW_TOOLS is an environment variable pointing to your XtensaTools location.  For example:
      /home/joeuser/xplorer/RG-2018.9/XtDevTools/install/tools/RG-2018.9-linux/XtensaTools

Linux Host Platform:  RHEL6, RHEL7

Host Compiler: 64-bit gcc

Host Debugger: Any gdb-compatible debugger

XTSC_VP IP Model Library location for Virtualizer Studio (Library Manager tab):
  ${XTENSA_SW_TOOLS}/misc/xtsc_vp/xtsc_vp_sh.xml

Binary Installation Location (Path Requirements):

xt-genvpmodel:
  $XTENSA_SW_TOOLS/bin/xt-genvpmodel

Synopsys Virtualizer M-2017.06 with 64-bit gcc 4.8:
  Shared libraries:
    $XTENSA_SW_TOOLS/lib64/iss-GCC-4.8/CoWare-M-2017.06/libxtsc_sh.so
    $XTENSA_SW_TOOLS/lib64/iss-GCC-4.8/CoWare-M-2017.06/libxtsc_comp_sh.so

Synopsys Virtualizer N-2017.12-1 with 64-bit gcc 4.8:
  Shared libraries:
    $XTENSA_SW_TOOLS/lib64/iss-GCC-4.8/CoWare-N-2017.12-1/libxtsc_sh.so
    $XTENSA_SW_TOOLS/lib64/iss-GCC-4.8/CoWare-N-2017.12-1/libxtsc_comp_sh.so

Synopsys Virtualizer N-2017.12-1 with 64-bit gcc 5.2:
  Shared libraries:
    $XTENSA_SW_TOOLS/lib64/iss-GCC-5.2/CoWare-N-2017.12-1/libxtsc_sh.so
    $XTENSA_SW_TOOLS/lib64/iss-GCC-5.2/CoWare-N-2017.12-1/libxtsc_comp_sh.so

Note: If you get link errors like this "unrecognized relocation (0x2a) in section .text",
      then you may need to define the SNPS_VP_UBUNTU environment variable.  Please consult
      the PA Virtualizer support team at Synopsys for more information.

