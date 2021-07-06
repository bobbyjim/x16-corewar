open OUT, ">text.bin";
print OUT 'xx';
while(<DATA>)
{
    print OUT pack "A*x", uc $_;
}
close OUT;


__DATA__
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
 ---------------------------- CORESHELL 1.0 ----------------------------------


                  https://github.com/bobbyjim/x16-corewar


      clear   : clear core                     logout: quit program           
      random  : randomize core                 help  : show this text        
      run     : begin or continue run          step  : run one epoch         

      d <nnn> : display core from <nnn> and set ip 
      new <n> : add warrior <n> at current ip

      load <redcode file> and add warrior


      * redcode can also be entered at the prompt


      --> begin every redcode file with three semicolons ';;;'


      --> put space between the operands


          

 -----------------------------------------------------------------------------

 ;  ***  opcode guidelines  ***
 ;
 ;
 ;  --> begin every redcode file with three semicolons ';;;'
 ;
 ;  - operand expressions are not supported
 ;
 ;  - empty lines and whitespace are fine
 ;  
 ; 
 ;  mode example 1:
    
    add #17 2

 ; 
 ;  - adds 17 to the address core.[ pc+2 ]'s B-field
 ;
 ;  - immediate numeric values start with '#'
 ;
 ;
 ;  mode example 2: 
 ;
    mov #3 @2

 ;
 ;  - moves the value '3' to address offset pc + 2 + core.[ pc+2 ]
 ;
 ;  - indirect addresses starts with '@'
 ;
 ;
 ;  mode example 3: 

    jmp -2 #-99

 ;
 ; - negative addresses refer to previous locations
 ;
 ; - negative numbers are forced to [0..coresize]
 ;
 ;    - hint: coresize-1 is knowable with #-1
 ;
 ;
 ;  ***  end guide  ***
 