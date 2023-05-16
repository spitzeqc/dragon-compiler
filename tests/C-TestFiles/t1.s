	.file	"t1.c"
	.text
	.globl	main
	.type	main, @function
main:
	pushl	%ebp
	movl	%esp, %ebp
	andl	$-16, %esp
	subl	$32, %esp
	movl	$23, 16(%esp)
	movl	$19, 12(%esp)
	movl	$17, 8(%esp)
	movl	$13, 4(%esp)
	movl	$11, (%esp)
	call	boo
	leave
	ret
	.size	main, .-main
	.globl	boo
	.type	boo, @function
boo:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$32, %esp
	movl	8(%ebp), %eax
	movl	%eax, -4(%ebp)
	movl	12(%ebp), %eax
	movl	%eax, -8(%ebp)
	movl	16(%ebp), %eax
	movl	%eax, -12(%ebp)
	movl	20(%ebp), %eax
	movl	%eax, -16(%ebp)
	movl	24(%ebp), %eax
	movl	%eax, -20(%ebp)
	leave
	ret
	.size	boo, .-boo
	.ident	"GCC: (GNU) 4.8.5 20150623 (Red Hat 4.8.5-44)"
	.section	.note.GNU-stack,"",@progbits
