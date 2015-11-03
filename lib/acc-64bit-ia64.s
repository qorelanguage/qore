	.file	"acc-64bit-ia64.c"
	.pred.safe_across_calls p1-p5,p16-p63
	.section	.text,	"ax",	"progbits"
	.align 16
	.global atomic_inc#
	.proc atomic_inc#
atomic_inc:
	.prologue
	.body
.L2:
	ld4.acq r14 = [r32]
	nop 0
	;;
	adds r15 = 1, r14
	mov ar.ccv=r14;;
	cmpxchg4.acq r15=[r32],r15,ar.ccv
	;;
	nop 0
	cmp4.ne p6, p7 = r15, r14
	(p6) br.cond.dptk .L2
	nop 0
	nop 0
	br.ret.sptk.many b0
	.endp do_atomic_inc#
	.align 16
	.global atomic_dec#
	.proc atomic_dec#
atomic_dec:
	.prologue
	.body
.L10:
	ld4.acq r14 = [r32]
	nop 0
	;;
	adds r16 = -1, r14
	mov ar.ccv=r14;;
	;;
	cmpxchg4.acq r15=[r32],r16,ar.ccv
	;;
	nop 0
	cmp4.ne p6, p7 = r15, r14
	(p6) br.cond.dptk .L10
	;;
	nop 0
	cmp4.eq p6, p7 = 0, r16
	nop 0
	;;
	(p6) addl r8 = 1, r0
	(p7) mov r8 = r0
	br.ret.sptk.many b0
	.endp do_atomic_dec#
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
