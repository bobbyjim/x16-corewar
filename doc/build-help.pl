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
          
 -------------------------- CORESHELL COMMANDS -------------------------------

 cls, clear, help, logout, run                                

 load <redcode>                                               

 new n: add a process                      d nnn: display arena from nnn


  - - - - - - - - - - - - - -  opcodes:  - - - - - - - - - - - - - - - - - - 


 hcf a b: halt-catch-fire                          mov a b 

 add a b: b += a                                   sub a b: b -= a 

 jmp   b                       jmn a b             jmz a b

 seq a b                       slt a b             sne a b

 xch a b: exchange a,b at a                        spl a b: split to b 

 -----------------------------------------------------------------------------