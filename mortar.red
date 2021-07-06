;;;
; name MORTAR
; 
; The strain that the lack of an address window puts on the game 
; can be summarized as follows:

start mov b @a
      add a b
      mov @a a
      jmp start
a     dat #5
b     dat #8       ; (optimized version of mortar)

;   It is very hard to hit this program and it succeeds in beating lots of
; programs in the first 1000-2000 cycles.
