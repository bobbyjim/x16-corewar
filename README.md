# x16-corewar - a Corewar VM

 ref: https://corewar.co.uk/standards/cwg.txt - 1984

 ref: https://corewar.co.uk/madtutor.txt - 1986 and 1988

# Build the Commander X16 binary

1. On the command, line, type:

make clean

2. Comment out "#define X16" in common.h
3. Uncomment "#undef X16" in common.h

4. Then, on the command line:

make

# Build the "CC" binary 

1. On the command line, type:

make clean

2. Comment out "#undef X16" in common.h
3. Uncomment "#define X16" in common.h

4. Then, on the command line:

make -f Makefile.cc

# THE MEMORY CELL

The memory cell is a 4 byte C struct, vaguely reminiscent of Lua opcodes:

    opcode:     4 bits
    a-mode:     2 bits
    b-mode:     2 bits
    a operand: 12 bits
    b operand: 12 bits

CC65 optimizes structures that are power-of-two sizes, so 4 bytes is the 
golden size.  If I increased the size for whatever reason, I'd probably
want to go straight up to 8 bytes and put the arena in banked RAM.  However,
that would take a performance hit, so I'd also want to put some logic up
in assembly language.  In other words, the whole effort would be a major
update requiring a lot of effort.

The operand size (signed 12 bits) limits the memory window size to 4096 cells. 
I think that's okay.  The arena/core is a bit short of 4K cells in size.

# ARCHITECTURE

The ARENA or CORE is the memory area; it represents a chunk of contiguous memory cells.
This memory area is circular (i.e. it wraps around), so that there is no absolute position 
available to programs running there.  Because of this, all code is written using relative 
addressing.

 Address+0 is the current instruction.

 Address-1 was the previous instruction.

 Address+1 is the next instruction.

Each instruction occupies a single location in the ARENA.
There are no registers available for programs; all
memory manipulations are in the ARENA.

All address arithmetic is done modulo the size of the ARENA.

CORE is typically initialized to DAT 0, 0.  However, it might instead be initialized to DAT #nnn, #nnn, where nnn is a number from 0 to 255.

I prefer to set CORE as a prime number, to confound trivial bombing programs a bit.

# OPCODES

    (0)  HCL   B   ; Remove process from the process queue.  The mnemonic comes from the 1970s "Halt and Catch Fire" joke opcode.

    (1)  MOV A B   ; Move A into location B.
    
    (2)  ADD A B   ; B += A

    (3)  SUB A B   ; B -= A 
        
    (4)  JMP   B   ; Jump to location B.
    
    (5)  JMN A B   ; Jump to location B if A != 0.
    
    (6)  JMZ A B   ; Jump to location B if A == 0.
        
    (7)  SEQ A B   ; Skip next instruction if A == B.
    
    (8)  SLT A B   ; Skip next instruction if A < B.

    (9)  SNE A B   ; Skip next instruction if A != B.

    (10) FLP A B   ; Jump to location B if system word < A.

    (11) DJN A B   ; Decrement B, and then jump to A if B > 0.

    (12) - (13)    ; reserved

    (14) XCH   B   ; Exchange operands at location A.

    (15) SPL A     ; Add A to the process queue.
    
I really didn't want DJN in the opcode inventory.  I felt it is responsible
for making Imps too easy.  In the end though there is SO MUCH history around
this opcode that I decided to leave it in.

SLT and SNE, however, may or may not be useful.  I suspect one of them is 
useful and the other one not.

FLP is completely silly.  I may remove it.

XCH was desired for at least three decades.

SPL, I think, is another mischief maker opcode.  I've implemented it in a way
that is kind of abusable: the process runs the next time its owner gets a time 
slice.  On the other hand, a warrior can only have eight running processes, so
I think that limitation helps blunt the power of SPL.

## Modes

Addressing modes are identified by sigils:

### Immediate '#'

The number following this symbol is the operand.

### Relative (no sigil)

The  number  specifies  an  offset from the current instruction. Mars
adds the  offset to the address of the current  instruction; the num-
ber stored at the location reached in this way is the operand.

### Indirect '@'

The number  following  this symbol specifies an  offset from the cur-
rent  instruction  to  a  location where the  relative address of the
operand is  found.  Mars  adds the offset to  the address of the cur-
rent instruction and retrieves the number stored at the specified lo-
cation; this number is then interpreted as  an offset  from its own
address. The number found  at this second location is the operand.

### Indirect predecrement

I totally jettisoned it.  This one is too powerful and encourages
excessively short IMP-like programs that have no brain whatsoever.
Gone, good bye, good riddance.

# Example

    MOV  3  @2	; move the instruction 3 cells down to the address pointed to at address [+2].
    ADD #5   1	; add 5 to the number stored at the next address.
    JMP -2	#5	; jump back to the MOV



