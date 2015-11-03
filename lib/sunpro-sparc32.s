	.file	"t.c"
	.section	".text"
	.align 4
	.global compare_and_swap
	.hidden	compare_and_swap
	.type	compare_and_swap, #function
	.proc	04
compare_and_swap:
	mov	%o2, %g1
	cas [%o0], %o1, %g1
	jmp	%o7+8
	 mov	%g1, %o0
	.size	compare_and_swap, .-compare_and_swap
	.align 4
	.global get_stack_pos
	.type	get_stack_pos, #function
	.proc	016
get_stack_pos:
	mov %sp,%o0
	jmp	%o7+8
	 nop
	.size	get_stack_pos, .-get_stack_pos
	.align 4
	.global atomic_fetch_and_add
	.hidden	atomic_fetch_and_add
	.type	atomic_fetch_and_add, #function
	.proc	04
atomic_fetch_and_add:
	mov	%o0, %g2
	ld	[%g2], %o0
.LL13:
	add	%o1, %o0, %g1
	cas [%g2], %o0, %g1
	cmp	%o0, %g1
	bne,a,pn %icc, .LL13
	 ld	[%g2], %o0
	jmp	%o7+8
	 nop
	.size	atomic_fetch_and_add, .-atomic_fetch_and_add
	.align 4
	.global atomic_dec
	.type	atomic_dec, #function
	.proc	04
atomic_dec:
.LL15:
	ld	[%o0], %g2
	add	%g2, -1, %g1
	cas [%o0], %g2, %g1
	cmp	%g2, %g1
	bne,pn	%icc, .LL15
	 subcc	%g0, %g2, %g0
	jmp	%o7+8
	 subx	%g0, -1, %o0
	.size	atomic_dec, .-atomic_dec
	.align 4
	.global atomic_inc
	.type	atomic_inc, #function
	.proc	020
atomic_inc:
	ld	[%o0], %g2
.LL26:
	add	%g2, 1, %g1
	cas [%o0], %g2, %g1
	cmp	%g2, %g1
	bne,a,pn %icc, .LL26
	 ld	[%o0], %g2
	jmp	%o7+8
	 nop
	.size	atomic_inc, .-atomic_inc
	.ident	"GCC: (GNU) 4.2.0"
