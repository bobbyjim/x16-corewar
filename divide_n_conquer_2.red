; ------------------------------------------------
;
; Divide And Conquer #2
;
; Based on the original by
; Mark A. Durham
; November 1, 1991
; ------------------------------------------------

; This code copies the warrior and splits the copy

start    MOV   #trap-source,        source   ; Init copy pointer
         MOV   #2432+source-trap,   target   ; Init destination pointer
copy     MOV   @source,             <target  ; Copy warrior
         SUB   
         DJN   copy,                source
         SPL   @target                       ; Start copy of warrior

; This code traps and kills potential enemy code

         MOV   #164,               target
enslave  MOV   trap,                @target
         DJN   enslave,             target

         MOV   #164,               target
kill     MOV   target,              @target
         DJN   kill,                target

target   DAT   #2432+source-target            ; Kill self when finished
trap     SPL   trap
source   DAT   #target
         END 
