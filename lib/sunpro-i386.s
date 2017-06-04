	.file "sunpro-cpu.c"
	.code32
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

	.section .data,"aw"
Ddata.data: / Offset 0

	.section .bss,"aw"
Bbss.bss:

	.section .bssf,"aw"

	.section .rodata,"a"
Drodata.rodata: / Offset 0
