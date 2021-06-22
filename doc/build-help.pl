
my @opcodes = <DATA>;

open OUT, ">help.bin";
print OUT, pack "A*", <DATA>;
print OUT, pack "A*", <DATA>;
print OUT, pack "A*", <DATA>;
print OUT, pack "A*", <DATA>;
close OUT;

open OUT, ">opcodes.bin";
foreach (@opcodes)
{
    print OUT, pack "A*", $_;
}
close OUT;


__DATA__
cls: clear screen                       logout: quit program 
reset: reset arena                      verbose: change output level 
run a: run code at address a            d a b: display arena from a to b 
opcodes: display opcode help            help: show this text

hcf a b: halt-catch-fire                mov a b 
add a b: b += a                         sub a b: b -= a 
mul a b: b *= a                         div a b: b /= a 
mod a b: b %= a 
jmn a b: jmp if a!=0                    jmz a b: jmp if a==0
djn a b: jmp if --a!=0                  djz a b: jmp if --a==0
ske a b: ++ip if a==b                   slt a b: ++ip if a<b
xch a b: exchange a,b at a              spl a b: split to b 
