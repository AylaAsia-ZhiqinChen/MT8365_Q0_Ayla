/* Default linker script, for normal executables */
ENTRY(_start)

PHDRS
{
  headers PT_PHDR PHDRS ;
  text PT_LOAD FILEHDR PHDRS;
  data PT_LOAD ;
  dynamic PT_DYNAMIC ;
}



SECTIONS
{
  . = SIZEOF_HEADERS;
  /* Read-only sections, merged into text segment: */
  .interp         : { *(.interp) } :text
  .hash           : { *(.hash) } :text
  .dynsym         : { *(.dynsym) } :text
  .dynstr         : { *(.dynstr) } :text
  .gnu.version    : { *(.gnu.version) } :text
  .gnu.version_d  : { *(.gnu.version_d) } :text
  .gnu.version_r  : { *(.gnu.version_r) } :text
  .rel.init       : { *(.rel.init) } :text
  .rela.init      : { *(.rela.init) } :text
  .rel.text       : { *(.rel.text .rel.text.* .rel.gnu.linkonce.t.*) } :text
  .rela.text      : { *(.rela.text .rela.text.* .rela.gnu.linkonce.t.*) } :text
  .rel.fini       : { *(.rel.fini) } :text
  .rela.fini      : { *(.rela.fini) } :text
  .rel.rodata     : { *(.rel.rodata .rel.rodata.* .rel.gnu.linkonce.r.*) } :text
  .rela.rodata    : { *(.rela.rodata .rela.rodata.* .rela.gnu.linkonce.r.*) } :text
  .rel.data.rel.ro   : { *(.rel.data.rel.ro*) } :text
  .rela.data.rel.ro   : { *(.rel.data.rel.ro*) } :text
  .rel.data       : { *(.rel.data .rel.data.* .rel.gnu.linkonce.d.*) } :text
  .rela.data      : { *(.rela.data .rela.data.* .rela.gnu.linkonce.d.*) } :text
  .rel.tdata	  : { *(.rel.tdata .rel.tdata.* .rel.gnu.linkonce.td.*) } :text
  .rela.tdata	  : { *(.rela.tdata .rela.tdata.* .rela.gnu.linkonce.td.*) } :text
  .rel.tbss	  : { *(.rel.tbss .rel.tbss.* .rel.gnu.linkonce.tb.*) } :text
  .rela.tbss	  : { *(.rela.tbss .rela.tbss.* .rela.gnu.linkonce.tb.*) } :text
  .rel.ctors      : { *(.rel.ctors) } :text
  .rela.ctors     : { *(.rela.ctors) } :text
  .rel.dtors      : { *(.rel.dtors) } :text
  .rela.dtors     : { *(.rela.dtors) } :text
  .rel.got        : { *(.rel.got) } :text
  .rela.got       : { *(.rela.got) } :text
  .rel.bss        : { *(.rel.bss .rel.bss.* .rel.gnu.linkonce.b.*) } :text
  .rela.bss       : { *(.rela.bss .rela.bss.* .rela.gnu.linkonce.b.*) } :text
  .rel.plt        : { *(.rel.plt) } :text
  .rela.plt       : { *(.rela.plt) } :text
  .text           :
  {
    *(.got.plt* .plt*)
    KEEP (*(.init.literal))
    KEEP (*(.init))
    *(.literal .text .stub .literal.* .text.* .gnu.linkonce.literal.* .gnu.linkonce.t.*.literal .gnu.linkonce.t.*)
    KEEP (*(.text.*personality*))
    /* .gnu.warning sections are handled specially by elf32.em.  */
    *(.gnu.warning)
    KEEP (*(.fini.literal))
    KEEP (*(.fini))
  }  :text
  PROVIDE (__etext = .);
  PROVIDE (_etext = .);
  PROVIDE (etext = .);
  .rodata         : { *(.rodata .rodata.* .gnu.linkonce.r.*) } :data
  .rodata1        : { *(.rodata1) } :data
  .got.loc        : { *(.got.loc) } :data
  .xt_except_table   : { KEEP (*(.xt_except_table)) } :data
  .eh_frame_hdr : { *(.eh_frame_hdr) } :data
  .eh_frame       : ONLY_IF_RO { KEEP (*(.eh_frame)) } :data
  .gcc_except_table   : ONLY_IF_RO { KEEP (*(.gcc_except_table)) *(.gcc_except_table.*) } :data
  /* Adjust the address for the data segment.  We want to adjust up to
     the same address within the page on the next page up.  */
  . = ALIGN(0x20000000) + (. & (0x20000000 - 1));
  /* Exception handling  */
  .eh_frame       : ONLY_IF_RW { KEEP (*(.eh_frame)) } :data
  .gcc_except_table   : ONLY_IF_RW { KEEP (*(.gcc_except_table)) *(.gcc_except_table.*) } :data
  /* Thread Local Storage sections  */
  .tdata	  : { *(.tdata .tdata.* .gnu.linkonce.td.*) } :data
  .tbss		  : { *(.tbss .tbss.* .gnu.linkonce.tb.*) *(.tcommon) } :data
  /* Ensure the __preinit_array_start label is properly aligned.  We
     could instead move the label definition inside the section, but
     the linker would then create the section even if it turns out to
     be empty, which isn't pretty.  */
  . = ALIGN(32 / 8);
  PROVIDE (__preinit_array_start = .);
  .preinit_array     : { KEEP (*(.preinit_array)) }
  PROVIDE (__preinit_array_end = .);
  PROVIDE (__init_array_start = .);
  .init_array     : { KEEP (*(.init_array)) }
  PROVIDE (__init_array_end = .);
  PROVIDE (__fini_array_start = .);
  .fini_array     : { KEEP (*(.fini_array)) }
  PROVIDE (__fini_array_end = .);
  .ctors          :
  {
    /* gcc uses crtbegin.o to find the start of
       the constructors, so we make sure it is
       first.  Because this is a wildcard, it
       doesn't matter if the user does not
       actually link against crtbegin.o; the
       linker won't look for a file to match a
       wildcard.  The wildcard also means that it
       doesn't matter which directory crtbegin.o
       is in.  */
    KEEP (*crtbegin*.o(.ctors))
    /* We don't want to include the .ctor section from
       from the crtend.o file until after the sorted ctors.
       The .ctor section from the crtend file contains the
       end of ctors marker and it must be last */
    KEEP (*(EXCLUDE_FILE (*crtend*.o ) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
  } :data
  .dtors          :
  {
    KEEP (*crtbegin*.o(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend*.o ) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
  } :data
  .jcr            : { KEEP (*(.jcr)) } :data
  .data.rel.ro : { *(.data.rel.ro.local) *(.data.rel.ro*) } :data
  .got            : { *(.got) } :data
  .data           :
  {
    *(.data .data.* .gnu.linkonce.d.*)
    KEEP (*(.gnu.linkonce.d.*personality*))
    SORT(CONSTRUCTORS)
  } :data
  .data1          : { *(.data1) } :data
  .xt_except_desc   :
  {
    *(.xt_except_desc .gnu.linkonce.h.*)
    *(.xt_except_desc_end)
  } :data
  _edata = .;
  PROVIDE (edata = .);
  __bss_start = .;
  .bss            :
  {
   *(.dynbss)
   *(.bss .bss.* .gnu.linkonce.b.*)
   *(COMMON)
   /* Align here to ensure that the .bss section occupies space up to
      _end.  Align after .bss to ensure correct alignment even if the
      .bss section disappears because there are no input sections.  */
   . = ALIGN(32 / 8);
  } :data
  .dynamic        : 
  {
    *(.dynamic) 
  } :dynamic :data
  . = ALIGN(32 / 8);
  _end = .;
  PROVIDE (end = .);
  /* Stabs debugging sections.  */
  .stab          0 : { *(.stab) }
  .stabstr       0 : { *(.stabstr) }
  .stab.excl     0 : { *(.stab.excl) }
  .stab.exclstr  0 : { *(.stab.exclstr) }
  .stab.index    0 : { *(.stab.index) }
  .stab.indexstr 0 : { *(.stab.indexstr) }
  .comment       0 : { *(.comment) }
  /* DWARF debug sections.
     Symbols in the DWARF debugging sections are relative to the beginning
     of the section so we begin them at 0.  */
  /* DWARF 1 */
  .debug          0 : { *(.debug) }
  .line           0 : { *(.line) }
  /* GNU DWARF 1 extensions */
  .debug_srcinfo  0 : { *(.debug_srcinfo) }
  .debug_sfnames  0 : { *(.debug_sfnames) }
  /* DWARF 1.1 and DWARF 2 */
  .debug_aranges  0 : { *(.debug_aranges) }
  .debug_pubnames 0 : { *(.debug_pubnames) }
  /* DWARF 2 */
  .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
  .debug_abbrev   0 : { *(.debug_abbrev) }
  .debug_line     0 : { *(.debug_line) }
  .debug_frame    0 : { *(.debug_frame) }
  .debug_str      0 : { *(.debug_str) }
  .debug_loc      0 : { *(.debug_loc) }
  .debug_macinfo  0 : { *(.debug_macinfo) }
  /* SGI/MIPS DWARF 2 extensions */
  .debug_weaknames 0 : { *(.debug_weaknames) }
  .debug_funcnames 0 : { *(.debug_funcnames) }
  .debug_typenames 0 : { *(.debug_typenames) }
  .debug_varnames  0 : { *(.debug_varnames) }
  .xt.lit         0 : { *(.xt.lit .xt.lit.* .gnu.linkonce.p.*) }
  .xt.insn        0 : { *(.xt.insn .gnu.linkonce.x.*) }
  .xt.prop        0 : { *(.xt.prop .gnu.linkonce.prop.*) }
  /DISCARD/ : { *(.note.GNU-stack) }
}
