# x16-corewar - a Core War VM

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

* X: All addressing modes are valid for all opcodes.

* X: All opcodes MUST have both operands specified.

# VM-specific Behavior

This core implementation is engineered with the following environment:

* Up to 8 warriors may run at a time.

* SPL supports up to 8 processes per warrior.

# Redcode file notes

* START EVERY FILE WITH THREE SEMICOLONS (';;;')!!

The Commander X16 literally throws away the first two bytes of any file it reads in; Therefore, I strongly suggest you begin every redcode file with three semicolons -- by the way, this is a great place to hold the name of the warrior.  I may check for that name in the future.

Otherwise, refer to the URL references for general instructions.

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

CORE is typically initialized to DAT #0, #0.  However, it might instead be initialized to DAT #nnn, #nnn, where nnn is a number from 0 to 255.

I prefer to set CORE as a prime number, to confound trivial bombing programs a bit.

## RAM Banking

The arena is stored RAM banks, starting at bank 10.  If each cell is 5 bytes, then a bank can 
hold up to 1638 cells.  I put 1500 cells in a bank, and since the arena is currently around 8200 cells, 
the arena takes up 6 banks.

# Opcodes

     DAT A B   ; Remove process from the process queue.  

     HCL A B   ; Same as DAT. Comes from the 1970s "Halt and Catch Fire" joke opcode.

     MOV A B   ; Move A into location B.
    
     ADD A B   ; B += A

     SUB A B   ; B -= A 
        
     JMP A B   ; Jump to location A.
    
     JMN A B   ; Jump to location A if B != 0.
    
     JMZ A B   ; Jump to location A if B == 0.
        
     FLP A B   ; Jump to location A if system word < B.

     CMP A B   ; Skip next instruction if A == B.

     SEQ A B   ; Same as CMP.

     SLT A B   ; Skip next instruction if A < B.

     SNE A B   ; Skip next instruction if A != B.

     DJN A B   ; Decrement B, and then jump to A if B > 0.

     XCH A B   ; Exchange operands at location A.

     SPL A B   ; Add A to the process queue.
    
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

### Indirect Predecrement '<'

Pre-Decrement Indirect decrements memory as it is evaluating the operands.  
It evaluates operand A before evaluating operand B.

### Example

    MOV  3  @2	; move the instruction 3 cells down to the address pointed to at address [+2].
    ADD #5   1	; add 5 to the number stored at the next address.
    JMP -2	#5	; jump back to the MOV

# THE MEMORY CELL

The memory cell is a 5 byte C struct, vaguely reminiscent of Lua opcodes:

    opcode:     4 bits
    a-mode:     2 bits
    b-mode:     2 bits
    a operand: 14 bits
    b operand: 14 bits

The operand size (unsigned 14 bits) limits the memory window size to 16K cells. 

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

# TO DO

## Larger Arena

At some point I am going to want an 8,000 cell arena.  This will require moving the arena to banked RAM.
Here's how I plan to do it:

(1) Change the cell structure to:

    opcode:     8 bits
    flags:      8 bits
    a-mode:     8 bits
    a operand: 16 bits
    b-mode:     8 bits
    b operand: 16 bits

I'd move to 8 bytes per cell because CC65 optimizes in powers of two.

(2) Move to banked RAM.

8000 cells x 8 bytes per cell = 1000 cells per bank = 8 banks of RAM.

(3) Move some code into assembly language.

I have no idea what code would move nicely into asm.  But it's on the table.
