	.file "sunpro-cpu.c"
	.code64
	.globl get_stack_pos
	.type get_stack_pos, @function

	.align 16
get_stack_pos:
	push	%rbp
	movq	%rsp,%rbp
	movq	%rsp,%rax
	leave
	ret

	.section .data,"aw"
Ddata.data: / Offset 0



	.section .bss,"aw"
Bbss.bss:


	.section .bssf,"aw"


	.section .rodata,"a"
Drodata.rodata: / Offset 0
