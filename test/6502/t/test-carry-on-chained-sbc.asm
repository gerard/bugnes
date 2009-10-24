; Substracts 0xFF(at addr B0-B1) from 0x300 (at addr C0-C1)

cpu 6502
	* = $600
code
	lda #$0
	sta $C0
	lda #$3
	sta $C1
	lda #$FF
	sta $B0
	lda #$0
	sta $B1
	cld
	sec
	lda $C0
	sbc $B0
	sta $C2
	lda $C1
	sbc $B1
	sta $C3
	brk
code
