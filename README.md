     @@@@@@@   @@@@@@   @@@@@@@   @@@@@@@@     @@@  @@@  @@@   @@@@@@   @@@@@@@   
    @@@@@@@@  @@@@@@@@  @@@@@@@@  @@@@@@@@     @@@  @@@  @@@  @@@@@@@@  @@@@@@@@  
    !@@       @@!  @@@  @@!  @@@  @@!          @@!  @@!  @@!  @@!  @@@  @@!  @@@  
    !@!       !@!  @!@  !@!  @!@  !@!          !@!  !@!  !@!  !@!  @!@  !@!  @!@  
    !@!       @!@  !@!  @!@!!@!   @!!!:!       @!!  !!@  @!@  @!@!@!@!  @!@!!@!   
    !!!       !@!  !!!  !!@!@!    !!!!!:       !@!  !!!  !@!  !!!@!!!!  !!@!@!    
    :!!       !!:  !!!  !!: :!!   !!:          !!:  !!:  !!:  !!:  !!!  !!: :!!   
    :!:       :!:  !:!  :!:  !:!  :!:          :!:  :!:  :!:  :!:  !:!  :!:  !:!  
     ::: :::  ::::: ::  ::   :::   :: ::::      :::: :: :::   ::   :::  ::   :::  
     :: :: :   : :  :    :   : :  : :: ::        :: :  : :     :   : :   :   : :  

# x16-corewar - a Core War VM

 Two computer programs in their native habitat -- the memory chips of a 
 digital computer -- stalk each other from address to address. Sometimes they
 go scouting for the enemy; sometimes they lay down a barrage of numeric bombs;
 sometimes they copy themselves out of danger or stop to repair damage. 
 
 This is the game I call Core War. It is unlike almost all other computer games
 in that people do not play at all! The contending programs are written by 
 people, of course, but once a battle is under way the creator of a program can
 do nothing but watch helplessly, as the product of hours spent in design and 
 implementation either lives or dies on the screen. The outcome depends
 entirely on which program is hit first in a vulnerable area.

 - A. K. Dewdney, 1984

 1984 PDF: https://github.com/bobbyjim/x16-corewar/blob/main/doc/Corewar%20Guidelines.pdf

 1984 ref: https://github.com/bobbyjim/x16-corewar/blob/main/doc/corewar%20guidelines%201984.txt

 1988 ref: https://github.com/bobbyjim/x16-corewar/blob/main/doc/corewar%20guidelines%201986%201988.txt

# Running the program

The binary is "COREWAR".  Load it directly from the X16 prompt, or run it in the emulator with:

x16emu -prg COREWAR -run

## Demo mode

When the program is first run, if you don't press a key for about 20 seconds, a demo will start up.
Otherwise, the program starts in its shell mode.

## Help

The "help" command provides a command summary for the CLI.

# CORESHELL

CORESHELL is the command-line interpreter/debugger. Within it, you can query and reset core data, 
inject single statements into core data, create new warrior processes at a designated point in the
core, and load redcode files.  You can step-execute one epoch, or run it all in a graphical "arena"
for several thousand epochs at a time.

The Coreshell prompt looks like this:

     CORESHELL 8281 (100)  

The "8281" is the remaining free memory in the X16.
The "100" is the core address the CLI is currently pointing to.

Remember: type "HELP" to see the command summary!

Core memory may be inspected with the "d" command:

     d 1000

The above command displays the contents instructions at core locations 1000 - 1059.
Executing this command also sets the CLI's instruction pointer to that location, so
if you next type in some redcode:

     mov 0 1

The instruction will be written into location 1000, and the CLI's instruction pointer
will be incremented to 1001.

To create a process at that address, go back to location 1000:

    d 1000

Then create warrior #1 there:

    new 1

Coreshell will affirm the entry with:

    ADD-PROCESS(1:0 @1000)

That means warrior 1, process 0 is now pointing to location 1000.  Now step one epoch:

    step

A memory display will show, and you'll see that the contents of location 1000 has just
copied itself to location 1001, and warrior 1, process 0 is now pointing to location 1001.
Once you're ready to let it run awhile, just type:

    run

