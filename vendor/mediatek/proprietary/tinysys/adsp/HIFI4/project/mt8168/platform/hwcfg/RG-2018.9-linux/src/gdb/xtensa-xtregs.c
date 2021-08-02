/* Customized table mapping between kernel xtregset and GDB register cache.

   Customer ID=13943; Build=0x75f5e; Copyright (c) 2007-2010 Tensilica Inc.

   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
   CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.  */


typedef struct {
  int   gdb_regnum;
  int   gdb_offset;
  int   ptrace_cp_offset;
  int   ptrace_offset;
  int   size;
  int   coproc;
  int   dbnum;
  char* name
;} xtensa_regtable_t;

#define XTENSA_ELF_XTREG_SIZE	224

const xtensa_regtable_t  xtensa_regmap_table[] = {
  /* gnum,gofs,cpofs,ofs,siz,cp, dbnum,  name */
  {   76, 304,   0,   0,  4, -1, 0x0204, "br" },
  {   80, 320,   4,   4,  4, -1, 0x0303, "accreg_0" },
  {   81, 324,   8,   8,  4, -1, 0x0304, "accreg_1" },
  {   83, 332,   8,  24,  4,  1, 0x03f0, "ae_ovf_sar" },
  {   84, 336,  12,  28,  4,  1, 0x03f1, "ae_bithead" },
  {   85, 340,  16,  32,  4,  1, 0x03f2, "ae_ts_fts_bu_bp" },
  {   86, 344,  20,  36,  4,  1, 0x03f3, "ae_cw_sd_no" },
  {   87, 348,  24,  40,  4,  1, 0x03f6, "ae_cbegin0" },
  {   88, 352,  28,  44,  4,  1, 0x03f7, "ae_cend0" },
  {   89, 356,  32,  48,  4,  1, 0x03f8, "ae_cbegin1" },
  {   90, 360,  36,  52,  4,  1, 0x03f9, "ae_cend1" },
  {   91, 364,  40,  56,  8,  1, 0x1000, "aed0" },
  {   92, 372,  48,  64,  8,  1, 0x1001, "aed1" },
  {   93, 380,  56,  72,  8,  1, 0x1002, "aed2" },
  {   94, 388,  64,  80,  8,  1, 0x1003, "aed3" },
  {   95, 396,  72,  88,  8,  1, 0x1004, "aed4" },
  {   96, 404,  80,  96,  8,  1, 0x1005, "aed5" },
  {   97, 412,  88, 104,  8,  1, 0x1006, "aed6" },
  {   98, 420,  96, 112,  8,  1, 0x1007, "aed7" },
  {   99, 428, 104, 120,  8,  1, 0x1008, "aed8" },
  {  100, 436, 112, 128,  8,  1, 0x1009, "aed9" },
  {  101, 444, 120, 136,  8,  1, 0x100a, "aed10" },
  {  102, 452, 128, 144,  8,  1, 0x100b, "aed11" },
  {  103, 460, 136, 152,  8,  1, 0x100c, "aed12" },
  {  104, 468, 144, 160,  8,  1, 0x100d, "aed13" },
  {  105, 476, 152, 168,  8,  1, 0x100e, "aed14" },
  {  106, 484, 160, 176,  8,  1, 0x100f, "aed15" },
  {  107, 492, 176, 192,  8,  1, 0x1010, "u0" },
  {  108, 500, 184, 200,  8,  1, 0x1011, "u1" },
  {  109, 508, 192, 208,  8,  1, 0x1012, "u2" },
  {  110, 516, 200, 216,  8,  1, 0x1013, "u3" },
  {  111, 524, 168, 184,  1,  1, 0x1014, "aep0" },
  {  112, 525, 169, 185,  1,  1, 0x1015, "aep1" },
  {  113, 526, 170, 186,  1,  1, 0x1016, "aep2" },
  {  114, 527, 171, 187,  1,  1, 0x1017, "aep3" },
  {  115, 528,   0,  16,  4,  1, 0x1019, "fcr_fsr" },
  { 0 }
};

