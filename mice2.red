;;;
; redcode
; name MICE
; author Chip Wendell
; this was the 1986 ICWST winner

ptr     dat             #0
start   mov     #12     ptr
loop    mov     @ptr    <copy
        djn     loop    ptr
        spl     @copy
        add     #653    copy
        jmz     start   ptr
copy    dat             #833
