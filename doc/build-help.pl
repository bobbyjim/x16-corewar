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

-------------------------- CORESHELL COMMANDS --------------------------------

cls: clear screen                       logout: quit program                  
reset: clear arena memory               verbose: change output level          
run: run!                               d nnn: display arena from nnn         
help: show this text

hcf a b: halt-catch-fire                mov a b         
add a b: b += a                         sub a b: b -= a   

jmp   b: jump to b                      jmn a b: jmp if a!=0
jmz a b: jmp if a==0                    seq a b: ++ip if a==b
slt a b: ++ip if a<b                    sne a b: ++ip if a!=b

xch a b: exchange a,b at a              spl a b: split to b 

------------------------------------------------------------------------------
