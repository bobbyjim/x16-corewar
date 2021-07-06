;;;
;
; This is FERRET, the winner of the 1987 ICWST.
;
; It examines the core for possible instructions.  If it finds one, it places
; an SPL 0 bomb there.  After a while, it gives up on looking for another program
; and just bombs the entire core with DATs, hoping to kill off any SPLed
; programs before it kills itself off.
;
; redcode
; name FERRET
; author Robert Reed III
;
;       2ND ICWS COMPETITION ENTRY
;
;       NAME :  FERRET
;
;       PROGRAMMER :  Robert Reed III, Windsor Locks, CN
;
START  MOV  #4908,B
F      CMP  <A,<B
       MOV  S,@B
       CMP  <A,<B
A      MOV  S,-5
       DJN  F,B
K      MOV  W,<W
       DJN  K,<W
       ADD  #3,W
B      JMP  K,0
W      DAT  #0,#-10
S      SPL  0,0
