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
