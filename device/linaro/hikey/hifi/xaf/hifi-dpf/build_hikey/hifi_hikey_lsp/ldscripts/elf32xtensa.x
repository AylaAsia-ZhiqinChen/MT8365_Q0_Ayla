/* This linker script generated from xt-genldscripts.tpp for LSP hifi_hikey_lsp */
/* Linker Script for default link */
MEMORY
{
  sram0_seg :                         	org = 0xC0000000, len = 0x408000
  old_vlpd_seg :                      	org = 0xC0408000, len = 0x10000
  efr_fr_hr_vlpd_seg :                	org = 0xC0418000, len = 0x10000
  amr_vlpd_seg :                      	org = 0xC0428000, len = 0x10000
  amrwb_vlpd_seg :                    	org = 0xC0438000, len = 0x10000
  evrc_evrcb_vlpt_seg :               	org = 0xC0448000, len = 0x48000
  efr_fr_hr_vlpt_seg :                	org = 0xC0490000, len = 0x30000
  amr_vlpt_seg :                      	org = 0xC04C0000, len = 0x20000
  amrwb_vlpt_seg :                    	org = 0xC04E0000, len = 0x30000
  vlpt_seg :                          	org = 0xC0510000, len = 0x48000
  vlpd_seg :                          	org = 0xC0558000, len = 0x20000
  ulpp_seg :                          	org = 0xC0578000, len = 0x40000
  dtsv3_seg :                         	org = 0xC05B8000, len = 0x20000
  dtsv4_seg :                         	org = 0xC05D8000, len = 0x28000
  dram0_0_seg :                       	org = 0xE8058000, len = 0x28000
  iram0_0_seg :                       	org = 0xE8080000, len = 0x300
  iram0_1_seg :                       	org = 0xE8080300, len = 0x100
  iram0_2_seg :                       	org = 0xE8080400, len = 0x178
  iram0_3_seg :                       	org = 0xE8080578, len = 0x8
  iram0_4_seg :                       	org = 0xE8080580, len = 0x38
  iram0_5_seg :                       	org = 0xE80805B8, len = 0x8
  iram0_6_seg :                       	org = 0xE80805C0, len = 0x38
  iram0_7_seg :                       	org = 0xE80805F8, len = 0x8
  iram0_8_seg :                       	org = 0xE8080600, len = 0x38
  iram0_9_seg :                       	org = 0xE8080638, len = 0x8
  iram0_10_seg :                      	org = 0xE8080640, len = 0x38
  iram0_11_seg :                      	org = 0xE8080678, len = 0x48
  iram0_12_seg :                      	org = 0xE80806C0, len = 0x38
  iram0_13_seg :                      	org = 0xE80806F8, len = 0x8
  iram0_14_seg :                      	org = 0xE8080700, len = 0x38
  iram0_15_seg :                      	org = 0xE8080738, len = 0x8
  iram0_16_seg :                      	org = 0xE8080740, len = 0x38
  iram0_17_seg :                      	org = 0xE8080778, len = 0x48
  iram0_18_seg :                      	org = 0xE80807C0, len = 0x40
  iram0_19_seg :                      	org = 0xE8080800, len = 0xB800
}

PHDRS
{
  sram0_phdr PT_LOAD;
  sram0_bss_phdr PT_LOAD;
  old_vlpd_phdr PT_LOAD;
  old_vlpd_bss_phdr PT_LOAD;
  efr_fr_hr_vlpd_phdr PT_LOAD;
  efr_fr_hr_vlpd_bss_phdr PT_LOAD;
  amr_vlpd_phdr PT_LOAD;
  amr_vlpd_bss_phdr PT_LOAD;
  amrwb_vlpd_phdr PT_LOAD;
  amrwb_vlpd_bss_phdr PT_LOAD;
  evrc_evrcb_vlpt_phdr PT_LOAD;
  efr_fr_hr_vlpt_phdr PT_LOAD;
  amr_vlpt_phdr PT_LOAD;
  amrwb_vlpt_phdr PT_LOAD;
  vlpt_phdr PT_LOAD;
  vlpd_phdr PT_LOAD;
  ulpp_phdr PT_LOAD;
  ulpp_bss_phdr PT_LOAD;
  dtsv3_phdr PT_LOAD;
  dtsv3_bss_phdr PT_LOAD;
  dtsv4_phdr PT_LOAD;
  dtsv4_bss_phdr PT_LOAD;
  dram0_0_phdr PT_LOAD;
  dram0_0_bss_phdr PT_LOAD;
  iram0_0_phdr PT_LOAD;
  iram0_1_phdr PT_LOAD;
  iram0_2_phdr PT_LOAD;
  iram0_3_phdr PT_LOAD;
  iram0_4_phdr PT_LOAD;
  iram0_5_phdr PT_LOAD;
  iram0_6_phdr PT_LOAD;
  iram0_7_phdr PT_LOAD;
  iram0_8_phdr PT_LOAD;
  iram0_9_phdr PT_LOAD;
  iram0_10_phdr PT_LOAD;
  iram0_11_phdr PT_LOAD;
  iram0_12_phdr PT_LOAD;
  iram0_13_phdr PT_LOAD;
  iram0_14_phdr PT_LOAD;
  iram0_15_phdr PT_LOAD;
  iram0_16_phdr PT_LOAD;
  iram0_17_phdr PT_LOAD;
  iram0_18_phdr PT_LOAD;
  iram0_19_phdr PT_LOAD;
}


