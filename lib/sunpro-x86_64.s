	.file "sunpro-cpu.c"
	.code64
	.globl get_stack_pos
	.type get_stack_pos, @function
	.globl atomic_dec
	.type atomic_dec, @function
	.globl atomic_inc
	.type atomic_inc, @function

	.align 16
get_stack_pos:
	push	%rbp
	movq	%rsp,%rbp
	movq	%rsp,%rax
	leave
	ret
	
	.align 16
atomic_dec:
.CG0:

.CG1:	push       %rbp
.CG2:				;/ line : 6
	movq       %rsp,%rbp
.CG3:				;/ line : 6
	movl	   $0,%eax
	lock
	decl       (%rdi)				;/ line : 8
	sete       %al
	leave      				;/ line : 10
	ret        				;/ line : 10
.CG4:
	.size atomic_dec, . - atomic_dec
	.align 16
atomic_inc:
.CG5:

.CG6:	push       %rbp
.CG7:				;/ line : 14
	movq       %rsp,%rbp
.CG8:				;/ line : 14
.CG6.18:
.CG7.19:
/ASM
	lock
	incl (%rdi)
/ASMEND
.CG8.20:
	leave      				;/ line : 16
	ret        				;/ line : 16
.CG9:
	.size atomic_inc, . - atomic_inc


	.section .data,"aw"
Ddata.data: / Offset 0



	.section .bss,"aw"
Bbss.bss:


	.section .bssf,"aw"


	.section .rodata,"a"
Drodata.rodata: / Offset 0
