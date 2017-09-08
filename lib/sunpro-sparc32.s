	.file	"t.c"
	.section	".text"
	.align 4
	.global get_stack_pos
	.type	get_stack_pos, #function
	.proc	016
get_stack_pos:
	mov %sp,%o0
	jmp	%o7+8
	 nop
	.size	get_stack_pos, .-get_stack_pos
	.ident	"GCC: (GNU) 4.2.0"
