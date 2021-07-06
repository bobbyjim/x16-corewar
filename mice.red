;;;
;redcode
;name MICE
;author Chip Wendell
;
;       MICE
;       by:
;         Chip Wendell
;
; Just to make sure that people out there have it, here is MICE, the program
; that won the 1986 ICWST.  It made a meger 336 points out of 1200 total
; in the KoTH which is a good sign that core wars programs have progressed
; beyond 1986.
;
; This program just multiplies itself throughout the core.
;
ptr     dat     #0     ,  #0
start   mov     #12    ,  ptr
loop    mov     @ptr   ,  <copy
        djn     loop   ,  ptr
        spl     @copy  ,  0
        add     #653   ,  copy
        jmz     start  ,  ptr
copy    dat     #0     ,  #833
        end     start
