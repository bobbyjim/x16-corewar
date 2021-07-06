;;
;  name: mad bomber 
;  source: rje 
;  this turns out to just be a variation on gnome
;        
         spl back
start    MOV bomb @bomb        ; mov 3 @3
         ADD #4 bomb           ; add #4 2
bomb2    JMP start #-5         ; jmp -2
bomb     HCF #0 #15            ; hcf #0
back     mov bomb @bomb2
         sub #4 bomb2
         jmp back
