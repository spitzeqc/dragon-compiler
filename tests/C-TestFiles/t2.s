	.file	"t2.c"
	.text
	.globl	main
	.type	main, @function
main:
	pushl	%ebp
	movl	%esp, %ebp
	andl	$-16, %esp
	call	boo
	leave
	ret
	.size	main, .-main
	.section	.rodata
.LC0:
	.string	"%d"
	.text
	.globl	boo
	.type	boo, @function
boo:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$40, %esp
	leal	-16(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$.LC0, (%esp)
	call	__isoc99_scanf
	movl	-16(%ebp), %eax
	leal	-2(%eax), %edx
	movl	-16(%ebp), %eax
	imull	%edx, %eax
	addl	$1, %eax
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$.LC0, (%esp)
	call	printf
	leave
	ret
	.size	boo, .-boo
	.ident	"GCC: (GNU) 4.8.5 20150623 (Red Hat 4.8.5-44)"
	.section	.note.GNU-stack,"",@progbits
