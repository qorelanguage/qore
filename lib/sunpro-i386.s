	.file "sunpro-cpu.c"
	.code32
	.globl atomic_dec
	.type atomic_dec, @function
	.globl atomic_inc
	.type atomic_inc, @function
	.globl get_stack_pos
	.type get_stack_pos, @function
	
	.align 4
get_stack_pos:
	push       %ebp
	movl       %esp,%ebp
	andl       $-16,%esp
	movl	   %esp,%eax
	leave
	ret
	
	.align 4
atomic_dec:
.CG0.44:
	push       %ebp				;/ line : 6
	movl       %esp,%ebp				;/ line : 6
	andl       $-16,%esp				;/ line : 6

	movl       $0,%eax				;/ line : 8

	movl       8(%ebp),%ebx
	lock
	decl       (%ebx)				;/ line : 8
	sete	%al

	leave      				;/ line : 10
	ret        				;/ line : 10
.CG6.50:
	.size atomic_dec, . - atomic_dec
	.align 4
atomic_inc:
.CG7.51:
	push       %ebp				;/ line : 14
	movl       %esp,%ebp				;/ line : 14
	andl       $-16,%esp				;/ line : 14

	movl       8(%ebp),%eax
	lock
	incl (%eax)

	leave      				;/ line : 16
	ret        				;/ line : 16
.CG8.52:
	.size atomic_inc, . - atomic_inc

	.section .data,"aw"
Ddata.data: / Offset 0

	.section .bss,"aw"
Bbss.bss:

	.section .bssf,"aw"

	.section .rodata,"a"
Drodata.rodata: / Offset 0