## Run mode

In run mode, the system runs for several thousand epochs (currently 16,000) before breaking back 
into the CLI.  After this run, the core may once again be inspected, adjusted, and new programs
loaded, and execution may be resumed right where it left off by invoking 'run' again.  The memory may also be cleared completely to set up a fresh run, or even randomized.

# VM Details

## Compatability Rating X

This implementation is an extension of the original Core War rules from the original spec ("84"), the ICWS'86 spec ("86"), ICWS'88 spec ("88"), 1994 draft ("94"), and variant extensions ("X").

## Opcodes

* 88: DAT MOV ADD SUB JMP JMZ JMN DJN CMP SLT SPL

* 94: SEQ SNE

* X: HCL XCH FLP

### Operands

* 84: Operand expressions are not supported.

* X: All addressing modes are valid for all opcodes.

## Redcode quirks

* Always start your redcode file with two (and preferably three) semicolons:

      ;;;
      ; name:   MYCOOLCODE
      ; author: rje
      ; date:   7/7/2021

This is because Commodore files always reserve their first two bytes as a load-address indicator,
and it's eaten by the operating system long before the file gets loaded.  The third semicolon makes
the first line a comment, which is safe.

* Separate elements with whitespace:

      foo   MOV  #<foo  @bar      ; yes!
      foo:MOV#<foo,@bar           ; NOOOOOOO

* Don't use colons with labels, nor commas with operands.  
  The code tries to do the right thing with them, but it's a little dodgy.

Otherwise, refer to the URL references for general instructions.

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

CORE is typically initialized to DAT #0, #0.  However, it might instead be initialized to DAT #nnn, #nnn, where nnn is a number from 0 to 255.

I prefer to set CORE as a prime number, to confound trivial bombing programs a bit.

## RAM Banking

The arena is currently stored in RAM banks 10 through 15.

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

     CMP A B   ; Same as SEQ.

     SEQ A B   ; Skip next instruction if A == B.

     SLT A B   ; Skip next instruction if A < B.

     SNE A B   ; Skip next instruction if A != B.

     DJN A B   ; Decrement B, and then jump to A if B > 0.

     XCH A B   ; Exchange operands at location A.

     SPL A B   ; Add A to the process queue.
    
FLP compares the system status word, which is an 8-bit register value that increments 
and wraps based on instructions executed by all programs.  Thus it is a completely
deterministic value that is nevertheless usable in a pseudo-random context. Generally,
the more processes that are running, the faster this value "spins".

SPL is implemented it in a way that is kind of abusable: the process runs the next 
time its owner gets a time slice.  On the other hand, a warrior can only have eight 
running processes, so I think that limitation helps blunt the power of SPL.

## Modes

Addressing modes are identified by sigils:

### Immediate '#'

The number following this symbol is the operand.

### Relative (no sigil)

The  number  specifies  an  offset from the current instruction. Mars adds the  offset to the address of the current  instruction; the number stored at the location reached in this way is the operand.

### Indirect '@'

The number  following  this symbol specifies an  offset from the current  instruction  to  a  location where the  relative address of the operand is  found.  Mars  adds the offset to  the address of the current instruction and retrieves the number stored at the specified location; this number is then interpreted as  an offset  from its own address. The number found  at this second location is the operand.

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
    a operand: 14 bits
    b-mode:     2 bits
    b operand: 14 bits

# Build the Commander X16 binary

1. On the command, line, type:

make

This will build the X16 executable file using CC65.
(NOTE: you have to have CC65 installed!)

# Build the "CC" binary 

1. On the command line, type:

make -f Makefile.cc

This will build a locally-executable file using whatever CC is linked to.  This is very useful for testing.

# TO DO

## Shortened Range

At some point I am going to want to limit programs' reach to only a portion of core memory. 
Here's how I plan to do it:

(1) Change the cell structure to:

    opcode:     4 bits
    a-mode:     2 bits
    b-mode:     2 bits
    a operand:  int
    b operand:  int

(2) Update the assembler to correctly assign values into the operands.

(3) Update the operand routines in the VM to convert them to local offset [-2048, +2047].
