# x16-corewar - a Corewar VM

# What is a core war?

 ref: https://corewar.co.uk/standards/cwg.txt - 1984

 ref: https://corewar.co.uk/madtutor.txt - 1986 and 1988

# Compatability Rating X

This implementation is an extension of the original Core War rules from 1984. The references below use the terms 84: (original spec), 86: (ICWS'86),
88: (ICWS'88), 94: (ICWS'94 draft), and X: (variant feature).

## Opcodes

* 88: DAT MOV ADD SUB JMP JMZ JMN DJN CMP SPL SLT

* 94: SEQ

* X: HCL XCH

### Operands

* 84: Labels are not supported.  

* 84: Operand expressions are not supported.

* 86: There is no "indirect predecrement".  On purpose.

* X: All addressing modes are valid for all opcodes.

* X: All opcodes MUST have both operands specified.

# VM-specific Behavior

This core implementation is engineered with the following environment:

* Up to 8 warriors may run at a time.

* SPL supports up to 8 processes per warrior.




# Arena Architecture

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

# Redcode file notes

* START EVERY FILE WITH THREE SEMICOLONS (';;;')!!

The Commander X16 literally throws away the first two bytes of any file it reads in; Therefore, I strongly suggest you begin every redcode file with three semicolons -- by the way, this is a great place to hold the name of the warrior.  I may check for that name in the future.

Otherwise, refer to the URL references for general instructions.

# Opcodes

     DAT   B   ; Remove process from the process queue.  

     HCL   B   ; Same as DAT. Comes from the 1970s "Halt and Catch Fire" joke opcode.

     MOV A B   ; Move A into location B.
    
     ADD A B   ; B += A

     SUB A B   ; B -= A 
        
     JMP   B   ; Jump to location B.
    
     JMN A B   ; Jump to location B if A != 0.
    
     JMZ A B   ; Jump to location B if A == 0.
        
     CMP A B   ; Skip next instruction if A == B.

     SEQ A B   ; Same as CMP.

     SLT A B   ; Skip next instruction if A < B.

     SNE A B   ; Skip next instruction if A != B.

     FLP A B   ; Jump to location B if system word < A.

     DJN A B   ; Decrement B, and then jump to A if B > 0.

     XCH   B   ; Exchange operands at location A.

     SPL A     ; Add A to the process queue.
    
Even though there are 16 opcodes, two are aliases, so in reality there are only 14 slots used.  This means there are still two unused opcode slots.  Suggestions welcome.

I really didn't want DJN in the opcode inventory.  I felt it is responsible
for making Imps too easy.  In the end though there is SO MUCH history around
this opcode that I decided to leave it in.

SLT and SNE may be useful.  I suspect one of them is useful and the other one not.

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