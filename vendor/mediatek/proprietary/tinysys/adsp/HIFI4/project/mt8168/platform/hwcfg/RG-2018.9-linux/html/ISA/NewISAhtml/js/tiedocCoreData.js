var tiedoc_core = {
    "ABS": {
        "name": "ABS",
        "syn": "Absolute Value",
        "Assembler Syntax": "ABS ar, at",
        "Description": "<P><code>ABS</code> calculates the absolute value of the contents of address register <code>at</code> and writes it to address register <code>ar</code>. Arithmetic overflow is not detected.</P>",
        "Operation": "<pre>AR[r] &#8592; if AR[t]<SUB>31</SUB> then &#8722;AR[t] else AR[t]</pre>"
    },
    "ADD": {
        "name": "ADD",
        "syn": "Add",
        "Assembler Syntax": "ADD ar, as, at",
        "Description": "<P><code>ADD</code> calculates the two's complement 32-bit sum of address registers <code>as</code> and <code>at</code>. The low 32 bits of the sum are written to address register <code>ar</code>. Arithmetic overflow is not detected.</P><P><code>ADD</code> is a 24-bit instruction. The <code>ADD.N</code> density-option instruction performs the same operation in a 16-bit encoding.</P>",
        "Operation": "<pre>AR[r] &#8592; AR[s] + AR[t]</pre>"
    },
    "ADD.N": {
        "name": "ADD.N",
        "syn": "Narrow Add",
        "Assembler Syntax": "ADD.N ar, as, at",
        "Description": "<P>This performs the same operation as the <code>ADD</code> instruction in a 16-bit encoding.</P><P><code>ADD.N</code> calculates the two's complement 32-bit sum of address registers <code>as</code> and <code>at</code>. The low 32 bits of the sum are written to address register <code>ar</code>. Arithmetic overflow is not detected.</P>",
        "Operation": "<pre>AR[r] &#8592; AR[s] + AR[t]</pre>"
    },
    "ADDI": {
        "name": "ADDI",
        "syn": "Add Immediate",
        "Assembler Syntax": "ADDI at, as, -128..127",
        "Description": "<P><code>ADDI</code> calculates the two's complement 32-bit sum of address register <code>as</code> and a constant encoded in the <code>imm8</code> field. The low 32 bits of the sum are written to address register <code>at</code>. Arithmetic overflow is not detected.</P><P>The immediate operand encoded in the instruction can range from -128 to 127. It is decoded by sign-extending <code>imm8</code>.</P><P><code>ADDI</code> is a 24-bit instruction. The <code>ADDI.N</code> density-option instruction performs a similar operation (the immediate operand has less range) in a 16-bit encoding.</P>",
        "Operation": "<pre>AR[t] &#8592; AR[s] + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8)</pre>"
    },
    "ADDI.N": {
        "name": "ADDI.N",
        "syn": "Narrow Add Immediate",
        "Assembler Syntax": "ADDI.N ar, as, imm",
        "Description": "<P><code>ADDI.N</code> is similar to <code>ADDI</code>, but has a 16-bit encoding and supports a smaller range of immediate operand values encoded in the instruction word.</P><P><code>ADDI.N</code> calculates the two's complement 32-bit sum of address register <code>as</code> and an operand encoded in the <code>t</code> field. The low 32 bits of the sum are written to address register <code>ar</code>. Arithmetic overflow is not detected.</P><P>The operand encoded in the instruction can be -1 or one to 15. If <code>t</code> is zero, then a value of -1 is used, otherwise the value is the zero-extension of <code>t</code>.</P>",
        "Operation": "<pre>AR[r] &#8592; AR[s] + (if t = 0<SUP>4</SUP> then 1<SUP>32</SUP> else 0<SUP>28</SUP>||t)</pre>"
    },
    "ADDMI": {
        "name": "ADDMI",
        "syn": "Add Immediate with Shift by 8",
        "Assembler Syntax": "ADDMI at, as, -32768..32512",
        "Description": "<P><code>ADDMI</code> extends the range of constant addition. It is often used in conjunction with load and store instructions to extend the range of the base, plus offset the calculation.</P><P><code>ADDMI</code> calculates the two's complement 32-bit sum of address register <code>as</code> and an operand encoded in the <code>imm8</code> field. The low 32 bits of the sum are written to address register <code>at</code>. Arithmetic overflow is not detected.</P><P>The operand encoded in the instruction can have values that are multiples of 256 ranging from -32768 to 32512. It is decoded by sign-extending <code>imm8</code> and shifting the result left by eight bits.</P>",
        "Operation": "<pre>AR[t] &#8592; AR[s] + (imm8<SUB>7</SUB><SUP>16</SUP>||imm8||0<SUP>8</SUP>)</pre>"
    },
    "ADDX2": {
        "name": "ADDX2",
        "syn": "Add with Shift by 1",
        "Assembler Syntax": "ADDX2 ar, as, at",
        "Description": "<P><code>ADDX2</code> calculates the two's complement 32-bit sum of address register <code>as</code> shifted left by one bit and address register <code>at</code>. The low 32 bits of the sum are written to address register <code>ar</code>. Arithmetic overflow is not detected.</P><P><code>ADDX2</code> is frequently used for address calculation and as part of sequences to multiply by small constants.</P>",
        "Operation": "<pre>AR[r] &#8592; (AR[s]<SUB>30..0</SUB>||0) + AR[t]</pre>"
    },
    "ADDX4": {
        "name": "ADDX4",
        "syn": "Add with Shift by 2",
        "Assembler Syntax": "ADDX4 ar, as, at",
        "Description": "<P><code>ADDX4</code> calculates the two's complement 32-bit sum of address register <code>as</code> shifted left by two bits and address register <code>at</code>. The low 32 bits of the sum are written to address register <code>ar</code>. Arithmetic overflow is not detected.</P><P><code>ADDX4</code> is frequently used for address calculation and as part of sequences to multiply by small constants.</P>",
        "Operation": "<pre>AR[r] &#8592; (AR[s]<SUB>29..0</SUB>||0<SUP>2</SUP>) + AR[t]</pre>"
    },
    "ADDX8": {
        "name": "ADDX8",
        "syn": "Add with Shift by 3",
        "Assembler Syntax": "ADDX8 ar, as, at",
        "Description": "<P><code>ADDX8</code> calculates the two's complement 32-bit sum of address register <code>as</code> shifted left by 3 bits and address register <code>at</code>. The low 32 bits of the sum are written to address register <code>ar</code>. Arithmetic overflow is not detected.</P><P><code>ADDX8</code> is frequently used for address calculation and as part of sequences to multiply by small constants.</P>",
        "Operation": "<pre>AR[r] &#8592; (AR[s]<SUB>28..0</SUB>||0<SUP>3</SUP>) + AR[t]</pre>"
    },
    "ALL4": {
        "name": "ALL4",
        "syn": "All 4 Booleans True",
        "Assembler Syntax": "ALL4 bt, bs",
        "Description": "<P><code>ALL4</code> sets Boolean register <code>bt</code> to the logical and of the four Boolean registers <code>bs</code><code>+</code><code>0</code>, <code>bs</code><code>+</code><code>1</code>, <code>bs</code><code>+</code><code>2</code>, and <code>bs</code><code>+</code><code>3</code>. <code>bs</code> must be a multiple of four (<code>b0</code>, <code>b4</code>, <code>b8</code>, or <code>b12</code>); otherwise the operation of this instruction is not defined. <code>ALL4</code> reduces four test results such that the result is true if all four tests are true.</P><P>When the sense of the <code>bs</code> Booleans is inverted (0 <code>&#174;</code> true, 1 <code>&#174;</code> false), use <code>ANY4</code> and an inverted test of the result.</P>",
        "Operation": "<pre>BR<SUB>t</SUB> &#8592; BR<SUB>s+3</SUB> and BR<SUB>s+2</SUB> and BR<SUB>s+1</SUB> and BR<SUB>s+0</SUB></pre>"
    },
    "ALL8": {
        "name": "ALL8",
        "syn": "All 8 Booleans True",
        "Assembler Syntax": "ALL8 bt, bs",
        "Description": "<P><code>ALL8</code> sets Boolean register <code>bt</code> to the logical and of the eight Boolean registers <code>bs</code><code>+</code><code>0</code>, <code>bs</code><code>+</code><code>1</code>, ... <code>bs</code><code>+</code><code>6</code>, and <code>bs</code><code>+</code><code>7</code>. <code>bs</code> must be a multiple of eight (<code>b0</code> or <code>b8</code>); otherwise the operation of this instruction is not defined. <code>ALL8</code> reduces eight test results such that the result is true if all eight tests are true.</P><P>When the sense of the <code>bs</code> Booleans is inverted (0 <code>&#174;</code> true, 1 <code>&#174;</code> false), use <code>ANY8</code> and an inverted test of the result.</P>",
        "Operation": "<pre>BR<SUB>t</SUB> &#8592; BR<SUB>s+7</SUB> and ... and BR<SUB>s+0</SUB></pre>"
    },
    "AND": {
        "name": "AND",
        "syn": "Bitwise Logical And",
        "Assembler Syntax": "AND ar, as, at",
        "Description": "<P><code>AND</code> calculates the bitwise logical and of address registers <code>as</code> and <code>at</code>. The result is written to address register <code>ar</code>.</P>",
        "Operation": "<pre>AR[r] &#8592; AR[s] and AR[t]</pre>"
    },
    "ANDB": {
        "name": "ANDB",
        "syn": "Boolean And",
        "Assembler Syntax": "ANDB br, bs, bt",
        "Description": "<P><code>ANDB</code> performs the logical and of Boolean registers <code>bs</code> and <code>bt</code> and writes the result to Boolean register <code>br</code>.</P><P>When the sense of one of the source Booleans is inverted (0 <code>&#174;</code> true, 1 <code>&#174;</code> false), use <code>ANDBC</code>. When the sense of both of the source Booleans is inverted, use <code>ORB</code> and an inverted test of the result.</P>",
        "Operation": "<pre>BR<SUB>r</SUB> &#8592; BR<SUB>s</SUB> and BR<SUB>t</SUB></pre>"
    },
    "ANDBC": {
        "name": "ANDBC",
        "syn": "Boolean And with Complement",
        "Assembler Syntax": "ANDBC br, bs, bt",
        "Description": "<P><code>ANDBC</code> performs the logical and of Boolean register <code>bs</code> with the logical complement of Boolean register <code>bt</code>, and writes the result to Boolean register <code>br</code>.</P>",
        "Operation": "<pre>BR<SUB>r</SUB> &#8592; BR<SUB>s</SUB> and not BR<SUB>t</SUB></pre>"
    },
    "ANY4": {
        "name": "ANY4",
        "syn": "Any 4 Booleans True",
        "Assembler Syntax": "ANY4 bt, bs",
        "Description": "<P><code>ANY4</code> sets Boolean register <code>bt</code> to the logical or of the four Boolean registers <code>bs</code><code>+</code><code>0</code>, <code>bs</code><code>+</code><code>1</code>, <code>bs</code><code>+</code><code>2</code>, and <code>bs</code><code>+</code><code>3</code>. <code>bs</code> must be a multiple of four (<code>b0</code>, <code>b4</code>, <code>b8</code>, or <code>b12</code>); otherwise the operation of this instruction is not defined. <code>ANY4</code> reduces four test results such that the result is true if any of the four tests are true.</P><P>When the sense of the <code>bs</code> Booleans is inverted (0 <code>&#174;</code> true, 1 <code>&#174;</code> false), use <code>ALL4</code> and an inverted test of the result.</P>",
        "Operation": "<pre>BR<SUB>t</SUB> &#8592; BR<SUB>s+3</SUB> or BR<SUB>s+2</SUB> or BR<SUB>s+1</SUB> or BR<SUB>s+0</SUB></pre>"
    },
    "ANY8": {
        "name": "ANY8",
        "syn": "Any 8 Booleans True",
        "Assembler Syntax": "ANY8 bt, bs",
        "Description": "<P><code>ANY8</code> sets Boolean register <code>bt</code> to the logical or of the eight Boolean registers <code>bs</code><code>+</code><code>0</code>, <code>bs</code><code>+</code><code>1</code>, ... <code>bs</code><code>+</code><code>6</code>, and <code>bs</code><code>+</code><code>7</code>. <code>bs</code> must be a multiple of eight (<code>b0</code> or <code>b8</code>); otherwise the operation of this instruction is not defined. <code>ANY8</code> reduces eight test results such that the result is true if any of the eight tests are true.</P><P>When the sense of the <code>bs</code> Booleans is inverted (0 <code>&#174;</code> true, 1 <code>&#174;</code> false), use <code>ALL8</code> and an inverted test of the result.</P>",
        "Operation": "<pre>BR<SUB>t</SUB> &#8592; BR<SUB>s+7</SUB> or ... or BR<SUB>s+0</SUB></pre>"
    },
    "BALL": {
        "name": "BALL",
        "syn": "Branch if All Bits Set",
        "Assembler Syntax": "BALL as, at, label",
        "Description": "<P><code>BALL</code> branches if all the bits specified by the mask in address register <code>at</code> are set in address register <code>as</code>. The test is performed by taking the bitwise logical and of <code>at</code> and the complement of <code>as</code>, and testing if the result is zero.</P><P>The target instruction address of the branch is given by the address of the <code>BALL</code> instruction, plus the sign-extended 8-bit <code>imm8</code> field of the instruction plus four. If any of the masked bits are clear, execution continues with the next sequential instruction.</P><P>The inverse of <code>BALL</code> is <code>BNALL</code>.</P>",
        "Operation": "<pre>if ((not AR[s]) and AR[t]) = 0<SUP>32</SUP> then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BALL.W15": {
        "name": "BALL.W15",
        "syn": "Branch if All Bits Set",
        "Assembler Syntax": "BALL.W15 as, at, label",
        "Description": "The operation of the <code>BALL.W15</code> branch instruction is very similar to the Xtensa ISA instruction <code>BALL</code>. The only difference is that the <code>BALL.W15</code> instruction provides for a wider immediate field of 15 bits to encode the branch offset, thus enabling branches over a greater address range.</P>",
        "Operation": "<pre>if ((not AR[s]) and AR[t]) = 0<SUP>32</SUP> then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BALL.W18": {
        "name": "BALL.W18",
        "syn": "Branch if All Bits Set",
        "Assembler Syntax": "BALL.W18 as, at, label",
        "Description": "The operation of the <code>BALL.W18</code> branch instruction is very similar to the Xtensa ISA instruction <code>BALL</code>. The only difference is that the <code>BALL.W18</code> instruction provides for a wider immediate field of 18 bits to encode the branch offset, thus enabling branches over a greater address range.</P>",
        "Operation": "<pre>if ((not AR[s]) and AR[t]) = 0<SUP>32</SUP> then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BANY": {
        "name": "BANY",
        "syn": "Branch if Any Bit Set",
        "Assembler Syntax": "BANY as, at, label",
        "Description": "<P><code>BANY</code> branches if any of the bits specified by the mask in address register <code>at</code> are set in address register <code>as</code>. The test is performed by taking the bitwise logical and of <code>as</code> and <code>at</code> and testing if the result is non-zero.</P><P>The target instruction address of the branch is given by the address of the <code>BANY</code> instruction, plus the sign-extended 8-bit <code>imm8</code> field of the instruction plus four. If all of the masked bits are clear, execution continues with the next sequential instruction.</P><P>The inverse of <code>BANY</code> is <code>BNONE</code>.</P>",
        "Operation": "<pre>if (AR[s] and AR[t]) != 0<SUP>32</SUP> then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BANY.W15": {
        "name": "BANY.W15",
        "syn": "Branch if Any Bit Set",
        "Assembler Syntax": "BANY.W15 as, at, label",
        "Description": "The operation of the <code>BANY.W15</code> branch instruction is very similar to the Xtensa ISA instruction <code>BANY</code>. The only difference is that the <code>BANY.W15</code> instruction provides for a wider immediate field of 15 bits to encode the branch offset, thus enabling branches over a greater address range.</P>",
        "Operation": "<pre>if (AR[s] and AR[t]) != 0<SUP>32</SUP> then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BANY.W18": {
        "name": "BANY.W18",
        "syn": "Branch if Any Bit Set",
        "Assembler Syntax": "BANY.W18 as, at, label",
        "Description": "The operation of the <code>BANY.W18</code> branch instruction is very similar to the Xtensa ISA instruction <code>BANY</code>. The only difference is that the <code>BANY.W18</code> instruction provides for a wider immediate field of 18 bits to encode the branch offset, thus enabling branches over a greater address range.</P>",
        "Operation": "<pre>if (AR[s] and AR[t]) != 0<SUP>32</SUP> then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BBC": {
        "name": "BBC",
        "syn": "Branch if Bit Clear",
        "Assembler Syntax": "BBC as, at, label",
        "Description": "<P><code>BBC</code> branches if the bit specified by the low five bits of address register <code>at</code> is clear in address register <code>as</code>. For little-endian processors, bit 0 is the least significant bit and bit 31 is the most significant bit. For big-endian processors, bit 0 is the most significant bit and bit 31 is the least significant bit.</P><P>The target instruction address of the branch is given by the address of the <code>BBC</code> instruction, plus the sign-extended 8-bit <code>imm8</code> field of the instruction plus four. If the specified bit is set, execution continues with the next sequential instruction.</P><P>The inverse of <code>BBC</code> is <code>BBS</code>.</P>",
        "Operation": "<pre>b &#8592; AR[t]<SUB>4..0</SUB> xor msbFirst<SUP>5</SUP>\nif AR[s]<SUB>b</SUB> = 0 then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BBC.W15": {
        "name": "BBC.W15",
        "syn": "Branch if Bit Clear",
        "Assembler Syntax": "BBC.W15 as, at, label",
        "Description": "The operation of the <code>BBC.W15</code> branch instruction is very similar to the Xtensa ISA instruction <code>BBC</code>. The only difference is that the <code>BBC.W15</code> instruction provides for a wider immediate field of 15 bits to encode the branch offset, thus enabling branches over a greater address range.</P>",
        "Operation": "<pre>b &#8592; AR[t]<SUB>4..0</SUB> xor msbFirst<SUP>5</SUP>\nif AR[s]<SUB>b</SUB> = 0 then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BBC.W18": {
        "name": "BBC.W18",
        "syn": "Branch if Bit Clear",
        "Assembler Syntax": "BBC.W18 as, at, label",
        "Description": "The operation of the <code>BBC.W18</code> branch instruction is very similar to the Xtensa ISA instruction <code>BBC</code>. The only difference is that the <code>BBC.W18</code> instruction provides for a wider immediate field of 18 bits to encode the branch offset, thus enabling branches over a greater address range.</P>",
        "Operation": "<pre>b &#8592; AR[t]<SUB>4..0</SUB> xor msbFirst<SUP>5</SUP>\nif AR[s]<SUB>b</SUB> = 0 then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BBCI": {
        "name": "BBCI",
        "syn": "Branch if Bit Clear Immediate",
        "Assembler Syntax": "BBCI as, 0..31, label",
        "Description": "<P><code>BBCI</code> branches if the bit specified by the constant encoded in the <code>bbi</code> field of the instruction word is clear in address register <code>as</code>. For little-endian processors, bit 0 is the least significant bit and bit 31 is the most significant bit. For big-endian processors bit 0 is the most significant bit and bit 31 is the least significant bit. The <code>bbi</code> field is split, with bits 3..0 in bits 7..4 of the instruction word, and bit 4 in bit 12 of the instruction word.</P><P>The target instruction address of the branch is given by the address of the <code>BBCI</code> instruction, plus the sign-extended 8-bit <code>imm8</code> field of the instruction plus four. If the specified bit is set, execution continues with the next sequential instruction.</P><P>The inverse of <code>BBCI</code> is <code>BBSI</code>.</P>",
        "Operation": "<pre>b &#8592; bbi xor msbFirst<SUP>5</SUP>\nif AR[s]<SUB>b</SUB> = 0 then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BBCI.L": {
        "name": "BBCI.L",
        "syn": "Branch if Bit Clear Immediate LE",
        "Assembler Syntax": "BBCI.L as, 0..31, label",
        "Description": "<P><code>BBCI.L</code> is an assembler macro for <code>BBCI</code> that always uses little-endian bit numbering. That is, it branches if the bit specified by its immediate is clear in address register <code>as</code>, where bit 0 is the least significant bit and bit 31 is the most significant bit.</P><P>The inverse of <code>BBCI.L</code> is <code>BBSI.L</code>.</P>"
    },
    "BBCI.W15": {
        "name": "BBCI.W15",
        "syn": "Branch if Bit Clear Immediate",
        "Assembler Syntax": "BBCI.W15 as, 0..31, label",
        "Description": "The operation of the <code>BBCI.W15</code> branch instruction is very similar to the Xtensa ISA instruction <code>BBCI</code>. The only difference is that the <code>BBCI.W15</code> instruction provides for a wider immediate field of 15 bits to encode the branch offset, thus enabling branches over a greater address range.</P>",
        "Operation": "<pre>b &#8592; bbi xor msbFirst<SUP>5</SUP>\nif AR[s]<SUB>b</SUB> = 0 then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BBCI.W18": {
        "name": "BBCI.W18",
        "syn": "Branch if Bit Clear Immediate",
        "Assembler Syntax": "BBCI.W18 as, 0..31, label",
        "Description": "The operation of the <code>BBCI.W18</code> branch instruction is very similar to the Xtensa ISA instruction <code>BBCI</code>. The only difference is that the <code>BBCI.W18</code> instruction provides for a wider immediate field of 18 bits to encode the branch offset, thus enabling branches over a greater address range.</P>",
        "Operation": "<pre>b &#8592; bbi xor msbFirst<SUP>5</SUP>\nif AR[s]<SUB>b</SUB> = 0 then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BBS": {
        "name": "BBS",
        "syn": "Branch if Bit Set",
        "Assembler Syntax": "BBS as, at, label",
        "Description": "<P><code>BBS</code> branches if the bit specified by the low five bits of address register <code>at</code> is set in address register <code>as</code>. For little-endian processors, bit 0 is the least significant bit and bit 31 is the most significant bit. For big-endian processors, bit 0 is the most significant bit and bit 31 is the least significant bit.</P><P>The target instruction address of the branch is given by the address of the <code>BBS</code> instruction, plus the sign-extended 8-bit <code>imm8</code> field of the instruction plus four. If the specified bit is clear, execution continues with the next sequential instruction.</P><P>The inverse of <code>BBS</code> is <code>BBC</code>.</P>",
        "Operation": "<pre>b &#8592; AR[t]<SUB>4..0</SUB> xor msbFirst<SUP>5</SUP>\nif AR[s]<SUB>b</SUB> != 0 then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BBS.W15": {
        "name": "BBS.W15",
        "syn": "Branch if Bit Set",
        "Assembler Syntax": "BBS.W15 as, at, label",
        "Description": "The operation of the <code>BBS.W15</code> branch instruction is very similar to the Xtensa ISA instruction <code>BBS</code>. The only difference is that the <code>BBS.W15</code> instruction provides for a wider immediate field of 15 bits to encode the branch offset, thus enabling branches over a greater address range.</P>",
        "Operation": "<pre>b &#8592; AR[t]<SUB>4..0</SUB> xor msbFirst<SUP>5</SUP>\nif AR[s]<SUB>b</SUB> != 0 then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BBS.W18": {
        "name": "BBS.W18",
        "syn": "Branch if Bit Set",
        "Assembler Syntax": "BBS.W18 as, at, label",
        "Description": "The operation of the <code>BBS.W18</code> branch instruction is very similar to the Xtensa ISA instruction <code>BBS</code>. The only difference is that the <code>BBS.W18</code> instruction provides for a wider immediate field of 18 bits to encode the branch offset, thus enabling branches over a greater address range.</P>",
        "Operation": "<pre>b &#8592; AR[t]<SUB>4..0</SUB> xor msbFirst<SUP>5</SUP>\nif AR[s]<SUB>b</SUB> != 0 then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BBSI": {
        "name": "BBSI",
        "syn": "Branch if Bit Set Immediate",
        "Assembler Syntax": "BBSI as, 0..31, label",
        "Description": "<P><code>BBSI</code> branches if the bit specified by the constant encoded in the <code>bbi</code> field of the instruction word is set in address register <code>as</code>. For little-endian processors, bit 0 is the least significant bit and bit 31 is the most significant bit. For big-endian processors, bit 0 is the most significant bit and bit 31 is the least significant bit. The <code>bbi</code> field is split, with bits 3..0 in bits 7..4 of the instruction word, and bit 4 in bit 12 of the instruction word.</P><P>The target instruction address of the branch is given by the address of the <code>BBSI</code> instruction, plus the sign-extended 8-bit <code>imm8</code> field of the instruction plus four. If the specified bit is clear, execution continues with the next sequential instruction.</P><P>The inverse of <code>BBSI</code> is <code>BBCI</code>.</P>",
        "Operation": "<pre>b &#8592; bbi xor msbFirst<SUP>5</SUP>\nif AR[s]<SUB>b</SUB> != 0 then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BBSI.L": {
        "name": "BBSI.L",
        "syn": "Branch if Bit Set Immediate LE",
        "Assembler Syntax": "BBSI.L as, 0..31, label",
        "Description": "<P><code>BBSI.L</code> is an assembler macro for <code>BBSI</code> that always uses little-endian bit numbering. That is, it branches if the bit specified by its immediate is set in address register <code>as</code>, where bit 0 is the least significant bit and bit 31 is the most significant bit.</P><P>The inverse of <code>BBSI.L</code> is <code>BBCI.L</code>.</P>"
    },
    "BBSI.W15": {
        "name": "BBSI.W15",
        "syn": "Branch if Bit Set Immediate",
        "Assembler Syntax": "BBSI.W15 as, 0..31, label",
        "Description": "The operation of the <code>BBSI.W15</code> branch instruction is very similar to the Xtensa ISA instruction <code>BBSI</code>. The only difference is that the <code>BBSI.W15</code> instruction provides for a wider immediate field of 15 bits to encode the branch offset, thus enabling branches over a greater address range.</P>",
        "Operation": "<pre>b &#8592; bbi xor msbFirst<SUP>5</SUP>\nif AR[s]<SUB>b</SUB> != 0 then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BBSI.W18": {
        "name": "BBSI.W18",
        "syn": "Branch if Bit Set Immediate",
        "Assembler Syntax": "BBSI.W18 as, 0..31, label",
        "Description": "The operation of the <code>BBSI.W18</code> branch instruction is very similar to the Xtensa ISA instruction <code>BBSI</code>. The only difference is that the <code>BBSI.W18</code> instruction provides for a wider immediate field of 18 bits to encode the branch offset, thus enabling branches over a greater address range.</P>",
        "Operation": "<pre>b &#8592; bbi xor msbFirst<SUP>5</SUP>\nif AR[s]<SUB>b</SUB> != 0 then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BEQ": {
        "name": "BEQ",
        "syn": "Branch if Equal",
        "Assembler Syntax": "BEQ as, at, label",
        "Description": "<P><code>BEQ</code> branches if address registers <code>as</code> and <code>at</code> are equal.</P><P>The target instruction address of the branch is given by the address of the <code>BEQ</code> instruction plus the sign-extended 8-bit <code>imm8</code> field of the instruction plus four. If the registers are not equal, execution continues with the next sequential instruction.</P><P>The inverse of <code>BEQ</code> is <code>BNE</code>.</P>",
        "Operation": "<pre>if AR[s] = AR[t] then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BEQ.W15": {
        "name": "BEQ.W15",
        "syn": "Branch if Equal",
        "Assembler Syntax": "BEQ.W15 as, at, label",
        "Description": "The operation of the <code>BEQ.W15</code> branch instruction is very similar to the Xtensa ISA instruction <code>BEQ</code>. The only difference is that the <code>BEQ.W15</code> instruction provides for a wider immediate field of 15 bits to encode the branch offset, thus enabling branches over a greater address range.</P>",
        "Operation": "<pre>if AR[s] = AR[t] then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BEQ.W18": {
        "name": "BEQ.W18",
        "syn": "Branch if Equal",
        "Assembler Syntax": "BEQ.W18 as, at, label",
        "Description": "The operation of the <code>BEQ.W18</code> branch instruction is very similar to the Xtensa ISA instruction <code>BEQ</code>. The only difference is that the <code>BEQ.W18</code> instruction provides for a wider immediate field of 18 bits to encode the branch offset, thus enabling branches over a greater address range.</P>",
        "Operation": "<pre>if AR[s] = AR[t] then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BEQI": {
        "name": "BEQI",
        "syn": "Branch if Equal Immediate",
        "Assembler Syntax": "BEQI as, imm, label",
        "Description": "<P><code>BEQI</code> branches if address register <code>as</code> and a constant encoded in the <code>r</code> field are equal. The constant values encoded in the <code>r</code> field are not simply 0..15. For the constant values that can be encoded by <code>r</code>, see <code> Xtensa Instruction Set Architecture Reference Manual</code>.</P><P>The target instruction address of the branch is given by the address of the <code>BEQI</code> instruction, plus the sign-extended 8-bit <code>imm8</code> field of the instruction plus four. If the register is not equal to the constant, execution continues with the next sequential instruction.</P><P>The inverse of <code>BEQI</code> is <code>BNEI</code>.</P>",
        "Operation": "<pre>if AR[s] = B4CONST(r) then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BEQI.W15": {
        "name": "BEQI.W15",
        "syn": "Branch if Equal Immediate",
        "Assembler Syntax": "BEQI.W15 as, imm, label",
        "Description": "The operation of the <code>BEQI.W15</code> branch instruction is very similar to the Xtensa ISA instruction <code>BEQI</code>. The only difference is that the <code>BEQI.W15</code> instruction provides for a wider immediate field of 15 bits to encode the branch offset, thus enabling branches over a greater address range.</P>",
        "Operation": "<pre>if AR[s] = B4CONST(r) then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BEQI.W18": {
        "name": "BEQI.W18",
        "syn": "Branch if Equal Immediate",
        "Assembler Syntax": "BEQI.W18 as, imm, label",
        "Description": "The operation of the <code>BEQI.W18</code> branch instruction is very similar to the Xtensa ISA instruction <code>BEQI</code>. The only difference is that the <code>BEQI.W18</code> instruction provides for a wider immediate field of 18 bits to encode the branch offset, thus enabling branches over a greater address range.</P>",
        "Operation": "<pre>if AR[s] = B4CONST(r) then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BEQZ": {
        "name": "BEQZ",
        "syn": "Branch if Equal to Zero",
        "Assembler Syntax": "BEQZ as, label",
        "Description": "<P><code>BEQZ</code> branches if address register <code>as</code> is equal to zero. <code>BEQZ</code> provides 12 bits of target range instead of the eight bits available in most conditional branches.</P><P>The target instruction address of the branch is given by the address of the <code>BEQZ</code> instruction, plus the sign-extended 12-bit <code>imm12</code> field of the instruction plus four. If register <code>as</code> is not equal to zero, execution continues with the next sequential instruction.</P><P>The inverse of <code>BEQZ</code> is <code>BNEZ</code>.</P>",
        "Operation": "<pre>if AR[s] = 0<SUP>32</SUP> then\n\tnextPC &#8592; PC + (imm12<SUB>11</SUB><SUP>20</SUP>||imm12) + 4\nendif</pre>"
    },
    "BEQZ.N": {
        "name": "BEQZ.N",
        "syn": "Narrow Branch if Equal Zero",
        "Assembler Syntax": "BEQZ.N as, label",
        "Description": "<P>This performs the same operation as the <code>BEQZ</code> instruction in a 16-bit encoding. <code>BEQZ.N</code> branches if address register <code>as</code> is equal to zero. <code>BEQZ.N</code> provides six bits of target range instead of the 12 bits available in <code>BEQZ</code>.</P><P>The target instruction address of the branch is given by the address of the <code>BEQZ.N</code> instruction, plus the zero-extended 6-bit <code>imm6</code> field of the instruction plus four. Because the offset is unsigned, this instruction can only be used to branch forward. If register <code>as</code> is not equal to zero, execution continues with the next sequential instruction.</P><P>The inverse of <code>BEQZ.N</code> is <code>BNEZ.N</code>.</P>",
        "Operation": "<pre>if AR[s] = 0<SUP>32</SUP> then\n\tnextPC &#8592; PC + (0<SUP>26</SUP>||imm6) + 4\nendif</pre>"
    },
    "BEQZ.W15": {
        "name": "BEQZ.W15",
        "syn": "Branch if Equal to Zero",
        "Assembler Syntax": "BEQZ.W15 as, label",
        "Description": "The operation of the <code>BEQZ.W15</code> branch instruction is very similar to the Xtensa ISA instruction <code>BEQZ</code>. The only difference is that the <code>BEQZ.W15</code> instruction provides for a wider immediate field of 15 bits to encode the branch offset, thus enabling branches over a greater address range.</P>",
        "Operation": "<pre>if AR[s] = 0<SUP>32</SUP> then\n\tnextPC &#8592; PC + (imm12<SUB>11</SUB><SUP>20</SUP>||imm12) + 4\nendif</pre>"
    },
    "BEQZ.W18": {
        "name": "BEQZ.W18",
        "syn": "Branch if Equal to Zero",
        "Assembler Syntax": "BEQZ.W18 as, label",
        "Description": "The operation of the <code>BEQZ.W18</code> branch instruction is very similar to the Xtensa ISA instruction <code>BEQZ</code>. The only difference is that the <code>BEQZ.W18</code> instruction provides for a wider immediate field of 18 bits to encode the branch offset, thus enabling branches over a greater address range.</P>",
        "Operation": "<pre>if AR[s] = 0<SUP>32</SUP> then\n\tnextPC &#8592; PC + (imm12<SUB>11</SUB><SUP>20</SUP>||imm12) + 4\nendif</pre>"
    },
    "BF": {
        "name": "BF",
        "syn": "Branch if False",
        "Assembler Syntax": "BF bs, label",
        "Description": "<P><code>BF</code> branches to the target address if Boolean register <code>bs</code> is false.</P><P>The target instruction address of the branch is given by the address of the <code>BF</code> instruction plus the sign-extended 8-bit <code>imm8</code> field of the instruction plus four. If the Boolean register <code>bs</code> is true, execution continues with the next sequential instruction.</P><P>The inverse of <code>BF</code> is <code>BT</code>.</P>",
        "Operation": "<pre>if not BR<SUB>s</SUB> then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BGE": {
        "name": "BGE",
        "syn": "Branch if Greater Than or Equal",
        "Assembler Syntax": "BGE as, at, label",
        "Description": "<P><code>BGE</code> branches if address register <code>as</code> is two's complement greater than or equal to address register <code>at</code>.</P><P>The target instruction address of the branch is given by the address of the <code>BGE</code> instruction, plus the sign-extended 8-bit <code>imm8</code> field of the instruction plus four. If the address register <code>as</code> is less than address register <code>at</code>, execution continues with the next sequential instruction.</P><P>The inverse of <code>BGE</code> is <code>BLT</code>.</P>",
        "Operation": "<pre>if AR[s] >= AR[t] then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BGE.W15": {
        "name": "BGE.W15",
        "syn": "Branch if Greater Than or Equal",
        "Assembler Syntax": "BGE.W15 as, at, label",
        "Description": "The operation of the <code>BGE.W15</code> branch instruction is very similar to the Xtensa ISA instruction <code>BGE</code>. The only difference is that the <code>BGE.W15</code> instruction provides for a wider immediate field of 15 bits to encode the branch offset, thus enabling branches over a greater address range.</P>",
        "Operation": "<pre>if AR[s] >= AR[t] then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BGE.W18": {
        "name": "BGE.W18",
        "syn": "Branch if Greater Than or Equal",
        "Assembler Syntax": "BGE.W18 as, at, label",
        "Description": "The operation of the <code>BGE.W18</code> branch instruction is very similar to the Xtensa ISA instruction <code>BGE</code>. The only difference is that the <code>BGE.W18</code> instruction provides for a wider immediate field of 18 bits to encode the branch offset, thus enabling branches over a greater address range.</P>",
        "Operation": "<pre>if AR[s] >= AR[t] then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BGEI": {
        "name": "BGEI",
        "syn": "Branch if Greater Than or Equal Immediate",
        "Assembler Syntax": "BGEI as, imm, label",
        "Description": "<P><code>BGEI</code> branches if address register <code>as</code> is two's complement greater than or equal to the constant encoded in the <code>r</code> field. The constant values encoded in the <code>r</code> field are not simply 0..15. For the constant values that can be encoded by <code>r</code>, see  Xtensa Instruction Set Architecture Reference Manual.</P><P>The target instruction address of the branch is given by the address of the <code>BGEI</code> instruction, plus the sign-extended 8-bit <code>imm8</code> field of the instruction plus four. If the address register <code>as</code> is less than the constant, execution continues with the next sequential instruction.</P><P>The inverse of <code>BGEI</code> is <code>BLTI</code>.</P>",
        "Operation": "<pre>if AR[s] >= B4CONST(r) then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BGEI.W15": {
        "name": "BGEI.W15",
        "syn": "Branch if Greater Than or Equal Immediate",
        "Assembler Syntax": "BGEI.W15 as, imm, label",
        "Description": "The operation of the <code>BGEI.W15</code> branch instruction is very similar to the Xtensa ISA instruction <code>BGEI</code>. The only difference is that the <code>BGEI.W15</code> instruction provides for a wider immediate field of 15 bits to encode the branch offset, thus enabling branches over a greater address range.</P>",
        "Operation": "<pre>if AR[s] >= B4CONST(r) then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BGEI.W18": {
        "name": "BGEI.W18",
        "syn": "Branch if Greater Than or Equal Immediate",
        "Assembler Syntax": "BGEI.W18 as, imm, label",
        "Description": "The operation of the <code>BGEI.W18</code> branch instruction is very similar to the Xtensa ISA instruction <code>BGEI</code>. The only difference is that the <code>BGEI.W18</code> instruction provides for a wider immediate field of 18 bits to encode the branch offset, thus enabling branches over a greater address range.</P>",
        "Operation": "<pre>if AR[s] >= B4CONST(r) then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BGEU": {
        "name": "BGEU",
        "syn": "Branch if Greater Than or Equal Unsigned",
        "Assembler Syntax": "BGEU as, at, label",
        "Description": "<P><code>BGEU</code> branches if address register <code>as</code> is unsigned greater than or equal to address register <code>at</code>.</P><P>The target instruction address of the branch is given by the address of the <code>BGEU</code> instruction, plus the sign-extended 8-bit <code>imm8</code> field of the instruction plus four. If the address register <code>as</code> is unsigned less than address register <code>at</code>, execution continues with the next sequential instruction.</P><P>The inverse of <code>BGEU</code> is <code>BLTU</code>.</P>",
        "Operation": "<pre>if (0||AR[s]) >= (0||AR[t]) then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BGEU.W15": {
        "name": "BGEU.W15",
        "syn": "Branch if Greater Than or Equal Unsigned",
        "Assembler Syntax": "BGEU.W15 as, at, label",
        "Description": "The operation of the <code>BGEU.W15</code> branch instruction is very similar to the Xtensa ISA instruction <code>BGEU</code>. The only difference is that the <code>BGEU.W15</code> instruction provides for a wider immediate field of 15 bits to encode the branch offset, thus enabling branches over a greater address range.</P>",
        "Operation": "<pre>if (0||AR[s]) >= (0||AR[t]) then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BGEU.W18": {
        "name": "BGEU.W18",
        "syn": "Branch if Greater Than or Equal Unsigned",
        "Assembler Syntax": "BGEU.W18 as, at, label",
        "Description": "The operation of the <code>BGEU.W18</code> branch instruction is very similar to the Xtensa ISA instruction <code>BGEU</code>. The only difference is that the <code>BGEU.W18</code> instruction provides for a wider immediate field of 18 bits to encode the branch offset, thus enabling branches over a greater address range.</P>",
        "Operation": "<pre>if (0||AR[s]) >= (0||AR[t]) then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BGEUI": {
        "name": "BGEUI",
        "syn": "Branch if Greater Than or Equal Unsigned Immediate",
        "Assembler Syntax": "BGEUI as, imm, label",
        "Description": "<P><code>BGEUI</code> branches if address register <code>as</code> is unsigned greater than or equal to the constant encoded in the <code>r</code> field. The constant values encoded in the <code>r</code> field are not simply 0..15. For the constant values that can be encoded by <code>r</code>, see  Branch Unsigned Immediate (b4constu) Encodings.</P><P>The target instruction address of the branch is given by the address of the <code>BGEUI</code> instruction plus the sign-extended 8-bit <code>imm8</code> field of the instruction plus four. If the address register <code>as</code> is less than the constant, execution continues with the next sequential instruction.</P><P>The inverse of <code>BGEUI</code> is <code>BLTUI</code>.</P>",
        "Operation": "<pre>if (0||AR[s]) >= (0||B4CONSTU(r)) then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BGEUI.W15": {
        "name": "BGEUI.W15",
        "syn": "Branch if Greater Than or Equal Unsigned Immediate",
        "Assembler Syntax": "BGEUI.W15 as, imm, label",
        "Description": "The operation of the <code>BGEUI.W15</code> branch instruction is very similar to the Xtensa ISA instruction <code>BGEUI</code>. The only difference is that the <code>BGEUI.W15</code> instruction provides for a wider immediate field of 15 bits to encode the branch offset, thus enabling branches over a greater address range.</P>",
        "Operation": "<pre>if (0||AR[s]) >= (0||B4CONSTU(r)) then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BGEUI.W18": {
        "name": "BGEUI.W18",
        "syn": "Branch if Greater Than or Equal Unsigned Immediate",
        "Assembler Syntax": "BGEUI.W18 as, imm, label",
        "Description": "The operation of the <code>BGEUI.W18</code> branch instruction is very similar to the Xtensa ISA instruction <code>BGEUI</code>. The only difference is that the <code>BGEUI.W18</code> instruction provides for a wider immediate field of 18 bits to encode the branch offset, thus enabling branches over a greater address range.</P>",
        "Operation": "<pre>if (0||AR[s]) >= (0||B4CONSTU(r)) then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BGEZ": {
        "name": "BGEZ",
        "syn": "Branch if Greater Than or Equal to Zero",
        "Assembler Syntax": "BGEZ as, label",
        "Description": "<P><code>BGEZ</code> branches if address register <code>as</code> is greater than or equal to zero (the most significant bit is clear). <code>BGEZ</code> provides 12 bits of target range instead of the eight bits available in most conditional branches.</P><P>The target instruction address of the branch is given by the address of the <code>BGEZ</code> instruction plus the sign-extended 12-bit <code>imm12</code> field of the instruction plus four. If register <code>as</code> is less than zero, execution continues with the next sequential instruction.</P><P>The inverse of <code>BGEZ</code> is <code>BLTZ</code>.</P>",
        "Operation": "<pre>if AR[s]<SUB>31</SUB> = 0 then\n\tnextPC &#8592; PC + (imm12<SUB>11</SUB><SUP>20</SUP>||imm12) + 4\nendif</pre>"
    },
    "BGEZ.W15": {
        "name": "BGEZ.W15",
        "syn": "Branch if Greater Than or Equal to Zero",
        "Assembler Syntax": "BGEZ.W15 as, label",
        "Description": "The operation of the <code>BGEZ.W15</code> branch instruction is very similar to the Xtensa ISA instruction <code>BGEZ</code>. The only difference is that the <code>BGEZ.W15</code> instruction provides for a wider immediate field of 15 bits to encode the branch offset, thus enabling branches over a greater address range.</P>",
        "Operation": "<pre>if AR[s]<SUB>31</SUB> = 0 then\n\tnextPC &#8592; PC + (imm12<SUB>11</SUB><SUP>20</SUP>||imm12) + 4\nendif</pre>"
    },
    "BGEZ.W18": {
        "name": "BGEZ.W18",
        "syn": "Branch if Greater Than or Equal to Zero",
        "Assembler Syntax": "BGEZ.W18 as, label",
        "Description": "The operation of the <code>BGEZ.W18</code> branch instruction is very similar to the Xtensa ISA instruction <code>BGEZ</code>. The only difference is that the <code>BGEZ.W18</code> instruction provides for a wider immediate field of 18 bits to encode the branch offset, thus enabling branches over a greater address range.</P>",
        "Operation": "<pre>if AR[s]<SUB>31</SUB> = 0 then\n\tnextPC &#8592; PC + (imm12<SUB>11</SUB><SUP>20</SUP>||imm12) + 4\nendif</pre>"
    },
    "BLT": {
        "name": "BLT",
        "syn": "Branch if Less Than",
        "Assembler Syntax": "BLT as, at, label",
        "Description": "<P><code>BLT</code> branches if address register <code>as</code> is two's complement less than address register <code>at</code>.</P><P>The target instruction address of the branch is given by the address of the <code>BLT</code> instruction plus the sign-extended 8-bit <code>imm8</code> field of the instruction plus four. If the address register <code>as</code> is greater than or equal to address register <code>at</code>, execution continues with the next sequential instruction.</P><P>The inverse of <code>BLT</code> is <code>BGE</code>.</P>",
        "Operation": "<pre>if AR[s] &lt; AR[t] then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BLT.W15": {
        "name": "BLT.W15",
        "syn": "Branch if Less Than",
        "Assembler Syntax": "BLT.W15 as, at, label",
        "Description": "The operation of the <code>BLT.W15</code> branch instruction is very similar to the Xtensa ISA instruction <code>BLT</code>. The only difference is that the <code>BLT.W15</code> instruction provides for a wider immediate field of 15 bits to encode the branch offset, thus enabling branches over a greater address range.</P>",
        "Operation": "<pre>if AR[s] &lt; AR[t] then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BLT.W18": {
        "name": "BLT.W18",
        "syn": "Branch if Less Than",
        "Assembler Syntax": "BLT.W18 as, at, label",
        "Description": "The operation of the <code>BLT.W18</code> branch instruction is very similar to the Xtensa ISA instruction <code>BLT</code>. The only difference is that the <code>BLT.W18</code> instruction provides for a wider immediate field of 18 bits to encode the branch offset, thus enabling branches over a greater address range.</P>",
        "Operation": "<pre>if AR[s] &lt; AR[t] then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BLTI": {
        "name": "BLTI",
        "syn": "Branch if Less Than Immediate",
        "Assembler Syntax": "BLTI as, imm, label",
        "Description": "<P><code>BLTI</code> branches if address register <code>as</code> is two's complement less than the constant encoded in the <code>r</code> field. The constant values encoded in the <code>r</code> field are not simply 0..15. For the constant values that can be encoded by <code>r</code>, see  Xtensa Instruction Set Architecture Reference Manual.</P><P>The target instruction address of the branch is given by the address of the <code>BLTI</code> instruction plus the sign-extended 8-bit <code>imm8</code> field of the instruction plus four. If the address register <code>as</code> is greater than or equal to the constant, execution continues with the next sequential instruction.</P><P>The inverse of <code>BLTI</code> is <code>BGEI</code>.</P>",
        "Operation": "<pre>if AR[s] &lt; B4CONST(r) then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BLTI.W15": {
        "name": "BLTI.W15",
        "syn": "Branch if Less Than Immediate",
        "Assembler Syntax": "BLTI.W15 as, imm, label",
        "Description": "The operation of the <code>BLTI.W15</code> branch instruction is very similar to the Xtensa ISA instruction <code>BLTI</code>. The only difference is that the <code>BLTI.W15</code> instruction provides for a wider immediate field of 15 bits to encode the branch offset, thus enabling branches over a greater address range.</P>",
        "Operation": "<pre>if AR[s] &lt; B4CONST(r) then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BLTI.W18": {
        "name": "BLTI.W18",
        "syn": "Branch if Less Than Immediate",
        "Assembler Syntax": "BLTI.W18 as, imm, label",
        "Description": "The operation of the <code>BLTI.W18</code> branch instruction is very similar to the Xtensa ISA instruction <code>BLTI</code>. The only difference is that the <code>BLTI.W18</code> instruction provides for a wider immediate field of 18 bits to encode the branch offset, thus enabling branches over a greater address range.</P>",
        "Operation": "<pre>if AR[s] &lt; B4CONST(r) then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BLTU": {
        "name": "BLTU",
        "syn": "Branch if Less Than Unsigned",
        "Assembler Syntax": "BLTU as, at, label",
        "Description": "<P><code>BLTU</code> branches if address register <code>as</code> is unsigned less than address register <code>at</code>.</P><P>The target instruction address of the branch is given by the address of the <code>BLTU</code> instruction, plus the sign-extended 8-bit <code>imm8</code> field of the instruction plus four. If the address register <code>as</code> is greater than or equal to address register <code>at</code>, execution continues with the next sequential instruction.</P><P>The inverse of <code>BLTU</code> is <code>BGEU</code>.</P>",
        "Operation": "<pre>if (0||AR[s]) &lt; (0||AR[t]) then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BLTU.W15": {
        "name": "BLTU.W15",
        "syn": "Branch if Less Than Unsigned",
        "Assembler Syntax": "BLTU.W15 as, at, label",
        "Description": "The operation of the <code>BLTU.W15</code> branch instruction is very similar to the Xtensa ISA instruction <code>BLTU</code>. The only difference is that the <code>BLTU.W15</code> instruction provides for a wider immediate field of 15 bits to encode the branch offset, thus enabling branches over a greater address range.</P>",
        "Operation": "<pre>if (0||AR[s]) &lt; (0||AR[t]) then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BLTU.W18": {
        "name": "BLTU.W18",
        "syn": "Branch if Less Than Unsigned",
        "Assembler Syntax": "BLTU.W18 as, at, label",
        "Description": "The operation of the <code>BLTU.W18</code> branch instruction is very similar to the Xtensa ISA instruction <code>BLTU</code>. The only difference is that the <code>BLTU.W18</code> instruction provides for a wider immediate field of 18 bits to encode the branch offset, thus enabling branches over a greater address range.</P>",
        "Operation": "<pre>if (0||AR[s]) &lt; (0||AR[t]) then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BLTUI": {
        "name": "BLTUI",
        "syn": "Branch if Less Than Unsigned Immediate",
        "Assembler Syntax": "BLTUI as, imm, label",
        "Description": "<P><code>BLTUI</code> branches if address register <code>as</code> is unsigned less than the constant encoded in the <code>r</code> field. The constant values encoded in the <code>r</code> field are not simply 0..15. For the constant values that can be encoded by <code>r</code>, see  Branch Unsigned Immediate (b4constu) Encodings.</P><P>The target instruction address of the branch is given by the address of the <code>BLTUI</code> instruction, plus the sign-extended 8-bit <code>imm8</code> field of the instruction plus four. If the address register <code>as</code> is greater than or equal to the constant, execution continues with the next sequential instruction.</P><P>The inverse of <code>BLTUI</code> is <code>BGEUI</code>.</P>",
        "Operation": "<pre>if (0||AR[s]) &lt; (0||B4CONSTU(r)) then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BLTUI.W15": {
        "name": "BLTUI.W15",
        "syn": "Branch if Less Than Unsigned Immediate",
        "Assembler Syntax": "BLTUI.W15 as, imm, label",
        "Description": "The operation of the <code>BLTUI.W15</code> branch instruction is very similar to the Xtensa ISA instruction <code>BLTUI</code>. The only difference is that the <code>BLTUI.W15</code> instruction provides for a wider immediate field of 15 bits to encode the branch offset, thus enabling branches over a greater address range.</P>",
        "Operation": "<pre>if (0||AR[s]) &lt; (0||B4CONSTU(r)) then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BLTUI.W18": {
        "name": "BLTUI.W18",
        "syn": "Branch if Less Than Unsigned Immediate",
        "Assembler Syntax": "BLTUI.W18 as, imm, label",
        "Description": "The operation of the <code>BLTUI.W18</code> branch instruction is very similar to the Xtensa ISA instruction <code>BLTUI</code>. The only difference is that the <code>BLTUI.W18</code> instruction provides for a wider immediate field of 18 bits to encode the branch offset, thus enabling branches over a greater address range.</P>",
        "Operation": "<pre>if (0||AR[s]) &lt; (0||B4CONSTU(r)) then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BLTZ": {
        "name": "BLTZ",
        "syn": "Branch if Less Than Zero",
        "Assembler Syntax": "BLTZ as, label",
        "Description": "<P><code>BLTZ</code> branches if address register <code>as</code> is less than zero (the most significant bit is set). <code>BLTZ</code> provides 12 bits of target range instead of the eight bits available in most conditional branches.</P><P>The target instruction address of the branch is given by the address of the <code>BLTZ</code> instruction, plus the sign-extended 12-bit <code>imm12</code> field of the instruction plus four. If register <code>as</code> is greater than or equal to zero, execution continues with the next sequential instruction.</P><P>The inverse of <code>BLTZ</code> is <code>BGEZ</code>.</P>",
        "Operation": "<pre>if AR[s]<SUB>31</SUB> != 0 then\n\tnextPC &#8592; PC + (imm12<SUB>11</SUB><SUP>20</SUP>||imm12) + 4\nendif</pre>"
    },
    "BLTZ.W15": {
        "name": "BLTZ.W15",
        "syn": "Branch if Less Than Zero",
        "Assembler Syntax": "BLTZ.W15 as, label",
        "Description": "The operation of the <code>BLTZ.W15</code> branch instruction is very similar to the Xtensa ISA instruction <code>BLTZ</code>. The only difference is that the <code>BLTZ.W15</code> instruction provides for a wider immediate field of 15 bits to encode the branch offset, thus enabling branches over a greater address range.</P>",
        "Operation": "<pre>if AR[s]<SUB>31</SUB> != 0 then\n\tnextPC &#8592; PC + (imm12<SUB>11</SUB><SUP>20</SUP>||imm12) + 4\nendif</pre>"
    },
    "BLTZ.W18": {
        "name": "BLTZ.W18",
        "syn": "Branch if Less Than Zero",
        "Assembler Syntax": "BLTZ.W18 as, label",
        "Description": "The operation of the <code>BLTZ.W18</code> branch instruction is very similar to the Xtensa ISA instruction <code>BLTZ</code>. The only difference is that the <code>BLTZ.W18</code> instruction provides for a wider immediate field of 18 bits to encode the branch offset, thus enabling branches over a greater address range.</P>",
        "Operation": "<pre>if AR[s]<SUB>31</SUB> != 0 then\n\tnextPC &#8592; PC + (imm12<SUB>11</SUB><SUP>20</SUP>||imm12) + 4\nendif</pre>"
    },
    "BNALL": {
        "name": "BNALL",
        "syn": "Branch if Not-All Bits Set",
        "Assembler Syntax": "BNALL as, at, label",
        "Description": "<P><code>BNALL</code> branches if any of the bits specified by the mask in address register <code>at</code> are clear in address register <code>as</code> (that is, if they are not all set). The test is performed by taking the bitwise logical and of <code>at</code> with the complement of <code>as</code> and testing if the result is non-zero.</P><P>The target instruction address of the branch is given by the address of the <code>BNALL</code> instruction, plus the sign-extended 8-bit <code>imm8</code> field of the instruction plus four. If all of the masked bits are set, execution continues with the next sequential instruction.</P><P>The inverse of <code>BNALL</code> is <code>BALL</code>.</P>",
        "Operation": "<pre>if ((not AR[s]) and AR[t]) != 0<SUP>32</SUP> then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BNALL.W15": {
        "name": "BNALL.W15",
        "syn": "Branch if Not-All Bits Set",
        "Assembler Syntax": "BNALL.W15 as, at, label",
        "Description": "The operation of the <code>BNALL.W15</code> branch instruction is very similar to the Xtensa ISA instruction <code>BNALL</code>. The only difference is that the <code>BNALL.W15</code> instruction provides for a wider immediate field of 15 bits to encode the branch offset, thus enabling branches over a greater address range.</P>",
        "Operation": "<pre>if ((not AR[s]) and AR[t]) != 0<SUP>32</SUP> then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BNALL.W18": {
        "name": "BNALL.W18",
        "syn": "Branch if Not-All Bits Set",
        "Assembler Syntax": "BNALL.W18 as, at, label",
        "Description": "The operation of the <code>BNALL.W18</code> branch instruction is very similar to the Xtensa ISA instruction <code>BNALL</code>. The only difference is that the <code>BNALL.W18</code> instruction provides for a wider immediate field of 18 bits to encode the branch offset, thus enabling branches over a greater address range.</P>",
        "Operation": "<pre>if ((not AR[s]) and AR[t]) != 0<SUP>32</SUP> then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BNE": {
        "name": "BNE",
        "syn": "Branch if Not Equal",
        "Assembler Syntax": "BNE as, at, label",
        "Description": "<P><code>BNE</code> branches if address registers <code>as</code> and <code>at</code> are not equal.</P><P>The target instruction address of the branch is given by the address of the <code>BNE</code> instruction, plus the sign-extended 8-bit <code>imm8</code> field of the instruction plus four. If the registers are equal, execution continues with the next sequential instruction.</P><P>The inverse of <code>BNE</code> is <code>BEQ</code>.</P>",
        "Operation": "<pre>if AR[s] != AR[t] then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BNE.W15": {
        "name": "BNE.W15",
        "syn": "Branch if Not Equal",
        "Assembler Syntax": "BNE.W15 as, at, label",
        "Description": "The operation of the <code>BNE.W15</code> branch instruction is very similar to the Xtensa ISA instruction <code>BNE</code>. The only difference is that the <code>BNE.W15</code> instruction provides for a wider immediate field of 15 bits to encode the branch offset, thus enabling branches over a greater address range.</P>",
        "Operation": "<pre>if AR[s] != AR[t] then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BNE.W18": {
        "name": "BNE.W18",
        "syn": "Branch if Not Equal",
        "Assembler Syntax": "BNE.W18 as, at, label",
        "Description": "The operation of the <code>BNE.W18</code> branch instruction is very similar to the Xtensa ISA instruction <code>BNE</code>. The only difference is that the <code>BNE.W18</code> instruction provides for a wider immediate field of 18 bits to encode the branch offset, thus enabling branches over a greater address range.</P>",
        "Operation": "<pre>if AR[s] != AR[t] then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BNEI": {
        "name": "BNEI",
        "syn": "Branch if Not Equal Immediate",
        "Assembler Syntax": "BNEI as, imm, label",
        "Description": "<P><code>BNEI</code> branches if address register <code>as</code> and a constant encoded in the <code>r</code> field are not equal. The constant values encoded in the <code>r</code> field are not simply 0..15. For the constant values that can be encoded by <code>r</code>, see  Xtensa Instruction Set Architecture Reference Manual.</P><P>The target instruction address of the branch is given by the address of the <code>BNEI</code> instruction, plus the sign-extended 8-bit <code>imm8</code> field of the instruction plus four. If the register is equal to the constant, execution continues with the next sequential instruction.</P><P>The inverse of <code>BNEI</code> is <code>BEQI</code>.</P>",
        "Operation": "<pre>if AR[s] != B4CONST(r) then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BNEI.W15": {
        "name": "BNEI.W15",
        "syn": "Branch if Not Equal Immediate",
        "Assembler Syntax": "BNEI.W15 as, imm, label",
        "Description": "The operation of the <code>BNEI.W15</code> branch instruction is very similar to the Xtensa ISA instruction <code>BNEI</code>. The only difference is that the <code>BNEI.W15</code> instruction provides for a wider immediate field of 15 bits to encode the branch offset, thus enabling branches over a greater address range.</P>",
        "Operation": "<pre>if AR[s] != B4CONST(r) then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BNEI.W18": {
        "name": "BNEI.W18",
        "syn": "Branch if Not Equal Immediate",
        "Assembler Syntax": "BNEI.W18 as, imm, label",
        "Description": "The operation of the <code>BNEI.W18</code> branch instruction is very similar to the Xtensa ISA instruction <code>BNEI</code>. The only difference is that the <code>BNEI.W18</code> instruction provides for a wider immediate field of 18 bits to encode the branch offset, thus enabling branches over a greater address range.</P>",
        "Operation": "<pre>if AR[s] != B4CONST(r) then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BNEZ": {
        "name": "BNEZ",
        "syn": "Branch if Not-Equal to Zero",
        "Assembler Syntax": "BNEZ as, label",
        "Description": "<P><code>BNEZ</code> branches if address register <code>as</code> is not equal to zero. <code>BNEZ</code> provides 12 bits of target range instead of the eight bits available in most conditional branches.</P><P>The target instruction address of the branch is given by the address of the <code>BNEZ</code> instruction, plus the sign-extended 12-bit <code>imm12</code> field of the instruction plus four. If register <code>as</code> is equal to zero, execution continues with the next sequential instruction.</P><P>The inverse of <code>BNEZ</code> is <code>BEQZ</code>.</P>",
        "Operation": "<pre>if AR[s] != 0<SUP>32</SUP> then\n\tnextPC &#8592; PC + (imm12<SUB>11</SUB><SUP>20</SUP>||imm12) + 4\nendif</pre>"
    },
    "BNEZ.N": {
        "name": "BNEZ.N",
        "syn": "Narrow Branch if Not Equal Zero",
        "Assembler Syntax": "BNEZ.N as, label",
        "Description": "<P>This performs the same operation as the <code>BNEZ</code> instruction in a 16-bit encoding. <code>BNEZ.N</code> branches if address register <code>as</code> is not equal to zero. <code>BNEZ.N</code> provides six bits of target range instead of the 12 bits available in <code>BNEZ</code>.</P><P>The target instruction address of the branch is given by the address of the <code>BNEZ.N</code> instruction, plus the zero-extended 6-bit <code>imm6</code> field of the instruction plus four. Because the offset is unsigned, this instruction can only be used to branch forward. If register <code>as</code> is equal to zero, execution continues with the next sequential instruction.</P><P>The inverse of <code>BNEZ.N</code> is <code>BEQZ.N</code>.</P>",
        "Operation": "<pre>if AR[s] != 0<SUP>32</SUP> then\n\tnextPC &#8592; PC + (0<SUP>26</SUP>||imm6) + 4\nendif</pre>"
    },
    "BNEZ.W15": {
        "name": "BNEZ.W15",
        "syn": "Branch if Not-Equal to Zero",
        "Assembler Syntax": "BNEZ.W15 as, label",
        "Description": "The operation of the <code>BNEZ.W15</code> branch instruction is very similar to the Xtensa ISA instruction <code>BNEZ</code>. The only difference is that the <code>BNEZ.W15</code> instruction provides for a wider immediate field of 15 bits to encode the branch offset, thus enabling branches over a greater address range.</P>",
        "Operation": "<pre>if AR[s] != 0<SUP>32</SUP> then\n\tnextPC &#8592; PC + (imm12<SUB>11</SUB><SUP>20</SUP>||imm12) + 4\nendif</pre>"
    },
    "BNEZ.W18": {
        "name": "BNEZ.W18",
        "syn": "Branch if Not-Equal to Zero",
        "Assembler Syntax": "BNEZ.W18 as, label",
        "Description": "The operation of the <code>BNEZ.W18</code> branch instruction is very similar to the Xtensa ISA instruction <code>BNEZ</code>. The only difference is that the <code>BNEZ.W18</code> instruction provides for a wider immediate field of 18 bits to encode the branch offset, thus enabling branches over a greater address range.</P>",
        "Operation": "<pre>if AR[s] != 0<SUP>32</SUP> then\n\tnextPC &#8592; PC + (imm12<SUB>11</SUB><SUP>20</SUP>||imm12) + 4\nendif</pre>"
    },
    "BNONE": {
        "name": "BNONE",
        "syn": "Branch if No Bit Set",
        "Assembler Syntax": "BNONE as, at, label",
        "Description": "<P><code>BNONE</code> branches if all of the bits specified by the mask in address register <code>at</code> are clear in address register <code>as</code> (that is, if none of them are set). The test is performed by taking the bitwise logical and of <code>as</code> with <code>at</code> and testing if the result is zero.</P><P>The target instruction address of the branch is given by the address of the <code>BNONE</code> instruction, plus the sign-extended 8-bit <code>imm8</code> field of the instruction plus four. If any of the masked bits are set, execution continues with the next sequential instruction.</P><P>The inverse of <code>BNONE</code> is <code>BANY</code>.</P>",
        "Operation": "<pre>if (AR[s] and AR[t]) = 0<SUP>32</SUP> then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BNONE.W15": {
        "name": "BNONE.W15",
        "syn": "Branch if No Bit Set",
        "Assembler Syntax": "BNONE.W15 as, at, label",
        "Description": "The operation of the <code>BNONE.W15</code> branch instruction is very similar to the Xtensa ISA instruction <code>BNONE</code>. The only difference is that the <code>BNONE.W15</code> instruction provides for a wider immediate field of 15 bits to encode the branch offset, thus enabling branches over a greater address range.</P>",
        "Operation": "<pre>if (AR[s] and AR[t]) = 0<SUP>32</SUP> then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BNONE.W18": {
        "name": "BNONE.W18",
        "syn": "Branch if No Bit Set",
        "Assembler Syntax": "BNONE.W18 as, at, label",
        "Description": "The operation of the <code>BNONE.W18</code> branch instruction is very similar to the Xtensa ISA instruction <code>BNONE</code>. The only difference is that the <code>BNONE.W18</code> instruction provides for a wider immediate field of 18 bits to encode the branch offset, thus enabling branches over a greater address range.</P>",
        "Operation": "<pre>if (AR[s] and AR[t]) = 0<SUP>32</SUP> then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "BREAK": {
        "name": "BREAK",
        "syn": "Breakpoint",
        "Assembler Syntax": "BREAK 0..15, 0..15",
        "Description": "<P>This instruction simply raises an exception when it is executed and <code>PS.INTLEVEL</code> <code>&lt;</code> <code>DEBUGLEVEL</code>. The high-priority vector for <code>DEBUGLEVEL</code> is used. The <code>DEBUGCAUSE</code> register is written as part of raising the exception to indicate that <code>BREAK</code> raised the debug exception. The address of the <code>BREAK</code> instruction is stored in <code>EPC[DEBUGLEVEL]</code>. The <code>s</code> and <code>t</code> fields of the instruction word are not used by the processor; they are available for use by the software. When <code>PS.INTLEVEL</code> <code>>=</code> <code>DEBUGLEVEL</code>, <code>BREAK</code> is a no-op.</P><P>The <code>BREAK</code> instruction typically calls a debugger when program execution reaches a certain point (a \"breakpoint\"). The instruction at the breakpoint is replaced with the <code>BREAK</code> instruction. To continue execution after a breakpoint is reached, the debugger must re-write the <code>BREAK</code> to the original instruction, single-step by one instruction, and then put back the <code>BREAK </code>instruction again.</P><P>Writing instructions requires special consideration. See the <code>ISYNC</code> instruction for more information.</P><P>When it is not possible to write the instruction memory (for example, for ROM code), the <code>IBREAKA</code> feature provides breakpoint capabilities (see Debug Option).</P><P>Software can also use <code>BREAK</code> to indicate an error condition that requires the programmer's attention. The <code>s</code> and <code>t</code> fields may encode information about the situation.</P><P><code>BREAK</code> is a 24-bit instruction. The <code>BREAK.N</code> density-option instruction performs a similar operation in a 16-bit encoding.</P>",
        "Operation": "<pre>if PS.INTLEVEL &lt; DEBUGLEVEL then\n\tEPC[DEBUGLEVEL] &#8592; PC\n\tEPS[DEBUGLEVEL] &#8592; PS\n\tDEBUGCAUSE &#8592; 001000\n\tnextPC &#8592; InterruptVector[DEBUGLEVEL]\n\tPS.EXCM &#8592; 1\n\tPS.INTLEVEL &#8592; DEBUGLEVEL\nendif</pre>"
    },
    "BREAK.N": {
        "name": "BREAK.N",
        "syn": "Narrow Breakpoint",
        "Assembler Syntax": "BREAK.N 0..15",
        "Description": "<P><code>BREAK.N</code> is similar in operation to <code>BREAK</code> ( BREAK 0..15, 0..15), except that it is encoded in a <BR>16-bit format instead of 24 bits, there is only a 4-bit <code>imm</code> field, and a different bit is set in <code>D</code>EBUGC<code>AUSE</code>. Use this instruction to set breakpoints on 16-bit instructions.</P>",
        "Operation": "<pre>if PS.INTLEVEL &lt; DEBUGLEVEL then\n\tEPC[DEBUGLEVEL] &#8592; PC\n\tEPS[DEBUGLEVEL] &#8592; PS\n\tDEBUGCAUSE &#8592; 010000\n\tnextPC &#8592; InterruptVector[DEBUGLEVEL]\n\tPS.EXCM &#8592; 1\n\tPS.INTLEVEL &#8592; DEBUGLEVEL\nendif</pre>"
    },
    "BT": {
        "name": "BT",
        "syn": "Branch if True",
        "Assembler Syntax": "BT bs, label",
        "Description": "<P><code>BT</code> branches to the target address if Boolean register <code>bs</code> is true.</P><P>The target instruction address of the branch is given by the address of the <code>BT</code> instruction, plus the sign-extended 8-bit <code>imm8</code> field of the instruction plus four. If the Boolean register <code>bs</code> is false, execution continues with the next sequential instruction.</P><P>The inverse of <code>BT</code> is <code>BF</code>.</P>",
        "Operation": "<pre>if BR<SUB>s</SUB> then\n\tnextPC &#8592; PC + (imm8<SUB>7</SUB><SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "CALL0": {
        "name": "CALL0",
        "syn": "Non-windowed Call",
        "Assembler Syntax": "CALL0 label",
        "Description": "<P><code>CALL0</code> calls subroutines without using register windows. The return address is placed in <code>a0</code>, and the processor then branches to the target address. The return address is the address of the <code>CALL0</code> instruction plus three.</P><P>The target instruction address must be 32-bit aligned. This allows <code>CALL0</code> to have a larger effective range (-524284 to 524288 bytes). The target instruction address of the call is given by the address of the <code>CALL0</code> instruction with the least significant two bits set to zero plus the sign-extended 18-bit <code>offset</code> field of the instruction shifted by two, plus four.</P><P>The <code>RET</code> and <code>RET.N</code> instructions are used to return from a subroutine called by <code>CALL0</code>.</P><P>See the <code>CALLX0</code> instruction  CALLX0 as for calling routines where the target address is given by the contents of a register.</P><P>To call using the register window mechanism, see the <code>CALL4</code>, <code>CALL8</code>, and <code>CALL12</code> instructions.</P>",
        "Operation": "<pre>AR[0] &#8592; nextPC\nnextPC &#8592; (PC<SUB>31..2</SUB> + (offset<SUB>17</SUB><SUP>12</SUP>||offset) + 1)||00</pre>"
    },
    "CALL12": {
        "name": "CALL12",
        "syn": "Call PC-relative, Rotate Window by 12",
        "Assembler Syntax": "CALL12 label",
        "Description": "<P><code>CALL12</code> calls subroutines using the register windows mechanism, requesting the callee rotate the window by 12 registers. The <code>CALL12</code> instruction does not rotate the window itself, but instead stores the window increment for later use by the <code>ENTRY</code> instruction. The return address and window increment are placed in the caller's <code>a12</code> (the callee's <code>a0</code>), and the processor then branches to the target address. The return address is the address of the next instruction (the address of the <code>CALL12</code> instruction plus three). The window increment is also stored in the <code>CALLINC</code> field of the <code>PS</code> register, where it is accessed by the <code>ENTRY</code> instruction.</P><P>The target instruction address must be a 32-bit aligned <code>ENTRY</code> instruction. This allows <code>CALL12</code> to have a larger effective range (<code>&#8722;</code>524284 to 524288 bytes). The target instruction address of the call is given by the address of the <code>CALL12</code> instruction with the two least significant bits set to zero, plus the sign-extended 18-bit <code>offset</code> field of the instruction shifted by two, plus four.</P><P>See the <code>CALLX12</code> instruction for calling routines where the target address is given by the contents of a register.</P><P>The <code>RETW</code> and <code>RETW.N</code> instructions return from a subroutine called by <code>CALL12</code>.</P><P>The window increment stored with the return address register in <code>a12</code> occupies the two most significant bits of the register, and therefore those bits must be filled in by the subroutine return. The <code>RETW</code> and <code>RETW.N</code> instructions fill in these bits from the two most significant bits of their own address. This prevents register-window calls from being used to call a routine in a different 1GB region of the address space.</P><P>See the <code>CALL0</code> instruction for calling routines using the non-windowed subroutine protocol.</P><P>The caller's <code>a12</code>..<code>a15</code> are the same registers as the callee's <code>a0</code>..<code>a3</code> after the callee executes the <code>ENTRY</code> instruction. You can use these registers for parameter passing. The caller's <code>a0</code>..<code>a11</code> are hidden by <code>CALL12</code>, and therefore you may use them to keep values that are live across the call.</P>",
        "Operation": "<pre>WindowCheck (00, 00, 11)\nPS.CALLINC &#8592; 11\nAR[1100] &#8592; 11||(nextPC)<SUB>29..0</SUB>\nnextPC &#8592; (PC<SUB>31..2</SUB> + (offset<SUB>17</SUB><SUP>12</SUP>||offset) + 1)||00</pre>"
    },
    "CALL4": {
        "name": "CALL4",
        "syn": "Call PC-relative, Rotate Window by 4",
        "Assembler Syntax": "CALL4 label",
        "Description": "<P><code>CALL4</code> calls subroutines using the register windows mechanism, requesting the callee rotate the window by four registers. The <code>CALL4</code> instruction does not rotate the window itself, but instead stores the window increment for later use by the <code>ENTRY</code> instruction. The return address and window increment are placed in the caller's <code>a4</code> (the callee's <code>a0</code>), and the processor then branches to the target address. The return address is the address of the next instruction (the address of the <code>CALL4</code> instruction plus three). The window increment is also stored in the <code>CALLINC</code> field of the <code>PS</code> register, where it is accessed by the <code>ENTRY</code> instruction.</P><P>The target instruction address must be a 32-bit aligned <code>ENTRY</code> instruction. This allows <code>CALL4</code> to have a larger effective range (<code>&#8722;</code>524284 to 524288 bytes). The target instruction address of the call is given by the address of the <code>CALL4</code> instruction with the two least significant bits set to zero plus the sign-extended 18-bit <code>offset</code> field of the instruction shifted by two, plus four.</P><P>See the <code>CALLX4</code> instruction for calling routines where the target address is given by the contents of a register.</P><P>Use the <code>RETW</code> and <code>RETW.N</code> instructions to return from a subroutine called by <code>CALL4</code>.</P><P>The window increment stored with the return address register in <code>a4</code> occupies the two most significant bits of the register, and therefore those bits must be filled in by the subroutine return. The <code>RETW</code> and <code>RETW.N</code> instructions fill in these bits from the two most significant bits of their own address. This prevents register-window calls from being used to call a routine in a different 1GB region of the address space.</P><P>See the <code>CALL0</code> instruction for calling routines using the non-windowed subroutine protocol.</P><P>The caller's <code>a4..a15</code> are the same registers as the callee's <code>a0..a11</code> after the callee executes the <code>ENTRY</code> instruction. You can use these registers for parameter passing. The caller's <code>a0..a3</code> are hidden by <code>CALL4</code>, and therefore you can use them to keep values that are live across the call.</P>",
        "Operation": "<pre>WindowCheck (00, 00, 01)\nPS.CALLINC &#8592; 01\nAR[0100] &#8592; 01||(nextPC3)<SUB>29..0</SUB>\nnextPC &#8592; (PC<SUB>31..2</SUB> + (offset<SUB>17</SUB><SUP>12</SUP>||offset) + 1)||00</pre>"
    },
    "CALL8": {
        "name": "CALL8",
        "syn": "Call PC-relative, Rotate Window by 8",
        "Assembler Syntax": "CALL8 label",
        "Description": "<P><code>CALL8</code> calls subroutines using the register windows mechanism, requesting the callee rotate the window by eight registers. The <code>CALL8</code> instruction does not rotate the window itself, but instead stores the window increment for later use by the <code>ENTRY</code> instruction. The return address and window increment are placed in the caller's <code>a8</code> (the callee's <code>a0</code>), and the processor then branches to the target address. The return address is the address of the next instruction (the address of the <code>CALL8</code> instruction plus three). The window increment is also stored in the <code>CALLINC</code> field of the <code>PS</code> register, where it is accessed by the <code>ENTRY</code> instruction.</P><P>The target instruction address must be a 32-bit aligned <code>ENTRY</code> instruction. This allows <code>CALL8</code> to have a larger effective range (<code>&#8722;</code>524284 to 524288 bytes). The target instruction address of the call is given by the address of the <code>CALL8</code> instruction with the two least significant bits set to zero, plus the sign-extended 18-bit <code>offset</code> field of the instruction shifted by two, plus four.</P><P>See the <code>CALLX8</code> instruction for calling routines where the target address is given by the contents of a register.</P><P>Use the <code>RETW</code> and <code>RETW.N</code> instructions to return from a subroutine called by <code>CALL8</code>.</P><P>The window increment stored with the return address register in <code>a8</code> occupies the two most significant bits of the register, and therefore those bits must be filled in by the subroutine return. The <code>RETW</code> and <code>RETW.N</code> instructions fill in these bits from the two most significant bits of their own address. This prevents register-window calls from being used to call a routine in a different 1GB region of the address space.</P><P>See the <code>CALL0</code> instruction for calling routines using the non-windowed subroutine protocol.</P><P>The caller's <code>a8</code>..<code>a15</code> are the same registers as the callee's <code>a0</code>..<code>a7</code> after the callee executes the <code>ENTRY</code> instruction. You can use these registers for parameter passing. The caller's <code>a0</code>..<code>a7</code> are hidden by <code>CALL8</code>, and therefore you may use them to keep values that are live across the call.</P>",
        "Operation": "<pre>WindowCheck (00, 00, 10)\nPS.CALLINC &#8592; 10\nAR[1000] &#8592; 10||(nextPC)<SUB>29..0</SUB>\nnextPC &#8592; (PC<SUB>31..2</SUB> + (offset<SUB>17</SUB><SUP>12</SUP>||offset) + 1)||00</pre>"
    },
    "CALLX0": {
        "name": "CALLX0",
        "syn": "Non-windowed Call Register",
        "Assembler Syntax": "CALLX0 as",
        "Description": "<P><code>CALLX0</code> calls subroutines without using register windows. The return address is placed in <code>a0</code>, and the processor then branches to the target address. The return address is the address of the <code>CALLX0</code> instruction, plus three.</P><P>The target instruction address of the call is given by the contents of address register <code>as</code>.</P><P>The <code>RET</code> and <code>RET.N</code> instructions return from a subroutine called by <code>CALLX0</code>.</P><P>To call using the register window mechanism, see the <code>CALLX4</code>, <code>CALLX8</code>, and <code>CALLX12</code> instructions.</P>",
        "Operation": "<pre>tmp &#8592; nextPC\nnextPC &#8592; AR[s]\nAR[0] &#8592; tmp</pre>"
    },
    "CALLX12": {
        "name": "CALLX12",
        "syn": "Call Register, Rotate Window by 12",
        "Assembler Syntax": "CALLX12 as",
        "Description": "<P><code>CALLX12</code> calls subroutines using the register windows mechanism, requesting the callee rotate the window by 12 registers. The <code>CALLX12</code> instruction does not rotate the window itself, but instead stores the window increment for later use by the <code>ENTRY</code> instruction. The return address and window increment are placed in the caller's <code>a12</code> (the callee's <code>a0</code>), and the processor then branches to the target address. The return address is the address of the next instruction (the address of the <code>CALLX12</code> instruction plus three). The window increment is also stored in the <code>CALLINC</code> field of the <code>PS</code> register, where it is accessed by the <code>ENTRY</code> instruction.</P><P>The target instruction address of the call is given by the contents of address register <code>as</code>. The target instruction must be an <code>ENTRY</code> instruction.</P><P>See the <code>CALL12</code> instruction for calling routines where the target address is given by a PC-relative offset in the instruction.</P><P>The <code>RETW</code> and <code>RETW.N</code> instructions return from a subroutine called by <code>CALLX12</code>.</P><P>The window increment stored with the return address register in <code>a12</code> occupies the two most significant bits of the register, and therefore those bits must be filled in by the subroutine return. The <code>RETW</code> and <code>RETW.N</code> instructions fill in these bits from the two most significant bits of their own address. This prevents register-window calls from being used to call a routine in a different 1GB region of the address space. </P><P>See the <code>CALLX0</code> instruction for calling routines using the non-windowed subroutine protocol.</P><P>The caller's <code>a12</code>..<code>a15</code> are the same registers as the callee's <code>a0</code>..<code>a3</code> after the callee executes the <code>ENTRY</code> instruction. These registers may be used for parameter passing. The caller's <code>a0</code>..<code>a11</code> are hidden by <code>CALLX12</code>, and therefore may be used to keep values that are live across the call.</P>",
        "Operation": "<pre>WindowCheck (00, 00, 11)\nPS.CALLINC &#8592; 11\ntmp &#8592; nextPC\nnextPC &#8592; AR[s]\nAR[11||00] &#8592; 11||(tmp)<SUB>29..0</SUB></pre>"
    },
    "CALLX4": {
        "name": "CALLX4",
        "syn": "Call Register, Rotate Window by 4",
        "Assembler Syntax": "CALLX4 as",
        "Description": "<P><code>CALLX4</code> calls subroutines using the register windows mechanism, requesting the callee rotate the window by four registers. The <code>CALLX4</code> instruction does not rotate the window itself, but instead stores the window increment for later use by the <code>ENTRY</code> instruction. The return address and window increment are placed in the caller's <code>a4</code> (the callee's <code>a0</code>), and the processor then branches to the target address. The return address is the address of the next instruction (the address of the <code>CALLX4</code> instruction plus three). The window increment is also stored in the <code>CALLINC</code> field of the <code>PS</code> register, where it is accessed by the <code>ENTRY</code> instruction.</P><P>The target instruction address of the call is given by the contents of address register <code>as</code>. The target instruction must be an <code>ENTRY</code> instruction.</P><P>See the <code>CALL4</code> instruction for calling routines where the target address is given by a PC-relative offset in the instruction.</P><P>The <code>RETW</code> and <code>RETW.N</code> instructions return from a subroutine called by <code>CALLX4</code>.</P><P>The window increment stored with the return address register in <code>a4</code> occupies the two most significant bits of the register, and therefore those bits must be filled in by the subroutine return. The <code>RETW</code> and <code>RETW.N</code> instructions fill in these bits from the two most significant bits of their own address. This prevents register-window calls from being used to call a routine in a different 1GB region of the address space.</P><P>See the <code>CALLX0</code> instruction for calling routines using the non-windowed subroutine protocol.</P><P>The caller's <code>a4</code>..<code>a15</code> are the same registers as the callee's <code>a0</code>..<code>a11</code> after the callee executes the <code>ENTRY</code> instruction. You can use these registers for parameter passing. The caller's <code>a0</code>..<code>a3</code> are hidden by <code>CALLX4</code>, and therefore you may use them to keep values that are live across the call.</P>",
        "Operation": "<pre>WindowCheck (00, 00, 01)\nPS.CALLINC &#8592; 01\ntmp &#8592; nextPC\nnextPC &#8592; AR[s]\nAR[01||00] &#8592; 01||(tmp)<SUB>29..0</SUB></pre>"
    },
    "CALLX8": {
        "name": "CALLX8",
        "syn": "Call Register, Rotate Window by 8",
        "Assembler Syntax": "CALLX8 as",
        "Description": "<P><code>CALLX8</code> calls subroutines using the register windows mechanism, requesting the callee rotate the window by eight registers. The <code>CALLX8</code> instruction does not rotate the window itself, but instead stores the window increment for later use by the <code>ENTRY</code> instruction. The return address and window increment are placed in the caller's <code>a8</code> (the callee's <code>a0</code>), and the processor then branches to the target address. The return address is the address of the next instruction (the address of the <code>CALLX8</code> instruction plus three). The window increment is also stored in the <code>CALLINC</code> field of the <code>PS</code> register, where it is accessed by the <code>ENTRY</code> instruction.</P><P>The target instruction address of the call is given by the contents of address register <code>as</code>. The target instruction must be an <code>ENTRY</code> instruction.</P><P>See the <code>CALL8</code> instruction for calling routines where the target address is given by a PC-relative offset in the instruction.</P><P>The <code>RETW</code> and <code>RETW.N</code>  RETW.N instructions return from a subroutine called by <code>CALLX8</code>.</P><P>The window increment stored with the return address register in <code>a8</code> occupies the two most significant bits of the register, and therefore those bits must be filled in by the subroutine return. The <code>RETW</code> and <code>RETW.N</code> instructions fill in these bits from the two most significant bits of their own address. This prevents register-window calls from being used to call a routine in a different 1GB region of the address space.</P><P>See the <code>CALLX0</code> instruction for calling routines using the non-windowed subroutine protocol.</P><P>The caller's <code>a8</code>..<code>a15</code> are the same registers as the callee's <code>a0</code>..<code>a7</code> after the callee executes the <code>ENTRY</code> instruction. You can use these registers for parameter passing. The caller's <code>a0</code>..<code>a7</code> are hidden by <code>CALLX8</code>, and therefore you may use them to keep values that are live across the call.</P>",
        "Operation": "<pre>WindowCheck (00, 00, 10)\nPS.CALLINC &#8592; 10\ntmp &#8592; nextPC\nnextPC &#8592; AR[s]\nAR[10||00] &#8592; 10||(tmp)<SUB>29..0</SUB></pre>"
    },
    "CHECK_IPQ": {
        "name": "CHECK_IPQ",
        "syn": "Check Status of Input Queue IPQ",
        "Assembler Syntax": "CHECK_IPQ at",
        "Description": "<P><code>CHECK_IPQ</code> sets bit 0 of address register <code>at</code> to 0 or 1 based on the status of the input queue <code>IPQ</code>. If the input queue is not empty, implying that a subsequent read of the queue is guaranteed to get valid data and not stall the processor, the bit is set to 0. If the bit is set to a 1, a subsequent read of the input queue may (or may not) result in a processor stall until valid data is available in the queue. The upper 31 bits of address register <code>at</code> are always cleared to 0 by this instruction.</P>",
        "Operation": "<pre>AR[t] &#8592; (0<SUP>31</SUP> || IPQ status)</pre>"
    },
    "CHECK_OPQ": {
        "name": "CHECK_OPQ",
        "syn": "Check Status of Output Queue OPQ",
        "Assembler Syntax": "CHECK_OPQ at",
        "Description": "<P><code>CHECK_OPQ</code> sets bit 0 of address register <code>at</code> to 0 or 1 based on the status of the output queue <code>OPQ</code>. If the output queue is not full, implying that a subsequent write to the queue is guaranteed to not stall the processor, the bit is set to 0. If the bit is set to a 1, a subsequent write to the output queue may (or may not) result in a processor stall until the external queue is not full. The upper 31 bits of address register <code>at</code> are always cleared to 0 by this instruction.</P>",
        "Operation": "<pre>AR[t] &#8592; (0<SUP>31</SUP> || OPQ status)</pre>"
    },
    "CLAMPS": {
        "name": "CLAMPS",
        "syn": "Signed Clamp",
        "Assembler Syntax": "CLAMPS ar, as, 7..22",
        "Description": "<P><code>CLAMPS</code> tests whether the contents of address register <code>as</code> fits as a signed value of <code>imm</code><code>+</code><code>1</code> bits (in the range 7 to 22). If so, the value is written to address register <code>ar</code>; if not, the largest value of <code>imm</code><code>+</code><code>1</code> bits with the same sign as <code>as</code> is written to <code>ar</code>. Thus <code>CLAMPS</code> performs the function</P><P>y <code>&#8592;</code> min(max(x, <code>&#8722;</code>2<SUP>imm</SUP>), 2<SUP>imm</SUP><code>&#8722;</code>1)</P><P><code>CLAMPS</code> may be used in conjunction with instructions such as <code>ADD</code>, <code>SUB</code>, <code>MUL16S</code>, and so forth to implement saturating arithmetic.</P>",
        "Operation": "<pre>sign &#8592; AR[s]<SUB>31</SUB>\nAR[r] &#8592; if AR[s]<SUB>30..t+7</SUB> = sign24-t\n\t\tthen AR[s]\n\t\telse sign<SUP>25-t</SUP>||(not sign)<SUP>t</SUP>+<SUP>7</SUP></pre>"
    },
    "CLRB_EXPSTATE": {
        "name": "CLRB_EXPSTATE",
        "syn": "Clear Bit of State EXPSTATE",
        "Assembler Syntax": "CLEARB_EXPSTATE bitindex (0..31)",
        "Description": "<P><code>CLRB_EXPSTATE</code> clears a single bit of the exported state <code>EXPSTATE</code> while not changing the value of any other bit of that state. The bit to be cleared is specified by the 5-bit immediate operand <code>bitindex</code>.</P>",
        "Operation": "<pre>EXPSTATE &#8592; EXPSTATE & ~(32'b1 << bitindex)</pre>"
    },
    "CLREX": {
        "name": "CLREX",
        "syn": "Clear Exclusive",
        "Assembler Syntax": "CLREX",
        "Description": "<P><code>CLREX</code> clears the exclusive state set by <code>L32EX</code> so that a succeeding <code>S32EX</code> will fail unless another <code>L32EX</code> is done first. This should be necessary only in certain operating system code, such as a full process context swap.</P>",
        "Operation": "<pre>clrmonitor</pre>"
    },
    "CONST16": {
        "name": "CONST16",
        "syn": "Shift In 16-bit Constant",
        "Assembler Syntax": "CONST16 at, 0..65535",
        "Description": "<P>A pair of <code>CONST16</code> instructions form a load of a 32-bit constant from the instruction stream into an address register. It is typically used to load constant values into a register when the constant cannot be encoded in a <code>MOVI</code> instruction.</P><P><code>CONST16</code> does a logical shift left by 16 of address register <code>at</code> and then inserts a 16-bit immediate in the low 16 bits. The low 32 bits of the result are written to address register <code>at</code>.</P><P>If <code>CONST16</code> operates twice on the same address register, it replaces the original contents of the address register with the concatenation of the 16-bit immediates of the two instructions. The pair, then, inserts a 32-bit immediate into an address register.</P><P>The <code>CONST16</code> instruction requires a large amount of encoding space and is not used in most configurations. It is, therefore, not allocated a permanent encoding. Documentation for the particular configuration gives the encoding. This instruction is a leading candidate for a future variable encoding mechanism.</P>",
        "Operation": "<pre>AR[t] &#8592; AR[t]15..0 || immed16</pre>"
    },
    "DEPBITS": {
        "name": "DEPBITS",
        "syn": "Deposit Bits",
        "Assembler Syntax": "DEPBITS as, at, shiftimm, maskimm",
        "Description": "<P><code>DEPBITS</code> deposits a field into an arbitrary position in a 32-bit address register. Specifically, it shifts the <code>maskimm</code> low bits of address register <code>as</code> left by <code>shiftimm</code> and replaces the corresponding bits of address register <code>at</code>. <code>maskimm</code> can take the values 1 to 16 and is encoded as <code>maskimm-1</code> in bits 15 to 12 of the instruction word. <code>shiftimm</code> can take the values 0 to 31 and is placed in bits 16 and 23 to 20 of the instruction word (the <code>sa</code> fields).</P><P>The operation of this instruction when <code>shiftimm</code> + <code>maskimm</code> <code>&gt;</code> <code>32</code> is undefined and reserved for future use.</P>",
        "Operation": "<pre>mask &#8592; 1<SUP>32-maskimm-shiftimm </SUP>|| 0<SUP>maskimm </SUP>|| 1<SUP>shiftimm</SUP>\nAR[t] &#8592; (mask and AR[t]) or (0<SUP>32-maskimm-shiftimm </SUP>|| AR[s]maskimm-1..0 || 0shiftimm)</pre>"
    },
    "DHI": {
        "name": "DHI",
        "syn": "Data Cache Hit Invalidate",
        "Assembler Syntax": "DHI as, 0..1020",
        "Description": "<P><code>DHI</code> invalidates the specified line in the level-1 data cache, if it is present. If the specified address is not in the data cache, then this instruction has no effect. If the specified address is present, it is invalidated even if it contains dirty data. If the specified line has been locked by a <code>DPFL</code> instruction, then no invalidation is done and no exception is raised because of the lock. The line remains in the cache and must be unlocked by a <code>DHU</code> or <code>DIU</code> instruction before it can be invalidated. This instruction is useful before a DMA write to memory that overwrites the entire line.</P><P><code>DHI</code> forms a virtual address by adding the contents of address register <code>as</code> and an 8-bit zero-extended constant value encoded in the instruction word shifted left by two. Therefore, the offset can specify multiples of four from zero to 1020. If the Region Translation Option  or the MMU Option  is enabled, the virtual address is translated to the physical address. If not, the physical address is identical to the virtual address. If the translation encounters an error (for example, protection violation), the processor raises an exception (see ). Protection is tested as if the instruction were storing to the virtual address.</P><P>Because the organization of caches is implementation-specific, the operation below specifies only a call to the implementation's <code>dhitinval</code> function.</P><P><code>DHI</code> is a privileged instruction.</P>",
        "Operation": "<pre>if CRING != 0 then\n\tException (PrivilegedCause)\nelse\n\tvAddr &#8592; AR[s] +<SUB> </SUB>(0<SUP>22</SUP>||imm8||0<SUP>2</SUP>)\n\t(pAddr, attributes, cause) &#8592; ltranslate(vAddr, CRING)\n\tif invalid(attributes) then\n\t\tEXCVADDR &#8592; vAddr\n\t\tException (cause)\n\telse\n\t\tdhitinval(vAddr, pAddr)\n\tendif\nendif</pre>"
    },
    "DHI.B": {
        "name": "DHI.B",
        "syn": "Block Data Cache Hit Invalidate",
        "Assembler Syntax": "DHI.B as, at",
        "Description": "<P><code>DHI.B</code> operates on a block of bytes in the data cache which begins at the virtual address contained in address register <code>as</code>. The block is contiguous in virtual address space and its length is indicated by address register <code>at</code>. Execution breaks up the block into zero, one or two partial cache lines at the beginning and/or end of the block and some number of full cache lines between. It does a <code>DHWBI</code> operation on the partial cache lines at the beginning and/or end and a <code>DHI</code> operation on each full cache line between.</P><P>To maintain locally immediate functionality, if the processor does a subsequent load, store or software prefetch instruction to a memory location which is within the block but has not yet been operated on, the subsequent instruction waits until after the block operation has been completed on its location.</P>"
    },
    "DHU": {
        "name": "DHU",
        "syn": "Data Cache Hit Unlock",
        "Assembler Syntax": "DHU as, 0..240",
        "Description": "<P><code>DHU</code> performs a data cache unlock if hit. The purpose of <code>DHU</code> is to remove the lock created by a DPFL instruction. Xtensa ISA implementations that do not implement cache locking must raise an illegal instruction exception when this opcode is executed.</P><P><code>DHU</code> checks whether the line containing the specified address is present in the data cache, and if so, it clears the lock associated with that line. To unlock by index without knowing the address of the locked line, use the DIU instruction.</P><P><code>DHU</code> forms a virtual address by adding the contents of address register <code>as</code> and a 4-bit zero-extended constant value encoded in the instruction word shifted left by four. Therefore, the offset can specify multiples of 16 from zero to 240. If the Region Translation Option  or the MMU Option  is enabled, the virtual address is translated to the physical address. If not, the physical address is identical to the virtual address. If the translation encounters an error (for example, protection violation), the processor raises an exception  as if it were loading from the virtual address.</P><P><code>DHU</code> is a privileged instruction.</P>",
        "Operation": "<pre>if CRING != 0 then\n\tException (PrivilegedCause)\nelse\n\tvAddr &#8592; AR[s] + (0<SUP>24</SUP>||imm4||0<SUP>4</SUP>)\n\t(pAddr, attributes, cause) &#8592; ltranslate(vAddr, CRING)\n\tif invalid(attributes) then\n\t\tEXCVADDR &#8592; vAddr\n\t\tException (cause)\n\telse\n\t\tdhitunlock(vAddr, pAddr)\n\tendif\nendif</pre>"
    },
    "DHWB": {
        "name": "DHWB",
        "syn": "Data Cache Hit Writeback",
        "Assembler Syntax": "DHWB as, 0..1020",
        "Description": "<P>This instruction forces dirty data in the data cache to be written back to memory. If the specified address is not in the data cache or is present but unmodified, then this instruction has no effect. If the specified address is present and modified in the data cache, the line containing it is written back, and marked unmodified. This instruction is useful before a DMA read from memory, to force writes to a frame buffer to become visible, or to force writes to memory shared by two processors.</P><P><code>DHWB</code> forms a virtual address by adding the contents of address register <code>as</code> and an 8-bit zero-extended constant value encoded in the instruction word shifted left by two. Therefore, the offset can specify multiples of four from zero to 1020. If the Region Translation Option  or the MMU Option  is enabled, the virtual address is translated to the physical address. If not, the physical address is identical to the virtual address. If the translation encounters an error (for example, protection violation), the processor raises an exception  as if it were loading from the virtual address.</P><P>Because the organization of caches is implementation-specific, the operation below specifies only a call to the implementation's <code>dhitwriteback</code> function.</P>",
        "Operation": "<pre>vAddr &#8592; AR[s] + (0<SUP>22</SUP>||imm8||0<SUP>2</SUP>)\n(pAddr, attributes, cause) &#8592; ltranslate(vAddr, CRING)\nif invalid(attributes) then\n\tEXCVADDR &#8592; vAddr\n\tException (cause)\nelse\n\tdhitwriteback(vAddr, pAddr)\nendif</pre>"
    },
    "DHWB.B": {
        "name": "DHWB.B",
        "syn": "Block Data Cache Hit Writeback",
        "Assembler Syntax": "DHWB.B as, at",
        "Description": "<P><code>DHWB.B</code> operates on a block of bytes in the data cache which begins at the virtual address contained in address register <code>as</code>. The block is contiguous in virtual address space and its length is indicated by address register <code>at</code>. Execution breaks up the block into zero, one or two partial cache lines at the beginning and/or end of the block and some number of full cache lines between. It does a <code>DHWB</code> operation on the partial cache lines and on each full cache line between.</P><P>To maintain locally immediate functionality, if the processor does a subsequent store to a memory location which is within the block but has not yet been operated on, the store waits until after the operation has been completed on its location.</P>"
    },
    "DHWBI": {
        "name": "DHWBI",
        "syn": "Data Cache Hit Writeback Invalidate",
        "Assembler Syntax": "DHWBI as, 0..1020",
        "Description": "<P><code>DHWBI</code> forces dirty data in the data cache to be written back to memory. If the specified address is not in the data cache, then this instruction has no effect. If the specified address is present and modified in the data cache, the line containing it is written back. After the write-back, if any, the line containing the specified address is invalidated if present. If the specified line has been locked by a <code>DPFL</code> instruction, then no invalidation is done and no exception is raised because of the lock. The line is written back but remains in the cache unmodified and must be unlocked by a <code>DHU</code> or <code>DIU</code> instruction before it can be invalidated. This instruction is useful in the same circumstances as <code>DHWB</code> and before a DMA write to memory or write from another processor to memory. If the line is certain to be completely overwritten by the write, you can use a <code>DHI</code> (as it is faster), but otherwise use a <code>DHWBI</code>.</P><P><code>DHWBI</code> forms a virtual address by adding the contents of address register <code>as</code> and an <BR>8-bit zero-extended constant value encoded in the instruction word shifted left by two. Therefore, the offset can specify multiples of four from zero to 1020. If the Region Translation Option  or the MMU Option  is enabled, the virtual address is translated to the physical address. If not, the physical address is identical to the virtual address. If the translation encounters an error (for example, protection violation), the processor raises an exception  as if it were loading from the virtual address.</P><P>Because the organization of caches is implementation-specific, the operation section below specifies only a call to the implementation's <code>dhitwritebackinval</code> function.</P>",
        "Operation": "<pre>vAddr &#8592; AR[s] + (0<SUP>22</SUP>||imm8||0<SUP>2</SUP>)\n(pAddr, attributes, cause) &#8592; ltranslate(vAddr, CRING)\nif invalid(attributes) then\n\tEXCVADDR &#8592; vAddr\n\tException (cause)\nelse\n\tdhitwritebackinval(vAddr, pAddr)\nendif</pre>"
    },
    "DHWBI.B": {
        "name": "DHWBI.B",
        "syn": "Block Data Cache Hit Writeback Inv.",
        "Assembler Syntax": "DHWBI.B as, at",
        "Description": "<P><code>DHWBI.B</code> operates on a block of bytes in the data cache which begins at the virtual address contained in address register <code>as</code>. The block is contiguous in virtual address space and its length is indicated by address register <code>at</code>. Execution breaks up the block into zero, one or two partial cache lines at the beginning and/or end of the block and some number of full cache lines between. It does a <code>DHWBI</code> operation on the partial cache lines and on each full cache line between.</P><P>To maintain locally immediate functionality, if the processor does a subsequent load, store or software prefetch instruction to a memory location which is within the block but has not yet been operated on, the subsequent instruction waits until after the block operation has been completed on its location.</P>"
    },
    "DII": {
        "name": "DII",
        "syn": "Data Cache Index Invalidate",
        "Assembler Syntax": "DII as, 0..1020",
        "Description": "<P><code>DII</code> uses the virtual address to choose a location in the data cache and invalidates the specified line. If the chosen line has been locked by a <code>DPFL</code> instruction, then no invalidation is done and no exception is raised because of the lock. The line remains in the cache and must be unlocked by a <code>DHU</code> or <code>DIU</code> instruction before it can be invalidated.The method for mapping the virtual address to a data cache location is implementation-specific. This instruction is primarily useful for data cache initialization after power-up.</P><P><code>DII</code> forms a virtual address by adding the contents of address register <code>as</code> and an 8-bit zero-extended constant value encoded in the instruction word shifted left by two. Therefore, the offset can specify multiples of four from zero to 1020. The virtual address chooses a cache line without translation and without raising the associated exceptions.</P><P>Because the organization of caches is implementation-specific, the operation section below specifies only a call to the implementation's <code>dindexinval</code> function.</P><P><code>DII</code> is a privileged instruction.</P>",
        "Operation": "<pre>if CRING != 0 then\n\tException (PrivilegedCause)\nelse\n\tvAddr &#8592; AR[s] + (0<SUP>22</SUP>||imm8||0<SUP>2</SUP>)\n\tdindexinval(vAddr)\nendif</pre>"
    },
    "DIU": {
        "name": "DIU",
        "syn": "Data Cache Index Unlock",
        "Assembler Syntax": "DIU as, 0..240",
        "Description": "<P><code>DIU </code>uses the virtual address to choose a location in the data cache and unlocks the chosen line. The purpose of DIU is to remove the lock created by a DPFL instruction. The method for mapping the virtual address to a data cache location is implementation-specific. This instruction is primarily useful for unlocking the entire data cache. Xtensa ISA implementations that do not implement cache locking must raise an illegal instruction exception when this opcode is executed.</P><P>To unlock a specific cache line if it is in the cache, use the DHU instruction.</P><P><code>DIU</code> forms a virtual address by adding the contents of address register <code>as</code> and a 4-bit zero-extended constant value encoded in the instruction word shifted left by four. Therefore, the offset can specify multiples of 16 from zero to 240. The virtual address chooses a cache line without translation and without raising the associated exceptions.</P><P>Because the organization of caches is implementation-specific, the operation section below specifies only a call to the implementation's <code>dindexunlock</code> function.</P><P>DIU is a privileged instruction.</P>",
        "Operation": "<pre>if CRING != 0 then\n\tException (PrivilegedCause)\nelse\n\tvAddr &#8592; AR[s] + (0<SUP>24</SUP>||imm4||0<SUP>4</SUP>)\n\tdindexunlock(vAddr)\nendif</pre>"
    },
    "DIWB": {
        "name": "DIWB",
        "syn": "Data Cache Index Write Back",
        "Assembler Syntax": "DIWB as, 0..240",
        "Description": "<P><code>DIWB</code> uses the virtual address to choose a line in the data cache and writes that line back to memory if it is dirty. The method for mapping the virtual address to a data cache line is implementation-specific. This instruction is primarily useful for forcing all dirty data in the cache back to memory. If the chosen line is present but unmodified, then this instruction has no effect. If the chosen line is present and modified in the data cache, it is written back, and marked unmodified. For set-associative caches, only one line out of one way of the cache is written back. Some Xtensa ISA implementations do not support writeback caches. For these implementations <code>DIWB</code> does nothing.</P><P>This instruction is useful for the same purposes as <code>DHWB</code>, but when either the address is not known or when the range of addresses is large enough that it is faster to operate on the entire cache.</P><P><code>DIWB</code> forms a virtual address by adding the contents of address register <code>as</code> and a 4-bit zero-extended constant value encoded in the instruction word shifted left by four. Therefore, the offset can specify multiples of 16 from zero to 240. The virtual address chooses a cache line without translation and without raising the associated exceptions.</P><P>Because the organization of caches is implementation-specific, the operation section below specifies only a call to the implementation's <code>dindexwriteback</code> function.</P><P><code>DIWB</code> is a privileged instruction.</P>",
        "Operation": "<pre>if CRING != 0 then\n\tException (PrivilegedCause)\nelse\n\tvAddr &#8592; AR[s] + (0<SUP>24</SUP>||imm4||0<SUP>4</SUP>)\n\tdindexwriteback(vAddr)\nendif</pre>"
    },
    "DIWBI": {
        "name": "DIWBI",
        "syn": "Data Cache Index Write Back Invalidate",
        "Assembler Syntax": "DIWBI as, 0..240",
        "Description": "<P><code>DIWBI</code> uses the virtual address to choose a line in the data cache and forces that line to be written back to memory if it is dirty. After the writeback, if any, the line is invalidated. The method for mapping the virtual address to a data cache location is implementation-specific. If the chosen line is already invalid, then this instruction has no effect. If the chosen line has been locked by a <code>DPFL</code> instruction, then dirty data is written back but no invalidation is done and no exception is raised because of the lock. The line remains in the cache and must be unlocked by a <code>DHU</code> or <code>DIU</code> instruction before it can be invalidated. For set-associative caches, only one line out of one way of the cache is written back and invalidated. Some Xtensa ISA implementations do not support write-back caches. For these implementations <code>DIWBI</code> is similar to <code>DII</code> but invalidates only one line.</P><P>This instruction is useful for the same purposes as the <code>DHWBI</code> but when either the address is not known, or when the range of addresses is large enough that it is faster to operate on the entire cache.</P><P><code>DIWBI</code> forms a virtual address by adding the contents of address register <code>as</code> and a 4-bit zero-extended constant value encoded in the instruction word shifted left by four. Therefore, the offset can specify multiples of 16 from zero to 240. The virtual address chooses a cache line without translation and without raising the associated exceptions.</P><P>Because the organization of caches is implementation-specific, the operation section below specifies only a call to the implementation's <code>dindexwritebackinval</code> function.</P><P><code>DIWBI</code> is a privileged instruction.</P>",
        "Operation": "<pre>if CRING != 0 then\n\tException (PrivilegedCause)\nelse\n\tvAddr &#8592; AR[s] + (0<SUP>24</SUP>||imm4||0<SUP>4</SUP>)\n\tdindexwritebackinval(vAddr)\nendif</pre>"
    },
    "DIWBUI.P": {
        "name": "DIWBUI.P",
        "syn": "Data Cache Empty",
        "Assembler Syntax": "DIWBUI.P as",
        "Description": "<P><code>DIWBUI.P</code> uses the virtual address to choose a line in the data cache, unlocks that line, forces that line to be written back to memory if it is dirty, invalidates the line, and increments the address register <code>as</code> by the size of a data cache line. The method for mapping the virtual address to a data cache location is implementation-specific. For set-associative caches, only one line out of one way of the cache is written back and invalidated. Some Xtensa ISA implementations do not support write-back caches.</P><P>This instruction is useful for the fastest clearing of the data cache, including locked lines, without destruction of data. It may be used before shutting down all or part of the cache.</P><P><code>DIWBUI.P</code> forms a virtual address simply by using the contents of address register <code>as</code>. The virtual address chooses a cache line without translation and without raising the associated exceptions.</P><P>Because the organization of caches is implementation-specific, the operation section below specifies only a call to the implementation's <code>dindexwritebackinval</code> function.</P><P><code>DIWBUI.P</code> is a privileged instruction.</P>",
        "Operation": "<pre>if CRING != 0 then\n\tException (PrivilegedCause)\nelse\n\tvAddr &#8592; AR[s]\n\tdindexunlockwritebackinval(vAddr)\n\tAR[s] &#8592; AR[s] + DataCacheLineBytes\nendif</pre>"
    },
    "DPFL": {
        "name": "DPFL",
        "syn": "Data Cache Prefetch and Lock",
        "Assembler Syntax": "DPFL as, 0..240",
        "Description": "<P><code>DPFL</code> performs a data cache prefetch and lock. The purpose of <code>DPFL</code> is to improve performance, and not to affect state defined by the ISA. Xtensa ISA implementations that do not implement cache locking must raise an illegal instruction exception when this opcode is executed. In general, the performance improvement from using this instruction is implementation-dependent.</P><P><code>DPFL</code> checks if the line containing the specified address is present in the data cache, and if not, it begins the transfer of the line from memory to the cache. The line is placed in the data cache and the line marked as locked, that is not replaceable by ordinary data cache misses. To unlock the line, use <code>DHU</code> or <code>DIU</code>. To prefetch without locking, use the <code>DPFR</code>, <code>DPFW</code>, <code>DPFRO</code>, or <code>DPFWO</code> instructions.</P><P><code>DPFL</code> forms a virtual address by adding the contents of address register <code>as</code> and a 4-bit zero-extended constant value encoded in the instruction word shifted left by four. Therefore, the offset can specify multiples of 16 from zero to 240. If the Region Translation Option  or the MMU Option  is enabled, the virtual address is translated to the physical address. If not, the physical address is identical to the virtual address. The translation is done as if the address were for a load.</P><P>Because the organization of caches is implementation-specific, the operation section below specifies only a call to the implementation's <code>dprefetch</code> function.</P><P><code>DPFL</code> is a privileged instruction.</P>",
        "Operation": "<pre>if CRING != 0 then\n\tException (PrivilegedCause)\nelse\n\tvAddr &#8592; AR[s] + (0<SUP>24</SUP>||imm4||0<SUP>4</SUP>)\n\t(pAddr, attributes, cause) &#8592; ltranslate(vAddr, CRING)\n\tif invalid(attributes) then\n\t\tEXCVADDR &#8592; vAddr\n\t\tException (cause)\n\telse\n\t\tdprefetch(vAddr, pAddr, 0, 0, 1)\n\tendif\nendif</pre>"
    },
    "DPFM.B": {
        "name": "DPFM.B",
        "syn": "Block Data Cache Prefetch and Modify",
        "Assembler Syntax": "DPFM.B as, at",
        "Description": "<P><code>DPFM.B</code> operates on a block of bytes in the data cache which begins at the virtual address contained in address register <code>as</code>. The block is contiguous in virtual address space and its length is indicated by address register <code>at</code>. Execution breaks up the block into zero, one or two partial cache lines at the beginning and/or end of the block and some number of full cache lines between. It does a <code>DDPFW</code> operation on the partial cache lines at the beginning and/or end and for every full cache line between it allocates a line and sets the data in the line to an arbitrary value without necessarily reading the current value of the line. The purpose is to reduce the bandwidth that would otherwise have been wasted reading the current value of the line. Coherency is maintained in coherent systems.</P><P>To maintain locally immediate functionality, if the processor does a subsequent load or store instruction to a memory location which is within the block but has not yet been prefetched or allocated, the subsequent instruction waits until after the block operation has been completed on its location. Similarly, if the processor does a subsequent cache operation which would invalidate a memory location which is within the block but has not yet been prefetched or allocated, the subsequent instruction waits until after the block operation has been completed on its location.</P>"
    },
    "DPFM.BF": {
        "name": "DPFM.BF",
        "syn": "Block Data Cache Prefetch/Modify First",
        "Assembler Syntax": "DPFM.BF as, at",
        "Description": "<P><code>DPFM.BF</code> operates on a block of bytes in the data cache which begins at the virtual address contained in address register <code>as</code>. The block is contiguous in virtual address space and its length is indicated by address register <code>at</code>. Execution breaks up the block into zero, one or two partial cache lines at the beginning and/or end of the block and some number of full cache lines between. It does a <code>DDPFW</code> operation on the partial cache lines at the beginning and/or end and for every full cache line between it allocates a line and sets the data in the line to an arbitrary value without necessarily reading the current value of the line. The purpose is to reduce the bandwidth that would otherwise have been wasted reading the current value of the line. Coherency is maintained in coherent systems.</P><P>In addition to its operation, <code>DPFM.BF</code> affects the execution of multiple block operations. Instead of interleaving its operation with previous block operations, it waits until all previous block operations have completed. It also causes all following block operations to wait in the same way.</P><P>To maintain locally immediate functionality, if the processor does a subsequent load or store instruction to a memory location which is within the block but has not yet been prefetched or allocated, the subsequent instruction waits until after the block operation has been completed on its location. Similarly, if the processor does a subsequent cache operation which would invalidate a memory location which is within the block but has not yet been prefetched or allocated, the subsequent instruction waits until after the block operation has been completed on its location.</P>"
    },
    "DPFR": {
        "name": "DPFR",
        "syn": "Data Cache Prefetch for Read",
        "Assembler Syntax": "DPFR as, 0..1020",
        "Description": "<P><code>DPFR</code> performs a data cache prefetch for read. The purpose of <code>DPFR</code> is to improve performance, but not to affect state defined by the ISA. Therefore, some Xtensa ISA implementations may choose to implement this instruction as a simple \"no-operation\" instruction. In general, the performance improvement from using this instruction is implementation-dependent. Refer to a specific <code>Xtensa Microprocessor Data Book</code> for more details.</P><P>In some Xtensa ISA implementations, <code>DPFR</code> checks whether the line containing the specified address is present in the data cache, and if not, it begins the transfer of the line from memory. The four data prefetch instructions provide different \"hints\" about how the data is likely to be used in the future. <code>DPFR</code> indicates that the data is only likely to be read, possibly more than once, before it is replaced by another line in the cache.</P><P><code>DPFR</code> forms a virtual address by adding the contents of address register <code>as</code> and an 8-bit zero-extended constant value encoded in the instruction word shifted left by two. Therefore, the offset can specify multiples of four from zero to 1020. If the Region Translation Option  or the MMU Option  is enabled, the virtual address is translated to the physical address. If not, the physical address is identical to the virtual address. No exception is raised on either translation or memory reference. Instead of raising an exception, the prefetch is dropped and the instruction becomes a nop.</P><P>Because the organization of caches is implementation-specific, the operation section below specifies only a call to the implementation's <code>dprefetch</code> function.</P>",
        "Operation": "<pre>vAddr &#8592; AR[s] + (0<SUP>22</SUP>||imm8||0<SUP>2</SUP>)\n(pAddr, attributes, cause) &#8592; ltranslate(vAddr, CRING)\nif not invalid(attributes) then\n\tdprefetch(vAddr, pAddr, 0, 0, 0)\nendif</pre>"
    },
    "DPFR.B": {
        "name": "DPFR.B",
        "syn": "Block Data Cache Prefetch for Read",
        "Assembler Syntax": "DPFR.B as, at",
        "Description": "<P><code>DPFR.B</code> operates on a block of bytes in the data cache which begins at the virtual address contained in address register <code>as</code>. The block is contiguous in virtual address space and its length is indicated by address register <code>at</code>. Execution breaks up the block into zero, one or two partial cache lines at the beginning and/or end of the block and some number of full cache lines between. It does a <code>DPFR</code> operation on the partial cache lines and on each full cache line between.</P><P>To maintain locally immediate functionality, if the processor does a subsequent cache operation which would invalidate a memory location which is within the block but has not yet been prefetched, the subsequent instruction waits until after the block operation has been completed on its location.</P>"
    },
    "DPFR.BF": {
        "name": "DPFR.BF",
        "syn": "Block Data Cache Prefetch for Read First",
        "Assembler Syntax": "DPFR.BF as, at",
        "Description": "<P><code>DPFR.BF</code> operates on a block of bytes in the data cache which begins at the virtual address contained in address register <code>as</code>. The block is contiguous in virtual address space and its length is indicated by address register <code>at</code>. Execution breaks up the block into zero, one or two partial cache lines at the beginning and/or end of the block and some number of full cache lines between. It does a <code>DPFR</code> operation on the partial cache lines and on each full cache line between.</P><P>In addition to its operation, <code>DPFR.BF</code> affects the execution of multiple block operations. Instead of interleaving its prefetches with previous block operations, it waits until all previous block operations have completed. It also causes all following block operations to wait in the same way.</P><P>To maintain locally immediate functionality, if the processor does a subsequent cache operation which would invalidate a memory location which is within the block but has not yet been prefetched, the subsequent instruction waits until after the block operation has been completed on its location.</P>"
    },
    "DPFRO": {
        "name": "DPFRO",
        "syn": "Data Cache Prefetch for Read Once",
        "Assembler Syntax": "DPFRO as, 0..1020",
        "Description": "<P><code>DPFRO</code> performs a data cache prefetch for read once. The purpose of <code>DPFRO</code> is to improve performance, but not to affect state defined by the ISA. Therefore, some Xtensa ISA implementations may choose to implement this instruction as a simple \"no-operation\" instruction. In general, the performance improvement from using this instruction is implementation-dependent. Refer to a specific <code>Xtensa Microprocessor Data Book</code> for more details.</P><P>In some Xtensa ISA implementations, <code>DPFRO</code> checks whether the line containing the specified address is present in the data cache, and if not, it begins the transfer of the line from memory. Four data prefetch instructions provide different \"hints\" about how the data is likely to be used in the future. <code>DPFRO</code> indicates that the data is only likely to be read once before it is replaced by another line in the cache. In some implementations, this hint might be used to select a specific cache way or to select a streaming buffer instead of the cache.</P><P><code>DPFRO</code> forms a virtual address by adding the contents of address register <code>as</code> and an 8-bit zero-extended constant value encoded in the instruction word shifted left by two. Therefore, the offset can specify multiples of four from zero to 1020. If the Region Translation Option  or the MMU Option  is enabled, the virtual address is translated to the physical address. If not, the physical address is identical to the virtual address. No exception is raised on either translation or memory reference. Instead of raising an exception, the prefetch is dropped and the instruction becomes a nop.</P><P>Because the organization of caches is implementation-specific, the operation section below specifies only a call to the implementation's <code>dprefetch</code> function.</P>",
        "Operation": "<pre>vAddr &#8592; AR[s] + (0<SUP>22</SUP>||imm8||0<SUP>2</SUP>)\n(pAddr, attributes, cause) &#8592; ltranslate(vAddr, CRING)\nif not invalid(attributes) then\n\tdprefetch(vAddr, pAddr, 0, 1, 0)\nendif</pre>"
    },
    "DPFW": {
        "name": "DPFW",
        "syn": "Data Cache Prefetch for Write",
        "Assembler Syntax": "DPFW as, 0..1020",
        "Description": "<P><code>DPFW</code> performs a data cache prefetch for write. The purpose of <code>DPFW</code> is to improve performance, but not to affect the ISA state. Therefore, some Xtensa ISA implementations may choose to implement this instruction as a simple \"no-operation\" instruction. In general, the performance improvement from using this instruction is implementation-dependent. Refer to a specific <code>Xtensa Microprocessor Data Book</code> for more details.</P><P>In some Xtensa ISA implementations, <code>DPFW</code> checks whether the line containing the specified address is present in the data cache, and if not, begins the transfer of the line from memory. Four data prefetch instructions provide different \"hints\" about how the data is likely to be used in the future. <code>DPFW</code> indicates that the data is likely to be written before it is replaced by another line in the cache. In some implementations, this fetches the data with write permission (for example, in a system with shared and exclusive states).</P><P><code>DPFW</code> forms a virtual address by adding the contents of address register <code>as</code> and an 8-bit zero-extended constant value encoded in the instruction word shifted left by two. Therefore, the offset can specify multiples of four from zero to 1020. If the Region Translation Option  or the MMU Option  is enabled, the virtual address is translated to the physical address. If not, the physical address is identical to the virtual address. No exception is raised on either translation or memory reference. Instead of raising an exception, the prefetch is dropped and the instruction becomes a nop.</P><P>Because the organization of caches is implementation-specific, the operation section below specifies only a call to the implementation's <code>dprefetch</code> function.</P>",
        "Operation": "<pre>vAddr &#8592; AR[s] + (0<SUP>22</SUP>||imm8||0<SUP>2</SUP>)\n(pAddr, attributes, cause) &#8592; ltranslate(vAddr, CRING)\nif not invalid(attributes) then\n\tdprefetch(vAddr, pAddr, 1, 0, 0)\nendif</pre>"
    },
    "DPFW.B": {
        "name": "DPFW.B",
        "syn": "Block Data Cache Prefetch for Write",
        "Assembler Syntax": "DPFW.B as, at",
        "Description": "<P><code>DPFW.B</code> operates on a block of bytes in the data cache which begins at the virtual address contained in address register <code>as</code>. The block is contiguous in virtual address space and its length is indicated by address register <code>at</code>. Execution breaks up the block into zero, one or two partial cache lines at the beginning and/or end of the block and some number of full cache lines between. It does a <code>DPFW</code> operation on the partial cache lines and on each full cache line between.</P><P>To maintain locally immediate functionality, if the processor does a subsequent cache operation which would invalidate a memory location which is within the block but has not yet been prefetched, the subsequent instruction waits until after the block operation has been completed on its location.</P>"
    },
    "DPFW.BF": {
        "name": "DPFW.BF",
        "syn": "Block Data Cache Prefetch for Write First",
        "Assembler Syntax": "DPFW.BF as, at",
        "Description": "<P><code>DPFW.BF</code> operates on a block of bytes in the data cache which begins at the virtual address contained in address register <code>as</code>. The block is contiguous in virtual address space and its length is indicated by address register <code>at</code>. Execution breaks up the block into zero, one or two partial cache lines at the beginning and/or end of the block and some number of full cache lines between. It does a <code>DPFW</code> operation on the partial cache lines and on each full cache line between.</P><P>In addition to its operation, <code>DPFW.BF</code> affects the execution of multiple block operations. Instead of interleaving its prefetches with previous block operations, it waits until all previous block operations have completed. It also causes all following block operations to wait in the same way.</P><P>To maintain locally immediate functionality, if the processor does a subsequent cache operation which would invalidate a memory location which is within the block but has not yet been prefetched, the subsequent instruction waits until after the block operation has been completed on its location.</P>"
    },
    "DPFWO": {
        "name": "DPFWO",
        "syn": "Data Cache Prefetch for Write Once",
        "Assembler Syntax": "DPFWO as, 0..1020",
        "Description": "<P><code>DPFWO</code> performs a data cache prefetch for write once. The purpose of <code>DPFWO</code> is to improve performance, but not to affect the ISA state. Therefore, some Xtensa ISA implementations may choose to implement this instruction as a simple \"no-operation\" instruction. In general, the performance improvement from using this instruction is implementation-dependent. Refer to a specific <code>Xtensa Microprocessor Data Book</code> for more details.</P><P>In some Xtensa ISA implementations, <code>DPFWO</code> checks whether the line containing the specified address is present in the data cache, and if not, begins the transfer of the line from memory. Four data prefetch instructions provide different \"hints\" about how the data is likely to be used in the future. <code>DPFWO</code> indicates that the data is likely to be read and written once before it is replaced by another line in the cache. In some implementations, this write hint fetches the data with write permission (for example, in a system with shared and exclusive states). The write-once hint might be used to select a specific cache way or to select a streaming buffer instead of the cache.</P><P><code>DPFWO</code> forms a virtual address by adding the contents of address register <code>as</code> and an <BR>8-bit zero-extended constant value encoded in the instruction word shifted left by two. Therefore, the offset can specify multiples of four from zero to 1020. If the Region Translation Option  or the MMU Option  is enabled, the virtual address is translated to the physical address. If not, the physical address is identical to the virtual address. No exception is raised on either translation or memory reference. Instead of raising an exception, the prefetch is dropped and the instruction becomes a nop.</P><P>Because the organization of caches is implementation-specific, the operation section below specifies only a call to the implementation's <code>dprefetch</code> function.</P>",
        "Operation": "<pre>vAddr &#8592; AR[s] +<SUB> </SUB>(0<SUP>22</SUP>||imm8||0<SUP>2</SUP>)\n(pAddr, attributes, cause) &#8592; ltranslate(vAddr, CRING)\nif not invalid(attributes) then\n\tdprefetch(vAddr, pAddr, 1, 1, 0)\nendif</pre>"
    },
    "DSYNC": {
        "name": "DSYNC",
        "syn": "Load/Store Synchronize",
        "Assembler Syntax": "DSYNC",
        "Description": "<P><code>DSYNC</code> waits for all previously fetched <code>WSR.*</code><code>, </code><code>XSR.*</code>, <code>WDTLB</code>, and <code>IDTLB</code> instructions to be performed before interpreting the virtual address of the next load or store instruction. This operation is also performed as part of <code>ISYNC</code>, <code>RSYNC</code>, and <code>ESYNC</code>.</P><P>This instruction is appropriate after <code>WSR.DBREAKC*</code><code> and </code><code>WSR.DBREAKA* </code>instructions. See the Special Register Tables in <code> Special Registers</code> and <code> TLB Entries</code> for a complete description of the uses of the <code>DSYNC</code> instruction.</P><P>Because the instruction execution pipeline is implementation-specific, the operation section below specifies only a call to the implementation's <code>dsync</code> function.</P>",
        "Operation": "<pre>dsync</pre>"
    },
    "ENTRY": {
        "name": "ENTRY",
        "syn": "Subroutine Entry",
        "Assembler Syntax": "ENTRY as, 0..32760",
        "Description": "<P><code>ENTRY</code> is intended to be the first instruction of all subroutines called with <code>CALL4</code>, <code>CALL8</code>, <code>CALL12</code>, <code>CALLX4</code>, <code>CALLX8</code>, or <code>CALLX12</code>. This instruction is not intended to be used by a routine called by <code>CALL0</code> or <code>CALLX0</code>.</P><P><code>ENTRY</code> serves two purposes:</P><OL><LI>First, it increments the register window pointer (<code>WindowBase</code>) by the amount requested by the caller (as recorded in the <code>PS.CALLINC</code> field). </LI><LI>Second, it copies the stack pointer from caller to callee and allocates the callee's stack frame. The <code>as</code> operand specifies the stack pointer register; it must specify one of <code>a0..a3</code> or the operation of <code>ENTRY</code> is undefined. It is read before the window is moved, the stack frame size is subtracted, and then the <code>as</code> register in the moved window is written.</LI></OL><P>The stack frame size is specified as the 12-bit unsigned <code>imm12</code> field in units of eight bytes. The size is zero-extended, shifted left by 3, and subtracted from the caller's stack pointer to get the callee's stack pointer. Therefore, stack frames up to 32760 bytes can be specified. The initial stack frame size must be a constant, but subsequently the <code>MOVSP</code> instruction can be used to allocate dynamically-sized objects on the stack, or to further extend a constant stack frame larger than 32760 bytes.</P><P>The windowed subroutine call protocol is described in .</P><P><code>ENTRY</code> is undefined if <code>PS.WOE</code> is <code>0</code> or if <code>PS.EXCM</code> is <code>1</code>. Some implementations raise an illegal instruction exception in these cases, as a debugging aid.</P>",
        "Operation": "<pre>WindowCheck (00, PS.CALLINC, 00)\nif as &gt; 3 | PS.WOE = 0 | PS.EXCM = 1 then\n\t-- undefined operation\n\t-- may raise illegal instruction exception\nelse\n\tAR[PS.CALLINC||s<SUB>1..0</SUB>] &#8592; AR[s] &#8722; (0<SUP>17</SUP>||imm12||0<SUP>3</SUP>)\n\tWindowBase &#8592; WindowBase + (02||PS.CALLINC)\n\tWindowStart<SUB>WindowBase</SUB> &#8592; 1\nendif</pre>"
    },
    "ESYNC": {
        "name": "ESYNC",
        "syn": "Execute Synchronize",
        "Assembler Syntax": "ESYNC",
        "Description": "<P><code>ESYNC</code> waits for all previously fetched <code>WSR.*</code><code>, and </code><code>XSR.*</code><code> </code>instructions to be performed before the next instruction uses any register values. This operation is also performed as part of <code>ISYNC</code> and <code>RSYNC</code>. <code>DSYNC</code> is performed as part of this instruction.</P><P>This instruction is appropriate after <code>WSR.EPC*</code><code> in</code>structions. See the Special Register Tables in <code> Special Registers</code> for a complete description of the uses of the <code>ESYNC</code> instruction.</P><P>Because the instruction execution pipeline is implementation-specific, the operation section below specifies only a call to the implementation's <code>esync</code> function.</P>",
        "Operation": "<pre>esync</pre>"
    },
    "EXCW": {
        "name": "EXCW",
        "syn": "Exception Wait",
        "Assembler Syntax": "EXCW",
        "Description": "<P><code>EXCW</code> waits for any exceptions of previously fetched instructions to be handled. Some Xtensa ISA implementations may have imprecise exceptions; on these implementations <code>EXCW</code> waits until all previous instruction exceptions are taken or the instructions are known to be exception-free. Because the instruction execution pipeline and exception handling is implementation-specific, the operation section below specifies only a call to the implementation's <code>E</code>xcepti<code>onWait</code> function.</P>",
        "Operation": "<pre>ExceptionWait</pre>"
    },
    "EXTUI": {
        "name": "EXTUI",
        "syn": "Extract Unsigned Immediate",
        "Assembler Syntax": "EXTUI ar, at, shiftimm, maskimm",
        "Description": "<P><code>EXTUI</code> performs an unsigned bit field extraction from a 32-bit register value. Specifically, it shifts the contents of address register <code>at</code> right by the shift amount <code>shiftimm</code>, which is a value <code>0..31</code> stored in bits <code>16</code> and <code>11..8</code> of the instruction word (the <code>sa</code> fields). The shift result is then ANDed with a mask of <code>maskimm</code> least-significant 1 bits and the result is written to address register <code>ar</code>. The number of mask bits, <code>maskimm</code>, may take the values <code>1..16</code>, and is stored in the <code>op2</code> field as <code>maskimm-1</code>. The bits extracted are therefore <code>sa</code><code>+</code><code>op2..sa</code>.</P><P>The operation of this instruction when <code>sa</code><code>+</code><code>op2</code> <code>&gt;</code> <code>31</code> is undefined and reserved for future use.</P>",
        "Operation": "<pre>mask &#8592; 0<SUP>31-op2</SUP>||1<SUP>op2</SUP>+<SUP>1</SUP>\nAR[r] &#8592; (0<SUP>32</SUP>||AR[t])<SUB>31+sa..sa</SUB> and mask</pre>"
    },
    "EXTW": {
        "name": "EXTW",
        "syn": "External Wait",
        "Assembler Syntax": "EXTW",
        "Description": "<P><code>EXTW</code> is a superset of <code>MEMW</code>. <code>EXTW</code> ensures that both</P><UL><LI>all previous load, store, acquire, release, prefetch, and cache instructions; and</LI><LI>any other effect of any previous instruction which is visible at the pins of the Xtensa processor</LI></UL><P>complete (or perform as described in ) before either</P><UL><LI>any subsequent load, store, acquire, release, prefetch, or cache instructions; or</LI><LI>external effects of the execution of any following instruction is visible at the pins of the Xtensa processor (not including instruction prefetch or TIE Queue pops)</LI></UL><P>is allowed to begin.</P><P>While <code>MEMW</code> is intended to implement the <code>volatile</code> attribute of languages such as C and C++, <code>EXTW</code> is intended to be an ordering guarantee for all external effects that the processor can have, including processor pins defined in TIE.</P><P>Because the instruction execution pipeline is implementation-specific, the operation section below specifies only a call to the implementation's <code>extw</code> function.</P>",
        "Operation": "<pre>extw</pre>"
    },
    "GETEX": {
        "name": "GETEX",
        "syn": "Get Exclusive Result",
        "Assembler Syntax": "GETEX",
        "Description": "<P><code>GETEX</code> waits for any outstanding result of a <code>S32EX</code> instruction in the special register, <code>ATOMCTL</code>. It then exchanges bit[8] of <code>ATOMCTL</code> with bit[0] of the address register <code>as</code> and zeros the remaining bits of address register <code>as</code>. It is used to bring the result of the <code>S32EX</code> instruction to an AR register where it can be tested with a standard branch and to restore the original contents of <code>ATOMCTL[8]</code> that were saved by the <code>S32EX</code> instruction.</P>",
        "Operation": "<pre>temp &#8592; ATOMCTL8\nATOMCTL8 &#8592; AR[r]0\nAR[r] &#8592; 0<SUP>31</SUP>||temp</pre>"
    },
    "HALT": {
        "name": "HALT",
        "syn": "Halt",
        "Assembler Syntax": "HALT",
        "Description": "<P><code>HALT</code> halts the processor in the way provided for by the implementation. For a 16-bit version, see <code>HALT.N</code>.</P>",
        "Operation": "<pre>Halt Processor</pre>"
    },
    "HALT.N": {
        "name": "HALT.N",
        "syn": "Narrow Halt",
        "Assembler Syntax": "HALT.N",
        "Description": "<P><code>HALT.N</code> is a 16-bit opcode that halts the processor in the way provided for by the implementation. For a 24-bit version, see <code>HALT</code>.</P>",
        "Operation": "<pre>Halt Processor</pre>"
    },
    "IDTLB": {
        "name": "IDTLB",
        "syn": "Invalidate Data TLB Entry",
        "Assembler Syntax": "IDTLB as",
        "Description": "<P><code>IDTLB</code> invalidates the data TLB entry specified by the contents of address register <code>as</code>. See  for information on the address register formats for specific Memory Protection and Translation Options. The point at which the invalidation is effected is implementation-specific. Any translation that would be affected by this invalidation before the execution of a <code>DSYNC</code> instruction is therefore undefined.</P><P><code>IDTLB</code> is a privileged instruction.</P><P>The representation of validity in Xtensa TLBs is implementation-specific, and thus the operation section below writes the implementation-specific value <BR><code>InvalidDataTLBEntry</code>.</P>",
        "Operation": "<pre>if CRING != 0 then\n\tException (PrivilegedCause)\nelse\n\t(vpn, ei, wi) &#8592; SplitDataTLBEntrySpec(AR[s])\n\tDataTLB[wi][ei] &#8592; InvalidDataTLBEntry\nendif</pre>"
    },
    "IHI": {
        "name": "IHI",
        "syn": "Instruction Cache Hit Invalidate",
        "Assembler Syntax": "IHI as, 0..1020",
        "Description": "<P><code>IHI</code> performs an instruction cache hit invalidate. It invalidates the specified line in the instruction cache, if it is present. If the specified address is not in the instruction cache, then this instruction has no effect. If the specified line is already invalid, then this instruction has no effect. If the specified line has been locked by an <code>IPFL</code> instruction, then no invalidation is done and no exception is raised because of the lock. The line remains in the cache and must be unlocked by an <code>IHU</code> or <code>IIU</code> instruction before it can be invalidated. Otherwise, if the specified line is present, it is invalidated.</P><P>This instruction is required before executing instructions from the instruction cache that have been written by this processor, another processor, or DMA. The writes must first be forced out of the data cache, either by using <code>DHWB</code> or by using stores that bypass or write through the data cache. An <code>ISYNC</code> instruction should then be used to guarantee that the modified instructions are visible to instruction cache misses. The instruction cache should then be invalidated for the affected addresses using a series of <code>IHI</code> instructions. An <code>ISYNC</code> instruction should then be used to guarantee that this processor's fetch pipeline does not contain instructions from the invalidated lines.</P><P>Because the organization of caches is implementation-specific, the operation section below specifies only a call to the implementation's <code>ihitinval</code> function.</P><P><code>IHI</code> forms a virtual address by adding the contents of address register <code>as</code> and an 8-bit zero-extended constant value encoded in the instruction word shifted left by two. Therefore, the offset can specify multiples of four from zero to 1020. If the Region Translation Option  or the MMU Option  is enabled, the virtual address is translated to the physical address. If not, the physical address is identical to the virtual address. If the translation encounters an error (for example, protection violation), the processor raises one of several exceptions . The translation is done as if the address were for an instruction fetch.</P>",
        "Operation": "<pre>vAddr &#8592; AR[s] + (0<SUP>22</SUP>||imm8||0<SUP>2</SUP>)\n(pAddr, attributes, cause) &#8592; ftranslate(vAddr, CRING)\nif invalid(attributes) then\n\tEXCVADDR &#8592; vAddr\n\tException (cause)\nelse\n\tihitinval(vAddr, pAddr)\nendif</pre>"
    },
    "IHU": {
        "name": "IHU",
        "syn": "Instruction Cache Hit Unlock",
        "Assembler Syntax": "IHU as, 0..240",
        "Description": "<P><code>IHU</code> performs an instruction cache unlock if hit. The purpose of <code>IHU</code> is to remove the lock created by an IPFL instruction. Xtensa ISA implementations that do not implement cache locking must raise an illegal instruction exception when this opcode is executed.</P><P><code>IHU</code> checks whether the line containing the specified address is present in the instruction cache, and if so, it clears the lock associated with that line. To unlock by index without knowing the address of the locked line, use the IIU instruction.</P><P><code>IHU</code> forms a virtual address by adding the contents of address register <code>as</code> and a 4-bit zero-extended constant value encoded in the instruction word shifted left by four. Therefore, the offset can specify multiples of 16 from zero to 240. If the Region Translation Option  or the MMU Option  is enabled, the virtual address is translated to the physical address. If not, the physical address is identical to the virtual address. If the translation encounters an error (for example or protection violation), the processor takes one of several exceptions . The translation is done as if the address were for an instruction fetch.</P><P><code>IHU</code> is a privileged instruction.</P>",
        "Operation": "<pre>if CRING != 0 then\n\tException (PrivilegedCause)\nelse\n\tvAddr &#8592; AR[s] + (0<SUP>24</SUP>||imm4||0<SUP>4</SUP>)\n\t(pAddr, attributes, cause) &#8592; ftranslate(vAddr, CRING)\n\tif invalid(attributes) then\n\t\tEXCVADDR &#8592; vAddr\n\t\tException (cause)\n\telse\n\t\tihitunlock(vAddr, pAddr)\n\tendif\nendif</pre>"
    },
    "III": {
        "name": "III",
        "syn": "Instruction Cache Index Invalidate",
        "Assembler Syntax": "III as, 0..1020",
        "Description": "<P><code>III</code> performs an instruction cache index invalidate. This instruction uses the virtual address to choose a location in the instruction cache and invalidates the specified line. The method for mapping the virtual address to an instruction cache location is implementation-specific. If the chosen line is already invalid, then this instruction has no effect. If the chosen line has been locked by an <code>IPFL</code> instruction, then no invalidation is done and no exception is raised because of the lock. The line remains in the cache and must be unlocked by an <code>IHU</code> or <code>IIU</code> instruction before it can be invalidated. This instruction is useful for instruction cache initialization after power-up or for invalidating the entire instruction cache. An <code>ISYNC</code> instruction should then be used to guarantee that this processor's fetch pipeline does not contain instructions from the invalidated lines.</P><P><code>III</code> forms a virtual address by adding the contents of address register <code>as</code> and an 8-bit zero-extended constant value encoded in the instruction word shifted left by two. Therefore, the offset can specify multiples of four from zero to 1020. The virtual address chooses a cache line without translation and without raising the associated exceptions.</P><P>Because the organization of caches is implementation-specific, the operation section below specifies only a call to the implementation's <code>iindexinval</code> function.</P><P><code>III</code> is a privileged instruction.</P>",
        "Operation": "<pre>if CRING != 0 then\n\tException (PrivilegedCause)\nelse\n\tvAddr &#8592; AR[s] + (0<SUP>22</SUP>||imm8||0<SUP>2</SUP>)\n\tiindexinval(vAddr, pAddr)\nendif</pre>"
    },
    "IITLB": {
        "name": "IITLB",
        "syn": "Invalidate Instruction TLB Entry",
        "Assembler Syntax": "IITLB as",
        "Description": "<P><code>IITLB</code> invalidates the instruction TLB entry specified by the contents of address register <code>as</code>. See  for information on the address register formats for specific Memory Protection and Translation options. The point at which the invalidation is effected is implementation-specific. Any translation that would be affected by this invalidation before the execution of an <code>ISYNC</code> instruction is therefore undefined.</P><P><code>IITLB</code> is a privileged instruction.</P><P>The representation of validity in Xtensa TLBs is implementation-specific, and thus the operation section below writes the implementation-specific value <BR><code>InvalidInstTLBEntry</code>.</P>",
        "Operation": "<pre>if CRING != 0 then\n\tException (PrivilegedCause)\nelse\n\t(vpn, ei, wi) &#8592; SplitInstTLBEntrySpec(AR[s])\n\tInstTLB[wi][ei] &#8592; InvalidInstTLBEntry\nendif</pre>"
    },
    "IIU": {
        "name": "IIU",
        "syn": "Instruction Cache Index Unlock",
        "Assembler Syntax": "IIU as, 0..240",
        "Description": "<P><code>IIU </code>uses<code> </code>the virtual address to choose a location in the instruction cache and unlocks the chosen line. The purpose of IIU is to remove the lock created by an IPFL instruction. The method for mapping the virtual address to an instruction cache location is implementation-specific. This instruction is primarily useful for unlocking the entire instruction cache. Xtensa ISA implementations that do not implement cache locking must raise an illegal instruction exception when this opcode is executed. In some implementations, <code>IIU</code> invalidates the cache line in addition to unlocking it.</P><P>To unlock a specific cache line if it is in the cache, use the IHU instruction.</P><P><code>IIU</code> forms a virtual address by adding the contents of address register <code>as</code> and a 4-bit zero-extended constant value encoded in the instruction word shifted left by four. Therefore, the offset can specify multiples of 16 from zero to 240. The virtual address chooses a cache line without translation and without raising the associated exceptions.</P><P>Because the organization of caches is implementation-specific, the operation section below specifies only a call to the implementation's <code>iindexunlock</code> function.</P><P>IIU is a privileged instruction.</P>",
        "Operation": "<pre>if CRING != 0 then\n\tException (PrivilegedCause)\nelse\n\tvAddr &#8592; AR[s] + (0<SUP>24</SUP>||imm4||0<SUP>4</SUP>)\n\tiindexunlock(vAddr)\nendif</pre>"
    },
    "ILL": {
        "name": "ILL",
        "syn": "Illegal Instruction",
        "Assembler Syntax": "ILL",
        "Description": "<P><code>ILL</code> is an opcode that does whatever illegal opcodes do in the implementation. Often that is to raise an illegal instruction exception. It provides a way to test what happens to an illegal opcode and reduces the probability that data will be successfully executed. For a 16-bit version, see <code>ILL.N</code>.</P>",
        "Operation": "<pre>Exception(IllegalInstructionCause)</pre>"
    },
    "ILL.N": {
        "name": "ILL.N",
        "syn": "Narrow Illegal Instruction",
        "Assembler Syntax": "ILL.N",
        "Description": "<P><code>ILL.N</code> is a 16-bit opcode that does whatever illegal opcodes do in the implementation. For a 24-bit version, see <code>ILL</code>.</P>",
        "Operation": "<pre>Exception(IllegalInstructionCause)</pre>"
    },
    "IPF": {
        "name": "IPF",
        "syn": "Instruction Cache Prefetch",
        "Assembler Syntax": "IPF as, 0..1020",
        "Description": "<P><code>IPF</code> performs an instruction cache prefetch. The purpose of <code>IPF</code> is to improve performance, but not to affect state defined by the ISA. Therefore, some Xtensa ISA implementations may choose to implement this instruction as a simple \"no-operation\" instruction. In general, the performance improvement from using this instruction is implementation-dependent. In some implementations, <code>IPF</code> checks whether the line containing the specified address is present in the instruction cache, and if not, it begins the transfer of the line from memory to the instruction cache. Prefetching an instruction line may prevent the processor from taking an instruction cache miss later. Refer to a specific <code>Xtensa Microprocessor Data Book</code> for more details.</P><P><code>IPF</code> forms a virtual address by adding the contents of address register <code>as</code> and an 8-bit zero-extended constant value encoded in the instruction word shifted left by two. Therefore, the offset can specify multiples of four from zero to 1020. If the Region Translation Option  or the MMU Option  is enabled, the virtual address is translated to the physical address. If not, the physical address is identical to the virtual address. The translation is done as if the address were for an instruction fetch. No exception is raised on either translation or memory reference. Instead of raising an exception, the prefetch is dropped and the instruction becomes a nop.</P>",
        "Operation": "<pre>vAddr &#8592; AR[s] + (0<SUP>22</SUP>||imm8||0<SUP>2</SUP>)\n(pAddr, attributes, cause) &#8592; ftranslate(vAddr, CRING)\nif not invalid(attributes) then\n\tiprefetch(vAddr, pAddr, 0)\nendif</pre>"
    },
    "IPFL": {
        "name": "IPFL",
        "syn": "Instruction Cache Prefetch and Lock",
        "Assembler Syntax": "IPFL as, 0..240",
        "Description": "<P><code>IPFL</code> performs an instruction cache prefetch and lock. The purpose of <code>IPFL</code> is to improve performance, but not to affect state defined by the ISA. Xtensa ISA implementations that do not implement cache locking must raise an illegal instruction exception when this opcode is executed. In general, the performance improvement from using this instruction is implementation-dependent as implementations may not overlap the cache fill with the execution of other instructions.</P><P>In some implementations, <code>IPFL</code> checks whether the line containing the specified address is present in the instruction cache, and if not, begins the transfer of the line from memory to the instruction cache. The line is placed in the instruction cache and marked as locked, so it is not replaceable by ordinary instruction cache misses. To unlock the line, use <code>IHU</code> or <code>IIU</code>. To prefetch without locking, use the <code>IPF</code> instruction.</P><P><code>IPFL</code> forms a virtual address by adding the contents of address register <code>as</code> and a 4-bit zero-extended constant value encoded in the instruction word shifted left by four. Therefore, the offset can specify multiples of 16 from zero to 240. If the Region Translation Option  or the MMU Option  is enabled, the virtual address is translated to the physical address. If not, the physical address is identical to the virtual address. The translation is done as if the address were for an instruction fetch. Exceptions are reported exactly as they would be for an instruction fetch. For exceptions fetching the <code>IPFL</code> instruction, <code>EXCVADDR</code> will point to one of the bytes of the <code>IPFL</code> instruction. For exceptions fetching the cache line, <code>EXCVADDR</code> will point to the cache line. <code>EPC</code> points to the <code>IPFL</code> instruction in both cases. </P><P><code>IPFL</code> is a privileged instruction.</P>",
        "Operation": "<pre>if CRING != 0 then\n\tException (PrivilegedCause)\nelse\n\tvAddr &#8592; AR[s] + (0<SUP>24</SUP>||imm4||0<SUP>4</SUP>)\n\t(pAddr, attributes, cause) &#8592; ftranslate(vAddr, CRING)\n\tif invalid(attributes) then\n\t\tEXCVADDR &#8592; vAddr\n\t\tException (cause)\n\telse\n\t\tiprefetch(vAddr, pAddr, 1)\n\tendif\nendif</pre>"
    },
    "ISYNC": {
        "name": "ISYNC",
        "syn": "Instruction Fetch Synchronize",
        "Assembler Syntax": "ISYNC",
        "Description": "<P><code>ISYNC</code> waits for all previously fetched load, store, cache, TLB, <code>WSR.*</code>, and <code>XSR.*</code> instructions that affect instruction fetch to be performed before fetching the next instruction. <code>RSYNC</code>, <code>ESYNC</code>, and <code>DSYNC</code> are performed as part of this instruction.</P><P>The proper sequence for writing instructions and then executing them is:</P>write instructionsuse <code>DHWB</code> to force the data out of the data cache (this step may be skipped if it is not possible for the data to be dirty in the data cache)use <code>MEMW</code> to wait for the writes to be visible to instruction cache missesuse multiple <code>IHI</code> instructions to invalidate the instruction cache for any lines that were modified (this step may be skipped, along with one of the <code>ISYNC</code> steps on either side, if the affected instructions are in InstRAM or cannot be cached)use <code>ISYNC</code> to ensure that fetch pipeline will see the new instructions<P>This instruction also waits for all previously executed <code>WSR.*</code> and <code>XSR.*</code> instructions that affect instruction fetch or register access processor state, including:</P><code>WSR.LCOUNT</code>, <code>WSR.LBEG</code>, <code>WSR.LEND</code><code>WSR.IBREAKENABLE</code>, <code>WSR.IBREAKA[i]</code><code>WSR.CCOMPAREn</code> <P>See the Special Register Tables in <code> Special Registers</code> and <code> Caches and Local Memories</code>, for a complete description of the <code>ISYNC</code> instruction's uses.</P>",
        "Operation": "<pre>isync</pre>"
    },
    "J": {
        "name": "J",
        "syn": "Unconditional Jump",
        "Assembler Syntax": "J label",
        "Description": "<P><code>J</code> performs an unconditional branch to the target address. It uses a signed, 18-bit PC-relative offset to specify the target address. The target address is given by the address of the <code>J</code> instruction plus the sign-extended 18-bit <code>offset</code> field of the instruction plus four, giving a range of <code>-131068</code> to <code>+131075</code> bytes.</P>",
        "Operation": "<pre>nextPC &#8592; PC + (offset<SUB>17</SUB><SUP>14</SUP>||offset) + 4</pre>"
    },
    "J.L": {
        "name": "J.L",
        "syn": "Unconditional Jump Long",
        "Assembler Syntax": "J.L label, an",
        "Description": "<P><code>J.L</code> is an assembler macro which generates exactly a <code>J</code> instruction as long as the offset will reach the label. If the offset is not long enough, the assembler relaxes the instruction to a literal load into <code>an</code> followed by a <code>JX an</code>. The AR register <code>an</code> may or may not be modified.</P>"
    },
    "JX": {
        "name": "JX",
        "syn": "Unconditional Jump Register",
        "Assembler Syntax": "JX as",
        "Description": "<P><code>JX</code> performs an unconditional jump to the address in register <code>as</code>.</P>",
        "Operation": "<pre>nextPC &#8592; AR[s]</pre>"
    },
    "L16SI": {
        "name": "L16SI",
        "syn": "Load 16-bit Signed",
        "Assembler Syntax": "L16SI at, as, 0..510",
        "Description": "<P><code>L16SI</code> is a 16-bit signed load from memory. It forms a virtual address by adding the contents of address register <code>as</code> and an 8-bit zero-extended constant value encoded in the instruction word shifted left by 1. Therefore, the offset can specify multiples of two from zero to 510. Sixteen bits (two bytes) are read from the physical address. This data is then sign-extended and written to address register <code>at</code>.</P><P>If the Region Translation Option  or the MMU Option is enabled, the virtual address is translated to the physical address. If not, the physical address is identical to the virtual address. If the translation or memory reference encounters an error (for example, protection violation, non-existent memory), the processor raises one of several exceptions .</P><P>Without the Unaligned Exception Option , the least significant address bit is ignored; a reference to an odd address produces the same result as a reference to the address minus one. With the Unaligned Exception Option, such an access raises an exception.</P>",
        "Operation": "<pre>vAddr &#8592; AR[s] + (0<SUP>23</SUP>||imm8||0)\n(mem16, error) &#8592; Load16(vAddr)\nif error then\n\tEXCVADDR &#8592; vAddr\n\tException (LoadStoreErrorCause)\nelse\n\tAR[t] &#8592; mem16<SUB>15</SUB><SUP>16</SUP>||mem16\nendif</pre>"
    },
    "L16UI": {
        "name": "L16UI",
        "syn": "Load 16-bit Unsigned",
        "Assembler Syntax": "L16UI at, as, 0..510",
        "Description": "<P><code>L16UI</code> is a 16-bit unsigned load from memory. It forms a virtual address by adding the contents of address register <code>as</code> and an 8-bit zero-extended constant value encoded in the instruction word shifted left by 1. Therefore, the offset can specify multiples of two from zero to 510. Sixteen bits (two bytes) are read from the physical address. This data is then zero-extended and written to address register <code>at</code>.</P><P>If the Region Translation Option  or the MMU Option  is enabled, the virtual address is translated to the physical address. If not, the physical address is identical to the virtual address. If the translation or memory reference encounters an error (for example, protection violation or non-existent memory), the processor raises one of several exceptions .</P><P>Without the Unaligned Exception Option , the least significant address bit is ignored; a reference to an odd address produces the same result as a reference to the address minus one. With the Unaligned Exception Option, such an access raises an exception.</P>",
        "Operation": "<pre>vAddr &#8592; AR[s] + (0<SUP>23</SUP>||imm8||0)\n(mem16, error) &#8592; Load16(vAddr)\nif error then\n\tEXCVADDR &#8592; vAddr\n\tException (LoadStoreErrorCause)\nelse\n\tAR[t] &#8592; 0<SUP>16</SUP>||mem16\nendif</pre>"
    },
    "L32AI": {
        "name": "L32AI",
        "syn": "Load 32-bit Acquire",
        "Assembler Syntax": "L32AI at, as, 0..1020",
        "Description": "<P><code>L32AI</code> is a 32-bit load from memory with \"acquire\" semantics. This load performs before any subsequent loads, stores, acquires, or releases are performed. It is typically used to test a synchronization variable protecting a critical region (for example, to acquire a lock).</P><P><code>L32AI</code> forms a virtual address by adding the contents of address register <code>as</code> and an <BR>8-bit zero-extended constant value encoded in the instruction word shifted left by two. Therefore, the offset can specify multiples of four from zero to 1020. 32 bits (four bytes) are read from the physical address. This data is then written to address register <code>at</code>. <code>L32AI</code> causes the processor to delay processing of subsequent loads, stores, acquires, and releases until the <code>L32AI</code> is performed. In some Xtensa ISA implementations, this occurs automatically and <code>L32AI</code> is identical to <code>L32I</code>. Other implementations (for example, those with multiple outstanding loads and stores) delay processing as described above. Because the method of delay is implementation-dependent, this is indicated in the operation section below by the implementation function acquire.</P><P>If the Region Translation Option  or the MMU Option  is enabled, the virtual address is translated to the physical address. If not, the physical address is identical to the virtual address. If the translation or memory reference encounters an error (for example, protection violation or non-existent memory), the processor raises one of several exceptions .</P><P>Without the Unaligned Exception Option , the two least significant bits of the address are ignored. A reference to an address that is not 0 mod 4 produces the same result as a reference to the address with the least significant bits cleared. With the Unaligned Exception Option, such an access raises an exception.</P>",
        "Operation": "<pre>vAddr &#8592; AR[s] + (022||imm8||02)\n(mem32, error) &#8592; Load32(vAddr)\nif error then\n\tEXCVADDR &#8592; vAddr\n\tException (LoadStoreErrorCause)\nelse\n\tAR[t] &#8592; mem32\n\tacquire\nendif</pre>"
    },
    "L32E": {
        "name": "L32E",
        "syn": "Load 32-bit for Window Exceptions",
        "Assembler Syntax": "L32E at, as, -64..-4",
        "Description": "<P><code>L32E</code> is a 32-bit load instruction similar to <code>L32I</code> but with semantics required by window overflow and window underflow exception handlers. In particular, memory access checking is done with <code>PS.RING</code> instead of <code>CRING</code>, and the offset used to form the virtual address is a 4-bit one-extended immediate. Therefore, the offset can specify multiples of four from -64 to -4. In configurations without the <code>MMU Option</code>, there is no <code>PS.RING</code>, and <code>L32E</code> is similar to <code>L32I</code> with a negative offset.</P><P>If the Region Translation Option  or the MMU Option  is enabled, the virtual address is translated to the physical address. If not, the physical address is identical to the virtual address. If the translation or memory reference encounters an error (for example, protection violation or non-existent memory), the processor raises one of several exceptions .</P><P>Without the Unaligned Exception Option , the two least significant bits of the address are ignored. A reference to an address that is not 0 mod 4 produces the same result as a reference to the address with the least significant bits cleared. With the Unaligned Exception Option, such an access raises an exception.</P><P><code>L32E</code> is a privileged instruction.</P>",
        "Operation": "<pre>if CRING != 0 then\n\tException (PrivilegedCause)\nelse\n\tvAddr &#8592; AR[s] + (1<SUP>26</SUP>||r||0<SUP>2</SUP>)\n\tring &#8592; if MMU Option then PS.RING else 0\n\t(mem32, error) &#8592; Load32Ring(vAddr, ring)\n\tif error then\n\t\tEXCVADDR &#8592; vAddr\n\t\tException (LoadStoreErrorCause)\n\telse\n\t\tAR[t] &#8592; mem32\n\tendif\nendif</pre>"
    },
    "L32EX": {
        "name": "L32EX",
        "syn": "Load 32-bit Exclusive",
        "Assembler Syntax": "L32EX at, as",
        "Description": "<P><code>L32EX</code> is a 32-bit load from memory. Its virtual address is the contents of address register <code>as</code>. This data is then written to address register <code>at</code>. In addition, exclusive state is set to monitor whether the memory location is modified. If the target of the virtual address is not able to properly handle exclusive accesses, the instruction raises the <code>ExclusiveErrorCause</code> exception.</P><P>If the Region Translation Option  or the MMU Option  is enabled, the virtual address is translated to the physical address. If not, the physical address is identical to the virtual address. If the translation or memory reference encounters an error (for example, protection violation, non-existent memory), the processor raises one of several exceptions .</P>",
        "Operation": "<pre>vAddr &#8592; AR[s]\n(mem32, error) &#8592; Load32(vAddr)\nif error then\n\tEXCVADDR &#8592; vAddr\n\tException (LoadStoreErrorCause)\nelse\n\tAR[t] &#8592; mem32\n\tsetmonitor\nendif</pre>"
    },
    "L32I": {
        "name": "L32I",
        "syn": "Load 32-bit",
        "Assembler Syntax": "L32I at, as, 0..1020",
        "Description": "<P><code>L32I</code> is a 32-bit load from memory. It forms a virtual address by adding the contents of address register <code>as</code> and an 8-bit zero-extended constant value encoded in the instruction word shifted left by two. Therefore, the offset can specify multiples of four from zero to 1020. Thirty-two bits (four bytes) are read from the physical address. This data is then written to address register <code>at</code>.</P><P>If the Region Translation Option  or the MMU Option  is enabled, the virtual address is translated to the physical address. If not, the physical address is identical to the virtual address. If the translation or memory reference encounters an error (for example, protection violation, non-existent memory), the processor raises one of several exceptions .</P><P>Without the Unaligned Exception Option , the two least significant bits of the address are ignored. A reference to an address that is not 0 mod 4 produces the same result as a reference to the address with the least significant bits cleared. With the Unaligned Exception Option, such an access raises an exception.</P><P>If the Instruction Memory Access Option  is configured, <code>L32I</code> is one of only a few memory reference instructions that can access instruction RAM/ROM.</P>",
        "Operation": "<pre>vAddr &#8592; AR[s] +<SUB> </SUB>(0<SUP>22</SUP>||imm8||0<SUP>2</SUP>)\n(mem32, error) &#8592; Load32(vAddr)\nif error then\n\tEXCVADDR &#8592; vAddr\n\tException (LoadStoreErrorCause)\nelse\n\tAR[t] &#8592; mem32\nendif</pre>"
    },
    "L32I.N": {
        "name": "L32I.N",
        "syn": "Narrow Load 32-bit",
        "Assembler Syntax": "L32I.N at, as, 0..60",
        "Description": "<P><code>L32I.N</code> is similar to <code>L32I</code>, but has a 16-bit encoding and supports a smaller range of offset values encoded in the instruction word.</P><P><code>L32I.N</code> is a 32-bit load from memory. It forms a virtual address by adding the contents of address register <code>as</code> and a 4-bit zero-extended constant value encoded in the instruction word shifted left by two. Therefore, the offset can specify multiples of four from zero to 60. Thirty-two bits (four bytes) are read from the physical address. This data is then written to address register <code>at</code>.</P><P>If the Region Translation Option  or the MMU Option  is enabled, the virtual address is translated to the physical address. If not, the physical address is identical to the virtual address. If the translation or memory reference encounters an error (for example, protection violation or non-existent memory), the processor raises one of several exceptions .</P><P>Without the Unaligned Exception Option , the two least significant bits of the address are ignored. A reference to an address that is not 0 mod 4 produces the same result as a reference to the address with the least significant bits cleared. With the Unaligned Exception Option, such an access raises an exception.</P><P>If the Instruction Memory Access Option  is configured, <code>L32I.N</code> is one of only a few memory reference instructions that can access instruction RAM/ROM.</P>",
        "Operation": "<pre>vAddr &#8592; AR[s] +<SUB> </SUB>(0<SUP>26</SUP>||imm4||0<SUP>2</SUP>)\n(mem32, error) &#8592; Load32(vAddr)\nif error then\n\tEXCVADDR &#8592; vAddr\n\tException (LoadStoreErrorCause)\nelse\n\tAR[t] &#8592; mem32\nendif</pre>"
    },
    "L32R": {
        "name": "L32R",
        "syn": "Load 32-bit PC-Relative",
        "Assembler Syntax": "L32R at, label",
        "Description": "<P><code>L32R</code> is a PC-relative 32-bit load from memory. It is typically used to load constant values into a register when the constant cannot be encoded in a <code>MOVI</code> instruction.</P><P><code>L32R</code> forms a virtual address by adding the 16-bit one-extended constant value encoded in the instruction word shifted left by two to the address of the <code>L32R</code> plus three with the two least significant bits cleared. Therefore, the offset can always specify 32-bit aligned addresses from -262141 to -4 bytes from the address of the <code>L32R</code> instruction. 32 bits (four bytes) are read from the physical address. This data is then written to address register <code>at</code>.</P><P>In the presence of the Extended L32R Option  when <code>LITBASE[0]</code> is clear, the instruction has the identical operation. When <code>LITBASE[0]</code> is set, <code>L32R</code> forms a virtual address by adding the 16-bit one extended constant value encoded in the instruction word shifted left by two to the literal base address indicated by the upper 20 bits of <code>LITBASE</code>. The offset can specify 32-bit aligned addresses from -262144 to -4 bytes from the literal base address.</P><P>If the Region Translation Option  or the MMU Option  is enabled, the virtual address is translated to the physical address. If not, the physical address is identical to the virtual address. If the translation or memory reference encounters an error (for example, protection violation or non-existent memory), the processor raises one of several exceptions .</P><P>It is not possible to specify an unaligned address.</P><P>If the Instruction Memory Access Option  is configured, <code>L32R</code> is one of only a few memory reference instructions that can access instruction RAM/ROM.</P>",
        "Operation": "<pre>if Extended L32R Option and LITBASE<SUB>0</SUB> then\n\tvAddr &#8592; (LITBASE<SUB>31..12</SUB>||0<SUP>12</SUP>) + (1<SUP>14</SUP>||imm16||0<SUP>2</SUP>)\nelse\n\tvAddr &#8592; ((PC + 3)<SUB>31..2</SUB>||0<SUP>2</SUP>) + (1<SUP>14</SUP>||imm16||0<SUP>2</SUP>)\nendif\n(mem32, error) &#8592; Load32(vAddr)\nif error then\n\tEXCVADDR &#8592; vAddr\n\tException (LoadStoreErrorCause)\nelse\n\tAR[t] &#8592; mem32\nendif</pre>"
    },
    "L8UI": {
        "name": "L8UI",
        "syn": "Load 8-bit Unsigned",
        "Assembler Syntax": "L8UI at, as, 0..255",
        "Description": "<P><code>L8UI</code> is an 8-bit unsigned load from memory. It forms a virtual address by adding the contents of address register <code>as</code> and an 8-bit zero-extended constant value encoded in the instruction word. Therefore, the offset ranges from 0 to 255. Eight bits (one byte) are read from the physical address. This data is then zero-extended and written to address register <code>at</code>.</P><P>If the Region Translation Option  or the MMU Option is enabled, the virtual address is translated to the physical address. If not, the physical address is identical to the virtual address. If the translation or memory reference encounters an error (for example, protection violation or non-existent memory), the processor raises one of several exceptions .</P>",
        "Operation": "<pre>vAddr &#8592; AR[s] + (0<SUP>24</SUP>||imm8)\n(mem8, error) &#8592; Load8(vAddr)\nif error then\n\tEXCVADDR &#8592; vAddr\n\tException (LoadStoreErrorCause)\nelse\n\tAR[t] &#8592; 0<SUP>24</SUP>||mem8\nendif</pre>"
    },
    "LDCT": {
        "name": "LDCT",
        "syn": "Load Data Cache Tag",
        "Assembler Syntax": "LDCT at, as",
        "Description": "<P><code>LDCT</code> is not part of the Xtensa Instruction Set Architecture, but is instead specific to an implementation. That is, it may not exist in all implementations of the Xtensa ISA and its exact method of addressing the cache may depend on the implementation.</P><P><code>LDCT</code> is intended for reading the RAM array that implements the data cache tags as part of manufacturing test.</P><P><code>LDCT</code> uses the contents of address register <code>as</code> to select a line in the data cache, reads the tag associated with this line, and writes the result to address register <code>at</code>. The value written to <code>at</code> is described under Cache Tag Format in .</P><P><code>LDCT</code> is a privileged instruction.</P>",
        "Operation": "<pre>if CRING != 0 then\n\tException (PrivilegedCause)\nelse\n\tindex &#8592; AR[s]<SUB>x-1..z-2</SUB>\n\tAR[t] &#8592; DataCacheTag[index] // see Implementation Notes below\nendif</pre>"
    },
    "LDCW": {
        "name": "LDCW",
        "syn": "Load Data Cache Word",
        "Assembler Syntax": "LDCW at, as",
        "Description": "<P><code>LDCW</code> is not part of the Xtensa Instruction Set Architecture, but is instead specific to an implementation. That is, it may not exist in all implementations of the Xtensa ISA and its exact method of addressing the cache may depend on the implementation.</P><P><code>LDCW</code> is intended for reading the RAM array that implements the data cache as part of manufacturing test.</P><P><code>LDCW</code> uses the contents of address register <code>as</code> to select a line in the data cache and one 32-bit quantity within that line, reads that data, and writes the result to address register <code>at</code>.</P><P><code>LDCW</code> is a privileged instruction.</P>",
        "Operation": "<pre>if CRING != 0 then\n\tException (PrivilegedCause)\nelse\n\tindex &#8592; AR[s]<SUB>x-1..2</SUB>\n\tAR[t] &#8592; DataCacheData [index] // see Implementation Notes below\nendif</pre>"
    },
    "LDDEC": {
        "name": "LDDEC",
        "syn": "Load with Autodecrement",
        "Assembler Syntax": "LDDEC mw, as",
        "Description": "<P><code>LDDEC</code> loads MAC16 register <code>mw</code> from memory using auto-decrement addressing. It forms a virtual address by subtracting 4 from the contents of address register <code>as</code>. 32 bits (four bytes) are read from the physical address. This data is then written to MAC16 register <code>mw</code>, and the virtual address is written back to address register <code>as</code>.</P><P>If the Region Translation Option  or the MMU Option  is enabled, the virtual address is translated to the physical address. If not, the physical address is identical to the virtual address. If the translation or memory reference encounters an error (for example, protection violation or non-existent memory), the processor raises one of several exceptions .</P><P>Without the Unaligned Exception Option , the two least significant bits of the address are ignored. A reference to an address that is not 0 mod 4 produces the same result as a reference to the address with the least significant bits cleared. With the Unaligned Exception Option, such an access raises an exception.</P>",
        "Operation": "<pre>vAddr &#8592; AR[s] &#8722; 4\n(mem32, error) &#8592; Load32(vAddr)\nif error then\n\tEXCVADDR &#8592; vAddr\n\tException (LoadStoreErrorCause)\nelse\n\tMR[w] &#8592; mem32\n\tAR[s] &#8592; vAddr\nendif</pre>"
    },
    "LDDR32.P": {
        "name": "LDDR32.P",
        "syn": "Load to DDR Register",
        "Assembler Syntax": "LDDR32.P as",
        "Description": "<P>This instruction is used only in On-Chip Debug Mode and exists only in some implementations. It is an illegal instruction when the processor is not in On-Chip Debug Mode. See the <code>Xtensa Debug Guide</code> for a description of its operation.</P>"
    },
    "LDINC": {
        "name": "LDINC",
        "syn": "Load with Autoincrement",
        "Assembler Syntax": "LDINC mw, as",
        "Description": "<P><code>LDINC</code> loads MAC16 register <code>mw</code> from memory using auto-increment addressing. It forms a virtual address by adding 4 to the contents of address register <code>as</code>. 32 bits (four bytes) are read from the physical address. This data is then written to MAC16 register <code>mw</code>, and the virtual address is written back to address register <code>as</code>.</P><P>If the Region Translation Option  or the MMU Option is enabled, the virtual address is translated to the physical address. If not, the physical address is identical to the virtual address. If the translation or memory reference encounters an error (for example, protection violation or non-existent memory), the processor raises one of several exceptions .</P><P>Without the Unaligned Exception Option , the two least significant bits of the address are ignored. A reference to an address that is not 0 mod 4 produces the same result as a reference to the address with the least significant bits cleared. With the Unaligned Exception Option, such an access raises an exception.</P>",
        "Operation": "<pre>vAddr &#8592; AR[s] + 4\n(mem32, error) &#8592; Load32(vAddr)\nif error then\n\tEXCVADDR &#8592; vAddr\n\tException (LoadStoreErrorCause)\nelse\n\tMR[w] &#8592; mem32\n\tAR[s] &#8592; vAddr\nendif</pre>"
    },
    "LICT": {
        "name": "LICT",
        "syn": "Load Instruction Cache Tag",
        "Assembler Syntax": "LICT at, as",
        "Description": "<P><code>LICT</code> is not part of the Xtensa Instruction Set Architecture, but is instead specific to an implementation. That is, it may not exist in all implementations of the Xtensa ISA and its exact method of addressing the cache may depend on the implementation.</P><P><code>LICT</code> is intended for reading the RAM array that implements the instruction cache tags as part of manufacturing test.</P><P><code>LICT</code> uses the contents of address register <code>as</code> to select a line in the instruction cache, reads the tag associated with this line, and writes the result to address register <code>at</code>. The value written to <code>at</code> is described under Cache Tag Format in .</P><P><code>LICT</code> is a privileged instruction.</P>",
        "Operation": "<pre>if CRING != 0 then\n\tException (PrivilegedCause)\nelse\n\tindex &#8592; AR[s]<SUB>x-1..z-2</SUB>\n\tAR[t] &#8592; InstCacheTag[index] // see Implementation Notes below\nendif</pre>"
    },
    "LICW": {
        "name": "LICW",
        "syn": "Load Instruction Cache Word",
        "Assembler Syntax": "LICW at, as",
        "Description": "<P><code>LICW</code> is not part of the Xtensa Instruction Set Architecture, but is instead specific to an implementation. That is, it may not exist in all implementations of the Xtensa ISA and its exact method of addressing the cache may depend on the implementation.</P><P><code>LICW</code> is intended for reading the RAM array that implements the instruction cache as part of manufacturing test.</P><P><code>LICW</code> uses the contents of address register <code>as</code> to select a line in the instruction cache and one 32-bit quantity within that line, reads that data, and writes the result to address register <code>at</code>.</P><P><code>LICW</code> is a privileged instruction.</P>",
        "Operation": "<pre>if CRING != 0 then\n\tException (PrivilegedCause)\nelse\n\tindex &#8592; AR[s]<SUB>x-1..2</SUB>\n\tAR[t] &#8592; InstCacheData [index] // see Implementation Notes below\nendif</pre>"
    },
    "LOOP": {
        "name": "LOOP",
        "syn": "Loop",
        "Assembler Syntax": "LOOP as, label",
        "Description": "<P><code>LOOP</code> sets up a zero-overhead loop by setting the <code>LCOUNT</code>, <code>LBEG</code>, and <code>LEND</code> special registers, which control instruction fetch. The loop will iterate the number of times specified by address register <code>as</code>, with 0 causing the loop to iterate 2<SUP>32</SUP> times. <code>LCOUNT</code>, the current loop iteration counter, is loaded from the contents of address register <code>as</code> minus one. <code>LEND</code> is the loop end address and is loaded with the address of the <code>LOOP</code> instruction plus four, plus the zero-extended 8-bit offset encoded in the instruction (therefore, the loop code may be up to 256 bytes in length). <code>LBEG</code>, the loop begin address, is loaded with the address of the following instruction.</P><P>After the processor fetches an instruction that increments the <code>PC</code> to the value contained in <code>LEND</code>, and <code>LCOUNT</code> is not zero, it loads the <code>PC</code> with the contents of <code>LBEG</code> and decrements <code>LCOUNT</code>. <code>LOOP</code> is intended to be implemented with help from the instruction fetch engine of the processor, and therefore should not incur a mispredict or taken branch penalty. Branches and jumps to the address contained in <code>LEND</code> do not cause a loop back, and therefore may be used to exit the loop prematurely. Likewise, a return from a call instruction as the last instruction of the loop would not trigger loop back; this case should be avoided.</P><P>There is no mechanism to proceed to the next iteration of the loop from the middle of the loop. The compiler may insert a branch to a <code>NOP</code> placed as the last instruction of the loop to implement this function if required.</P><P>Because <code>LCOUNT</code>, <code>LBEG</code>, and <code>LEND</code> are single registers, zero-overhead loops may not be nested. Using conditional branch instructions to implement outer level loops is typically not a performance issue. Because loops cannot be nested, it is usually inappropriate to include a procedure call inside a loop (the callee might itself use a zero-overhead loop).</P><P>To simplify the implementation of zero-overhead loops, the <code>LBEG</code> address must be such that the first instruction must entirely fit within a naturally aligned four byte region or, if the fetch width is larger than four bytes, a naturally aligned region which is the size of the fetch width. Some implementations require, in addition, that the fetch width is any greater than the naturally aligned power of two region (of four bytes or larger) which is no smaller than that first instruction. When the <code>LOOP</code> instruction would not naturally be placed at such an address, the insertion of <code>NOP</code> instructions or adjustment of which instructions are 16-bit density instructions is sufficient to give it the required alignment.</P><P>The automatic loop-back when the <code>PC</code> increments to match <code>LEND</code> is disabled when <code>PS.EXCM</code> is set. This prevents non-privileged code from affecting the operation of the privileged exception vector code. Dynamic loaders need to avoid mixing new code and old register values as the combination may execute in unexpected ways.</P>",
        "Operation": "<pre>LCOUNT &#8592; AR[s] &#8722; 1\nLBEG &#8592; nextPC\nLEND &#8592; PC + (0<SUP>24</SUP>||imm8) + 4</pre>"
    },
    "LOOPGTZ": {
        "name": "LOOPGTZ",
        "syn": "Loop if Greater Than Zero",
        "Assembler Syntax": "LOOPGTZ as, label",
        "Description": "<P><code>LOOPGTZ</code> sets up a zero-overhead loop by setting the <code>LCOUNT</code>, <code>LBEG</code>, and <code>LEND</code> special registers, which control instruction fetch. The loop will iterate the number of times specified by address register <code>as</code> with values <code><=</code> 0 causing the loop to be skipped altogether by branching directly to the loop end address. <code>LCOUNT</code>, the current loop iteration counter, is loaded from the contents of address register <code>as</code> minus one. <code>LEND</code> is the loop end address and is loaded with the address of the <code>LOOPGTZ</code> instruction plus four, plus the zero-extended 8-bit offset encoded in the instruction (therefore, the loop code may be up to 256 bytes in length). <code>LBEG</code>, the loop begin address, is loaded with the address of the following instruction. <code>LCOUNT</code>, <code>LEND</code>, and <code>LBEG</code> are still loaded even when the loop is skipped.</P><P>After the processor fetches an instruction that increments the <code>PC</code> to the value contained in <code>LEND</code>, and <code>LCOUNT</code> is not zero, it loads the <code>PC</code> with the contents of <code>LBEG</code> and decrements <code>LCOUNT</code>. <code>LOOPGTZ</code> is intended to be implemented with help from the instruction fetch engine of the processor, and therefore should not incur a mispredict or taken branch penalty. Branches and jumps to the address contained in <code>LEND</code> do not cause a loop back, and therefore may be used to exit the loop prematurely. Similarly, a return from a call instruction as the last instruction of the loop would not trigger loop back; this case should be avoided.</P><P>There is no mechanism to proceed to the next iteration of the loop from the middle of the loop. The compiler may insert a branch to a <code>NOP</code> placed as the last instruction of the loop to implement this function if required.</P><P>Because <code>LCOUNT</code>, <code>LBEG</code>, and <code>LEND</code> are single registers, zero-overhead loops may not be nested. Using conditional branch instructions to implement outer level loops is typically not a performance issue. Because loops cannot be nested, it is usually inappropriate to include a procedure call inside a loop (the callee might itself use a zero-overhead loop).</P><P>To simplify the implementation of zero-overhead loops, the <code>LBEG</code> address must be such that the first instruction must entirely fit within a naturally aligned four byte region or, if the fetch width is larger than four bytes, a naturally aligned region which is the next power of two equal to or larger than the fetch width. Some implementations require, in addition, that the fetch width is any greater than the naturally aligned power of two region (of four bytes or larger) which is no smaller than that first instruction. When the <code>LOOP</code> instruction would not naturally be placed at such an address, the insertion of <code>NOP</code> instructions or adjustment of which instructions are 16-bit density instructions is sufficient to give it the required alignment.</P><P>The automatic loop-back when the <code>PC</code> increments to match <code>LEND</code> is disabled when <code>PS.EXCM</code> is set. This prevents non-privileged code from affecting the operation of the privileged exception vector code. Dynamic loaders need to avoid mixing new code and old register values as the combination may execute in unexpected ways.</P>",
        "Operation": "<pre>LCOUNT &#8592; AR[s] &#8722; 1\nLBEG &#8592; nextPC\nLEND &#8592; PC + (0<SUP>24</SUP>||imm8) + 4\nif AR[s] <= 0<SUP>32</SUP> \tthen\n\tnextPC &#8592; PC + (0<SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "LOOPNEZ": {
        "name": "LOOPNEZ",
        "syn": "Loop if Not-Equal Zero",
        "Assembler Syntax": "LOOPNEZ as, label",
        "Description": "<P><code>LOOPNEZ</code> sets up a zero-overhead loop by setting the <code>LCOUNT</code>, <code>LBEG</code>, and <code>LEND</code> special registers, which control instruction fetch. The loop will iterate the number of times specified by address register <code>as</code> with the zero value causing the loop to be skipped altogether by branching directly to the loop end address. <code>LCOUNT</code>, the current loop iteration counter, is loaded from the contents of address register <code>as</code> minus 1. <code>LEND</code> is the loop end address and is loaded with the address of the <code>LOOPNEZ</code> instruction plus four plus the zero-extended 8-bit offset encoded in the instruction (therefore, the loop code may be up to 256 bytes in length). <code>LBEG</code> is loaded with the address of the following instruction. <code>LCOUNT</code>, <code>LEND</code>, and <code>LBEG</code> are still loaded even when the loop is skipped.</P><P>After the processor fetches an instruction that increments the <code>PC</code> to the value contained in <code>LEND</code>, and <code>LCOUNT</code> is not zero, it loads the <code>PC</code> with the contents of <code>LBEG</code> and decrements <code>LCOUNT</code>. <code>LOOPNEZ</code> is intended to be implemented with help from the instruction fetch engine of the processor, and therefore should not incur a mispredict or taken branch penalty. Branches and jumps to the address contained in <code>LEND</code> do not cause a loop back, and therefore may be used to exit the loop prematurely. Similarly a return from a call instruction as the last instruction of the loop would not trigger loop back; this case should be avoided.</P><P>There is no mechanism to proceed to the next iteration of the loop from the middle of the loop. The compiler may insert a branch to a <code>NOP</code> placed as the last instruction of the loop to implement this function if required.</P><P>Because <code>LCOUNT</code>, <code>LBEG</code>, and <code>LEND</code> are single registers, zero-overhead loops may not be nested. Using conditional branch instructions to implement outer level loops is typically not a performance issue. Because loops cannot be nested, it is usually inappropriate to include a procedure call inside a loop (the callee might itself use a zero-overhead loop).</P><P>To simplify the implementation of zero-overhead loops, the <code>LBEG</code> address must be such that the first instruction must entirely fit within a naturally aligned four byte region or, if the fetch width is larger than four bytes, a naturally aligned region which is the next power of two equal to or larger than the fetch width. Some implementations require, in addition, that the fetch width is any greater than the naturally aligned power of two region (of four bytes or larger) which is no smaller than that first instruction. When the <code>LOOP</code> instruction would not naturally be placed at such an address, the insertion of <code>NOP</code> instructions or adjustment of which instructions are 16-bit density instructions is sufficient to give it the required alignment.</P><P>The automatic loop-back when the <code>PC</code> increments to match <code>LEND</code> is disabled when <code>PS.EXCM</code> is set. This prevents non-privileged code from affecting the operation of the privileged exception vector code. Dynamic loaders need to avoid mixing new code and old register values as the combination may execute in unexpected ways.</P>",
        "Operation": "<pre>LCOUNT &#8592; AR[s] &#8722; 1\nLBEG &#8592; nextPC\nLEND &#8592; PC + (0<SUP>24</SUP>||imm8) + 4)\nif AR[s] = 0<SUP>32</SUP> then\n\tnextPC &#8592; PC + (0<SUP>24</SUP>||imm8) + 4\nendif</pre>"
    },
    "MAX": {
        "name": "MAX",
        "syn": "Maximum Value",
        "Assembler Syntax": "MAX ar, as, at",
        "Description": "<P><code>MAX</code> computes the maximum of the twos complement contents of address registers <code>as</code> and <code>at</code> and writes the result to address register <code>ar</code>.</P>",
        "Operation": "<pre>AR[r] &#8592; if AR[s] &lt; AR[t] then AR[t] else AR[s]</pre>"
    },
    "MAXU": {
        "name": "MAXU",
        "syn": "Maximum Value Unsigned",
        "Assembler Syntax": "MAXU ar, as, at",
        "Description": "<P><code>MAXU</code> computes the maximum of the unsigned contents of address registers <code>as</code> and <code>at</code> and writes the result to address register <code>ar</code>.</P>",
        "Operation": "<pre>AR[r] &#8592; if (0||AR[s]) &lt; (0||AR[t]) then AR[t] else AR[s]</pre>"
    },
    "MEMW": {
        "name": "MEMW",
        "syn": "Memory Wait",
        "Assembler Syntax": "MEMW",
        "Description": "<P><code>MEMW</code> ensures that all previous load, store, acquire, release, prefetch, and cache instructions along with any writebacks caused by previous cache instructions perform before performing any subsequent load, store, acquire, release, prefetch, or cache instructions. <code>MEMW</code> is intended to implement the <code>volatile</code> attribute of languages such as C and C++. The compiler should separate all <code>volatile</code> loads and stores with a <code>MEMW</code> instruction. <code>ISYNC</code> should be used to cause instruction fetches to wait as <code>MEMW</code> will have no effect on them.</P><P>On processor/system implementations that always reference memory in program order, <code>MEMW</code> may be a no-op. Implementations that reorder load, store, or cache instructions, or which perform merging of stores (for example, in a write buffer) must order such memory references so that all memory references executed before <code>MEMW</code> are performed before any memory references that are executed after <code>MEMW</code>.</P><P>Because the instruction execution pipeline is implementation-specific, the operation section below specifies only a call to the implementation's <code>memw</code> function.</P>",
        "Operation": "<pre>memw</pre>"
    },
    "MIN": {
        "name": "MIN",
        "syn": "Minimum Value",
        "Assembler Syntax": "MIN ar, as, at",
        "Description": "<P><code>MIN</code> computes the minimum of the twos complement contents of address registers <code>as</code> and <code>at</code> and writes the result to address register <code>ar</code>.</P>",
        "Operation": "<pre>AR[r] &#8592; if AR[s] &lt; AR[t] then AR[s] else AR[t]</pre>"
    },
    "MINU": {
        "name": "MINU",
        "syn": "Minimum Value Unsigned",
        "Assembler Syntax": "MINU ar, as, at",
        "Description": "<P><code>MINU</code> computes the minimum of the unsigned contents of address registers <code>as</code> and <code>at</code>, and writes the result to address register <code>ar</code>.</P>",
        "Operation": "<pre>AR[r] &#8592; if (0||AR[s]) &lt; (0||AR[t]) then AR[s] else AR[t]</pre>"
    },
    "MOV": {
        "name": "MOV",
        "syn": "Move",
        "Assembler Syntax": "MOV ar, as",
        "Description": "<P><code>MOV</code> is an assembler macro that uses the <code>OR</code> instruction  OR ar, as, at to move the contents of address register <code>as</code> to address register <code>ar</code>. The assembler input</P><P>&nbsp;</P><P><code>\tMOV\tar, as</code></P><P>expands into</P><P>&nbsp;</P><P><code>\tOR \tar, as, as</code></P>",
        "Operation": "<pre>AR[r] &#8592; AR[s]</pre>"
    },
    "MOV.N": {
        "name": "MOV.N",
        "syn": "Narrow Move",
        "Assembler Syntax": "MOV.N at, as",
        "Description": "<P><code>MOV.N</code> is similar in function to the assembler macro <code>MOV</code>, but has a 16-bit encoding. <code>MOV.N</code> moves the contents of address register <code>as</code> to address register <code>at</code>.</P>",
        "Operation": "<pre>AR[t] &#8592; AR[s]</pre>"
    },
    "MOVEQZ": {
        "name": "MOVEQZ",
        "syn": "Move if Equal to Zero",
        "Assembler Syntax": "MOVEQZ ar, as, at",
        "Description": "<P><code>MOVEQZ</code> performs a conditional move if equal to zero. If the contents of address register <code>at</code> are zero, then the processor sets address register <code>ar</code> to the contents of address register <code>as</code>. Otherwise, <code>MOVEQZ</code> performs no operation and leaves address register <code>ar</code> unchanged.</P><P>The inverse of <code>MOVEQZ</code> is <code>MOVNEZ</code>.</P>",
        "Operation": "<pre>if AR[t] = 0<SUP>32</SUP> then\n\tAR[r] &#8592; AR[s]\nendif</pre>"
    },
    "MOVEQZ.D": {
        "name": "MOVEQZ.D",
        "syn": "Move Double if Equal to Zero",
        "Assembler Syntax": "MOVEQZ.D fr, fs, at",
        "Description": "<P>MOVEQZ.D is an assembler macro that uses the MOVEQZ.S instruction  MOVEQZ.S fr, fs, at to move the contents of floating-point register <code>fs</code> to floating-point register <code>fr</code>, if address register <code>at</code> contains zero. The assembler input</P><P>&nbsp;</P><P><code>\t</code>MOVEQZ.D fr, fs, at</P><P>expands into</P><P>&nbsp;</P><P><code>\t</code>MOVEQZ.S fr, fs, at</P><P><code>MOVEQZ.D</code> is non-arithmetic; no floating-point exceptions are raised.</P><P>The inverse of <code>MOVEQZ.D</code> is <code>MOVNEZ.D</code>.</P>",
        "Operation": "<pre>if AR[t] = 0<SUP>32</SUP> then\n\tFR[r] &#8592; FR[s]\nendif</pre>"
    },
    "MOVF": {
        "name": "MOVF",
        "syn": "Move if False",
        "Assembler Syntax": "MOVF ar, as, bt",
        "Description": "<P><code>MOVF</code> moves the contents of address register <code>as</code> to address register <code>ar</code> if Boolean register <code>bt</code> is false. Address register <code>ar</code> is left unchanged if Boolean register <code>bt</code> is true.</P><P>The inverse of <code>MOVF</code> is <code>MOVT</code>.</P>",
        "Operation": "<pre>if not BR<SUB>t</SUB> then\n\tAR[r] &#8592; AR[s]\nendif</pre>"
    },
    "MOVF.D": {
        "name": "MOVF.D",
        "syn": "Move Double if False",
        "Assembler Syntax": "MOVF.D fr, fs, bt",
        "Description": "<P>MOVF.D is an assembler macro that uses the MOVF.S instruction  MOVF.S fr, fs, bt to move the contents of floating-point register <code>fs</code> to floating-point register <code>fr</code>, if Boolean register <code>bt</code> contains zero. The assembler input</P><P>&nbsp;</P><P><code>\t</code>MOVF.D fr, fs, bt</P><P>expands into</P><P>&nbsp;</P><P><code>\t</code>MOVF.S fr, fs, bt</P><P><code>MOVF.D</code> is non-arithmetic; no floating-point exceptions are raised.</P><P>The inverse of <code>MOVF.D</code> is <code>MOVT.D</code>.</P>",
        "Operation": "<pre>if not BR<SUB>t</SUB> then\n\tFR[r] &#8592; FR[s]\nendif</pre>"
    },
    "MOVGEZ": {
        "name": "MOVGEZ",
        "syn": "Move if Greater Than or Equal to Zero",
        "Assembler Syntax": "MOVGEZ ar, as, at",
        "Description": "<P><code>MOVGEZ</code> performs a conditional move if greater than or equal to zero. If the contents of address register <code>at</code> are greater than or equal to zero (that is, the most significant bit is clear), then the processor sets address register <code>ar</code> to the contents of address register <code>as</code>. Otherwise, <code>MOVGEZ</code> performs no operation and leaves address register <code>ar</code> unchanged.</P><P>The inverse of <code>MOVGEZ</code> is <code>MOVLTZ</code>.</P>",
        "Operation": "<pre>if AR[t]<SUB>31</SUB> = 0 then\n\tAR[r] &#8592; AR[s]\nendif</pre>"
    },
    "MOVGEZ.D": {
        "name": "MOVGEZ.D",
        "syn": "Move Double if Greater Than or Equal Zero",
        "Assembler Syntax": "MOVGEZ.D fr, fs, at",
        "Description": "<P>MOVGEZ.D is an assembler macro that uses the MOVGEZ.S instruction  MOVGEZ.S fr, fs, at to move the contents of floating-point register <code>fs</code> to floating-point register <code>fr</code>, if address register <code>at</code> is greater than or equal to zero (that is, the most significant bit is clear). The assembler input</P><P>&nbsp;</P><P><code>\t</code>MOVGEZ.D fr, fs, at</P><P>expands into</P><P>&nbsp;</P><P><code>\t</code>MOVGEZ.S fr, fs, at</P><P><code>MOVGEZ.D</code> is non-arithmetic; no floating-point exceptions are raised.</P><P>The inverse of <code>MOVGEZ.D</code> is <code>MOVLTZ.D</code>.</P>",
        "Operation": "<pre>if AR[t]<SUB>31</SUB> = 0 then\n\tFR[r] &#8592; FR[s]\nendif</pre>"
    },
    "MOVI": {
        "name": "MOVI",
        "syn": "Move Immediate",
        "Assembler Syntax": "MOVI at, -2048..2047",
        "Description": "<P><code>MOVI</code> sets address register <code>at</code> to a constant in the range <code>-2048..2047</code> encoded in the instruction word. The constant is stored in two non-contiguous fields of the instruction word. The processor decodes the constant specification by concatenating the two fields and sign-extending the 12-bit value.</P>",
        "Operation": "<pre>AR[t] &#8592; imm12<SUB>11</SUB><SUP>20</SUP>||imm12</pre>"
    },
    "MOVI.N": {
        "name": "MOVI.N",
        "syn": "Narrow Move Immediate",
        "Assembler Syntax": "MOVI.N as, -32..95",
        "Description": "<P><code>MOVI.N</code> is similar to <code>MOVI</code>, but has a 16-bit encoding and supports a smaller range of constant values encoded in the instruction word.</P><P><code>MOVI.N</code> sets address register <code>as</code> to a constant in the range <code>-32..95</code> encoded in the instruction word. The constant is stored in two non-contiguous fields of the instruction word. The range is asymmetric around zero because positive constants are more frequent than negative constants. The processor decodes the constant specification by concatenating the two fields and sign-extending the 7-bit value with the logical and of its two most significant bits.</P>",
        "Operation": "<pre>AR[s] &#8592; (imm7<SUB>6</SUB> and imm7<SUB>5</SUB>)<SUP>25</SUP>||imm7</pre>"
    },
    "MOVLTZ": {
        "name": "MOVLTZ",
        "syn": "Move if Less Than Zero",
        "Assembler Syntax": "MOVLTZ ar, as, at",
        "Description": "<P><code>MOVLTZ</code> performs a conditional move if less than zero. If the contents of address register <code>at</code> are less than zero (that is, the most significant bit is set), then the processor sets address register <code>ar</code> to the contents of address register <code>as</code>. Otherwise, <code>MOVLTZ</code> performs no operation and leaves address register <code>ar</code> unchanged.</P><P>The inverse of <code>MOVLTZ</code> is <code>MOVGEZ</code>.</P>",
        "Operation": "<pre>if AR[t]<SUB>31</SUB> != 0 then\n\tAR[r] &#8592; AR[s]\nendif</pre>"
    },
    "MOVLTZ.D": {
        "name": "MOVLTZ.D",
        "syn": "Move Doubgle if Less Than Zero",
        "Assembler Syntax": "MOVLTZ.D fr, fs, at",
        "Description": "<P>MOVLTZ.D is an assembler macro that uses the MOVLTZ.S instruction  MOVLTZ.S fr, fs, at to move the contents of floating-point register <code>fs</code> to floating-point register <code>fr</code>, if address register <code>at</code> is less than zero (that is, the most significant bit is set). The assembler input</P><P>&nbsp;</P><P><code>\t</code>MOVLTZ.D fr, fs, at</P><P>expands into</P><P>&nbsp;</P><P><code>\t</code>MOVLTZ.S fr, fs, at</P><P><code>MOVLTZ.D</code> is non-arithmetic; no floating-point exceptions are raised.</P><P>The inverse of <code>MOVLTZ.D</code> is <code>MOVGEZ.D</code>.</P>",
        "Operation": "<pre>if AR[t]<SUB>31</SUB> != 0 then\n\tFR[r] &#8592; FR[s]\nendif</pre>"
    },
    "MOVNEZ": {
        "name": "MOVNEZ",
        "syn": "Move if Not-Equal to Zero",
        "Assembler Syntax": "MOVNEZ ar, as, at",
        "Description": "<P><code>MOVNEZ</code> performs a conditional move if not equal to zero. If the contents of address register <code>at</code> are non-zero, then the processor sets address register <code>ar</code> to the contents of address register <code>as</code>. Otherwise, <code>MOVNEZ</code> performs no operation and leaves address register <code>ar</code> unchanged.</P><P>The inverse of <code>MOVNEZ</code> is <code>MOVEQZ</code>.</P>",
        "Operation": "<pre>if AR[t] != 0<SUP>32</SUP> then\n\tAR[r] &#8592; AR[s]\nendif</pre>"
    },
    "MOVNEZ.D": {
        "name": "MOVNEZ.D",
        "syn": "Move Double if Not Equal to Zero",
        "Assembler Syntax": "MOVNEZ.D fr, fs, at",
        "Description": "<P>MOVNEZ.D is an assembler macro that uses the MOVNEZ.S instruction  MOVNEZ.S fr, fs, at to move the contents of floating-point register <code>fs</code> to floating-point register <code>fr</code>, if the contents of address register <code>at</code> is non-zero. The assembler input</P><P>&nbsp;</P><P><code>\t</code>MOVNEZ.D fr, fs, at</P><P>expands into</P><P>&nbsp;</P><P><code>\t</code>MOVNEZ.S fr, fs, at</P><P><code>MOVNEZ.D</code> is non-arithmetic; no floating-point exceptions are raised.</P><P>The inverse of <code>MOVNEZ.D</code> is <code>MOVEQZ.D</code>.</P>",
        "Operation": "<pre>if AR[t] != 0<SUP>32</SUP> then\n\tFR[r] &#8592; FR[s]\nendif</pre>"
    },
    "MOVSP": {
        "name": "MOVSP",
        "syn": "Move to Stack Pointer",
        "Assembler Syntax": "MOVSP at, as",
        "Description": "<P><code>MOVSP</code> provides an atomic window check and register-to-register move. If the caller's registers are present in the register file, this instruction simply moves the contents of address register <code>as</code> to address register <code>at</code>. If the caller's registers are not present, <code>MOVSP</code> raises an Alloca exception.</P><P><code>MOVSP</code> is typically used to perform variable-size stack frame allocation. The Xtensa Windowed Register ABI specifies that the caller's <code>a0</code>-<code>a3</code> may be stored just below the callee's stack pointer. When the stack frame is extended, these values may need to be moved. They can only be moved with interrupts and exceptions disabled. This instruction provides a mechanism to test if they must be moved, and if so, to raise an exception to move the data with interrupts and exceptions disabled. The Xtensa ABI also requires that the caller's return address be in <code>a0</code> when <code>MOVSP</code> is executed.</P>",
        "Operation": "<pre>if WindowStart<SUB>WindowBase-0011..WindowBase-0001</SUB> = 03 then\n\tException (AllocaCause)\nelse\n\tAR[t] &#8592; AR[s]\nendif</pre>"
    },
    "MOVT": {
        "name": "MOVT",
        "syn": "Move if True",
        "Assembler Syntax": "MOVT ar, as, bt",
        "Description": "<P><code>MOVT</code> moves the contents of address register <code>as</code> to address register <code>ar</code> if Boolean register <code>bt</code> is true. Address register <code>ar</code> is left unchanged if Boolean register <code>bt</code> is false.</P><P>The inverse of <code>MOVT</code> is <code>MOVF</code>.</P>",
        "Operation": "<pre>if BR<SUB>t</SUB> then\n\tAR[r] &#8592; AR[s]\nendif</pre>"
    },
    "MOVT.D": {
        "name": "MOVT.D",
        "syn": "Move Double if True",
        "Assembler Syntax": "MOVT.D fr, fs, bt",
        "Description": "<P>MOVT.D is an assembler macro that uses the MOVT.S instruction  MOVT.S fr, fs, bt to move the contents of floating-point register <code>fs</code> to floating-point register <code>fr</code>, if Boolean register <code>bt</code> is set. The assembler input</P><P>&nbsp;</P><P><code>\t</code>MOVT.D fr, fs, bt</P><P>expands into</P><P>&nbsp;</P><P><code>\t</code>MOVT.S fr, fs, bt</P><P><code>MOVT.D</code> is non-arithmetic; no floating-point exceptions are raised.</P><P>The inverse of <code>MOVT.D</code> is <code>MOVF.D</code>.</P>",
        "Operation": "<pre>if BR<SUB>t</SUB> then\n\tFR[r] &#8592; FR[s]\nendif</pre>"
    },
    "MUL.AA.*": {
        "name": "MUL.AA.*",
        "syn": "Signed Multiply",
        "Assembler Syntax": "MUL.AA.* as, at\tWhere * expands as follows:<P>MUL.AA.LL - for (half=0)</P><P>MUL.AA.HL - for (half=1)</P><P>MUL.AA.LH - for (half=2)</P><P>MUL.AA.<code>HH - for (half=3)</code></P>",
        "Description": "<P><code>MUL.AA.*</code> performs a two's complement multiply of half of each of the address registers <code>as</code> and <code>at</code>, producing a 32-bit result. The result is sign-extended to 40 bits and written to the MAC16 accumulator.</P>",
        "Operation": "<pre>m1 &#8592; if half<SUB>0</SUB> then AR[s]<SUB>31..16</SUB> else AR[s]<SUB>15..0</SUB>\nm2 &#8592; if half<SUB>1</SUB> then AR[t]<SUB>31..16</SUB> else AR[t]<SUB>15..0</SUB>\nACC &#8592; (m1<SUB>15</SUB><SUP>24</SUP>||m1) * (m2<SUB>15</SUB><SUP>24</SUP>||m2)</pre>"
    },
    "MUL.AD.*": {
        "name": "MUL.AD.*",
        "syn": "Signed Multiply",
        "Assembler Syntax": "MUL.AD.* as, my\tWhere * expands as follows:<P>MUL.AD.<code>LL - for (half=0)</code></P><P>MUL.AD.<code>HL - for (half=1)</code></P><P>MUL.AD.<code>LH - for (half=2)</code></P><P>MUL.AD.<code>HH - for (half=3)</code></P>",
        "Description": "<P><code>MUL.AD.*</code> performs a two's complement multiply of half of address register <code>as</code> and half of MAC16 register <code>my</code>, producing a 32-bit result. The result is sign-extended to 40 bits and written to the MAC16 accumulator. The <code>my</code> operand can designate either MAC16 register <code>m2</code> or <code>m3</code>.</P>",
        "Operation": "<pre>m1 &#8592; if half<SUB>0</SUB> then AR[s]<SUB>31..16</SUB> else AR[s]<SUB>15..0</SUB>\nm2 &#8592; if half<SUB>1</SUB> then MR[1||y]<SUB>31..16</SUB> else MR[1||y]<SUB>15..0</SUB>\nACC &#8592; (m1<SUB>15</SUB><SUP>24</SUP>||m1) * (m2<SUB>15</SUB><SUP>24</SUP>||m2)</pre>"
    },
    "MUL.DA.*": {
        "name": "MUL.DA.*",
        "syn": "Signed Multiply",
        "Assembler Syntax": "MUL.DA.* mx, at\tWhere * expands as follows:<P>MUL.DA.LL - <code>for (half=0)</code></P><P>MUL.DA.HL - <code>for (half=1)</code></P><P>MUL.DA.LH - <code>for (half=2)</code></P><P>MUL.DA.HH - <code>for (half=3)</code></P>",
        "Description": "<P><code>MUL.DA.*</code> performs a two's complement multiply of half of MAC16 register <code>mx</code> and half of address register <code>at</code>, producing a 32-bit result. The result is sign-extended to 40 bits and written to the MAC16 accumulator. The <code>mx</code> operand can designate either MAC16 register <code>m0</code> or <code>m1</code>.</P>",
        "Operation": "<pre>m1 &#8592; if half<SUB>0</SUB> then MR[0||x]<SUB>31..16</SUB> else MR[0||x]<SUB>15..0</SUB>\nm2 &#8592; if half<SUB>1</SUB> then AR[t]<SUB>31..16</SUB> else AR[t]<SUB>15..0</SUB>\nACC &#8592; (m1<SUB>15</SUB><SUP>24</SUP>||m1) * (m2<SUB>15</SUB><SUP>24</SUP>||m2)</pre>"
    },
    "MUL.DD.*": {
        "name": "MUL.DD.*",
        "syn": "Signed Multiply",
        "Assembler Syntax": "MUL.DD.* mx, my\tWhere * expands as follows:<P>MUL.DD.LL - <code>for (half=0)</code></P><P>MUL.DD.HL - <code>for (half=1)</code> </P><P>MUL.DD.LH - <code>for (half=2)</code> </P><P>MUL.DD.HH - <code>for (half=3)</code> </P>",
        "Description": "<P><code>MUL.DD.*</code> performs a two's complement multiply of half of the MAC16 registers <code>mx</code> and <code>my</code>, producing a 32-bit result. The result is sign-extended to 40 bits and written to the MAC16 accumulator. The <code>mx</code> operand can designate either MAC16 register <code>m0</code> or <code>m1</code>. The <code>my</code> operand can designate either MAC16 register <code>m2</code> or <code>m3</code>.</P>",
        "Operation": "<pre>m1 &#8592; if half<SUB>0</SUB> then MR[0||x]<SUB>31..16</SUB> else MR[0||x]<SUB>15..0</SUB>\nm2 &#8592; if half<SUB>1</SUB> then MR[1||y]<SUB>31..16</SUB> else MR[1||y]<SUB>15..0</SUB>\nACC &#8592; (m1<SUB>15</SUB><SUP>24</SUP>||m1) * (m2<SUB>15</SUB><SUP>24</SUP>||m2)</pre>"
    },
    "MUL16S": {
        "name": "MUL16S",
        "syn": "Multiply 16-bit Signed",
        "Assembler Syntax": "MUL16S ar, as, at",
        "Description": "<P><code>MUL16S</code> performs a two's complement multiplication of the least-significant 16 bits of the contents of address registers <code>as</code> and <code>at</code> and writes the 32-bit product to address register <code>ar</code>.</P>",
        "Operation": "<pre>AR[r] &#8592; (AR[s]<SUB>15</SUB><SUP>16</SUP>||AR[s]<SUB>15..0</SUB>) * (AR[t]<SUB>15</SUB><SUP>16</SUP>||AR[t]<SUB>15..0</SUB>)</pre>"
    },
    "MUL16U": {
        "name": "MUL16U",
        "syn": "Multiply 16-bit Unsigned",
        "Assembler Syntax": "MUL16U ar, as, at",
        "Description": "<P><code>MUL16U</code> performs an unsigned multiplication of the least-significant 16 bits of the contents of address registers <code>as</code> and <code>at</code> and writes the 32-bit product to address register <code>ar</code>.</P>",
        "Operation": "<pre>AR[r] &#8592; (0<SUP>16</SUP>||AR[s]<SUB>15..0</SUB>) * (0<SUP>16</SUP>||AR[t]<SUB>15..0</SUB>)</pre>"
    },
    "MULA.AA.*": {
        "name": "MULA.AA.*",
        "syn": "Signed Multiply/Accumulate",
        "Assembler Syntax": "MULA.AA.* as, at\tWhere * expands as follows:<P>MULA.AA.LL - <code>for (half=0)</code> </P><P>MULA.AA.HL - <code>for (half=1)</code> </P><P>MULA.AA.LH - <code>for (half=2)</code> </P><P>MULA.AA.HH - <code>for (half=3)</code> </P>",
        "Description": "<P><code>MULA.AA.*</code> performs a two's complement multiply of half of each of the address registers <code>as</code> and <code>at</code>, producing a 32-bit result. The result is sign-extended to 40 bits and added to the contents of the MAC16 accumulator.</P>",
        "Operation": "<pre>m1 &#8592; if half<SUB>0</SUB> then AR[s]<SUB>31..16</SUB> else AR[s]<SUB>15..0</SUB>\nm2 &#8592; if half<SUB>1</SUB> then AR[t]<SUB>31..16</SUB> else AR[t]<SUB>15..0</SUB>\nACC &#8592; ACC + (m1<SUB>15</SUB><SUP>24</SUP>||m1) * (m2<SUB>15</SUB><SUP>24</SUP>||m2)</pre>"
    },
    "MULA.AD.*": {
        "name": "MULA.AD.*",
        "syn": "Signed Multiply/Accumulate",
        "Assembler Syntax": "MULA.AD.* as, my\tWhere * expands as follows:<P>MULA.AD.LL - <code>for (half=0)</code> </P><P>MULA.AD.HL - <code>for (half=1)</code> </P><P>MULA.AD.LH - <code>for (half=2)</code> </P><P>MULA.AD.HH - <code>for (half=3)</code> </P>",
        "Description": "<P><code>MULA.AD.*</code> performs a two's complement multiply of half of address register <code>as</code> and half of MAC16 register <code>my</code>, producing a 32-bit result. The result is sign-extended to 40 bits and added to the contents of the MAC16 accumulator. The <code>my</code> operand can designate either MAC16 register <code>m2</code> or <code>m3</code>.</P>",
        "Operation": "<pre>m1 &#8592; if half<SUB>0</SUB> then AR[s]<SUB>31..16</SUB> else AR[s]<SUB>15..0</SUB>\nm2 &#8592; if half<SUB>1</SUB> then MR[1||y]<SUB>31..16</SUB> else MR[1||y]<SUB>15..0</SUB>\nACC &#8592; ACC + (m1<SUB>15</SUB><SUP>24</SUP>||m1) * (m2<SUB>15</SUB><SUP>24</SUP>||m2)</pre>"
    },
    "MULA.DA.*": {
        "name": "MULA.DA.*",
        "syn": "Signed Multiply/Accumulate",
        "Assembler Syntax": "MULA.DA.* mx, at\tWhere * expands as follows:<P>MULA.DA.LL - <code>for (half=0)</code> </P><P>MULA.DA.HL - <code>for (half=1)</code> </P><P>MULA.DA.LH - <code>for (half=2)</code> </P><P>MULA.DA.HH - <code>for (half=3)</code> </P>",
        "Description": "<P><code>MULA.DA.*</code> performs a two's complement multiply of half of MAC16 register <code>mx</code> and half of address register <code>at</code>, producing a 32-bit result. The result is sign-extended to 40 bits and added to the contents of the MAC16 accumulator. The <code>mx</code> operand can designate either MAC16 register <code>m0</code> or <code>m1</code>.</P>",
        "Operation": "<pre>m1 &#8592; if half<SUB>0</SUB> then MR[0||x]<SUB>31..16</SUB> else MR[0||x]<SUB>15..0</SUB>\nm2 &#8592; if half<SUB>1</SUB> then AR[t]<SUB>31..16</SUB> else AR[t]<SUB>15..0</SUB>\nACC &#8592; ACC + (m1<SUB>15</SUB><SUP>24</SUP>||m1) * (m2<SUB>15</SUB><SUP>24</SUP>||m2)</pre>"
    },
    "MULA.DA.*.LDDEC": {
        "name": "MULA.DA.*.LDDEC",
        "syn": "Signed Multiply/Accumulate, Load with Autodecrement",
        "Assembler Syntax": "MULA.DA.*.LDDEC mw, as, mx, at\tWhere * expands as follows:<P>MULA.DA.LL.LDDEC - <code>for (half=0)</code> </P><P>MULA.DA.HL.LDDEC - <code>for (half=1)</code> </P><P>MULA.DA.LH.LDDEC - <code>for (half=2)</code> </P><P>MULA.DA.HH.LDDEC - <code>for (half=3)</code> </P>",
        "Description": "<P>MULA.DA.*.LDDEC performs a parallel load and multiply/accumulate.</P><P>First, it performs a two's complement multiply of half of MAC16 register <code>mx</code> and half of address register <code>at</code>, producing a 32-bit result. The result is sign-extended to 40 bits and added to the contents of the MAC16 accumulator. The <code>mx</code> operand can designate either MAC16 register <code>m0</code> or <code>m1</code>.</P><P>Next, it loads MAC16 register <code>mw</code> from memory using auto-decrement addressing. It forms a virtual address by subtracting 4 from the contents of address register <code>as</code>. Thirty-two bits (four bytes) are read from the physical address. This data is then written to MAC16 register <code>mw</code>, and the virtual address is written back to address register <code>as</code>. The <code>mw</code> operand can designate any of the four MAC16 registers.</P><P>If the Region Translation Option  or the MMU Option  is enabled, the virtual address is translated to the physical address. If not, the physical address is identical to the virtual address. If the translation or memory reference encounters an error (for example, protection violation or non-existent memory), the processor raises one of several exceptions .</P><P>Without the Unaligned Exception Option , the two least significant bits of the address are ignored. A reference to an address that is not 0 mod 4 produces the same result as a reference to the address with the least significant bits cleared. With the Unaligned Exception Option, such an access raises an exception.</P><P>The MAC16 register source <code>mx</code> and the MAC16 register destination <code>mw</code> may be the same. In this case, the instruction uses the contents of <code>mx</code> as the source operand prior to loading <code>mx</code> with the load data.</P>",
        "Operation": "<pre>vAddr &#8592; AR[s] &#8722; 4\n(mem32, error) &#8592; Load32(vAddr)\nif error then\n\tEXCVADDR &#8592; vAddr\n\tException (LoadStoreErrorCause)\nelse\n\tm1 &#8592; if half<SUB>0</SUB> then MR[0||x]<SUB>31..16</SUB> else MR[0||x]<SUB>15..0</SUB>\n\tm2 &#8592; if half<SUB>1</SUB> then AR[t]<SUB>31..16</SUB> else AR[t]<SUB>15..0</SUB>\n\tACC &#8592; ACC + (m1<SUB>15</SUB><SUP>24</SUP>||m1) * (m2<SUB>15</SUB><SUP>24</SUP>||m2)\n\tAR[s] &#8592; vAddr\n\tMR[w] &#8592; mem32\nendif\n&nbsp;</pre>"
    },
    "MULA.DA.*.LDINC": {
        "name": "MULA.DA.*.LDINC",
        "syn": "Signed Multiply/Accumulate, Load with Autoincrement",
        "Assembler Syntax": "MULA.DA.*.LDINC mw, as, mx, at\tWhere * expands as follows:<P>MULA.DA.LL.LDINC - <code>for (half=0)</code> </P><P>MULA.DA.HL.LDINC - <code>for (half=1)</code> </P><P>MULA.DA.LH.LDINC - <code>for (half=2)</code> </P><P>MULA.DA.HH.LDINC - <code>for (half=3)</code> </P>",
        "Description": "<P>MULA.DA.*.LDINC performs a parallel load and multiply/accumulate.</P><P>First, it performs a two's complement multiply of half of MAC16 register <code>mx</code> and half of address register <code>at</code>, producing a 32-bit result. The result is sign-extended to 40 bits and added to the contents of the MAC16 accumulator. The <code>mx</code> operand can designate either MAC16 register <code>m0</code> or <code>m1</code>.</P><P>Next, it loads MAC16 register <code>mw</code> from memory using auto-increment addressing. It forms a virtual address by adding 4 to the contents of address register <code>as</code>. 32 bits (four bytes) are read from the physical address. This data is then written to MAC16 register <code>mw</code>, and the virtual address is written back to address register <code>as</code>. The <code>mw</code> operand can designate any of the four MAC16 registers.</P><P>If the Region Translation Option  or the MMU Option is enabled, the virtual address is translated to the physical address. If not, the physical address is identical to the virtual address. If the translation or memory reference encounters an error (for example, protection violation or non-existent memory), the processor raises one of several exceptions .</P><P>Without the Unaligned Exception Option , the two least significant bits of the address are ignored. A reference to an address that is not 0 mod 4 produces the same result as a reference to the address with the least significant bits cleared. With the Unaligned Exception Option, such an access raises an exception.</P><P>The MAC16 register source <code>mx</code> and the MAC16 register destination <code>mw</code> may be the same. In this case, the instruction uses the contents of <code>mx</code> as the source operand prior to loading <code>mx</code> with the load data.</P>",
        "Operation": "<pre>vAddr &#8592; AR[s] + 4\n(mem32, error) &#8592; Load32(vAddr)\nif error then\n\tEXCVADDR &#8592; vAddr\n\tException (LoadStoreErrorCause)\nelse\n\tm1 &#8592; if half<SUB>0</SUB> then MR[0||x]<SUB>31..16</SUB> else MR[0||x]<SUB>15..0</SUB>\n\tm2 &#8592; if half<SUB>1</SUB> then AR[t]<SUB>31..16</SUB> else AR[t]<SUB>15..0</SUB>\n\tACC &#8592; ACC + (m1<SUB>15</SUB><SUP>24</SUP>||m1) * (m2<SUB>15</SUB><SUP>24</SUP>||m2)\n\tAR[s] &#8592; vAddr\n\tMR[w] &#8592; mem32\nendif\n&nbsp;</pre>"
    },
    "MULA.DD.*": {
        "name": "MULA.DD.*",
        "syn": "Signed Multiply/Accumulate",
        "Assembler Syntax": "MULA.DD.* mx, my\tWhere * expands as follows:<P>MULA.DD.LL - <code>for (half=0)</code> </P><P>MULA.DD.HL - <code>for (half=1)</code> </P><P>MULA.DD.LH - <code>for (half=2)</code> </P><P>MULA.DD.HH - <code>for (half=3)</code> </P>",
        "Description": "<P><code>MULA.DD.*</code> performs a two's complement multiply of half of each of the MAC16 registers <code>mx</code> and <code>my</code>, producing a 32-bit result. The result is sign-extended to 40 bits and added to the contents of the MAC16 accumulator. The <code>mx</code> operand can designate either MAC16 register <code>m0</code> or <code>m1</code>. The <code>my</code> operand can designate either MAC16 register <code>m2</code> or <code>m3</code>.</P>",
        "Operation": "<pre>m1 &#8592; if half<SUB>0</SUB> then MR[0||x]<SUB>31..16</SUB> else MR[0||x]<SUB>15..0</SUB>\nm2 &#8592; if half<SUB>1</SUB> then MR[1||y]<SUB>31..16</SUB> else MR[1||y]<SUB>15..0</SUB>\nACC &#8592; ACC + (m1<SUB>15</SUB><SUP>24</SUP>||m1) * (m2<SUB>15</SUB><SUP>24</SUP>||m2)</pre>"
    },
    "MULA.DD.*.LDDEC": {
        "name": "MULA.DD.*.LDDEC",
        "syn": "Signed Multiply/Accumulate, Load with Autodecrement",
        "Assembler Syntax": "MULA.DD.*.LDDEC mw, as, mx, my\tWhere * expands as follows:<P>MULA.DD.LL.LDDEC - <code>for (half=0)</code> </P><P>MULA.DD.HL.LDDEC - <code>for (half=1)</code> </P><P>MULA.DD.LH.LDDEC - <code>for (half=2)</code> </P><P>MULA.DD.HH.LDDEC - <code>for (half=3)</code> </P>",
        "Description": "<P>MULA.DD.*.LDDEC performs a parallel load and multiply/accumulate.</P><P>First, it performs a two's complement multiply of half of the MAC16 registers <code>mx</code> and <code>my</code>, producing a 32-bit result. The result is sign-extended to 40 bits and added to the contents of the MAC16 accumulator. The <code>mx</code> operand can designate either MAC16 register <code>m0</code> or <code>m1</code>. The <code>my</code> operand can designate either MAC16 register <code>m2</code> or <code>m3</code>.</P><P>Next, it loads MAC16 register <code>mw</code> from memory using auto-decrement addressing. It forms a virtual address by subtracting 4 from the contents of address register <code>as</code>. Thirty-two bits (four bytes) are read from the physical address. This data is then written to MAC16 register <code>mw</code>, and the virtual address is written back to address register <code>as</code>. The <code>mw</code> operand can designate any of the four MAC16 registers.</P><P>If the Region Translation Option  or the MMU Option  is enabled, the virtual address is translated to the physical address. If not, the physical address is identical to the virtual address. If the translation or memory reference encounters an error (for example, protection violation or non-existent memory), the processor raises one of several exceptions .</P><P>Without the Unaligned Exception Option , the two least significant bits of the address are ignored. A reference to an address that is not 0 mod 4 produces the same result as a reference to the address with the least significant bits cleared. With the Unaligned Exception Option, such an access raises an exception.</P><P>The MAC16 register destination <code>mw</code> may be the same as either MAC16 register source <code>mx</code> or <code>my</code>. In this case, the instruction uses the contents of <code>mx</code> and <code>my</code> as the source operands prior to loading <code>mw</code> with the load data.</P>",
        "Operation": "<pre>vAddr &#8592; AR[s] &#8722; 4\n(mem32, error) &#8592; Load32(vAddr)\nif error then\n\tEXCVADDR &#8592; vAddr\n\tException (LoadStoreErrorCause)\nelse\n\tm1 &#8592; if half<SUB>0</SUB> then MR[0||x]<SUB>31..16</SUB> else MR[0||x]<SUB>15..0</SUB>\n\tm2 &#8592; if half<SUB>1</SUB> then MR[1||y]<SUB>31..16</SUB> else MR[1||y]<SUB>15..0</SUB>\n\tACC &#8592; ACC + (m1<SUB>15</SUB><SUP>24</SUP>||m1) * (m2<SUB>15</SUB><SUP>24</SUP>||m2)\n\tAR[s] &#8592; vAddr\n\tMR[w] &#8592; mem32\nendif\n&nbsp;</pre>"
    },
    "MULA.DD.*.LDINC": {
        "name": "MULA.DD.*.LDINC",
        "syn": "Signed Multiply/Accumulate, Load with Autoincrement",
        "Assembler Syntax": "MULA.DD.*.LDINC mw, as, mx, my\tWhere * expands as follows:<P>MULA.DD.LL.LDINC - <code>for (half=0)</code> </P><P>MULA.DD.HL.LDINC - <code>for (half=1)</code> </P><P>MULA.DD.LH.LDINC - <code>for (half=2)</code> </P><P>MULA.DD.HH.LDINC - <code>for (half=3)</code> </P>",
        "Description": "<P>MULA.DD.*.LDINC performs a parallel load and multiply/accumulate.</P><P>First, it performs a two's complement multiply of half of each of the MAC16 registers <code>mx</code> and <code>my</code>, producing a 32-bit result. The result is sign-extended to 40 bits and added to the contents of the MAC16 accumulator. The <code>mx</code> operand can designate either MAC16 register <code>m0</code> or <code>m1</code>. The <code>my</code> operand can designate either MAC16 register <code>m2</code> or <code>m3</code>.</P><P>Next, it loads MAC16 register <code>mw</code> from memory using auto-increment addressing. It forms a virtual address by adding 4 to the contents of address register <code>as</code>. Thirty-two bits (four bytes) are read from the physical address. This data is then written to MAC16 register <code>mw</code>, and the virtual address is written back to address register <code>as</code>. The <code>mw</code> operand can designate any of the four MAC16 registers.</P><P>If the Region Translation Option  or the MMU Option  is enabled, the virtual address is translated to the physical address. If not, the physical address is identical to the virtual address. If the translation or memory reference encounters an error (for example, protection violation or non-existent memory), the processor raises one of several exceptions .</P><P>Without the Unaligned Exception Option , the two least significant bits of the address are ignored. A reference to an address that is not 0 mod 4 produces the same result as a reference to the address with the least significant bits cleared. With the Unaligned Exception Option, such an access raises an exception.</P><P>The MAC16 register destination <code>mw</code> may be the same as either MAC16 register source <code>mx</code> or <code>my</code>. In this case, the instruction uses the contents of <code>mx</code> and <code>my</code> as the source operands prior to loading <code>mw</code> with the load data.</P>",
        "Operation": "<pre>vAddr &#8592; AR[s] + 4\n(mem32, error) &#8592; Load32(vAddr)\nif error then\n\tEXCVADDR &#8592; vAddr\n\tException (LoadStoreErrorCause)\nelse\n\tm1 &#8592; if half<SUB>0</SUB> then MR[0||x]<SUB>31..16</SUB> else MR[0||x]<SUB>15..0</SUB>\n\tm2 &#8592; if half<SUB>1</SUB> then MR[1||y]<SUB>31..16</SUB> else MR[1||y]<SUB>15..0</SUB>\n\tACC &#8592; ACC + (m1<SUB>15</SUB><SUP>24</SUP>||m1) * (m2<SUB>15</SUB><SUP>24</SUP>||m2)\n\tAR[s] &#8592; vAddr\n\tMR[w] &#8592; mem32\nendif\n&nbsp;</pre>"
    },
    "MULL": {
        "name": "MULL",
        "syn": "Multiply Low",
        "Assembler Syntax": "MULL ar, as, at",
        "Description": "<P><code>MULL</code> performs a 32-bit multiplication of the contents of address registers <code>as</code> and <code>at</code>, and writes the least significant 32 bits of the product to address register <code>ar</code>. Because the least significant product bits are unaffected by the multiplicand and multiplier sign, MULL is useful for both signed and unsigned multiplication.</P>",
        "Operation": "<pre>AR[r] &#8592; AR[s] * AR[t]</pre>"
    },
    "MULS.AA.*": {
        "name": "MULS.AA.*",
        "syn": "Signed Multiply/Subtract",
        "Assembler Syntax": "MULS.AA.* as, at\tWhere * expands as follows:<P>MULS.AA.LL - <code>for (half=0)</code> </P><P>MULS.AA.HL - <code>for (half=1)</code> </P><P>MULS.AA.LH - <code>for (half=2)</code> </P><P>MULS.AA.HH - <code>for (half=3)</code> </P>",
        "Description": "<P><code>MULS.AA.*</code> performs a two's complement multiply of half of each of the address registers <code>as</code> and <code>at</code>, producing a 32-bit result. The result is sign-extended to 40 bits and subtracted from the contents of the MAC16 accumulator.</P>",
        "Operation": "<pre>m1 &#8592; if half<SUB>0</SUB> then AR[s]<SUB>31..16</SUB> else AR[s]<SUB>15..0</SUB>\nm2 &#8592; if half<SUB>1</SUB> then AR[t]<SUB>31..16</SUB> else AR[t]<SUB>15..0</SUB>\nACC &#8592; ACC &#8722; (m1<SUB>15</SUB><SUP>24</SUP>||m1) * (m2<SUB>15</SUB><SUP>24</SUP>||m2)</pre>"
    },
    "MULS.AD.*": {
        "name": "MULS.AD.*",
        "syn": "Signed Multiply/Subtract",
        "Assembler Syntax": "MULS.AD.* as, my\tWhere * expands as follows:<P>MULS.AD.LL - <code>for (half=0)</code> </P><P>MULS.AD.HL - <code>for (half=1)</code> </P><P>MULS.AD.LH - <code>for (half=2)</code> </P><P>MULS.AD.HH - <code>for (half=3)</code> </P>",
        "Description": "<P><code>MULS.AD.*</code> performs a two's complement multiply of half of address register <code>as</code> and half of MAC16 register <code>my</code>, producing a 32-bit result. The result is sign-extended to 40 bits and subtracted from the contents of the MAC16 accumulator. The <code>my</code> operand can designate either MAC16 register <code>m2</code> or <code>m3</code>.</P>",
        "Operation": "<pre>m1 &#8592; if half<SUB>0</SUB> then AR[s]<SUB>31..16</SUB> else AR[s]<SUB>15..0</SUB>\nm2 &#8592; if half<SUB>1</SUB> then MR[1||y]<SUB>31..16</SUB> else MR[1||y]<SUB>15..0</SUB>\nACC &#8592; ACC &#8722; (m1<SUB>15</SUB><SUP>24</SUP>||m1) * (m2<SUB>15</SUB><SUP>24</SUP>||m2)</pre>"
    },
    "MULS.DA.*": {
        "name": "MULS.DA.*",
        "syn": "Signed Multiply/Subtract",
        "Assembler Syntax": "MULS.DA.* mx, at\tWhere * expands as follows:<P>MULS.DA.LL - <code>for (half=0)</code> </P><P>MULS.DA.HL - <code>for (half=1)</code> </P><P>MULS.DA.LH - <code>for (half=2)</code> </P><P>MULS.DA.HH - <code>for (half=3)</code> </P>",
        "Description": "<P><code>MULS.DA.*</code> performs a two's complement multiply of half of MAC16 register <code>mx</code> and half of address register <code>at</code>, producing a 32-bit result. The result is sign-extended to 40 bits and subtracted from the contents of the MAC16 accumulator. The <code>mx</code> operand can designate either MAC16 register <code>m0</code> or <code>m1</code>.</P>",
        "Operation": "<pre>m1 &#8592; if half<SUB>0</SUB> then MR[0||x]<SUB>31..16</SUB> else MR[0||x]<SUB>15..0</SUB>\nm2 &#8592; if half<SUB>1</SUB> then AR[t]<SUB>31..16</SUB> else AR[t]<SUB>15..0</SUB>\nACC &#8592; ACC &#8722; (m1<SUB>15</SUB><SUP>24</SUP>||m1) * (m2<SUB>15</SUB><SUP>24</SUP>||m2)</pre>"
    },
    "MULS.DD.*": {
        "name": "MULS.DD.*",
        "syn": "Signed Multiply/Subtract",
        "Assembler Syntax": "MULS.DD.* mx, my\tWhere * expands as follows:<P>MULS.DD.LL - <code>for (half=0)</code> </P><P>MULS.DD.HL - <code>for (half=1)</code> </P><P>MULS.DD.LH - <code>for (half=2)</code> </P><P>MULS.DD.HH - <code>for (half=3)</code> </P>",
        "Description": "<P><code>MULS.DD.*</code> performs a two's complement multiply of half of each of MAC16 registers <code>mx</code> and <code>my</code>, producing a 32-bit result. The result is sign-extended to 40 bits and subtracted from the contents of the MAC16 accumulator. The <code>mx</code> operand can designate either MAC16 register <code>m0</code> or <code>m1</code>. The <code>my</code> operand can designate either MAC16 register <code>m2</code> or <code>m3</code>.</P>",
        "Operation": "<pre>m1 &#8592; if half<SUB>0</SUB> then MR[0||x]<SUB>31..16</SUB> else MR[0||x]<SUB>15..0</SUB>\nm2 &#8592; if half<SUB>1</SUB> then MR[1||y]<SUB>31..16</SUB> else MR[1||y]<SUB>15..0</SUB>\nACC &#8592; ACC &#8722; (m1<SUB>15</SUB><SUP>24</SUP>||m1) * (m2<SUB>15</SUB><SUP>24</SUP>||m2)</pre>"
    },
    "MULSH": {
        "name": "MULSH",
        "syn": "Multiply Signed High",
        "Assembler Syntax": "MULSH ar, as, at",
        "Description": "<P><code>MULSH</code> performs a 32-bit two's complement multiplication of the contents of address registers <code>as</code> and <code>at</code> and writes the most significant 32 bits of the product to address register <code>ar</code>.</P>",
        "Operation": "<pre>tp &#8592; (AR[s]3132||AR[s]) * (AR[t]3132||AR[t])\nAR[r] &#8592; tp63..32</pre>"
    },
    "MULUH": {
        "name": "MULUH",
        "syn": "Multiply Unsigned High",
        "Assembler Syntax": "MULUH ar, as, at",
        "Description": "<P><code>MULUH</code> performs an unsigned multiplication of the contents of address registers <code>as</code> and <code>at</code>, and writes the most significant 32 bits of the product to address register <code>ar</code>.</P>",
        "Operation": "<pre>tp &#8592; (0<SUP>32</SUP>||AR[s]) * (0<SUP>32</SUP>||AR[t])\nAR[r] &#8592; tp63..32</pre>"
    },
    "NEG": {
        "name": "NEG",
        "syn": "Negate",
        "Assembler Syntax": "NEG ar, at",
        "Description": "<P><code>NEG</code> calculates the two's complement negation of the contents of address register <code>at</code> and writes it to address register <code>ar</code>. Arithmetic overflow is not detected.</P>",
        "Operation": "<pre>AR[r] &#8592; 0 &#8722; AR[t]</pre>"
    },
    "NOP": {
        "name": "NOP",
        "syn": "No-operation",
        "Assembler Syntax": "NOP",
        "Description": "<P>This instruction performs no operation. It is typically used for instruction alignment. <code>NOP</code> is a 24-bit instruction. For a 16-bit version, see <code>NOP.N</code>. </P>",
        "Operation": "<pre>none</pre>"
    },
    "NOP.N": {
        "name": "NOP.N",
        "syn": "Narrow No-operation",
        "Assembler Syntax": "NOP.N",
        "Description": "<P>This instruction performs no operation. It is typically used for instruction alignment. <code>NOP.N</code> is a 16-bit instruction. For a 24-bit version, see <code>NOP</code>.</P>",
        "Operation": "<pre>none</pre>"
    },
    "NSA": {
        "name": "NSA",
        "syn": "Normalization Shift Amount",
        "Assembler Syntax": "NSA at, as",
        "Description": "<P><code>NSA</code> calculates the left shift amount that will normalize the twos complement contents of address register <code>as</code> and writes this amount (in the range 0 to 31) to address register <code>at</code>. If <code>as</code> contains 0 or -1, <code>NSA</code> returns 31. Using <code>SSL</code> and <code>SLL</code> to shift <code>as</code> left by the <code>NSA</code> result yields the smallest value for which bits 31 and 30 differ unless <code>as</code> contains 0.</P>",
        "Operation": "<pre>sign &#8592; AR[s]<SUB>31</SUB>\nif AR[s]<SUB>30..0</SUB> = sign<SUP>31</SUP> then\n\tAR[t] &#8592; 31\nelse\n\tb4 &#8592; AR[s]<SUB>30..16</SUB> = sign<SUP>15</SUP>\n\tt3 &#8592; if b4 then AR[s]<SUB>15..0</SUB> else AR[s]<SUB>31..16</SUB>\n\tb3 &#8592; t3<SUB>15..8</SUB> = sign<SUP>8</SUP>\n\tt2 &#8592; if b3 then t3<SUB>7..0</SUB> else t3<SUB>15..8</SUB>\n\tb2 &#8592; t3<SUB>7..4</SUB> = sign<SUP>4</SUP>\n\tt1 &#8592; if b2 then t2<SUB>3..0</SUB> else t2<SUB>7..4</SUB>\n\tb1 &#8592; t3<SUB>3..2</SUB> = sign<SUP>2</SUP>\n\tb0 &#8592; if b1 then t1<SUB>1</SUB> = sign\t else t1<SUB>3</SUB> = sign\n\tAR[t] &#8592; 0<SUP>27</SUP>||((b4||b3||b2||b1||b0) &#8722; 1)\nendif</pre>"
    },
    "NSAU": {
        "name": "NSAU",
        "syn": "Normalization Shift Amount Unsigned",
        "Assembler Syntax": "NSAU at, as",
        "Description": "<P><code>NSAU</code> calculates the left shift amount that will normalize the unsigned contents of address register <code>as</code> and writes this amount (in the range 0 to 32) to address register <code>at</code>. If <code>as</code> contains 0, <code>NSAU</code> returns 32. Using <code>SSL</code> and <code>SLL</code> to shift <code>as</code> left by the <code>NSAU</code> result yields the smallest value for which bit 31 is set, unless <code>as</code> contains 0.</P>",
        "Operation": "<pre>if AR[s] = 0<SUP>32</SUP> then\n\tAR[t] &#8592; 32\nelse\n\tb4 &#8592; AR[s]<SUB>31..16</SUB> = 0<SUP>16</SUP>\n\tt3 &#8592; if b4 then AR[s]<SUB>15..0</SUB> else AR[s]<SUB>31..16</SUB>\n\tb3 &#8592; t3<SUB>15..8</SUB> = 0<SUP>8</SUP>\n\tt2 &#8592; if b3 then t3<SUB>7..0</SUB> else t3<SUB>15..8</SUB>\n\tb2 &#8592; t2<SUB>7..4</SUB> = 0<SUP>4</SUP>\n\tt1 &#8592; if b2 then t2<SUB>3..0</SUB> else t2<SUB>7..4</SUB>\n\tb1 &#8592; t1<SUB>3..2</SUB> = 0<SUP>2</SUP>\n\tb0 &#8592; if b1 then t1<SUB>1</SUB> = 0 else t1<SUB>3</SUB> = 0\n\tAR[t] &#8592; 0<SUP>27</SUP>||b4||b3||b2||b1||b0\nendif\n&nbsp;</pre>"
    },
    "OR": {
        "name": "OR",
        "syn": "Bitwise Logical Or",
        "Assembler Syntax": "OR ar, as, at",
        "Description": "<P><code>OR</code> calculates the bitwise logical or of address registers <code>as</code> and <code>at</code>. The result is written to address register <code>ar</code>.</P>",
        "Operation": "<pre>AR[r] &#8592; AR[s] or AR[t]</pre>"
    },
    "ORB": {
        "name": "ORB",
        "syn": "Boolean Or",
        "Assembler Syntax": "ORB br, bs, bt",
        "Description": "<P><code>ORB</code> performs the logical or of Boolean registers <code>bs</code> and <code>bt</code>, and writes the result to Boolean register <code>br</code>.</P><P>When the sense of one of the source Booleans is inverted (0 <code>&#174;</code> true, 1 <code>&#174;</code> false), use <code>ORBC</code>. When the sense of both of the source Booleans is inverted, use <code>ANDB</code> and an inverted test of the result.</P>",
        "Operation": "<pre>BR<SUB>r</SUB> &#8592; BR<SUB>s</SUB> or BR<SUB>t</SUB></pre>"
    },
    "ORBC": {
        "name": "ORBC",
        "syn": "Boolean Or with Complement",
        "Assembler Syntax": "ORBC br, bs, bt",
        "Description": "<P><code>ORBC</code> performs the logical or of Boolean register <code>bs</code> with the logical complement of Boolean register <code>bt</code> and writes the result to Boolean register <code>br</code>.</P>",
        "Operation": "<pre>BR<SUB>r</SUB> &#8592; BR<SUB>s</SUB> or not BR<SUB>t</SUB></pre>"
    },
    "PDTLB": {
        "name": "PDTLB",
        "syn": "Probe Data TLB",
        "Assembler Syntax": "PDTLB a<code>t</code>, a<code>s</code>",
        "Description": "<P><code>PDTLB</code> searches the data TLB for an entry that translates the virtual address in address register <code>as</code> and writes the way and index of that entry to address register <code>at</code>. If no entry matches, zero is written to the hit bit of <code>at</code>. The value written to <code>at</code> is implementation-specific, but in all implementations a value with the hit bit set is suitable as an input to the <code>IDTLB</code> or <code>WDTLB</code> instructions. See  for information on the result register formats for specific memory protection and translation options.</P><P><code>PDTLB</code> is a privileged instruction.</P>",
        "Operation": "<pre>if CRING != 0 then\n\tException (PrivilegedCause)\nelse\n\t(match, vpn, ei, wi) &#8592; ProbeDataTLB(AR[s])\n\tif match &gt; 1 then\n\t\tEXCVADDR &#8592; AR[s]\n\t\tException (LoadStoreTLBMultiHit)\n\telse\n\t\tAR[t] &#8592; PackDataTLBEntrySpec(match, vpn, ei, wi)\n\tendif\nendif</pre>"
    },
    "PFEND.A": {
        "name": "PFEND.A",
        "syn": "Prefetch End All",
        "Assembler Syntax": "PFEND.A",
        "Description": "<P><code>PFEND.A</code> causes all block operations to cease operation immediately and consider themselves complete.</P>"
    },
    "PFEND.O": {
        "name": "PFEND.O",
        "syn": "Prefetch End Optional",
        "Assembler Syntax": "PFEND.O",
        "Description": "<P><code>PFEND.O</code> causes optional block operations to cease operation immediately and consider themselves complete. Optional block operations are block operations that change only performance and not function. This includes operations that prefetch data into the cache but not operations that flush data from the cache.</P>"
    },
    "PFNXT.F": {
        "name": "PFNXT.F",
        "syn": "Prefetch Next First",
        "Assembler Syntax": "PFNXT.F",
        "Description": "<P><code>PFNXT.F</code> affects the execution of multiple block operations. Subsequent block operations do not interleave with previous block operations. Instead, subsequent block operations wait until all previous block operations have completed.</P>"
    },
    "PFWAIT.A": {
        "name": "PFWAIT.A",
        "syn": "Prefetch Wait All",
        "Assembler Syntax": "PFWAIT.A",
        "Description": "<P><code>PFWAIT.A</code> ensures that all block operations complete before any subsequent instruction executes.</P>"
    },
    "PFWAIT.R": {
        "name": "PFWAIT.R",
        "syn": "Prefetch Wait Required",
        "Assembler Syntax": "PFWAIT.R",
        "Description": "<P><code>PFWAIT.R</code> ensures that all required block operations complete before any subsequent instruction executes. Required block operations are block operations that are functionally required. This includes operations that flush data from the cache but not operations that prefetch data into the cache.</P>"
    },
    "PITLB": {
        "name": "PITLB",
        "syn": "Probe Instruction TLB",
        "Assembler Syntax": "PITLB a<code>t</code>, a<code>s</code>",
        "Description": "<P><code>PITLB</code> searches the Instruction TLB for an entry that translates the virtual address in address register <code>as</code> and writes the way and index of that entry to address register <code>at</code>. If no entry matches, zero is written to the hit bit of <code>at</code>. The value written to <code>at</code> is implementation-specific, but in all implementations a value with the hit bit set is suitable as an input to the <code>IITLB</code> or <code>WITLB</code> instructions. See  for information on the result register formats for specific memory protection and translation options.</P><P><code>PITLB</code> is a privileged instruction.</P>",
        "Operation": "<pre>if CRING != 0 then\n\tException (PrivilegedCause)\nelse\n\t(match, vpn, ei, wi) &#8592; ProbeInstTLB(AR[s])\n\tif match &gt; 1 then\n\t\tEXCVADDR &#8592; AR[s]\n\t\tException (InstructionFetchTLBMultiHit)\n\telse\n\t\tAR[t] &#8592; PackInstTLBEntrySpec(match, vpn, ei, wi)\n\tendif\nendif</pre>"
    },
    "PPTLB": {
        "name": "PPTLB",
        "syn": "Probe Protection TLB",
        "Assembler Syntax": "PPTLB a<code>t</code>, a<code>s</code>",
        "Description": "<P><code>PPTLB</code> searches the Protection TLB for a Foreground Segment that translates the virtual address in address register <code>as</code> and writes its number to address register <code>at</code>. If no entry matches, one is written to the miss bit of <code>at</code>. The value written to <code>at</code> is implementation-specific, but in all implementations a value with the miss bit clear is suitable as an input to the <code>RPTLB0</code><code>, </code><code>RPTLB1</code><code> </code>or <code>WPTLB</code> instructions. See  for information on the result register format.</P><P><code>PPTLB</code> is a privileged instruction.</P>",
        "Operation": "<pre>if CRING != 0 then\n\tException (PrivilegedCause)\nelse\n\tAR[t] &#8592; PackProtectionTLBEntrySpec\nendif</pre>"
    },
    "QUOS": {
        "name": "QUOS",
        "syn": "Quotient Signed",
        "Assembler Syntax": "QUOS ar, as, at",
        "Description": "<P><code>QUOS</code> performs a 32-bit two's complement division of the contents of address register <code>as</code> by the contents of address register <code>at</code> and writes the quotient to address register <code>ar</code>. The ambiguity which exists when either address register <code>as</code> or address register <code>at</code> is negative is resolved by requiring the product of the quotient and address register <code>at</code> to be smaller in absolute value than the address register <code>as</code>. If the contents of address register <code>at</code> are zero, <code>QUOS</code> raises an Integer Divide by Zero exception instead of writing a result. Overflow (<code>-2147483648</code> divided by <code>-1</code>) is not detected.</P>",
        "Operation": "<pre>if AR[t] = 0<SUP>32</SUP> then\n\tException (IntegerDivideByZero)\nelse\n\tAR[r] &#8592; AR[s] quo AR[t]\nendif</pre>"
    },
    "QUOU": {
        "name": "QUOU",
        "syn": "Quotient Unsigned",
        "Assembler Syntax": "QUOU ar, as, at",
        "Description": "<P><code>QUOU</code> performs a 32-bit unsigned division of the contents of address register <code>as</code> by the contents of address register <code>at</code> and writes the quotient to address register <code>ar</code>. If the contents of address register <code>at</code> are zero, <code>QUOU</code> raises an Integer Divide by Zero exception instead of writing a result.</P>",
        "Operation": "<pre>if AR[t] = 0<SUP>32</SUP> then\n\tException (IntegerDivideByZero)\nelse\n\ttq &#8592; (0||AR[s]) quo (0||AR[t])\n\tAR[r] &#8592; tq31..0\nendif</pre>"
    },
    "RDTLB0": {
        "name": "RDTLB0",
        "syn": "Read Data TLB Entry Virtual",
        "Assembler Syntax": "RDTLB0 a<code>t</code>, a<code>s</code>",
        "Description": "<P><code>RDTLB0</code> reads the data TLB entry specified by the contents of address register <code>as</code> and writes the Virtual Page Number (VPN) and address space ID (ASID) to address register <code>at</code>. See  for information on the address and result register formats for specific memory protection and translation options.</P><P><code>RDTLB0</code> is a privileged instruction.</P>",
        "Operation": "<pre>AR[t] &#8592; RDTLB0(AR[s])</pre>"
    },
    "RDTLB1": {
        "name": "RDTLB1",
        "syn": "Read Data TLB Entry Translation",
        "Assembler Syntax": "RDTLB1 a<code>t</code>, a<code>s</code>",
        "Description": "<P><code>RDTLB1</code> reads the data TLB entry specified by the contents of address register <code>as</code> and writes the Physical Page Number (PPN) and cache attribute (CA) to address register <code>at</code>. See  for information on the address and result register formats for specific memory protection and translation options.</P><P><code>RDTLB1</code> is a privileged instruction.</P>",
        "Operation": "<pre>AR[t] &#8592; RDTLB1(AR[s])</pre>"
    },
    "READ_IMPWIRE": {
        "name": "READ_IMPWIRE",
        "syn": "Read Import Wire IMPWIRE",
        "Assembler Syntax": "READ_IMPWIRE at",
        "Description": "<P><code>READ_IMPWIRE</code> reads the 32-bit data value from the import wire <code>IMPWIRE</code> and stores the value to address register <code>at</code>.</P>",
        "Operation": "<pre>AR[t] &#8592; TIE_IMPWIRE</pre>"
    },
    "READ_IPQ": {
        "name": "READ_IPQ",
        "syn": "Read Data from Input Queue IPQ",
        "Assembler Syntax": "READ_IPQ at",
        "Description": "<P><code>READ_IPQ</code> reads a 32-bit value from the input queue <code>IPQ</code> and writes this value into address register <code>at</code>. If the queue is empty, the processor will stall until valid data is available to be read from the queue.</P> <P>Note: The <code>CHECK_IPQ</code> instruction can be used to implement a \"test and branch\" sequence of code such that the <code>READ_IPQ</code> instruction is only issued when the input queue is not empty, thus avoiding stalls due to an empty queue.</P>",
        "Operation": "<pre>AR[t] &#8592; TIE_IPQ[31:0]</pre>"
    },
    "REMS": {
        "name": "REMS",
        "syn": "Remainder Signed",
        "Assembler Syntax": "REMS ar, as, at",
        "Description": "<P><code>REMS</code> performs a 32-bit two's complement division of the contents of address register <code>as</code> by the contents of address register <code>at</code> and writes the remainder to address register <code>ar</code>. The ambiguity which exists when either address register <code>as</code> or address register <code>at</code> is negative is resolved by requiring the remainder to have the same sign as address register <code>as</code>. If the contents of address register <code>at</code> are zero, <code>REMS</code> raises an Integer Divide by Zero exception instead of writing a result.</P>",
        "Operation": "<pre>if AR[t] = 0<SUP>32</SUP> then\n\tException (IntegerDivideByZero)\nelse\n\tAR[r] &#8592; AR[s] rem AR[t]\nendif</pre>"
    },
    "REMU": {
        "name": "REMU",
        "syn": "Remainder Unsigned",
        "Assembler Syntax": "REMU ar, as, at",
        "Description": "<P><code>REMU</code> performs a 32-bit unsigned division of the contents of address register <code>as</code> by the contents of address register <code>at</code> and writes the remainder to address register <code>ar</code>. If the contents of address register <code>at</code> are zero, <code>REMU</code> raises an Integer Divide by Zero exception instead of writing a result.</P>",
        "Operation": "<pre>if AR[t] = 0<SUP>32</SUP> then\n\tException (IntegerDivideByZero)\nelse\n\ttr &#8592; (0||AR[s]) rem (0||AR[t])\n\tAR[r] &#8592; tr31..0\nendif</pre>"
    },
    "RER": {
        "name": "RER",
        "syn": "Read ExternalRegister",
        "Assembler Syntax": "RER at, as",
        "Description": "<P><code>RER</code> reads one of a set of &quot;External Registers&quot;. It is in some ways similar to the <code>RSR.*</code> instruction except that the registers being read are not defined by the Xtensa ISA and are conceptually outside the processor core. They are read through processor ports.</P><P>Address register <code>as</code> is used to determine which register is to be read and the result is placed in address register <code>at</code>. When no External Register is addressed by the value in address register <code>as</code>, the result in address register <code>at</code> is undefined. The entire address space is reserved for use by Cadence. <code>RER</code> and <code>WER</code> are managed by the processor core so that the requests appear on the processor ports in program order. External logic is responsible for extending that order to the registers themselves.</P><P><code>RER</code> is a privileged instruction.</P>",
        "Operation": "<pre>if CRING != 0 then\n\tException (PrivilegedCause)\nelse\n\tRead External Register as defined outside the processor.\nendif</pre>"
    },
    "RET": {
        "name": "RET",
        "syn": "Non-Windowed Return",
        "Assembler Syntax": "RET",
        "Description": "<P><code>RET</code> returns from a routine called by <code>CALL0</code> or <code>CALLX0</code>. It is equivalent to the instruction</P><P>\tJX\tA0</P><P><code>RET</code> exists as a separate instruction because some Xtensa ISA implementations may realize performance advantages from treating this operation as a special case.</P>",
        "Operation": "<pre>nextPC &#8592; AR[0]</pre>"
    },
    "RET.N": {
        "name": "RET.N",
        "syn": "Narrow Non-Windowed Return",
        "Assembler Syntax": "RET.N",
        "Description": "<P><code>RET.N</code> is the same as <code>RET</code> in a 16-bit encoding. <code>RET</code> returns from a routine called by <code>CALL0</code> or <code>CALLX0</code>.</P>",
        "Operation": "<pre>nextPC &#8592; AR[0]</pre>"
    },
    "RETW": {
        "name": "RETW",
        "syn": "Windowed Return",
        "Assembler Syntax": "RETW",
        "Description": "<P><code>RETW</code> returns from a subroutine called by <code>CALL4</code>, <code>CALL8</code>, <code>CALL12</code>, <code>CALLX4</code>, <code>CALLX8</code>, or <code>CALLX12</code>, and that had <code>ENTRY</code> as its first instruction.</P><P><code>RETW</code> uses bits <code>29..0</code> of address register <code>a0</code> as the low 30 bits of the return address and bits <code>31..30</code> of the address of the <code>RETW</code> as the high two bits of the return address. Bits <code>31..30</code> of <code>a0</code> are used as the caller's window increment.</P><P><code>RETW</code> subtracts the window increment from <code>WindowBase</code> to return to the caller's registers. It then checks the <code>WindowStart</code> bit for this <code>WindowBase</code>. If it is set, then the caller's registers still reside in the register file, and <code>RETW</code> completes by clearing its own <code>WindowStart</code> bit, jumping to the return address, and, in some implementations, setting <code>PS.CALLINC</code> to bits <code>31</code><code>..</code><code>30</code> of <code>a0</code>. If the <code>WindowStart</code> bit is clear, then the caller's registers have been stored into the stack, so <code>RETW</code> signals one of window underflow's 4, 8, or 12, based on the size of the caller's window increment. The underflow handler is invoked with <code>Wi</code>ndowB<code>ase </code>decremented, a minor exception to the rule that instructions aborted by an exception have no side effects to the operating state of the processor. The processor stores the previous value of <code>WindowBase</code> in <code>PS.OWB</code> so that it can be restored by <code>RFWU</code>.</P><P>The window underflow handler is expected to restore the caller's registers, set the caller's <code>WindowStart</code> bit, and then return<code> </code>(see <code>RFWU</code>) to re-execute the <code>RETW</code>, which will then complete.</P><P>The operation of this instruction is undefined if AR[0]31..30 is <code>02</code>, if PS.WOE is <code>0</code>, if <code>PS.EXCM</code> is <code>1</code>, or if the first set bit among [WindowStartWindowBase-1, WindowStartWindowBase-2, WindowStartWindowBase-3] is anything other than WindowStartWindowBase-n, where <code>n</code> is <code>AR[0]</code><SUB>31..30</SUB>. (If none of the three bits is set, an underflow exception will be raised as described above, but if the wrong first one is set, the state is not legal.) Some implementations raise an illegal instruction exception in these cases as a debugging aid.</P>",
        "Operation": "<pre>n &#8592; AR[0]<SUB>31..30</SUB>\nnextPC &#8592; PC<SUB>31..30</SUB>||AR[0]<SUB>29..0</SUB>\nowb &#8592; WindowBase\nm &#8592; if WindowStart<SUB>WindowBase-4'b0001 </SUB>then 2'b01\n elsif WindowStart<SUB>WindowBase-4'b0010 </SUB>then 2'b10\n elsif WindowStart<SUB>WindowBase-4'b0011 </SUB>then 2'b11\n else 2'b00\nif n=2'b00 | (m!=2'b00 &amp; m!=n) | PS.WOE=0 | PS.EXCM=1 then\n\t-- undefined operation\n\t-- may raise illegal instruction exception\nelse\n\tWindowBase &#8592; WindowBase &#8722; (0<SUP>2</SUP>||n)\n\tif WindowStart<SUB>WindowBase</SUB> != 0 then\n\t\tWindowStart<SUB>owb</SUB> &#8592; 0\n\telse\n\t\t-- Underflow exception\n\t\tPS.EXCM &#8592; 1\n\t\tEPC[1] &#8592; PC\n\t\tPS.OWB &#8592; owb\n\t\tnextPC &#8592; if n = 2'b01 then WindowUnderflow4\n\t\t\telse if n = 2'b10 then WindowUnderflow8\n\t\t\telse WindowUnderflow12\n\tendif\n\tPS.CALLINC &#8592; n -- in some implementations\nendif</pre>"
    },
    "RETW.N": {
        "name": "RETW.N",
        "syn": "Narrow Windowed Return",
        "Assembler Syntax": "RETW.N",
        "Description": "<P><code>RETW.N</code> is the same as <code>RETW</code> in a 16-bit encoding.</P>",
        "Operation": "<pre>n &#8592; AR[0]<SUB>31..30</SUB>\nnextPC &#8592; PC<SUB>31..30</SUB>||AR[0]<SUB>29..0</SUB>\nowb &#8592; WindowBase\nm &#8592; if WindowStart<SUB>WindowBase-4'b0001 </SUB>then 2'b01\n elsif WindowStart<SUB>WindowBase-4'b0010 </SUB>then 2'b10\n elsif WindowStart<SUB>WindowBase-4'b0011 </SUB>then 2'b11\n else 2'b00\nif n=2'b00 | (m!=2'b00 &amp; m!=n) | PS.WOE=0 | PS.EXCM=1 then\n\t-- undefined operation\n\t-- may raise illegal instruction exception\nelse\n\tWindowBase &#8592; WindowBase &#8722; (0<SUP>2</SUP>||n)\n\tif WindowStart<SUB>WindowBase</SUB> != 0 then\n\t\tWindowStart<SUB>owb</SUB> &#8592; 0\n\telse\n\t\t-- Underflow exception\n\t\tPS.EXCM &#8592; 1\n\t\tEPC[1] &#8592; PC\n\t\tPS.OWB &#8592; owb\n\t\tnextPC &#8592; if n = 2'b01 then WindowUnderflow4\n\t\t\telse if n = 2'b10 then WindowUnderflow8\n\t\t\telse WindowUnderflow12\n\tendif\n\tPS.CALLINC &#8592; n -- in some implementations\nendif</pre>"
    },
    "RFDD": {
        "name": "RFDD",
        "syn": "Return from Debug and Dispatch",
        "Assembler Syntax": "RFDD",
        "Description": "<P>This instruction is used only in On-Chip Debug Mode and exists only in some implementations. It is an illegal instruction when the processor is not in On-Chip Debug Mode. See the <code>Xtensa Debug Guide</code> for a description of its operation.</P>"
    },
    "RFDE": {
        "name": "RFDE",
        "syn": "Return from Double Exception",
        "Assembler Syntax": "RFDE",
        "Description": "<P><code>RFDE</code> returns from an exception that went to the double exception vector (that is, an exception raised while the processor was executing with <code>PS.EXCM</code> set). It is similar to <code>RFE</code>, but <code>PS.EXCM</code> is not cleared, and <code>DEPC</code>, if it exists, is used instead of <code>EPC[1]</code>. <code>RFDE</code> simply jumps to the exception PC. <code>PS.UM</code> and <code>PS.WOE</code> are left unchanged.</P><P><code>RFDE</code> is a privileged instruction.</P>",
        "Operation": "<pre>if CRING != 0 then\n\tException (PrivilegedCause)\nelsif NDEPC=1 then\n\tnextPC \" DEPC\nelse\n\tnextPC &#8592; EPC[1]\nendif</pre>"
    },
    "RFDO": {
        "name": "RFDO",
        "syn": "Return from Debug Operation",
        "Assembler Syntax": "RFDO",
        "Description": "<P>This instruction is used only in On-Chip Debug Mode and exists only in some implementations. It is an illegal instruction when the processor is not in On-Chip Debug Mode. See the <code>Xtensa Debug Guide</code> for a description of its operation.</P>"
    },
    "RFE": {
        "name": "RFE",
        "syn": "Return from Exception",
        "Assembler Syntax": "RFE",
        "Description": "<P><code>RFE</code> returns from either the UserExceptionVector or the KernelExceptionVector. <code>RFE</code> sets <code>PS.EXCM</code> back to <code>0</code>, and then jumps to the address in <code>EPC[1]</code>. <code>PS.UM</code> and <code>PS.WOE</code> are left unchanged.</P><P>RFE is a privileged instruction.</P>",
        "Operation": "<pre>if CRING != 0 then\n\tException (PrivilegedCause)\nelse\n\tPS.EXCM &#8592; 0\n\tnextPC &#8592; EPC[1]\nendif</pre>"
    },
    "RFI": {
        "name": "RFI",
        "syn": "Return from High-Priority Interrupt",
        "Assembler Syntax": "RFI 0..15",
        "Description": "<P><code>RFI</code> returns from a high-priority interrupt. It restores the <code>PS</code> from <code>EPS[level]</code> and jumps to the address in <code>EPC[level]</code>. <code>Level</code> is given as a constant <code>2..</code>(NLEVEL<code>+</code>NNMI) in the instruction word. The operation of this opcode when level is 0 or 1 or greater than (NLEVEL<code>+</code>NNMI) is undefined.</P><P><code>RFI</code> is a privileged instruction.</P>",
        "Operation": "<pre>if CRING != 0 then\n\tException (PrivilegedCause)\nelse\n\tnextPC &#8592; EPC[level]\n\tPS &#8592; EPS[level]\nendif</pre>"
    },
    "RFME": {
        "name": "RFME",
        "syn": "Return from Memory Error",
        "Assembler Syntax": "RFME",
        "Description": "<P><code>RFME</code> returns from a memory error exception. It restores the <code>PS</code> from <code>MEPS</code> and jumps to the address in <code>MEPC</code>. In addition, the <code>MEME</code> bit of the <code>MESR</code> register is cleared.</P><P><code>RFME</code> is a privileged instruction.</P>",
        "Operation": "<pre>if CRING != 0 then\n\tException (PrivilegedCause)\nelse\n\tnextPC &#8592; MEPC\n\tPS &#8592; MEPS\n\tMESR.MEME &#8592; 0\nendif</pre>"
    },
    "RFUE": {
        "name": "RFUE",
        "syn": "Return from User-Mode Exception",
        "Assembler Syntax": "RFUE",
        "Description": "<P><code>RFUE</code> exists only in Xtensa Exception Architecture 1 (see  Xtensa Exception Architecture 1). It is an illegal instruction in current Xtensa implementations.</P>"
    },
    "RFWO": {
        "name": "RFWO",
        "syn": "Return from Window Overflow",
        "Assembler Syntax": "RFWO",
        "Description": "<P><code>RFWO</code> returns from an exception that went to one of the three window overflow vectors. It sets <code>PS.EXCM</code> back to <code>0</code>, clears the <code>WindowStart</code> bit of the registers that were spilled, restores <code>WindowBase</code> from <code>PS.OWB</code>, and then jumps to the address in <code>EPC[1]</code>. <code>PS.UM</code> is left unchanged.</P><P><code>RFWO</code> is a privileged instruction.</P>",
        "Operation": "<pre>if CRING != 0 then\n\tException (PrivilegedCause)\nelse\n\tPS.EXCM &#8592; 0\n\tnextPC &#8592; EPC[1]\n\tWindowStart<SUB>WindowBase</SUB> &#8592; 0\n\tWindowBase &#8592; PS.OWB\nendif</pre>"
    },
    "RFWU": {
        "name": "RFWU",
        "syn": "Return From Window Underflow",
        "Assembler Syntax": "RFWU",
        "Description": "<P><code>RFWU</code> returns from an exception that went to one of the three window underflow vectors. It sets <code>PS.EXCM</code> back to <code>0</code>, sets the <code>WindowStart</code> bit of the registers that were reloaded, restores <code>WindowBase</code> from <code>PS.OWB</code>, and then jumps to the address in <code>EPC[1]</code>. <code>PS.UM</code> is left unchanged.</P><P><code>RFWU</code> is a privileged instruction.</P>",
        "Operation": "<pre>if CRING != 0 then\n\tException (PrivilegedCause)\nelse\n\tPS.EXCM &#8592; 0\n\tnextPC &#8592; EPC[1]\n\tWindowStart<SUB>WindowBase</SUB> &#8592; 1\n\tWindowBase &#8592; PS.OWB\nendif</pre>"
    },
    "RITLB0": {
        "name": "RITLB0",
        "syn": "Read Instruction TLB Entry Virtual",
        "Assembler Syntax": "RITLB0 a<code>t</code>, a<code>s</code>",
        "Description": "<P><code>RITLB0</code> reads the instruction TLB entry specified by the contents of address register <code>as</code> and writes the Virtual Page Number (VPN) and address space ID (ASID) to address register <code>at</code>. See  for information on the address and result register formats for specific memory protection and translation options.</P><P><code>RITLB0</code> is a privileged instruction.</P>",
        "Operation": "<pre>AR[t] &#8592; RITLB0(AR[s])</pre>"
    },
    "RITLB1": {
        "name": "RITLB1",
        "syn": "Read Instruction TLB Entry Translation",
        "Assembler Syntax": "RITLB1 a<code>t</code>, a<code>s</code>",
        "Description": "<P><code>RITLB1</code> reads the instruction TLB entry specified by the contents of address register <code>as</code> and writes the Physical Page Number (PPN) and cache attribute (CA) to address register <code>at</code>. See  for information on the address and result register formats for specific memory protection and translation options.</P><P><code>RITLB1</code> is a privileged instruction.</P>",
        "Operation": "<pre>AR[t] &#8592; RITLB1(AR[s])</pre>"
    },
    "ROTW": {
        "name": "ROTW",
        "syn": "Rotate Window",
        "Assembler Syntax": "ROTW -8..7",
        "Description": "<P><code>ROTW</code> adds a constant to <code>WindowBase</code>, thereby moving the current window into the register file. <code>ROTW</code> is intended for use in exception handlers and context switch code.</P><P><code>ROTW</code> is a privileged instruction.</P>",
        "Operation": "<pre>if CRING != 0 then\n\tException (PrivilegedCause)\nelse\n\tWindowBase &#8592; WindowBase + imm4\nendif</pre>"
    },
    "RPTLB0": {
        "name": "RPTLB0",
        "syn": "Read Protection TLB Entry Address",
        "Assembler Syntax": "RPTLB0 a<code>t</code>, a<code>s</code>",
        "Description": "<P><code>RPTLB0</code> reads the Protection TLB segment specified by the contents of address register <code>as</code> and places the result in address register <code>at</code>. See  for information on address and result register formats.</P><P><code>RPTLB0</code> is a privileged instruction.</P>",
        "Operation": "<pre>AR[t] &#8592; RPTLB0(AR[s])</pre>"
    },
    "RPTLB1": {
        "name": "RPTLB1",
        "syn": "Read Protection TLB Entry Info",
        "Assembler Syntax": "RPTLB1 a<code>t</code>, a<code>s</code>",
        "Description": "<P><code>RPTLB1</code> reads the Protection TLB segment specified by the contents of address register <code>as</code> and places the result in address register <code>at</code>. See  for information on address and result register formats.</P><P><code>RPTLB1</code> is a privileged instruction.</P>",
        "Operation": "<pre>AR[t] &#8592; RPTLB1(AR[s])</pre>"
    },
    "RSIL": {
        "name": "RSIL",
        "syn": "Read and Set Interrupt Level",
        "Assembler Syntax": "RSIL at, 0..15",
        "Description": "<P><code>RSIL</code> first reads the <code>PS</code> Special Register (described in , PS Register Fields), writes this value to address register <code>at</code>, and then sets <code>PS.INTLEVEL</code> to a constant in the range <code>0..15</code> encoded in the instruction word. Interrupts at and below the <code>PS.INTLEVEL</code> level are disabled.</P><P>A <code>WSR.PS</code> or <code>XSR.PS</code> followed by an <code>RSIL</code> should be separated with an <code>ESYNC</code> to guarantee the value written is read back.</P><P>On some Xtensa ISA implementations the latency of <code>RSIL</code> is greater than one cycle, and so it is advantageous to schedule uses of the <code>RSIL</code> result later.</P><P><code>RSIL</code> is typically used as follows:</P><P>\tRSIL\ta2, newlevel</P><P>\tcode to be executed at newlevel</P><P>\tWSR.PS\t\ta2</P><P>The instruction following the <code>RSIL</code> is guaranteed to be executed at the new interrupt level specified in <code>PS.INTLEVEL</code>, therefore it is not necessary to insert one of the <code>SYNC</code> instructions to force the interrupt level change to take effect. </P><P><code>RSIL</code> is a privileged instruction.</P>",
        "Operation": "<pre>if CRING != 0 then\n\tException (PrivilegedCause)\nelse\n\tAR[t] &#8592; PS\n\tPS.INTLEVEL &#8592; s\nendif</pre>"
    },
    "RSR": {
        "name": "RSR",
        "syn": "Read Special Register",
        "Assembler Syntax": "RSR at, 0..255RSR at, *",
        "Description": "<P><code>RSR</code> is an assembler macro for <code>RSR.*</code> which provides compatibility with the older versions of the instruction containing either the name or the number of the Special Register.</P>"
    },
    "RSR.*": {
        "name": "RSR.*",
        "syn": "Read Special Register",
        "Assembler Syntax": "RSR.* atRSR at, *RSR at, 0..255",
        "Description": "<P><code>RSR.*</code> reads the Special Registers that are described in <code> Processor Control Instructions</code>. See  Special Registers for more detailed information on the operation of this instruction for each Special Register.</P><P>The contents of the Special Register designated by the 8-bit <code>sr</code> field of the instruction word are written to address register <code>at</code>. The name of the Special Register is used in place of the \'*' in the assembler syntax above and the translation is made to the 8-bit <code>sr</code> field by the assembler.</P><P><code>RSR</code> is an assembler macro for <code>RSR.*</code> that provides compatibility with the older versions of the instruction containing either the name or the number of the Special Register.</P><P>A <code>WSR.*</code> followed by an <code>RSR.*</code> to the same register should be separated with <code>ESYNC</code> to guarantee the value written is read back. On some Xtensa ISA implementations, the latency of <code>RSR.*</code> is greater than one cycle, and so it is advantageous to schedule other instructions before instructions that use the <code>RSR.*</code> result.</P><P><code>RSR.*</code> with Special Register numbers <code>>=</code> 64 is privileged. An <code>RSR.* </code>for an unconfigured register generally will raise an illegal instruction exception.</P>",
        "Operation": "<pre>sr &#8592; if msbFirst then s||r else r||s\nif sr >= 64 and CRING != 0 then\n\tException (PrivilegedCause)\nelse\n\tsee the Tables in  Special Registers\nendif</pre>"
    },
    "RSYNC": {
        "name": "RSYNC",
        "syn": "Register Read Synchronize",
        "Assembler Syntax": "RSYNC",
        "Description": "<P><code>RSYNC</code> waits for all previously fetched <code>WSR.*</code> instructions to be performed before interpreting the register fields of the next instruction. This operation is also performed as part of <code>ISYNC</code>. <code>ESYNC</code> and <code>DSYNC</code> are performed as part of this instruction.</P><P>This instruction is appropriate after <code>WSR.WindowBase</code><code>, </code><code>WSR.WindowStart</code><code>, </code><code>WSR.PS, WSR.CPENABLE</code><code>, or </code><code>WSR.EPS* </code>instructions before using their results. See the Special Register Tables in  Special Registers for a complete description of the uses of the <code>RSYNC</code> instruction.</P><P>Because the instruction execution pipeline is implementation-specific, the operation section below specifies only a call to the implementation's <code>rsync</code> function.</P>",
        "Operation": "<pre>rsync</pre>"
    },
    "RUR": {
        "name": "RUR",
        "syn": "Read User Register",
        "Assembler Syntax": "RUR ar, *",
        "Description": "<P><code>RUR</code> is an assembler macro for <code>RUR.*</code>, which provides compatibility with the older version of the instruction.</P>"
    },
    "RUR.*": {
        "name": "RUR.*",
        "syn": "Read User Register",
        "Assembler Syntax": "RUR.* arRUR ar, *",
        "Description": "<P><code>RUR.*</code> reads TIE state that has been grouped into 32-bit quantities by the TIE <code>user_register</code> statement. The name in the <code>user_register</code> statement replaces the \"*\" in the instruction name and causes the correct register number to be placed in the <code>st</code> field of the encoded instruction. The contents of the TIE <code>user_register</code> designated by the 8-bit number 16*<code>s+t</code> are written to address register <code>ar</code>. Here s and t are the numbers corresponding to the respective fields of the instruction word.</P><P><code>RUR</code> is an assembler macro for <code>RUR.*</code>, which provides compatibility with the older version of the instruction.</P>",
        "Operation": "<pre>AR[r] &#8592; user_register[st]</pre>"
    },
    "RUR.EXPSTATE": {
        "name": "RUR.EXPSTATE",
        "syn": "Read State EXPSTATE",
        "Assembler Syntax": "RUR.EXPSTATE ar",
        "Description": "<P><code>RUR.EXPSTATE</code> reads the 32-bit value of the exported state <code>EXPSTATE</code> and writes it to address register <code>ar</code>.</P>",
        "Operation": "<pre>AR[r] &#8592; EXPSTATE</pre>"
    },
    "S16I": {
        "name": "S16I",
        "syn": "Store 16-bit",
        "Assembler Syntax": "S16I at, as, 0..510",
        "Description": "<P><code>S16I</code> is a 16-bit store from address register <code>at</code> to memory. It forms a virtual address by adding the contents of address register <code>as</code> and an 8-bit zero-extended constant value encoded in the instruction word shifted left by one. Therefore, the offset can specify multiples of two from zero to 510. Sixteen bits (two bytes) from the least significant half of the register are written to memory at the physical address.</P><P>If the Region Translation Option  or the MMU Option  is enabled, the virtual address is translated to the physical address. If not, the physical address is identical to the virtual address. If the translation or memory reference encounters an error (for example, protection violation or non-existent memory), the processor raises one of several exceptions .</P><P>Without the Unaligned Exception Option , the least significant bit of the address is ignored. A reference to an odd address produces the same result as a reference to the address, minus one. With the Unaligned Exception Option, such an access raises an exception.</P>",
        "Operation": "<pre>vAddr &#8592; AR[s] + (0<SUP>23</SUP>||imm8||0)\nStore16 (vAddr, AR[t]<SUB>15..0</SUB>)</pre>"
    },
    "S32C1I": {
        "name": "S32C1I",
        "syn": "Store 32-bit Compare Conditional",
        "Assembler Syntax": "S32C1I at, as, 0..1020",
        "Description": "<P><code>S32C1I</code> is a conditional store instruction intended for updating synchronization variables in memory shared between multiple processors. It may also be used to atomically update variables shared between different interrupt levels or other pairs of processes on a single processor. <code>S32C1I</code> attempts to store the contents of address register <code>at</code> to the virtual address formed by adding the contents of address register <code>as</code> and an 8-bit zero-extended constant value encoded in the instruction word shifted left by two. If the old contents of memory at the physical address equals the contents of the <code>SCOMPARE1</code> Special Register, the new data is written; otherwise the memory is left unchanged. In either case, the value read from the location is written to address register <code>at</code>. In some implementations, under unusual circumstances, the bitwise not of <code>SCOMPARE1</code> may be returned when memory is left unchanged instead of the current value of the memory location (see  S32C1I Modification). The memory read, compare, and write may take place in the processor or the memory system, depending on the Xtensa ISA implementation, as long as these operations exclude other writes to this location. See  for more information on where the atomic operation takes place.</P><P>From a memory ordering point of view, the atomic pair of accesses has the characteristics of both an acquire and a release. That is, the atomic pair of accesses does not begin until all previous loads, stores, acquires, and releases have performed. The atomic pair must perform before any following load, store, acquire, or release may begin.</P><P>If the Region Translation Option  or the MMU Option is enabled, the virtual address is translated to the physical address. If not, the physical address is identical to the virtual address. If the translation or memory reference encounters an error (for example, protection violation or non-existent memory), the processor raises one of several exceptions .</P><P>Without the Unaligned Exception Option , the two least significant bits of the address are ignored. A reference to an address that is not 0 mod 4 produces the same result as a reference to the address with the least significant bits cleared. With the Unaligned Exception Option, such an access raises an exception.</P><P><code>S32C1I</code> does both a load and a store when the store is successful. However, memory protection tests check for store capability and the instruction may raise a StoreProhibitedCause exception, but will never raise a LoadProhibited Cause exception.</P>",
        "Operation": "<pre>vAddr &#8592; AR[s] + (022||imm8||02)\n(mem32, error) &#8592; Store32C1 (vAddr, AR[t], SCOMPARE1)\nif error then\n\tEXCVADDR &#8592; vAddr\n\tException (LoadStoreError)\nelse\n\tAR[t] &#8592; One Of (mem32, ~SCOMPARE1)\nendif</pre>"
    },
    "S32E": {
        "name": "S32E",
        "syn": "Store 32-bit for Window Exceptions",
        "Assembler Syntax": "S32E at, as, -64..-4",
        "Description": "<P><code>S32E</code> is a 32-bit store instruction similar to <code>S32I</code>, but with semantics required by window overflow and window underflow exception handlers. In particular, memory access checking is done with <code>PS.RING</code> instead of <code>CRING</code>, and the offset used to form the virtual address is a 4-bit one-extended immediate. Therefore, the offset can specify multiples of four from -64 to -4. In configurations without the <code>MMU Option</code>, there is no <code>PS.RING</code> and <code>S32E</code> is similar to <code>S32I</code> with a negative offset.</P><P>If the Region Translation Option  or the MMU Option  is enabled, the virtual address is translated to the physical address. If not, the physical address is identical to the virtual address. If the translation or memory reference encounters an error (for example, protection violation or non-existent memory), the processor raises one of several exceptions .</P><P>Without the Unaligned Exception Option , the two least significant bits of the address are ignored. A reference to an address that is not 0 mod 4 produces the same result as a reference to the address with the least significant bits cleared. With the Unaligned Exception Option, such an access raises an exception.</P><P><code>S32E</code> is a privileged instruction.</P>",
        "Operation": "<pre>if CRING != 0 then\n\tException (PrivilegedCause)\nelse\n\tvAddr &#8592; AR[s] + (1<SUP>26</SUP>||r||0<SUP>2</SUP>)\n\tring &#8592; if MMU Option then PS.RING else 0\n\tStore32Ring (vAddr, AR[t], ring)\nendif</pre>"
    },
    "S32EX": {
        "name": "S32EX",
        "syn": "Store 32-bit Exclusive",
        "Assembler Syntax": "S32EX at, as",
        "Description": "<P><code>S32EX</code> is a conditional 32-bit store from address register <code>at</code> to memory. It uses address register <code>as</code> for its virtual address. The data to be stored is taken from the contents of address register <code>at</code> and written to memory at the physical address. The store is conditional and occurs only if the Exclusive State set by the previous <code>L32EX</code> indicates that the memory location has not been modified. The previous value of <code>ATOMCTL[8]</code> is zero extended and moved to address register <code>at</code> and then <code>ATOMCTL[8]</code> is set if the store occurs and cleared if it does not.</P><P>If the Region Translation Option  or the MMU Option  is enabled, the virtual address is translated to the physical address. If not, the physical address is identical to the virtual address. If the translation or memory reference encounters an error (for example, protection violation or non-existent memory), the processor raises one of several exceptions .</P>",
        "Operation": "<pre>Store32 (AR[s], AR[t])</pre>"
    },
    "S32I": {
        "name": "S32I",
        "syn": "Store 32-bit",
        "Assembler Syntax": "S32I at, as, 0..1020",
        "Description": "<P><code>S32I</code> is a 32-bit store from address register <code>at</code> to memory. It forms a virtual address by adding the contents of address register <code>as</code> and an 8-bit zero-extended constant value encoded in the instruction word shifted left by two. Therefore, the offset can specify multiples of four from zero to 1020. The data to be stored is taken from the contents of address register <code>at</code> and written to memory at the physical address.</P><P>If the Region Translation Option  or the MMU Option  is enabled, the virtual address is translated to the physical address. If not, the physical address is identical to the virtual address. If the translation or memory reference encounters an error (for example, protection violation or non-existent memory), the processor raises one of several exceptions .</P><P>Without the Unaligned Exception Option , the two least significant bits of the address are ignored. A reference to an address that is not 0 mod 4 produces the same result as a reference to the address with the least significant bits cleared. With the Unaligned Exception Option, such an access raises an exception.</P><P>If the Instruction Memory Access Option  is configured, <code>S32I</code> is one of only a few memory reference instructions that can access instruction RAM.</P>",
        "Operation": "<pre>vAddr<SUB> </SUB>&#8592;<SUB> </SUB>AR[s]<SUB> </SUB>+<SUB> </SUB>(0<SUP>22</SUP>||imm8||0<SUP>2</SUP>)\nStore32 (vAddr, AR[t])</pre>"
    },
    "S32I.N": {
        "name": "S32I.N",
        "syn": "Narrow Store 32-bit",
        "Assembler Syntax": "S32I.N at, as, 0..60",
        "Description": "<P><code>S32I.N</code> is similar to <code>S32I</code>, but has a 16-bit encoding and supports a smaller range of offset values encoded in the instruction word.</P><P><code>S32I.N</code> is a 32-bit store to memory. It forms a virtual address by adding the contents of address register <code>as</code> and an 4-bit zero-extended constant value encoded in the instruction word shifted left by two. Therefore, the offset can specify multiples of four from zero to 60. The data to be stored is taken from the contents of address register <code>at</code> and written to memory at the physical address.</P><P>If the Instruction Memory Access Option  is configured, <code>S32I.N</code> is one of only a few memory reference instructions that can access instruction RAM.</P><P>If the Region Translation Option  or the MMU Option is enabled, the virtual address is translated to the physical address. If not, the physical address is identical to the virtual address. If the translation or memory reference encounters an error (for example, protection violation or non-existent memory), the processor raises one of several exceptions .</P><P>Without the Unaligned Exception Option , the two least significant bits of the address are ignored. A reference to an address that is not 0 mod 4 produces the same result as a reference to the address with the least significant bits cleared. With the Unaligned Exception Options, such an access raises an exception.</P>",
        "Operation": "<pre>vAddr<SUB> </SUB>&#8592;<SUB> </SUB>AR[s]<SUB> </SUB>+<SUB> </SUB>(0<SUP>26</SUP>||imm4||0<SUP>2</SUP>)\nStore32 (vAddr, AR[t])</pre>"
    },
    "S32NB": {
        "name": "S32NB",
        "syn": "Store 32-bit Non-Buffered",
        "Assembler Syntax": "S32NB at, as, 0..60",
        "Description": "<P><code>S32NB</code> is a 32-bit store from address register <code>at</code> to memory. It forms a virtual address by adding the contents of address register <code>as</code> and a 4-bit zero-extended constant value encoded in the instruction word shifted left by two. Therefore, the offset can specify multiples of four from zero to 60. The data to be stored is taken from the contents of address register <code>at</code> and written to memory at the physical address.</P><P>If the Region Translation Option  or the MMU Option  is enabled, the virtual address is translated to the physical address. If not, the physical address is identical to the virtual address. If the translation or memory reference encounters an error (for example, protection violation or non-existent memory), the processor raises one of several exceptions .</P><P>Without the Unaligned Exception Option , the two least significant bits of the address are ignored. A reference to an address that is not 0 mod 4 produces the same result as a reference to the address with the least significant bits cleared. With the Unaligned Exception Option, such an access raises an exception.</P><P><code>S32NB</code> provides the same functionality as <code>S32I</code> with two exceptions. First, when its operation leaves the processor, the external transaction is marked Non-Bufferable. Second, it may not be used to write to Instruction RAM.</P>",
        "Operation": "<pre>vAddr<SUB> </SUB>&#8592;<SUB> </SUB>AR[s]<SUB> </SUB>+<SUB> </SUB>(0<SUP>26</SUP>||imm4||0<SUP>2</SUP>)\nStore32 (vAddr, AR[t])</pre>"
    },
    "S32RI": {
        "name": "S32RI",
        "syn": "Store 32-bit Release",
        "Assembler Syntax": "S32RI at, as, 0..1020",
        "Description": "<P><code>S32RI</code> is a store barrier and 32-bit store from address register <code>at</code> to memory. <code>S32RI</code> stores to synchronization variables, which signals that previously written data is \"released\" for consumption by readers of the synchronization variable. This store will not perform until all previous loads, stores, acquires, and releases have performed. This ensures that any loads of the synchronization variable that see the new value will also find all previously written data available as well.</P><P><code>S32RI</code> forms a virtual address by adding the contents of address register <code>as</code> and an <BR>8-bit zero-extended constant value encoded in the instruction word shifted left by two. Therefore, the offset can specify multiples of four from zero to 1020. <code>S32RI</code> waits for previous loads, stores, acquires, and releases to be performed, and then the data to be stored is taken from the contents of address register <code>at</code> and written to memory at the physical address. Because the method of waiting is implementation dependent, this is indicated in the operation section below by the implementation function release.</P><P>If the Region Translation Option  or the MMU Option  is enabled, the virtual address is translated to the physical address. If not, the physical address is identical to the virtual address. If the translation or memory reference encounters an error (for example, protection violation or non-existent memory), the processor raises one of several exceptions .</P><P>Without theUnaligned Exception Option , the two least significant bits of the address are ignored. A reference to an address that is not 0 mod 4 produces the same result as a reference to the address with the least significant bits cleared. With the Unaligned Exception Option, such an access raises an exception.</P>",
        "Operation": "<pre>vAddr &#8592; AR[s] + (022||imm8||02)\nrelease\nStore32 (vAddr, AR[t])</pre>"
    },
    "S8I": {
        "name": "S8I",
        "syn": "Store 8-bit",
        "Assembler Syntax": "S8I at, as, 0..255",
        "Description": "<P><code>S8I</code> is an 8-bit store from address register <code>at</code> to memory. It forms a virtual address by adding the contents of address register <code>as</code> and an 8-bit zero-extended constant value encoded in the instruction word. Therefore, the offset has a range from 0 to 255. Eight bits (1 byte) from the least significant quarter of address register <code>at</code> are written to memory at the physical address.</P><P>If the Region Translation Option  or the MMU Option is enabled, the virtual address is translated to the physical address. If not, the physical address is identical to the virtual address. If the translation or memory reference encounters an error (for example, protection violation or non-existent memory), the processor raises one of several exceptions .</P>",
        "Operation": "<pre>vAddr<SUB> </SUB>&#8592;<SUB> </SUB>AR[s]<SUB> </SUB>+<SUB> </SUB>(024||imm8)\nStore8 (vAddr, AR[t]<SUB>7..0</SUB>)</pre>"
    },
    "SALT": {
        "name": "SALT",
        "syn": "Set AR if Less Than",
        "Assembler Syntax": "SALT ar, as, at",
        "Description": "<P>The <code>SALT</code> instruction exists to improve the performance of a magnitude comparison. If address register <code>as</code> considered as a signed integer is less than address register <code>at</code> considered as a signed integer, then address register <code>ar</code> is set to <code>0x1</code>. Otherwise address register <code>ar</code> is set to <code>0x0</code>.</P><P>By reversing the position of the as and at registers and/or considering the result in the opposite sense, all four conditions of less-than, greater-than, less-than-or-equal, and greater-than-or-equal can be tested.</P>",
        "Operation": "<pre>if AR[s] &lt; AR[t] then\n\tAR[r] &#8592; 031||1\nelse\n\tAR[r] &#8592; 032\nendif</pre>"
    },
    "SALTU": {
        "name": "SALTU",
        "syn": "Set AR if Less Than Unsigned",
        "Assembler Syntax": "SALTU ar, as, at",
        "Description": "<P>The <code>SALTU</code> instruction exists to improve the performance of an unsigned magnitude comparison. If address register <code>as</code> considered as an unsigned integer is less than address register <code>at</code> considered as an unsigned integer, then address register <code>ar</code> is set to <code>0x1</code>. Otherwise address register <code>ar</code> is set to <code>0x0</code>.</P><P>By reversing the position of the as and at registers and/or considering the result in the opposite sense, all four conditions of less-than, greater-than, less-than-or-equal, and greater-than-or-equal can be tested.</P>",
        "Operation": "<pre>if AR[s] &lt;<SUB>u</SUB> AR[t] then\n\tAR[r] &#8592; 031||1\nelse\n\tAR[r] &#8592; 032\nendif</pre>"
    },
    "SDCT": {
        "name": "SDCT",
        "syn": "Store Data Cache Tag",
        "Assembler Syntax": "SDCT at, as",
        "Description": "<P><code>SDCT</code> is not part of the Xtensa Instruction Set Architecture, but is instead specific to an implementation. That is, it may not exist in all implementations of the Xtensa ISA and its exact method of addressing the cache may depend on the implementation.</P><P><code>SDCT</code> is intended for writing the RAM array that implements the data cache tags as part of manufacturing test.</P><P><code>SDCT</code> uses the contents of address register <code>as</code> to select a line in the data cache and writes the contents of address register <code>at</code> to the tag associated with that line. The value written from <code>at</code> is described under Cache Tag Format in .</P><P><code>SDCT</code> is a privileged instruction.</P>",
        "Operation": "<pre>if CRING != 0 then\n\tException (PrivilegedCause)\nelse\n\tindex &#8592; AR[s]<SUB>x-1..z</SUB>\n\tDataCacheTag[index] &#8592; AR[t] // see Implementation Notes below\nendif</pre>"
    },
    "SDCW": {
        "name": "SDCW",
        "syn": "Store Data Cache Word",
        "Assembler Syntax": "SDCW at, as",
        "Description": "<P><code>SDCW</code> is not part of the Xtensa Instruction Set Architecture, but is instead specific to an implementation. That is, it may not exist in all implementations of the Xtensa ISA and its exact method of addressing the cache may depend on the implementation.</P><P><code>SDCW</code> is intended for writing the RAM array that implements the data cache as part of manufacturing tests.</P><P><code>SDCW</code> uses the contents of address register <code>as</code> to select a line in the data cache and one 32-bit quantity within that line, and writes the contents of address register <code>at</code> to the selected data location.</P><P><code>SDCW</code> is a privileged instruction.</P>",
        "Operation": "<pre>if CRING != 0 then\n\tException (PrivilegedCause)\nelse\n\tindex &#8592; AR[s]<SUB>x-1..2</SUB>\n\tDataCacheData [index] &#8592; AR[t] // see Implementation Notes below\nendif</pre>"
    },
    "SDDR32.P": {
        "name": "SDDR32.P",
        "syn": "Store from DDR Register",
        "Assembler Syntax": "SDDR32.P as",
        "Description": "<P>This instruction is used only in On-Chip Debug Mode and exists only in some implementations. It is an illegal instruction when the processor is not in On-Chip Debug Mode. See the <code>Xtensa Debug Guide</code> for a description of its operation.</P>"
    },
    "SETB_EXPSTATE": {
        "name": "SETB_EXPSTATE",
        "syn": "Set Bit of State EXPSTATE",
        "Assembler Syntax": "SETB_EXPSTATE bitindex (0..31)",
        "Description": "<P><code>SETB_EXPSTATE</code> sets a single bit of the 32-bit exported state <code>EXPSTATE</code> while not changing the value of any other bit of that state. The bit to be set is specified by the 5-bit immediate operand <code>bitindex</code>.</P>",
        "Operation": "<pre>EXPSTATE &#8592; EXPSTATE | (32'b1 << bitindex)</pre>"
    },
    "SEXT": {
        "name": "SEXT",
        "syn": "Sign Extend",
        "Assembler Syntax": "SEXT ar, as, 7..22",
        "Description": "<P><code>SEXT</code> takes the contents of address register <code>as</code> and replicates the bit specified by its immediate operand (in the range 7 to 22) to the high bits and writes the result to address register <code>ar</code>. The input can be thought of <code>as</code> an <code>imm</code><code>+</code><code>1</code> bit value with the high bits irrelevant and this instruction produces the 32-bit sign-extension of this value.</P>",
        "Operation": "<pre>b &#8592; t+7\nAR[r] &#8592; AR[s]<SUB>b</SUB><SUP>31&#8722;b</SUP>||AR[s]<SUB>b..0</SUB>\n&nbsp;</pre>"
    },
    "SICT": {
        "name": "SICT",
        "syn": "Store Instruction Cache Tag",
        "Assembler Syntax": "SICT at, as",
        "Description": "<P><code>SICT</code> is not part of the Xtensa Instruction Set Architecture, but is instead specific to an implementation. That is, it may not exist in all implementations of the Xtensa ISA and its exact method of addressing the cache may depend on the implementation.</P><P><code>SICT</code> is intended for writing the RAM array that implements the instruction cache tags as part of manufacturing test.</P><P><code>SICT</code> uses the contents of address register <code>as</code> to select a line in the instruction cache, and writes the contents of address register <code>at</code> to the tag associated with that line. The value written from <code>at</code> is described under Cache Tag Format in .</P><P><code>SICT</code> is a privileged instruction.</P>",
        "Operation": "<pre>if CRING != 0 then\n\tException (PrivilegedCause)\nelse\n\tindex &#8592; AR[s]<SUB>x-1..z</SUB>\n\tInstCacheTag[index] &#8592; AR[t] // see Implementation Notes below\nendif</pre>"
    },
    "SICW": {
        "name": "SICW",
        "syn": "Store Instruction Cache Word",
        "Assembler Syntax": "SICW at, as",
        "Description": "<P><code>SICW</code> is not part of the Xtensa Instruction Set Architecture, but is instead specific to an implementation. That is, it may not exist in all implementations of the Xtensa ISA and its exact method of addressing the cache may depend on the implementation.</P><P><code>SICW</code> is intended for writing the RAM array that implements the instruction cache as part of manufacturing tests.</P><P><code>SICW</code> uses the contents of address register <code>as</code> to select a line in the instruction cache, and writes the contents of address register <code>at</code> to the data associated with that line.</P><P><code>SICW</code> is a privileged instruction.</P>",
        "Operation": "<pre>if CRING != 0 then\n\tException (PrivilegedCause)\nelse\n\tindex &#8592; AR[s]<SUB>x-1..2</SUB>\n\tInstCacheData [index] &#8592; AR[t] // see Implementation Notes below\nendif</pre>"
    },
    "SIMCALL": {
        "name": "SIMCALL",
        "syn": "Simulator Call",
        "Assembler Syntax": "SIMCALL",
        "Description": "<P><code>SIMCALL</code> is not implemented as a simulator call by any Xtensa processor hardware. Some older processors may raise an illegal instruction exception for this opcode while newer processors treat it as a <code>NOP</code> instruction. It is implemented by the Xtensa Instruction Set Simulator to allow simulated programs to request services of the simulator host processor. <code>See the </code><code>Xtensa Instruction Set Simulator (ISS) User's Guide</code><code>.</code></P><P>The value in address register <code>a2</code> is the request code. Most codes request host system call services while others are used for special purposes such as debugging. Arguments needed by host system calls will be found in <code>a3</code>, <code>a4</code>, and <code>a5</code> and a return code will be stored to <code>a2</code> and an error number to <code>a3</code>.</P>",
        "Operation": "<pre>See the Xtensa Instruction Set Simulator (ISS) User's Guide. </pre>"
    },
    "SLL": {
        "name": "SLL",
        "syn": "Shift Left Logical",
        "Assembler Syntax": "SLL ar, as",
        "Description": "<P><code>SLL</code> shifts the contents of address register <code>as</code> left by the number of bit positions specified (as 32 minus number of bit positions) in the <code>SAR</code> (shift amount register) and writes the result to address register <code>ar</code>. Typically the <code>SSL</code> or <code>SSA8L</code> instructions are used to specify the left shift amount by loading <code>SAR</code> with <code>32-shift</code>. This transformation allows <code>SLL</code> to be implemented in the <code>SRC</code> funnel shifter (which only shifts right), using the <code>SLL</code> data as the most significant 32 bits and zero as the least significant 32 bits. Note the result of <code>SLL</code> is undefined if <code>SAR</code> <code>&gt;</code> <code>32</code>.</P>",
        "Operation": "<pre>sa &#8592; SAR<SUB>5..0</SUB>\nAR[r] &#8592; (AR[s]||0<SUP>32</SUP>)<SUB>31+sa..sa</SUB></pre>"
    },
    "SLLI": {
        "name": "SLLI",
        "syn": "Shift Left Logical Immediate",
        "Assembler Syntax": "SLLI ar, as, 1..31",
        "Description": "<P><code>SLLI</code> shifts the contents of address register <code>as</code> left by a constant amount in the range <code>1..31</code> encoded in the instruction. The shift amount <code>sa</code> field is split, with bits 3..0 in bits 7..4 of the instruction word and bit 4 in bit 20 of the instruction word. The shift amount is encoded as <code>32</code><code>&#8722;</code><code>shift</code>. When the <code>sa</code> field is 0, the result of this instruction is undefined.</P>",
        "Operation": "<pre>AR[r] &#8592; (AR[s]||0<SUP>32</SUP>)<SUB>31+sa..sa</SUB></pre>"
    },
    "SRA": {
        "name": "SRA",
        "syn": "Shift Right Arithmetic",
        "Assembler Syntax": "SRA ar, at",
        "Description": "<P><code>SRA</code> arithmetically shifts the contents of address register <code>at</code> right, inserting the sign of <code>at</code> on the left, by the number of bit positions specified by <code>SAR</code> (shift amount register) and writes the result to address register <code>ar</code>. Typically the <code>SSR</code> or <code>SSA8B</code> instructions are used to load <code>SAR</code> with the shift amount from an address register. Note the result of <code>SRA</code> is undefined if <code>SAR</code> <code>&gt;</code> <code>32</code>.</P>",
        "Operation": "<pre>sa &#8592; SAR<SUB>5..0</SUB>\nAR[r] &#8592; ((AR[t]<SUB>31</SUB>)<SUP>32</SUP>||AR[t])<SUB>31+sa..sa</SUB></pre>"
    },
    "SRAI": {
        "name": "SRAI",
        "syn": "Shift Right Arithmetic Immediate",
        "Assembler Syntax": "SRAI ar, at, 0..31",
        "Description": "<P><code>SRAI</code> arithmetically shifts the contents of address register <code>at</code> right, inserting the sign of <code>at</code> on the left, by a constant amount encoded in the instruction word in the range <code>0..31</code>. The shift amount <code>sa</code> field is split, with bits <code>3..0</code> in bits <code>11..8</code> of the instruction word, and bit <code>4</code> in bit <code>20</code> of the instruction word.</P>",
        "Operation": "<pre>AR[r] &#8592; ((AR[t]<SUB>31</SUB>)<SUP>32</SUP>||AR[t])<SUB>31+sa..sa</SUB></pre>"
    },
    "SRC": {
        "name": "SRC",
        "syn": "Shift Right Combined",
        "Assembler Syntax": "SRC ar, as, at",
        "Description": "<P><code>SRC</code> performs a right shift of the concatenation of address registers <code>as</code> and <code>at</code> by the shift amount in <code>SAR</code>. The least significant 32 bits of the shift result are written to address register <code>ar</code>. A shift with a wider input than output is called a funnel shift. <code>SRC</code> directly performs right funnel shifts. Left funnel shifts are done by swapping the high and low operands to <code>SRC</code> and setting <code>SAR</code> to 32 minus the shift amount. The <code>SSL</code> and <code>SSA8B</code> instructions directly implement such <code>SAR</code> settings. Note the result of <code>SRC</code> is undefined if <code>SAR</code> <code>&gt;</code> <code>32</code>.</P>",
        "Operation": "<pre>sa &#8592; SAR<SUB>5..0</SUB>\nAR[r] &#8592; (AR[s]||AR[t])<SUB>31+sa..sa</SUB></pre>"
    },
    "SRL": {
        "name": "SRL",
        "syn": "Shift Right Logical",
        "Assembler Syntax": "SRL ar, at",
        "Description": "<P><code>SRL</code> shifts the contents of address register <code>at</code> right, inserting zeros on the left, by the number of bits specified by <code>SAR</code> (shift amount register) and writes the result to address register <code>ar</code>. Typically the <code>SSR</code> or <code>SSA8B</code> instructions are used to load <code>SAR</code> with the shift amount from an address register. Note the result of <code>SRL</code> is undefined if <code>SAR</code> <code>&gt;</code> <code>32</code>.</P>",
        "Operation": "<pre>sa &#8592; SAR<SUB>5..0</SUB>\nAR[r] &#8592; (0<SUP>32</SUP>||AR[t])<SUB>31+sa..sa</SUB></pre>"
    },
    "SRLI": {
        "name": "SRLI",
        "syn": "Shift Right Logical Immediate",
        "Assembler Syntax": "SRLI ar, at, 0..15",
        "Description": "<P><code>SRLI</code> shifts the contents of address register <code>at</code> right, inserting zeros on the left, by a constant amount encoded in the instruction word in the range <code>0..15</code>. There is no <code>SRLI</code> for shifts <code>>=</code> 16. <code>EXTUI</code> replaces these shifts.</P>",
        "Operation": "<pre>AR[r] &#8592; (0<SUP>32</SUP>||AR[t])<SUB>31+sa..sa</SUB></pre>"
    },
    "SSA8B": {
        "name": "SSA8B",
        "syn": "Set Shift Amount for BE Byte Shift",
        "Assembler Syntax": "SSA8B as",
        "Description": "<P><code>SSA8B</code> sets the shift amount register (<code>SAR</code>) for a left shift by multiples of eight (for example, for big-endian (BE) byte alignment). The left shift amount is the two least significant bits of address register <code>as</code> multiplied by eight. Thirty-two minus this amount is written to <code>SAR</code>. Using 32 minus the left shift amount causes a funnel right shift and swapped high and low input operands to perform a left shift. <code>SSA8B</code> is similar to <code>SSL</code>, except the shift amount is multiplied by eight.</P><P><code>SSA8B</code> is typically used to set up for an <code>SRC</code> instruction to shift bytes. It may be used with big-endian byte ordering to extract a 32-bit value from a non-aligned byte address.</P>",
        "Operation": "<pre>SAR &#8592; 32 &#8722; (0||AR[s]<SUB>1..0</SUB>||0<SUP>3</SUP>)</pre>"
    },
    "SSA8L": {
        "name": "SSA8L",
        "syn": "Set Shift Amount for LE Byte Shift",
        "Assembler Syntax": "SSA8L as",
        "Description": "<P><code>SSA8L</code> sets the shift amount register (<code>SAR</code>) for a right shift by multiples of eight (for example, for little-endian (LE) byte alignment). The right shift amount is the two least significant bits of address register <code>as</code> multiplied by eight, and is written to <code>SAR</code>. <code>SSA8L</code> is similar to <code>SSR</code>, except the shift amount is multiplied by eight.</P><P><code>SSA8L</code> is typically used to set up for an <code>SRC</code> instruction to shift bytes. It may be used with little-endian byte ordering to extract a 32-bit value from a non-aligned byte address.</P>",
        "Operation": "<pre>SAR &#8592; 0||AR[s]<SUB>1..0</SUB>||0<SUP>3</SUP></pre>"
    },
    "SSAI": {
        "name": "SSAI",
        "syn": "Set Shift Amount Immediate",
        "Assembler Syntax": "SSAI 0..31",
        "Description": "<P><code>SSAI</code> sets the shift amount register (<code>SAR</code>) to a constant. The shift amount <code>sa</code> field is split, with bits <code>3..0</code> in bits <code>11..8</code> of the instruction word, and bit <code>4</code> in bit <code>4</code> of the instruction word. Because immediate forms exist of most shifts (<code>SLLI</code>, <code>SRLI</code>, <code>SRAI</code>), this is primarily useful to set the shift amount for <code>SRC</code>.</P>",
        "Operation": "<pre>SAR &#8592; 0||sa</pre>"
    },
    "SSL": {
        "name": "SSL",
        "syn": "Set Shift Amount for Left Shift",
        "Assembler Syntax": "SSL as",
        "Description": "<P><code>SSL</code> sets the shift amount register (<code>SAR</code>) for a left shift (for example, <code>SLL</code>). The left shift amount is the 5 least significant bits of address register <code>as</code>. 32 minus this amount is written to <code>SAR</code>. Using 32 minus the left shift amount causes a right funnel shift, and swapped high and low input operands to perform a left shift.</P>",
        "Operation": "<pre>sa &#8592; AR[s]<SUB>4..0</SUB>\nSAR &#8592; 32 &#8722; (0||sa)</pre>"
    },
    "SSR": {
        "name": "SSR",
        "syn": "Set Shift Amount for Right Shift",
        "Assembler Syntax": "SSR as",
        "Description": "<P><code>SSR</code> sets the shift amount register (<code>SAR</code>) for a right shift (for example, <code>SRL</code>, <code>SRA</code>, or <code>SRC</code>). The least significant five bits of address register <code>as</code> are written to <code>SAR</code>. The most significant bit of <code>SAR</code> is cleared. This instruction is similar to a <code>WSR.SAR</code>, but differs in that only <code>AR[s]</code><SUB>4..0</SUB> is used, instead of <code>AR[s]</code><SUB>5..0</SUB>.</P>",
        "Operation": "<pre>sa &#8592; AR[s]<SUB>4..0</SUB>\nSAR &#8592; 0||sa</pre>"
    },
    "SUB": {
        "name": "SUB",
        "syn": "Subtract",
        "Assembler Syntax": "SUB ar, as, at",
        "Description": "<P><code>SUB</code> calculates the two's complement 32-bit difference of address registers <code>as</code> and <code>at</code>. The low 32 bits of the difference are written to address register <code>ar</code>. Arithmetic overflow is not detected.</P>",
        "Operation": "<pre>AR[r] &#8592; AR[s] &#8722; AR[t]</pre>"
    },
    "SUBX2": {
        "name": "SUBX2",
        "syn": "Subtract with Shift by 1",
        "Assembler Syntax": "SUBX2 ar, as, at",
        "Description": "<P><code>SUBX2</code> calculates the two's complement 32-bit difference of address register <code>as</code> shifted left by 1 bit and address register <code>at</code>. The low 32 bits of the difference are written to address register <code>ar</code>. Arithmetic overflow is not detected.</P><P><code>SUBX2</code> is frequently used as part of sequences to multiply by small constants.</P>",
        "Operation": "<pre>AR[r] &#8592; (AR[s]<SUB>30..0</SUB>||0) &#8722; AR[t]</pre>"
    },
    "SUBX4": {
        "name": "SUBX4",
        "syn": "Subtract with Shift by 2",
        "Assembler Syntax": "SUBX4 ar, as, at",
        "Description": "<P><code>SUBX4</code> calculates the two's complement 32-bit difference of address register <code>as</code> shifted left by two bits and address register <code>at</code>. The low 32 bits of the difference are written to address register <code>ar</code>. Arithmetic overflow is not detected.</P><P><code>SUBX4</code> is frequently used as part of sequences to multiply by small constants.</P>",
        "Operation": "<pre>AR[r] &#8592; (AR[s]<SUB>29..0</SUB>||0<SUP>2</SUP>) &#8722; AR[t]</pre>"
    },
    "SUBX8": {
        "name": "SUBX8",
        "syn": "Subtract with Shift by 3",
        "Assembler Syntax": "SUBX8 ar, as, at",
        "Description": "<P><code>SUBX8</code> calculates the two's complement 32-bit difference of address register <code>as</code> shifted left by three bits and address register <code>at</code>. The low 32 bits of the difference are written to address register <code>ar</code>. Arithmetic overflow is not detected.</P><P><code>SUBX8</code> is frequently used as part of sequences to multiply by small constants.</P>",
        "Operation": "<pre>AR[r] &#8592; (AR[s]<SUB>28..0</SUB>||0<SUP>3</SUP>) &#8722; AR[t]</pre>"
    },
    "SYSCALL": {
        "name": "SYSCALL",
        "syn": "System Call",
        "Assembler Syntax": "SYSCALL",
        "Description": "<P>When executed, the <code>SYSCALL</code> instruction raises a system-call exception, redirecting execution to an exception vector . Therefore, <code>SYSCALL</code> instructions never complete. <code>EPC[1]</code> contains the address of the <code>S</code>YSC<code>ALL</code> and <code>ICOUNT</code> is not incremented. The system call handler should add 3 to <code>EPC[1]</code> before returning from the exception to continue execution.</P><P>The program may pass parameters to the system-call handler in the registers. There are no bits in <code>SYSCALL</code> instruction reserved for this purpose. See  System Calls for a description of software conventions for system call parameters.</P>",
        "Operation": "<pre>Exception (SyscallCause)</pre>"
    },
    "UMUL.AA.*": {
        "name": "UMUL.AA.*",
        "syn": "Unsigned Multiply",
        "Assembler Syntax": "UMUL.AA.* as, at\tWhere * expands as follows:<P>UMUL.AA.LL - <code>for (half=0)</code> </P><P>UMUL.AA.HL - <code>for (half=1)</code> </P><P>UMUL.AA.LH - <code>for (half=2)</code> </P><P>UMUL.AA.HH - <code>for (half=3)</code> </P>",
        "Description": "<P><code>UMUL.AA.*</code> performs an unsigned multiply of half of each of the address registers <code>as</code> and <code>at</code>, producing a 32-bit result. The result is zero-extended to 40 bits and written to the MAC16 accumulator.</P>",
        "Operation": "<pre>m1 &#8592; if half<SUB>0</SUB> then AR[s]<SUB>31..16</SUB> else AR[s]<SUB>15..0</SUB>\nm2 &#8592; if half<SUB>1</SUB> then AR[t]<SUB>31..16</SUB> else AR[t]<SUB>15..0</SUB>\nACC &#8592; (0<SUP>24</SUP>||m1) * (0<SUP>24</SUP>||m2)</pre>"
    },
    "WAITI": {
        "name": "WAITI",
        "syn": "Wait for Interrupt",
        "Assembler Syntax": "WAITI 0..15",
        "Description": "<P><code>WAITI</code> sets the interrupt level in <code>PS.INTLEVEL</code> to <code>imm4</code> and then, on some Xtensa ISA implementations, suspends processor operation until an interrupt occurs. <code>WAITI</code> is typically used in an idle loop to reduce power consumption. <code>CCOUNT</code> continues to increment during suspended operation, and a <code>CCOMPARE</code> interrupt will wake the processor.</P><P>When an interrupt is taken during suspended operation, <code>EPC[i]</code> will have the address of the instruction following <code>WAITI</code>. An implementation is not required to enter suspended operation and may leave suspended operation and continue execution at the following instruction at any time. Usually, therefore, the <code>WAITI</code> instruction should be within a loop.</P><P>The combination of setting the interrupt level and suspending operation avoids a race condition where an interrupt between the interrupt level setting and the suspension of operation would be ignored until a second interrupt occurred.</P><P><code>WAITI</code> is a privileged instruction.</P>",
        "Operation": "<pre>if CRING != 0 then\n\tException (PrivilegedCause)\nelse\n\tPS.INTLEVEL &#8592; imm4\nendif</pre>"
    },
    "WDTLB": {
        "name": "WDTLB",
        "syn": "Write Data TLB Entry",
        "Assembler Syntax": "WDTLB a<code>t</code>, a<code>s</code>",
        "Description": "<P><code>WDTLB</code> uses the contents of address register <code>as</code> to specify a data TLB entry and writes the contents of address register <code>at</code> into that entry. See  for information on the address and result register formats for specific memory protection and translation options. The point at which the data TLB write is effected is implementation-specific. Any translation that would be affected by this write before the execution of a <code>DSYNC</code> instruction is therefore undefined.</P><P><code>WDTLB</code> is a privileged instruction.</P>",
        "Operation": "<pre>if CRING != 0 then\n\tException (PrivilegedCause)\nelse\n\t(vpn, ei, wi) &#8592; SplitDataTLBEntrySpec(AR[s])\n\t(ppn, sr, ring, ca) &#8592; SplitDataEntry(wi, AR[t])\n\tDataTLB[wi][ei].ASID &#8592; ASID(ring)\n\tDataTLB[wi][ei].VPN &#8592; vpn\n\tDataTLB[wi][ei].PPN &#8592; ppn\n\tDataTLB[wi][ei].SR &#8592; sr\n\tDataTLB[wi][ei].CA &#8592; ca\nendif</pre>"
    },
    "WER": {
        "name": "WER",
        "syn": "Write ExternalRegister",
        "Assembler Syntax": "WER at, as",
        "Description": "<P><code>WER</code> writes one of a set of &quot;External Registers&quot;. It is in some ways similar to the <code>WSR.*</code> instruction except that the registers being written are not defined by the Xtensa ISA and are conceptually outside the processor core. They are written through processor ports.</P><P>Address register <code>as</code> is used to determine which register is to be written and address register <code>at</code> provides the write data. When no External Register is addressed by the value in address register <code>as</code>, no write occurs. The entire address space is reserved for use by Cadence. <code>RER</code> and <code>WER</code> are managed by the processor core so that the requests appear on the processor ports in program order. External logic is responsible for extending that order to the registers themselves.</P><P><code>WER</code> is a privileged instruction.</P>",
        "Operation": "<pre>if CRING != 0 then\n\tException (PrivilegedCause)\nelse\n\tWrite External Register as defined outside the processor.\nendif</pre>"
    },
    "WITLB": {
        "name": "WITLB",
        "syn": "Write Instruction TLB Entry",
        "Assembler Syntax": "WITLB a<code>t</code>, a<code>s</code>",
        "Description": "<P><code>WITLB</code> uses the contents of address register <code>as</code> to specify an instruction TLB entry and writes the contents of address register <code>at</code> into that entry. See  for information on the address and result register formats for specific memory protection and translation options. The point at which the instruction TLB write is effected is implementation-specific. Any translation that would be affected by this write before the execution of an <code>ISYNC</code> instruction is therefore undefined.</P><P><code>WITLB</code> is a privileged instruction.</P>",
        "Operation": "<pre>if CRING != 0 then\n\tException (PrivilegedCause)\nelse\n\t(vpn, ei, wi) &#8592; SplitInstTLBEntrySpec(AR[s])\n\t(ppn, sr, ring, ca) &#8592; SplitInstEntry(wi, AR[t])\n\tInstTLB[wi][ei].ASID &#8592; ASID(ring)\n\tInstTLB[wi][ei].VPN &#8592; vpn\n\tInstTLB[wi][ei].PPN &#8592; ppn\n\tInstTLB[wi][ei].SR &#8592; sr\n\tInstTLB[wi][ei].CA &#8592; ca\nendif</pre>"
    },
    "WPTLB": {
        "name": "WPTLB",
        "syn": "Write Protection TLB Entry",
        "Assembler Syntax": "WPTLB a<code>t</code>, a<code>s</code>",
        "Description": "<P><code>WPTLB</code> uses the contents of address registers <code>as</code> and <code>at</code> to specify a protection TLB entry and the information to be written to it. See  for information on the register formats. The point at which the TLB write is effected is implementation-specific.</P><P><code>WPTLB</code> is a privileged instruction.</P>",
        "Operation": "<pre>if CRING != 0 then\n\tException (PrivilegedCause)\nelse\n\tProtectionTLBWrite(AR[s], AR[t])\nendif</pre>"
    },
    "WRITE_OPQ": {
        "name": "WRITE_OPQ",
        "syn": "Write Data to Output Queue OPQ",
        "Assembler Syntax": "WRITE_OPQ at",
        "Description": "<P><code>WRITE_OPQ</code> writes the 32-bit value of address register <code>at</code> to the output queue <code>OPQ</code>. If the queue is full, the processor will stall until there is room in the queue to accept the write data.</P> <P>Note: The <code>CHECK_OPQ</code> instruction can be used to implement a \"test and branch\" sequence of code such that the <code>WRITE_OPQ</code> instruction is only issued when the output queue is not full, thus avoiding stalls due to a full queue.</P>",
        "Operation": "<pre>TIE_OPQ &#8592; AR[t]</pre>"
    },
    "WRMSK_EXPSTATE": {
        "name": "WRMSK_EXPSTATE",
        "syn": "Write State EXPSTATE With Mask",
        "Assembler Syntax": "WRMSK_EXPSTATE at, as",
        "Description": "<P><code>WRMSK_EXPSTATE</code> writes to the exported state <code>EXPSTATE</code>, the bit pattern defined by the value in address register <code>at</code> with address register <code>as</code> acting as a bit pattern mask.  For every bit that is a 1 in the mask <code>as</code>, the corresponding bit of state <code>EXPSTATE</code> is updated with the value of the corresponding bit in the data register <code>at</code>. For every bit that is a 0 in the mask <code>as</code>, the corresponding bit of state <code>EXPSTATE</code> is unchanged.</P>",
        "Operation": "<pre>EXPSTATE &#8592; (AR[s] & AR[t]) | (EXPSTATE & ~AR[s])</pre>"
    },
    "WSR": {
        "name": "WSR",
        "syn": "Write Special Register",
        "Assembler Syntax": "WSR at, 0..255WSR at, *",
        "Description": "<P><code>WSR</code> is an assembler macro for <code>WSR.*</code> which provides compatibility with the older versions of the instruction containing either the name or the number of the Special Register.</P>"
    },
    "WSR.*": {
        "name": "WSR.*",
        "syn": "Write Special Register",
        "Assembler Syntax": "WSR.* atWSR at, *WSR at, 0..255",
        "Description": "<P><code>WSR.*</code> writes the special registers that are described in <code> Processor Control Instructions</code>. See  Special Registers for more detailed information on the operation of this instruction for each Special Register.</P><P>The contents of address register <code>at</code> are written to the special register designated by the 8-bit <code>sr</code> field of the instruction word. The name of the Special Register is used in place of the \'*' in the assembler syntax above and the translation is made to the 8-bit <code>sr</code> field by the assembler.</P><P><code>WSR</code> is an assembler macro for <code>WSR.*</code> that provides compatibility with the older versions of the instruction containing either the name or the number of the Special Register.</P><P>The point at which <code>WSR.*</code> to certain registers affects subsequent instructions is not always defined (<code>SAR</code> and <code>ACC</code> are exceptions). In these cases, the Special Register Tables in  Special Registers explain how to ensure the effects are seen by a particular point in the instruction stream (typically involving the use of one of the <code>ISYNC</code>, <code>RSYNC</code>, <code>ESYNC</code>, or <code>DSYNC</code> instructions). A <code>WSR.*</code> followed by an <code>RSR.*</code> to the same register should be separated with an <code>ESYNC</code> to guarantee the value written is read back. A <code>WSR.PS</code> followed by <code>RSIL</code> also requires an <code>ESYNC</code>.</P><P><code>WSR.*</code> with Special Register numbers <code>>=</code> 64 is privileged. A <code>WSR.* </code>for an unconfigured register generally will raise an illegal instruction exception.</P>",
        "Operation": "<pre>sr &#8592; if msbFirst then s||r else r||s\nif sr >= 64 and CRING != 0 then\n\tException (PrivilegedCause)\nelse\n\tsee the Special Register Tables in  Special Registers\nendif</pre>"
    },
    "WUR": {
        "name": "WUR",
        "syn": "Write User Register",
        "Assembler Syntax": "WUR at,*",
        "Description": "<P><code>WUR</code> is an assembler macro for <code>WUR.*</code> which provides compatibility with the older version of the instruction</P>"
    },
    "WUR.*": {
        "name": "WUR.*",
        "syn": "Write User Register",
        "Assembler Syntax": "WUR.* atWUR at,*",
        "Description": "<P><code>WUR.*</code> writes TIE state that has been grouped into 32-bit quantities by the TIE <code>user_register</code> statement. The name in the <code>user_register</code> statement replaces the \"*\" in the instruction name and causes the correct register number to be placed in the <code>st</code> field of the encoded instruction. The contents of address register <code>at</code> are written to the TIE <code>user_register</code> designated by the 8-bit <code>sr</code> field of the instruction word.</P><P><code>WUR</code> is an assembler macro for <code>WUR.*</code> that provides compatibility with the older version of the instruction.</P>",
        "Operation": "<pre>user_register[sr] &#8592; AR[t]</pre>"
    },
    "WUR.EXPSTATE": {
        "name": "WUR.EXPSTATE",
        "syn": "Write EXPSTATE",
        "Assembler Syntax": "WUR.EXPSTATE at",
        "Description": "<P><code>WUR.EXPSTATE</code> writes the 32-bit contents of address register <code>at</code> to the exported state <code>EXPSTATE</code>.</P>",
        "Operation": "<pre>EXPSTATE &#8592; AR[t]</pre>"
    },
    "XOR": {
        "name": "XOR",
        "syn": "Bitwise Logical Exclusive Or",
        "Assembler Syntax": "XOR ar, as, at",
        "Description": "<P><code>XOR</code> calculates the bitwise logical exclusive or of address registers <code>as</code> and <code>at</code>. The result is written to address register <code>ar</code>.</P>",
        "Operation": "<pre>AR[r] &#8592; AR[s] xor AR[t]</pre>"
    },
    "XORB": {
        "name": "XORB",
        "syn": "Boolean Exclusive Or",
        "Assembler Syntax": "XORB br, bs, bt",
        "Description": "<P><code>XORB</code> performs the logical exclusive or of Boolean registers <code>bs</code> and <code>bt</code> and writes the result to Boolean register <code>br</code>.</P><P>When the sense of one of the source Booleans is inverted (0 <code>&#174;</code> true, 1 <code>&#174;</code> false), use an inverted test of the result. When the sense of both of the source Booleans is inverted, use a non-inverted test of the result.</P>",
        "Operation": "<pre>BR<SUB>r</SUB> &#8592; BR<SUB>s</SUB> xor BR<SUB>t</SUB></pre>"
    },
    "XSR": {
        "name": "XSR",
        "syn": "Exchange Special Register",
        "Assembler Syntax": "XSR at, 0..255XSR at, *",
        "Description": "<P><code>XSR</code> is an assembler macro for <code>XSR.*</code>, which provides compatibility with the older versions of the instruction containing either the name or the number of the Special Register.</P>"
    },
    "XSR.*": {
        "name": "XSR.*",
        "syn": "Exchange Special Register",
        "Assembler Syntax": "XSR.* atXSR at, *XSR at, 0..255",
        "Description": "<P><code>XSR.*</code> simultaneously reads and writes the special registers that are described in <code> Processor Control Instructions</code>. See  Special Registers for more detailed information on the operation of this instruction for each Special Register.</P><P>The contents of address register <code>at</code> and the Special Register designated by the immediate in the 8-bit <code>sr</code> field of the instruction word are both read. The read address register value is then written to the Special Register, and the read Special Register value is written to <code>at</code>. The name of the Special Register is used in place of the \'*' in the assembler syntax above and the translation is made to the 8-bit <code>sr</code> field by the assembler.</P><P><code>XSR</code> is an assembler macro for <code>XSR.*</code>, which provides compatibility with the older versions of the instruction containing either the name or the number of the Special Register.</P><P>The point at which <code>XSR.*</code> to certain registers affects subsequent instructions is not always defined (<code>SAR</code> and <code>ACC</code> are exceptions). In these cases, the Special Register Tables in  Special Registers explain how to ensure the effects are seen by a particular point in the instruction stream (typically involving the use of one of the <code>ISYNC</code>, <code>RSYNC</code>, <code>ESYNC</code>, or <code>DSYNC</code> instructions). An <code>XSR.*</code> followed by an <code>RSR.*</code> to the same register should be separated with an <code>ESYNC</code> to guarantee the value written is read back. An <code>XSR.PS</code> followed by <code>RSIL</code> also requires an <code>ESYNC</code>. In general, the restrictions on <code>XSR.*</code> include the union of the restrictions of the corresponding <code>RSR.*</code> and <code>WSR.*</code>.</P><P><code>XSR.*</code> with Special Register numbers <code>>=</code> 64 is privileged. An <code>XSR.* </code>for an unconfigured register generally will raise an illegal instruction exception.</P>",
        "Operation": "<pre>sr &#8592; if msbFirst then s||r else r||s\nif sr >= 64 and CRING != 0 then\n\tException (PrivilegedCause)\nelse\n\tt0 &#8592; AR[t]\n\tt1 &#8592; see RSR frame of the Tables in  Special Registers\n\tsee WSR frame of the Tables in  Special Registers &#8592; t0\n\tAR[t] &#8592; t1\nendif</pre>"
    },
};
