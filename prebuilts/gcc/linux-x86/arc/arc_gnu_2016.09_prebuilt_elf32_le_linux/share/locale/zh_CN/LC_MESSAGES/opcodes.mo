??    ?        ?   ?	      H     I  L   K  K   ?  4   ?  ?     3   ?  u     ?   ?  ?     /   ?  7   ?  k     }   ?  B     ?   I  [   ?  ?   6  [   ?  ?      ?   ?  (   D  (   m  (   ?  1   ?  %   ?  %     3   =  (   q  2   ?  ?   ?          "     /  /   ?  .   o     ?     ?     ?     ?     ?  	     	             6     I     ]  (   {     ?     ?     ?     ?          .  *   @  
   k  %   v  )   ?  %   ?  !   ?  !     $   0     U  :   q  9   ?     ?  "         #     6     I     _     u     ?     ?     ?     ?     ?  %     #   (  +   L  +   x  1   ?  1   ?  +     1   4  1   f     ?  %   ?     ?     ?     	     !     <     U  +   j     ?  !   ?  "   ?  +   ?      !      B      c     ?     ?  !   ?     ?     ?                 ;      Y      w      ?      ?      ?      ?   "   ?      !     .!     K!     f!  %   y!     ?!     ?!     ?!     ?!     ?!     "     !"     -"  !   9"     ["  $   l"  %   ?"  2   ?"  2   ?"  2   #     P#     g#     w#     ?#  #   ?#  '   ?#     ?#     $  -   %$  ;   S$  /   ?$  	   ?$     ?$     ?$     ?$     ?$     %      &%     G%     `%     x%  	  ?%     ?'  >   ?'  =   ?'  /   (  ?   I(  5   ?(  z   )  ?   ?)  ?   ,*  (   ?*  2   ?*  z   !+  T   ?+  1   ?+  l   #,  D   ?,  i   ?,  k   ?-  k   ?-  s   .  (   ?.  (   ?.  (   ?.  *   /  (   1/  (   Z/  /   ?/  +   ?/  +   ?/  9   0     E0     `0     m0  0   |0  /   ?0     ?0     ?0     ?0     1     *1     B1     K1     T1     o1     ?1     ?1  ,   ?1     ?1     ?1     
2  *   &2  *   Q2     |2  "   ?2     ?2     ?2  "   ?2     3     "3     =3     X3     v3  E   ?3  E   ?3     4     34     O4     a4     s4     ?4     ?4     ?4     ?4     ?4  %   ?4     5  '   *5  '   R5  1   z5  1   ?5  9   ?5  9   6  1   R6  9   ?6  9   ?6     ?6  0   7     B7     X7     n7     ?7     ?7     ?7  '   ?7     8     8     !8  #   18     U8     l8     ?8     ?8     ?8     ?8     ?8     ?8     9     9     (9     <9     P9     d9     x9     ?9  	   ?9     ?9     ?9     ?9     ?9     ?9  $   
:     /:     B:     ^:     w:     ?:     ?:     ?:  
   ?:  !   ?:     ?:     ;  $   ;  .   B;  /   q;  .   ?;     ?;     ?;     ?;     <     <     ;<     [<     t<  3   ?<  7   ?<  4   ?<  	   4=     >=     T=     [=     j=     y=     ?=     ?=     ?=     ?=             ;   G       \   u       8      C      g   6   t   %   4          r      .   9   S       `           I      E       -   y       x   3   Y   ?   ?   ?   X   j       7   &          <      =   +   /   v          ?       ?           ,       O   M           T   s   {      W       ?       ?   :      h   !   ?   D      Q   p   ?          m       A               V   U   @   ?           Z   H   B                  ?   #   e   ?   F   k           5   ?   ?      )   ?       (   z   l      }   ?   2   "          P       K       f       ?   |       0   L   ?      1   *   i   ~      ?       w       ?   o   N       a   b   ?                     >   
       ?   '       ?            ?   ?       $                  c   ?      q   ?   ?       ?       _   J          ^      	   [   ?   n   R   d      ?   ]   ?       
 
  For the options above, The following values are supported for "ARCH":
    
  For the options above, the following values are supported for "ABI":
    
  aliases            Do print instruction aliases.
 
  cp0-names=ARCH           Print CP0 register names according to
                           specified architecture.
                           Default: based on binary being disassembled.
 
  debug_dump         Temp switch for debug trace.
 
  fpr-names=ABI            Print FPR names according to specified ABI.
                           Default: numeric.
 
  gpr-names=ABI            Print GPR names according to  specified ABI.
                           Default: based on binary being disassembled.
 
  hwr-names=ARCH           Print HWR names according to specified 
			   architecture.
                           Default: based on binary being disassembled.
 
  msa             Recognize MSA instructions.
 
  no-aliases         Don't print instruction aliases.
 
  reg-names=ABI            Print GPR and FPR names according to
                           specified ABI.
 
  reg-names=ARCH           Print CP0 register and HWR names according to
                           specified architecture.
 
  virt            Recognize the virtualization ASE instructions.
 
The following AARCH64 specific disassembler options are supported for use
with the -M switch (multiple options should be separated by commas):
 
The following ARM specific disassembler options are supported for use with
the -M switch:
 
The following MIPS specific disassembler options are supported for use
with the -M switch (multiple options should be separated by commas):
 
The following PPC specific disassembler options are supported for use with
the -M switch:
 
The following S/390 specific disassembler options are supported for use
with the -M switch (multiple options should be separated by commas):
 
The following i386/x86-64 specific disassembler options are supported for use
with the -M switch (multiple options should be separated by commas):
   addr16      Assume 16bit address size
   addr32      Assume 32bit address size
   addr64      Assume 64bit address size
   att         Display instruction in AT&T syntax
   data16      Assume 16bit data size
   data32      Assume 32bit data size
   esa         Disassemble in ESA architecture mode
   i8086       Disassemble in 16bit mode
   intel       Display instruction in Intel syntax
   suffix      Always display instruction suffix in AT&T syntax
 # <dis error: %08lx> $<undefined> %02x		*unknown* %dsp16() takes a symbolic address, not a number %dsp8() takes a symbolic address, not a number %s: Error:  %s: Warning:  'ROR' operator not allowed (DP) offset out of range. (SP) offset out of range. (unknown) *unknown* 21-bit offset out of range <function code %d> <illegal precision> <internal disassembler error> <internal error in opcode table: %s %s>
 <unknown register %d> Address 0x%s is out of bounds.
 Bad immediate expression Bad register in postincrement Bad register in preincrement Bad register name Don't know how to specify # dependency %s
 Hmmmm 0x%x Immediate is out of range -128 to 127 Immediate is out of range -32768 to 32767 Immediate is out of range -512 to 511 Immediate is out of range -7 to 8 Immediate is out of range -8 to 7 Immediate is out of range 0 to 65535 Internal disassembler error Internal error:  bad sparc-opcode.h: "%s", %#.8lx, %#.8lx
 Internal error: bad sparc-opcode.h: "%s", %#.8lx, %#.8lx
 Label conflicts with `Rx' Label conflicts with register name Missing '#' prefix Missing '.' prefix Missing 'pag:' prefix Missing 'pof:' prefix Missing 'seg:' prefix Missing 'sof:' prefix Operand is not a symbol SR/SelID is out of range Syntax error: No trailing ')' Unknown error %d
 Unrecognised disassembler option: %s
 Unrecognised register name set: %s
 Unrecognized field %d while building insn.
 Unrecognized field %d while decoding insn.
 Unrecognized field %d while getting int operand.
 Unrecognized field %d while getting vma operand.
 Unrecognized field %d while printing insn.
 Unrecognized field %d while setting int operand.
 Unrecognized field %d while setting vma operand.
 Value is not aligned enough W keyword invalid in FR operand slot. W register expected address writeback expected bad instruction `%.50s' bad instruction `%.50s...' branch operand unaligned branch to odd offset branch value not in range and to odd offset branch value out of range displacement value is not aligned displacement value is out of range don't know how to specify %% dependency %s
 dsp:16 immediate is out of range dsp:20 immediate is out of range dsp:24 immediate is out of range dsp:8 immediate is out of range extraneous register floating-point immediate expected illegal bitmask illegal use of parentheses imm10 is out of range imm:6 immediate is out of range immediate is out of range 0-7 immediate is out of range 1-2 immediate is out of range 1-8 immediate is out of range 2-9 immediate offset immediate out of range immediate value immediate value cannot be register immediate value is out of range immediate value out of range invalid conditional option invalid mask field invalid register for stack adjustment invalid register name invalid register offset invalid shift amount invalid shift operator jump hint unaligned junk at end of line missing `)' missing `]' missing mnemonic in syntax string missing register negative immediate value not allowed negative or unaligned offset expected operand out of range (%ld not between %ld and %ld) operand out of range (%ld not between %ld and %lu) operand out of range (%lu not between %lu and %lu) register element index register number register number must be even shift amount shift amount expected to be 0 or 12 shift amount should be a multiple of 16 shift operator expected stack pointer register expected syntax error (expected char `%c', found `%c') syntax error (expected char `%c', found end of instruction) unable to change directory to "%s", errno = %s
 undefined unexpected address writeback unknown unknown	0x%02lx unknown	0x%04lx unknown constraint `%c' unrecognized form of instruction unrecognized instruction vector5 is out of range vector8 is out of range Project-Id-Version: opcodes 2.24.90
Report-Msgid-Bugs-To: bug-binutils@gnu.org
POT-Creation-Date: 2014-02-10 09:42+1030
PO-Revision-Date: 2015-10-30 09:13-0400
Last-Translator: Mingye Wang (Arthur2e5) <arthur200126@gmail.com>
Language-Team: Chinese (simplified) <i18n-zh@googlegroups.com>
Language: zh_CN
X-Bugs: Report translation errors to the Language-Team address.
MIME-Version: 1.0
Content-Type: text/plain; charset=UTF-8
Content-Transfer-Encoding: 8bit
Plural-Forms: nplurals=1; plural=0;
X-Generator: Poedit 1.8.4
 
 
  对于以上的选项，以下值可被用于 "ARCH"：
    
  对于以上的选项，以下值可被用于 "ABI"：
    
  aliases            要打印指令别名。
 
  cp0-names=ARCH           根据指定的架构打印 CP0 寄存器名。
                           默认：根据被反汇编的二进制代码。
 
  debug_dump         调试跟踪的临时开关。
 
  fpr-names=ABI            根据指定的 ABI 打印浮点寄存器名。
                           默认：数字。
 
  gpr-names=ABI            根据指定的 ABI 打印通用寄存器名。
                           默认：根据被反汇编的二进制文件。
 
  hwr-names=ARCH           根据指定的架构打印 HWR 寄存器名。
                           默认：根据被反汇编的二进制代码。
 
  msa             识别 MSA 指令。
 
  no-aliases         不要打印指令别名。
 
  reg-names=ABI            根据指定的 ABI 打印通用寄存器和浮点寄存
                           器名。
 
  reg-names=ARCH           根据指定的架构打印 CP0 和 HWR 寄存器名。
 
  virt            识别虚拟化 ASE 指令。
 
下列 AARCH64 特定的反汇编器选项可通过 -M 开关启用（使用逗号分隔多个选项）：
 
下列 ARM 特定的反汇编器选项可通过 -M 开关启用：
 
下列 MIPS 特定的反汇编器选项可通过 -M 开关启用（使用逗号分隔多个选项）：
 
下列 PPC 特定的反汇编器选项在使用 -M 开关时可用（使用逗号分隔多个选项）：
 

下列 S/390 特定的反汇编器选项可通过 -M 开关启用（使用逗号分隔多个选项）：
 
下列 i386/x86-64 特定的反汇编器选项在使用 -M 开关时可用（使用逗号分隔多个选项）：
   addr16      假定 16 位地址大小
   addr32      假定 32 位地址大小
   addr64      假定 64 位地址大小
   att         用 AT&T 语法显示指令
   data16      假定 16 位数据大小
   data32      假定 32 位数据大小
   esa         在 ESA 架构模式下反汇编
   i8086       在 16 位模式下反汇编
   intel       用 Intel 语法显示指令
   suffix      在 AT&T 语法中始终显示指令后缀
 # <反汇编出错: %08lx> $<未定义> %02x		*未知* %dsp16() 使用一个符号地址，而非数字 %dsp8() 使用一个符号地址，而非数字 %s：错误： %s：警告： 不允许 'ROR' 操作符 (DP) 偏移量越界 (SP) 偏移量越界。 (未知) *未知* 21位长的偏移量越界 <函数代码 %d> <非法的精度> <反汇编器内部错误> <操作数表中出现内部错误：%s %s>
 <未知的寄存器 %d> 地址 0x%s 越界。
 错误的立即数表达式 后置自增中使用了错误的寄存器 前置自增中使用了错误的寄存器 错误的寄存器名 不知道如何指定 # 依赖 %s
 咦... 0x%x 立即数越界 (-128 到 127) 立即数越界 (-32768 到 32767) 立即数越界 (-512 到 511) 立即数越界 (-7 到 8) 立即数越界 (-8 到 7) 立即数越界 (0 到 65535) 反汇编器内部错误 内部错误：错误的 sparc-opcode.h：“%s”，%#.8lx，%#.8lx
 内部错误：错误的 sparc-opcode.h：“%s”，%#.8lx，%#.8lx
 标号与‘Rx’冲突 标号与寄存器名冲突 缺失 '#' 前缀 缺失 '.' 前缀 缺失 'pag:' 前缀 缺失 'pof:' 前缀 缺失 'seg:' 前缀 缺失 'sof:' 前缀 操作数不是一个符号 SR/SelID 越界 语法错误：没有结尾的‘)’ 未知错误 %d
 无法识别的反汇编器选项：%s
 无法识别的寄存器名称集：%s
 建立 insn 时遇到无法识别的字段 %d。
 解码 insn 时遇到无法识别的字段 %d。
 获得 int 操作数时遇到无法识别的字段 %d。
 获得 vma 操作数时遇到无法识别的字段 %d。
 打印 insn 时遇到无法识别的字段 %d。
 设置 int 操作数时遇到无法识别的字段 %d。
 设置 vma 操作数时遇到无法识别的字段 %d。
 数值对齐程度不够 W 关键字非法，在 FR 操作数槽位中。 预期的 W 寄存器 预期的地址写回 错误的指令‘%.50s’ 错误的指令‘%.50s...’ 分支操作数未对齐 跳转偏移量为奇数 跳转越界且跳转偏移量为奇数 跳转越界 偏移值未对齐 偏移值越界 不知道如何指定 %% 依赖 %s
 dsp:16 立即数越界 dsp:20 立即数越界 dsp:24 立即数越界 dsp:8 立即数越界 多余寄存器 预期的浮点常量立即数 非法的位掩码 括号用法非法 imm10 越界 imm:6 立即数越界 立即数越界 0-7 立即数越界 1-2 立即数越界 1-8 立即数越界 2-9 立即数偏移 立即数越界 立即数 立即数不能是寄存器 立即数越界 立即数越界 无效的条件选项 无效的掩码字段 用于调整堆栈的寄存器无效 无效寄存器名 无效的寄存器偏移量 无效的移位操作数 无效的移位操作符 跳转提示未对齐 行尾有垃圾字符 缺少‘)’ 缺少 `]' 语法字符串中没有助记符 缺失寄存器 不允许负立即数 预期的负或未对齐的偏移量 操作数越界(%ld 不在 %ld 和 %ld 之间) 操作数越界 (%ld 不在 %ld 和 %lu 之间) 操作数越界(%lu 不在 %lu 和 %lu 之间) 寄存器元素下标 寄存器数 寄存器数必须是偶数 移位操作数 移位量预计为 0 或 12 移位量应该是 16 的倍数 预期的移位操作符 预期的堆栈指针寄存器 语法错误(需要字符‘%c’，得到‘%c’) 语法错误(需要字符‘%c’，却到达指令尾) 无法将当前目录切换至“%s”，errno = %s
 未定义 意外的地址写回 未知 未知	0x%02lx 未知	0x%04lx 未知的约束‘%c’ 无法识别的指令格式 无法识别的指令 vector5 越界 vector8 越界 