/*  Default entry point:  */
ENTRY(_ResetVector)

/*  Memory boundary addresses:  */
_memmap_mem_iram0_start = 0xe8080000;
_memmap_mem_iram0_end   = 0xe808c000;
_memmap_mem_dram0_start = 0xe8058000;
_memmap_mem_dram0_end   = 0xe8080000;
_memmap_mem_sram_start = 0xc0000000;
_memmap_mem_sram_end   = 0xc0600000;

/*  Memory segment boundary addresses:  */
_memmap_seg_sram0_start = 0xc0000000;
_memmap_seg_sram0_max   = 0xc0408000;
_memmap_seg_old_vlpd_start = 0xc0408000;
_memmap_seg_old_vlpd_max   = 0xc0418000;
_memmap_seg_efr_fr_hr_vlpd_start = 0xc0418000;
_memmap_seg_efr_fr_hr_vlpd_max   = 0xc0428000;
_memmap_seg_amr_vlpd_start = 0xc0428000;
_memmap_seg_amr_vlpd_max   = 0xc0438000;
_memmap_seg_amrwb_vlpd_start = 0xc0438000;
_memmap_seg_amrwb_vlpd_max   = 0xc0448000;
_memmap_seg_evrc_evrcb_vlpt_start = 0xc0448000;
_memmap_seg_evrc_evrcb_vlpt_max   = 0xc0490000;
_memmap_seg_efr_fr_hr_vlpt_start = 0xc0490000;
_memmap_seg_efr_fr_hr_vlpt_max   = 0xc04c0000;
_memmap_seg_amr_vlpt_start = 0xc04c0000;
_memmap_seg_amr_vlpt_max   = 0xc04e0000;
_memmap_seg_amrwb_vlpt_start = 0xc04e0000;
_memmap_seg_amrwb_vlpt_max   = 0xc0510000;
_memmap_seg_vlpt_start = 0xc0510000;
_memmap_seg_vlpt_max   = 0xc0558000;
_memmap_seg_vlpd_start = 0xc0558000;
_memmap_seg_vlpd_max   = 0xc0578000;
_memmap_seg_ulpp_start = 0xc0578000;
_memmap_seg_ulpp_max   = 0xc05b8000;
_memmap_seg_dtsv3_start = 0xc05b8000;
_memmap_seg_dtsv3_max   = 0xc05d8000;
_memmap_seg_dtsv4_start = 0xc05d8000;
_memmap_seg_dtsv4_max   = 0xc0600000;
_memmap_seg_dram0_0_start = 0xe8058000;
_memmap_seg_dram0_0_max   = 0xe8080000;
_memmap_seg_iram0_0_start = 0xe8080000;
_memmap_seg_iram0_0_max   = 0xe8080300;
_memmap_seg_iram0_1_start = 0xe8080300;
_memmap_seg_iram0_1_max   = 0xe8080400;
_memmap_seg_iram0_2_start = 0xe8080400;
_memmap_seg_iram0_2_max   = 0xe8080578;
_memmap_seg_iram0_3_start = 0xe8080578;
_memmap_seg_iram0_3_max   = 0xe8080580;
_memmap_seg_iram0_4_start = 0xe8080580;
_memmap_seg_iram0_4_max   = 0xe80805b8;
_memmap_seg_iram0_5_start = 0xe80805b8;
_memmap_seg_iram0_5_max   = 0xe80805c0;
_memmap_seg_iram0_6_start = 0xe80805c0;
_memmap_seg_iram0_6_max   = 0xe80805f8;
_memmap_seg_iram0_7_start = 0xe80805f8;
_memmap_seg_iram0_7_max   = 0xe8080600;
_memmap_seg_iram0_8_start = 0xe8080600;
_memmap_seg_iram0_8_max   = 0xe8080638;
_memmap_seg_iram0_9_start = 0xe8080638;
_memmap_seg_iram0_9_max   = 0xe8080640;
_memmap_seg_iram0_10_start = 0xe8080640;
_memmap_seg_iram0_10_max   = 0xe8080678;
_memmap_seg_iram0_11_start = 0xe8080678;
_memmap_seg_iram0_11_max   = 0xe80806c0;
_memmap_seg_iram0_12_start = 0xe80806c0;
_memmap_seg_iram0_12_max   = 0xe80806f8;
_memmap_seg_iram0_13_start = 0xe80806f8;
_memmap_seg_iram0_13_max   = 0xe8080700;
_memmap_seg_iram0_14_start = 0xe8080700;
_memmap_seg_iram0_14_max   = 0xe8080738;
_memmap_seg_iram0_15_start = 0xe8080738;
_memmap_seg_iram0_15_max   = 0xe8080740;
_memmap_seg_iram0_16_start = 0xe8080740;
_memmap_seg_iram0_16_max   = 0xe8080778;
_memmap_seg_iram0_17_start = 0xe8080778;
_memmap_seg_iram0_17_max   = 0xe80807c0;
_memmap_seg_iram0_18_start = 0xe80807c0;
_memmap_seg_iram0_18_max   = 0xe8080800;
_memmap_seg_iram0_19_start = 0xe8080800;
_memmap_seg_iram0_19_max   = 0xe808c000;

