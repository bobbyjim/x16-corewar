;;
; name: bigfoot
; source: ab-mars2
;
dat	#0	0		;pointer to source address
dat	#0	1752	;pointer to destination address
mov	@-2	@-1		;copy source to destination
sne	-3	#9		;if all 10 lines have been copied...
jmp	4	#0		;...then leave the loop
add	#1	-5		;otherwise, increment the source address
add	#1	-5		;...and the destination address
jmp -5	#0		;...and return to the loop
mov	#1752	1746	;restore the starting destination address
jmp	1746	#0		;jump to the new copy
