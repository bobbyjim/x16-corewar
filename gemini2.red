;;
; name: gemini2
; source: ab-mars2
;
dat		0		;pointer to source address
dat		99		;pointer to destination address
mov	@-2	@-1		;copy source to destination
cmp	-3	#9		;if all 10 lines have been copied...
jmp	4	 		;...then leave the loop
add	#1	-5		;otherwise, increment the source address
add	#1	-5		;...and the destination address
jmp -5	 		;...and return to the loop
mov	#99	93		;restore the starting destination address
jmp	93	 		;jump to the new copy