_rom_store_table = 0;
PROVIDE(_memmap_vecbase_reset = 0xe8080400);
PROVIDE(_memmap_reset_vector = 0xe8080000);
/* Various memory-map dependent cache attribute settings: */
_memmap_cacheattr_wb_base = 0x44000000;
_memmap_cacheattr_wt_base = 0x11000000;
_memmap_cacheattr_bp_base = 0x22000000;
_memmap_cacheattr_unused_mask = 0x00FFFFFF;
_memmap_cacheattr_wb_trapnull = 0x4422222F;
_memmap_cacheattr_wba_trapnull = 0x4422222F;
_memmap_cacheattr_wbna_trapnull = 0x5522222F;
_memmap_cacheattr_wt_trapnull = 0x1122222F;
_memmap_cacheattr_bp_trapnull = 0x2222222F;
_memmap_cacheattr_wb_strict = 0x44FFFFFF;
_memmap_cacheattr_wt_strict = 0x11FFFFFF;
_memmap_cacheattr_bp_strict = 0x22FFFFFF;
_memmap_cacheattr_wb_allvalid = 0x44222222;
_memmap_cacheattr_wt_allvalid = 0x11222222;
_memmap_cacheattr_bp_allvalid = 0x22222222;
PROVIDE(_memmap_cacheattr_reset = _memmap_cacheattr_wb_trapnull);

