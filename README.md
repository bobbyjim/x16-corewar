# x16-corewar - a Corewar VM

 ref: https://corewar.co.uk/standards/cwg.txt - 1984

 ref: https://corewar.co.uk/madtutor.txt - 1986 and 1988

# THE MEMORY CELL

The memory cell is a 4 byte C struct, vaguely reminiscent of Lua opcodes:

    opcode:     4 bits
    a-mode:     2 bits
    b-mode:     2 bits
    a operand: 12 bits
    b operand: 12 bits

The operand size (signed 12 bits) limits the max core size to 4096 cells. 
I set the value at 4056 cells, and each cell is 4 bytes, so core memory is 16,224 bytes.

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

CORE is initialized to DAT 0, 0.  

# OPCODES

    (0)  DAT   B   ; Remove process from the process queue.

    (1)  MOV A B   ; Move A into location B.
    
    (2)  ADD A B   ; B += A

         SUB A B   ; B -= A  ( implemented as "ADD inverse(A) B" )
        
    (3)  MUL A B   ; B *= A
    
    (4)  DIV A B   ; B /= A
    
    (5)  MOD A B   ; B %= A

         JMP   B   ; Jump to location B ( implemented as "JMZ #0 B" ).
    
    (6)            ; reserved

    (7)            ; reserved

    (8)  JMN A B   ; Jump to location B if A != 0.
    
    (9)  JMZ A B   ; Jump to location B if A == 0.

    (10) DJN A B   ; Jump to location B if --A != 0.

    (11) DJZ A B   ; Jump to location B if --A == 0.
        
    (12) SKE A B   ; Skip next instruction if A == B.
    
    (13) SLT A B   ; Skip next instruction if A < B.
    
    (14)           ; reserved

    (15) SPL A     ; Add A to the process queue.
    
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

### Predecrement Indirect "<"

As above, but the operand is decremented before use.


# Example

    DAT -1
    ADD #5, -1	; add 5 to the number stored at the previous address.
    MOV #0, @-2	; move the number 0 to the address pointed to at address [-2].
    JMP -2		; jump back to the ADD

The DAT statement serves to hold a value that will be used by the program
(in this case, -1) at the first address. The ADD statement adds 5 to the number
stored at the previous address, changing the -1 to a +4.  The MOV command moves
the number 0 into the memory cell referred to by @-2. Where is that?  The address
is found by referring to the DAT statement two lines in front of the MOV.
There one find the address where the program will put the number 0. The final
command, JMP, causes execution to jump back two lines, to the ADD command.


