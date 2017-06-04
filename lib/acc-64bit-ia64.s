	.file	"acc-64bit-ia64.c"
	.pred.safe_across_calls p1-p5,p16-p63
	.section	.text,	"ax",	"progbits"
        .align 16
        .global get_stack_pos#
        .proc get_stack_pos#
get_stack_pos:
        .prologue
        .body
        mov r8=sp
        nop 0
        nop 0
        br.ret.sptk.many b0
        .endp get_stack_pos#
        .align 16
        .global get_rse_bsp#
        .proc get_rse_bsp#
get_rse_bsp:
        .prologue
        .body
        mov r8=ar.bsp
        nop 0
        nop 0
        br.ret.sptk.many b0
        .endp get_rse_bsp#
	.ident	"GCC: (GNU) 4.2.3"