SECTIONS
{

  .vlpd.rodata : ALIGN(4)
  {
    _vlpd_rodata_start = ABSOLUTE(.);
    *(.vlpd.rodata)
    _vlpd_rodata_end = ABSOLUTE(.);
  } >old_vlpd_seg :old_vlpd_phdr

  .vlpd.data : ALIGN(4)
  {
    _vlpd_data_start = ABSOLUTE(.);
    *(.vlpd.data)
    _vlpd_data_end = ABSOLUTE(.);
  } >old_vlpd_seg :old_vlpd_phdr

  .vlpd.bss (NOLOAD) : ALIGN(8)
  {
    . = ALIGN (8);
    _vlpd_bss_start = ABSOLUTE(.);
    *(.vlpd.bss)
    . = ALIGN (8);
    _vlpd_bss_end = ABSOLUTE(.);
    _memmap_seg_old_vlpd_end = ALIGN(0x8);
  } >old_vlpd_seg :old_vlpd_bss_phdr

  .efr_fr_hr_vlpd.rodata : ALIGN(4)
  {
    _efr_fr_hr_vlpd_rodata_start = ABSOLUTE(.);
    *(.efr_fr_hr_vlpd.rodata)
    _efr_fr_hr_vlpd_rodata_end = ABSOLUTE(.);
  } >efr_fr_hr_vlpd_seg :efr_fr_hr_vlpd_phdr

  .efr_fr_hr_vlpd.data : ALIGN(4)
  {
    _efr_fr_hr_vlpd_data_start = ABSOLUTE(.);
    *(.efr_fr_hr_vlpd.data)
    _efr_fr_hr_vlpd_data_end = ABSOLUTE(.);
  } >efr_fr_hr_vlpd_seg :efr_fr_hr_vlpd_phdr

  .efr_fr_hr_vlpd.bss (NOLOAD) : ALIGN(8)
  {
    . = ALIGN (8);
    _efr_fr_hr_vlpd_bss_start = ABSOLUTE(.);
    *(.efr_fr_hr_vlpd.bss)
    . = ALIGN (8);
    _efr_fr_hr_vlpd_bss_end = ABSOLUTE(.);
    _memmap_seg_efr_fr_hr_vlpd_end = ALIGN(0x8);
  } >efr_fr_hr_vlpd_seg :efr_fr_hr_vlpd_bss_phdr

  .amr_vlpd.rodata : ALIGN(4)
  {
    _amr_vlpd_rodata_start = ABSOLUTE(.);
    *(.amr_vlpd.rodata)
    _amr_vlpd_rodata_end = ABSOLUTE(.);
  } >amr_vlpd_seg :amr_vlpd_phdr

  .amr_vlpd.data : ALIGN(4)
  {
    _amr_vlpd_data_start = ABSOLUTE(.);
    *(.amr_vlpd.data)
    _amr_vlpd_data_end = ABSOLUTE(.);
  } >amr_vlpd_seg :amr_vlpd_phdr

  .amr_vlpd.bss (NOLOAD) : ALIGN(8)
  {
    . = ALIGN (8);
    _amr_vlpd_bss_start = ABSOLUTE(.);
    *(.amr_vlpd.bss)
    . = ALIGN (8);
    _amr_vlpd_bss_end = ABSOLUTE(.);
    _memmap_seg_amr_vlpd_end = ALIGN(0x8);
  } >amr_vlpd_seg :amr_vlpd_bss_phdr

  .amrwb_vlpd.rodata : ALIGN(4)
  {
    _amrwb_vlpd_rodata_start = ABSOLUTE(.);
    *(.amrwb_vlpd.rodata)
    _amrwb_vlpd_rodata_end = ABSOLUTE(.);
  } >amrwb_vlpd_seg :amrwb_vlpd_phdr

  .amrwb_vlpd.data : ALIGN(4)
  {
    _amrwb_vlpd_data_start = ABSOLUTE(.);
    *(.amrwb_vlpd.data)
    _amrwb_vlpd_data_end = ABSOLUTE(.);
  } >amrwb_vlpd_seg :amrwb_vlpd_phdr

  .amrwb_vlpd.bss (NOLOAD) : ALIGN(8)
  {
    . = ALIGN (8);
    _amrwb_vlpd_bss_start = ABSOLUTE(.);
    *(.amrwb_vlpd.bss)
    . = ALIGN (8);
    _amrwb_vlpd_bss_end = ABSOLUTE(.);
    _memmap_seg_amrwb_vlpd_end = ALIGN(0x8);
  } >amrwb_vlpd_seg :amrwb_vlpd_bss_phdr

  .evrc_evrcb_vlpt.text : ALIGN(4)
  {
    _evrc_evrcb_vlpt_text_start = ABSOLUTE(.);
    *(.evrc_evrcb_vlpt.literal .evrc_evrcb_vlpt.text)
    _evrc_evrcb_vlpt_text_end = ABSOLUTE(.);
    _memmap_seg_evrc_evrcb_vlpt_end = ALIGN(0x8);
  } >evrc_evrcb_vlpt_seg :evrc_evrcb_vlpt_phdr

  .efr_fr_hr_vlpt.text : ALIGN(4)
  {
    _efr_fr_hr_vlpt_text_start = ABSOLUTE(.);
    *(.efr_fr_hr_vlpt.literal .efr_fr_hr_vlpt.text)
    _efr_fr_hr_vlpt_text_end = ABSOLUTE(.);
    _memmap_seg_efr_fr_hr_vlpt_end = ALIGN(0x8);
  } >efr_fr_hr_vlpt_seg :efr_fr_hr_vlpt_phdr

  .amr_vlpt.text : ALIGN(4)
  {
    _amr_vlpt_text_start = ABSOLUTE(.);
    *(.amr_vlpt.literal .amr_vlpt.text)
    _amr_vlpt_text_end = ABSOLUTE(.);
    _memmap_seg_amr_vlpt_end = ALIGN(0x8);
  } >amr_vlpt_seg :amr_vlpt_phdr

  .amrwb_vlpt.text : ALIGN(4)
  {
    _amrwb_vlpt_text_start = ABSOLUTE(.);
    *(.amrwb_vlpt.literal .amrwb_vlpt.text)
    _amrwb_vlpt_text_end = ABSOLUTE(.);
    _memmap_seg_amrwb_vlpt_end = ALIGN(0x8);
  } >amrwb_vlpt_seg :amrwb_vlpt_phdr

  .vlpt.text : ALIGN(4)
  {
    _vlpt_text_start = ABSOLUTE(.);
    *(.vlpt.literal .vlpt.text)
    _vlpt_text_end = ABSOLUTE(.);
    _memmap_seg_vlpt_end = ALIGN(0x8);
  } >vlpt_seg :vlpt_phdr

  .low_power_dyn_alloc : ALIGN(4)
  {
    _low_power_dyn_alloc_start = ABSOLUTE(.);
    *(.low_power_dyn_alloc)
    _low_power_dyn_alloc_end = ABSOLUTE(.);
    _memmap_seg_vlpd_end = ALIGN(0x8);
  } >vlpd_seg :vlpd_phdr

  .ulpp.rodata : ALIGN(4)
  {
    _ulpp_rodata_start = ABSOLUTE(.);
    *(.ulpp.rodata)
    _ulpp_rodata_end = ABSOLUTE(.);
  } >ulpp_seg :ulpp_phdr

  .ulpp.data : ALIGN(4)
  {
    _ulpp_data_start = ABSOLUTE(.);
    *(.ulpp.data)
    _ulpp_data_end = ABSOLUTE(.);
  } >ulpp_seg :ulpp_phdr

  .ulpp.text : ALIGN(4)
  {
    _ulpp_text_start = ABSOLUTE(.);
    *(.ulpp.literal .ulpp.text)
    _ulpp_text_end = ABSOLUTE(.);
  } >ulpp_seg :ulpp_phdr

  .ulpp.bss (NOLOAD) : ALIGN(8)
  {
    . = ALIGN (8);
    _ulpp_bss_start = ABSOLUTE(.);
    *(.ulpp.bss)
    . = ALIGN (8);
    _ulpp_bss_end = ABSOLUTE(.);
    _memmap_seg_ulpp_end = ALIGN(0x8);
  } >ulpp_seg :ulpp_bss_phdr

  .dtsv3.rodata : ALIGN(4)
  {
    _dtsv3_rodata_start = ABSOLUTE(.);
    *(.dtsv3.rodata)
    _dtsv3_rodata_end = ABSOLUTE(.);
  } >dtsv3_seg :dtsv3_phdr

  .dtsv3.data : ALIGN(4)
  {
    _dtsv3_data_start = ABSOLUTE(.);
    *(.dtsv3.data)
    _dtsv3_data_end = ABSOLUTE(.);
  } >dtsv3_seg :dtsv3_phdr

  .dtsv3.text : ALIGN(4)
  {
    _dtsv3_text_start = ABSOLUTE(.);
    *(.dtsv3.literal .dtsv3.text)
    _dtsv3_text_end = ABSOLUTE(.);
  } >dtsv3_seg :dtsv3_phdr

  .dtsv3.bss (NOLOAD) : ALIGN(8)
  {
    . = ALIGN (8);
    _dtsv3_bss_start = ABSOLUTE(.);
    *(.dtsv3.bss)
    . = ALIGN (8);
    _dtsv3_bss_end = ABSOLUTE(.);
    _memmap_seg_dtsv3_end = ALIGN(0x8);
  } >dtsv3_seg :dtsv3_bss_phdr

  .dtsv4.rodata : ALIGN(4)
  {
    _dtsv4_rodata_start = ABSOLUTE(.);
    *(.dtsv4.rodata)
    _dtsv4_rodata_end = ABSOLUTE(.);
  } >dtsv4_seg :dtsv4_phdr

  .dtsv4.data : ALIGN(4)
  {
    _dtsv4_data_start = ABSOLUTE(.);
    *(.dtsv4.data)
    _dtsv4_data_end = ABSOLUTE(.);
  } >dtsv4_seg :dtsv4_phdr

  .dtsv4.text : ALIGN(4)
  {
    _dtsv4_text_start = ABSOLUTE(.);
    *(.dtsv4.literal .dtsv4.text)
    _dtsv4_text_end = ABSOLUTE(.);
  } >dtsv4_seg :dtsv4_phdr

  .dtsv4.bss (NOLOAD) : ALIGN(8)
  {
    . = ALIGN (8);
    _dtsv4_bss_start = ABSOLUTE(.);
    *(.dtsv4.bss)
    . = ALIGN (8);
    _dtsv4_bss_end = ABSOLUTE(.);
    _memmap_seg_dtsv4_end = ALIGN(0x8);
  } >dtsv4_seg :dtsv4_bss_phdr

  .dram0.rodata : ALIGN(4)
  {
    _dram0_rodata_start = ABSOLUTE(.);
    *(.dram0.rodata)
    *(.dram.rodata)
    _dram0_rodata_end = ABSOLUTE(.);
  } >dram0_0_seg :dram0_0_phdr

  .dram0.literal : ALIGN(4)
  {
    _dram0_literal_start = ABSOLUTE(.);
    *(.dram0.literal)
    *(.dram.literal)
    _dram0_literal_end = ABSOLUTE(.);
  } >dram0_0_seg :dram0_0_phdr

  .dram0.data : ALIGN(4)
  {
    _dram0_data_start = ABSOLUTE(.);
    *(.dram0.data)
    *(.dram.data)
    _dram0_data_end = ABSOLUTE(.);
  } >dram0_0_seg :dram0_0_phdr

  .dram0.bss (NOLOAD) : ALIGN(8)
  {
    . = ALIGN (8);
    _dram0_bss_start = ABSOLUTE(.);
    *(.dram0.bss)
    *(.om.debug.bss)
    *(.os.stack.bss)
    . = ALIGN (8);
    _dram0_bss_end = ABSOLUTE(.);
    _end = ALIGN(0x8);
    PROVIDE(end = ALIGN(0x8));
    _stack_sentry = ALIGN(0x8);
    _memmap_seg_dram0_0_end = ALIGN(0x8);
  } >dram0_0_seg :dram0_0_bss_phdr
  __stack = 0xe8080000;
  _heap_sentry = 0xe8080000;

  .ResetVector.text : ALIGN(4)
  {
    _ResetVector_text_start = ABSOLUTE(.);
    KEEP (*(.ResetVector.text))
    _ResetVector_text_end = ABSOLUTE(.);
    _memmap_seg_iram0_0_end = ALIGN(0x8);
  } >iram0_0_seg :iram0_0_phdr

  .Reset.literal : ALIGN(4)
  {
    _Reset_literal_start = ABSOLUTE(.);
    *(.Reset.literal)
    _Reset_literal_end = ABSOLUTE(.);
    _memmap_seg_iram0_1_end = ALIGN(0x8);
  } >iram0_1_seg :iram0_1_phdr

  .WindowVectors.text : ALIGN(4)
  {
    _WindowVectors_text_start = ABSOLUTE(.);
    KEEP (*(.WindowVectors.text))
    _WindowVectors_text_end = ABSOLUTE(.);
    _memmap_seg_iram0_2_end = ALIGN(0x8);
  } >iram0_2_seg :iram0_2_phdr

  .Level2InterruptVector.literal : ALIGN(4)
  {
    _Level2InterruptVector_literal_start = ABSOLUTE(.);
    *(.Level2InterruptVector.literal)
    _Level2InterruptVector_literal_end = ABSOLUTE(.);
    _memmap_seg_iram0_3_end = ALIGN(0x8);
  } >iram0_3_seg :iram0_3_phdr

  .Level2InterruptVector.text : ALIGN(4)
  {
    _Level2InterruptVector_text_start = ABSOLUTE(.);
    KEEP (*(.Level2InterruptVector.text))
    _Level2InterruptVector_text_end = ABSOLUTE(.);
    _memmap_seg_iram0_4_end = ALIGN(0x8);
  } >iram0_4_seg :iram0_4_phdr

  .Level3InterruptVector.literal : ALIGN(4)
  {
    _Level3InterruptVector_literal_start = ABSOLUTE(.);
    *(.Level3InterruptVector.literal)
    _Level3InterruptVector_literal_end = ABSOLUTE(.);
    _memmap_seg_iram0_5_end = ALIGN(0x8);
  } >iram0_5_seg :iram0_5_phdr

  .Level3InterruptVector.text : ALIGN(4)
  {
    _Level3InterruptVector_text_start = ABSOLUTE(.);
    KEEP (*(.Level3InterruptVector.text))
    _Level3InterruptVector_text_end = ABSOLUTE(.);
    _memmap_seg_iram0_6_end = ALIGN(0x8);
  } >iram0_6_seg :iram0_6_phdr

  .Level4InterruptVector.literal : ALIGN(4)
  {
    _Level4InterruptVector_literal_start = ABSOLUTE(.);
    *(.Level4InterruptVector.literal)
    _Level4InterruptVector_literal_end = ABSOLUTE(.);
    _memmap_seg_iram0_7_end = ALIGN(0x8);
  } >iram0_7_seg :iram0_7_phdr

  .Level4InterruptVector.text : ALIGN(4)
  {
    _Level4InterruptVector_text_start = ABSOLUTE(.);
    KEEP (*(.Level4InterruptVector.text))
    _Level4InterruptVector_text_end = ABSOLUTE(.);
    _memmap_seg_iram0_8_end = ALIGN(0x8);
  } >iram0_8_seg :iram0_8_phdr

  .DebugExceptionVector.literal : ALIGN(4)
  {
    _DebugExceptionVector_literal_start = ABSOLUTE(.);
    *(.DebugExceptionVector.literal)
    _DebugExceptionVector_literal_end = ABSOLUTE(.);
    _memmap_seg_iram0_9_end = ALIGN(0x8);
  } >iram0_9_seg :iram0_9_phdr

  .DebugExceptionVector.text : ALIGN(4)
  {
    _DebugExceptionVector_text_start = ABSOLUTE(.);
    KEEP (*(.DebugExceptionVector.text))
    _DebugExceptionVector_text_end = ABSOLUTE(.);
    _memmap_seg_iram0_10_end = ALIGN(0x8);
  } >iram0_10_seg :iram0_10_phdr

  .NMIExceptionVector.literal : ALIGN(4)
  {
    _NMIExceptionVector_literal_start = ABSOLUTE(.);
    *(.NMIExceptionVector.literal)
    _NMIExceptionVector_literal_end = ABSOLUTE(.);
    _memmap_seg_iram0_11_end = ALIGN(0x8);
  } >iram0_11_seg :iram0_11_phdr

  .NMIExceptionVector.text : ALIGN(4)
  {
    _NMIExceptionVector_text_start = ABSOLUTE(.);
    KEEP (*(.NMIExceptionVector.text))
    _NMIExceptionVector_text_end = ABSOLUTE(.);
    _memmap_seg_iram0_12_end = ALIGN(0x8);
  } >iram0_12_seg :iram0_12_phdr

  .KernelExceptionVector.literal : ALIGN(4)
  {
    _KernelExceptionVector_literal_start = ABSOLUTE(.);
    *(.KernelExceptionVector.literal)
    _KernelExceptionVector_literal_end = ABSOLUTE(.);
    _memmap_seg_iram0_13_end = ALIGN(0x8);
  } >iram0_13_seg :iram0_13_phdr

  .KernelExceptionVector.text : ALIGN(4)
  {
    _KernelExceptionVector_text_start = ABSOLUTE(.);
    KEEP (*(.KernelExceptionVector.text))
    _KernelExceptionVector_text_end = ABSOLUTE(.);
    _memmap_seg_iram0_14_end = ALIGN(0x8);
  } >iram0_14_seg :iram0_14_phdr

  .UserExceptionVector.literal : ALIGN(4)
  {
    _UserExceptionVector_literal_start = ABSOLUTE(.);
    *(.UserExceptionVector.literal)
    _UserExceptionVector_literal_end = ABSOLUTE(.);
    _memmap_seg_iram0_15_end = ALIGN(0x8);
  } >iram0_15_seg :iram0_15_phdr

  .UserExceptionVector.text : ALIGN(4)
  {
    _UserExceptionVector_text_start = ABSOLUTE(.);
    KEEP (*(.UserExceptionVector.text))
    _UserExceptionVector_text_end = ABSOLUTE(.);
    _memmap_seg_iram0_16_end = ALIGN(0x8);
  } >iram0_16_seg :iram0_16_phdr

  .DoubleExceptionVector.literal : ALIGN(4)
  {
    _DoubleExceptionVector_literal_start = ABSOLUTE(.);
    *(.DoubleExceptionVector.literal)
    _DoubleExceptionVector_literal_end = ABSOLUTE(.);
    _memmap_seg_iram0_17_end = ALIGN(0x8);
  } >iram0_17_seg :iram0_17_phdr

  .DoubleExceptionVector.text : ALIGN(4)
  {
    _DoubleExceptionVector_text_start = ABSOLUTE(.);
    KEEP (*(.DoubleExceptionVector.text))
    _DoubleExceptionVector_text_end = ABSOLUTE(.);
    _memmap_seg_iram0_18_end = ALIGN(0x8);
  } >iram0_18_seg :iram0_18_phdr

  .Reset.text : ALIGN(4)
  {
    _Reset_text_start = ABSOLUTE(.);
    *(.Reset.text)
    _Reset_text_end = ABSOLUTE(.);
  } >iram0_19_seg :iram0_19_phdr

  .iram0.text : ALIGN(4)
  {
    _iram0_text_start = ABSOLUTE(.);
    *(.iram0.literal .iram.literal .iram.text.literal .iram0.text .iram.text)
    _iram0_text_end = ABSOLUTE(.);
    _memmap_seg_iram0_19_end = ALIGN(0x8);
  } >iram0_19_seg :iram0_19_phdr

  .sram.shareaddr : ALIGN(4)
  {
    _sram_shareaddr_start = ABSOLUTE(.);
    *(.sram.shareaddr)
    _sram_shareaddr_end = ABSOLUTE(.);
  } >sram0_seg :sram0_phdr

  .sram.rodata : ALIGN(4)
  {
    _sram_rodata_start = ABSOLUTE(.);
    *(.sram.rodata)
    _sram_rodata_end = ABSOLUTE(.);
  } >sram0_seg :sram0_phdr

  .rodata : ALIGN(4)
  {
    _rodata_start = ABSOLUTE(.);
    *(.rodata)
    *(.rodata.*)
    *(.gnu.linkonce.r.*)
    *(.rodata1)
    __XT_EXCEPTION_TABLE__ = ABSOLUTE(.);
    KEEP (*(.xt_except_table))
    KEEP (*(.gcc_except_table))
    *(.gnu.linkonce.e.*)
    *(.gnu.version_r)
    KEEP (*(.eh_frame))
    /*  C++ constructor and destructor tables, properly ordered:  */
    KEEP (*crtbegin.o(.ctors))
    KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
    KEEP (*crtbegin.o(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
    /*  C++ exception handlers table:  */
    __XT_EXCEPTION_DESCS__ = ABSOLUTE(.);
    *(.xt_except_desc)
    *(.gnu.linkonce.h.*)
    __XT_EXCEPTION_DESCS_END__ = ABSOLUTE(.);
    *(.xt_except_desc_end)
    *(.dynamic)
    *(.gnu.version_d)
    . = ALIGN(4);		/* this table MUST be 4-byte aligned */
    _bss_table_start = ABSOLUTE(.);
    LONG(_vlpd_bss_start)
    LONG(_vlpd_bss_end)
    LONG(_efr_fr_hr_vlpd_bss_start)
    LONG(_efr_fr_hr_vlpd_bss_end)
    LONG(_amr_vlpd_bss_start)
    LONG(_amr_vlpd_bss_end)
    LONG(_amrwb_vlpd_bss_start)
    LONG(_amrwb_vlpd_bss_end)
    LONG(_ulpp_bss_start)
    LONG(_ulpp_bss_end)
    LONG(_dtsv3_bss_start)
    LONG(_dtsv3_bss_end)
    LONG(_dtsv4_bss_start)
    LONG(_dtsv4_bss_end)
    LONG(_dram0_bss_start)
    LONG(_dram0_bss_end)
    LONG(_bss_start)
    LONG(_bss_end)
    _bss_table_end = ABSOLUTE(.);
    _rodata_end = ABSOLUTE(.);
  } >sram0_seg :sram0_phdr

  .sram.text : ALIGN(4)
  {
    _sram_text_start = ABSOLUTE(.);
    *(.sram.literal .sram.text)
    _sram_text_end = ABSOLUTE(.);
  } >sram0_seg :sram0_phdr

  .text : ALIGN(4)
  {
    _stext = .;
    _text_start = ABSOLUTE(.);
    *(.entry.text)
    *(.init.literal)
    KEEP(*(.init))
    *(.literal .text .literal.* .text.* .stub .gnu.warning .gnu.linkonce.literal.* .gnu.linkonce.t.*.literal .gnu.linkonce.t.*)
    *(.fini.literal)
    KEEP(*(.fini))
    *(.gnu.version)
    _text_end = ABSOLUTE(.);
    _etext = .;
  } >sram0_seg :sram0_phdr

  .sram.data : ALIGN(4)
  {
    _sram_data_start = ABSOLUTE(.);
    *(.sram.data)
    _sram_data_end = ABSOLUTE(.);
  } >sram0_seg :sram0_phdr

  .data : ALIGN(4)
  {
    _data_start = ABSOLUTE(.);
    *(.data)
    *(.data.*)
    *(.gnu.linkonce.d.*)
    KEEP(*(.gnu.linkonce.d.*personality*))
    *(.data1)
    *(.sdata)
    *(.sdata.*)
    *(.gnu.linkonce.s.*)
    *(.sdata2)
    *(.sdata2.*)
    *(.gnu.linkonce.s2.*)
    KEEP(*(.jcr))
    _data_end = ABSOLUTE(.);
  } >sram0_seg :sram0_phdr

  .sram.uninit : ALIGN(4)
  {
    _sram_uninit_start = ABSOLUTE(.);
    *(.sram.uninit)
    _sram_uninit_end = ABSOLUTE(.);
  } >sram0_seg :sram0_phdr

  .bss (NOLOAD) : ALIGN(8)
  {
    . = ALIGN (8);
    _bss_start = ABSOLUTE(.);
    *(.dynsbss)
    *(.sbss)
    *(.sbss.*)
    *(.gnu.linkonce.sb.*)
    *(.scommon)
    *(.sbss2)
    *(.sbss2.*)
    *(.gnu.linkonce.sb2.*)
    *(.dynbss)
    *(.bss)
    *(.bss.*)
    *(.gnu.linkonce.b.*)
    *(COMMON)
    *(.sram.pool.bss)
    *(.sram.bss)
    . = ALIGN (8);
    _bss_end = ABSOLUTE(.);
    _memmap_seg_sram0_end = ALIGN(0x8);
  } >sram0_seg :sram0_bss_phdr
  .debug  0 :  { *(.debug) }
  .line  0 :  { *(.line) }
  .debug_srcinfo  0 :  { *(.debug_srcinfo) }
  .debug_sfnames  0 :  { *(.debug_sfnames) }
  .debug_aranges  0 :  { *(.debug_aranges) }
  .debug_pubnames  0 :  { *(.debug_pubnames) }
  .debug_info  0 :  { *(.debug_info) }
  .debug_abbrev  0 :  { *(.debug_abbrev) }
  .debug_line  0 :  { *(.debug_line) }
  .debug_frame  0 :  { *(.debug_frame) }
  .debug_str  0 :  { *(.debug_str) }
  .debug_loc  0 :  { *(.debug_loc) }
  .debug_macinfo  0 :  { *(.debug_macinfo) }
  .debug_weaknames  0 :  { *(.debug_weaknames) }
  .debug_funcnames  0 :  { *(.debug_funcnames) }
  .debug_typenames  0 :  { *(.debug_typenames) }
  .debug_varnames  0 :  { *(.debug_varnames) }
  .xt.insn 0 :
  {
    KEEP (*(.xt.insn))
    KEEP (*(.gnu.linkonce.x.*))
  }
  .xt.prop 0 :
  {
    KEEP (*(.xt.prop))
    KEEP (*(.xt.prop.*))
    KEEP (*(.gnu.linkonce.prop.*))
  }
  .xt.lit 0 :
  {
    KEEP (*(.xt.lit))
    KEEP (*(.xt.lit.*))
    KEEP (*(.gnu.linkonce.p.*))
  }
  .debug.xt.callgraph 0 :
  {
    KEEP (*(.debug.xt.callgraph .debug.xt.callgraph.* .gnu.linkonce.xt.callgraph.*))
  }
}

