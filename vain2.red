;;;
;  This is a short but effective B-scanner, also known as 
;  a "replicator killer".
;
start   add     #1226   3
        jmz     -1      @2
        mov     grave   @1
        mov     prog    <0
        jmn     -4      -4
prog    spl     0       0
        mov     @10     <-1
grave   jmp     -1      0
