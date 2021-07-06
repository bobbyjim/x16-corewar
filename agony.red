;redcode
;name Agony 2.4b
;kill Agony
;author Stefan Strack
;strategy Small-interval CMP scanner that bombs with a SPL 0 carpet.

scan    sub incr   comp
comp    cmp -5341  -5353
        slt #in13  comp
        djn scan   <-5512
        mov #12    count
        mov comp   bptr
bptr    dat #0
split   mov bomb   <bptr
count   djn split  #0
        jmn scan   scan
bomb    spl 0
incr    mov -28    <-28
        dat     #0
        dat     #0
        dat     #0
        dat     #0
        dat     #0
        dat     #0
        dat     #0
        dat     #0
        dat     #0
        dat     #0
        dat     #0
in12    dat     #0
in13    dat     #0
