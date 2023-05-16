	.file	"t3.c"
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
	.globl	boo
	.type	boo, @function
boo:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$56, %esp
	movl	$2, -12(%ebp)
	movl	$3, -16(%ebp)
	movl	$5, -20(%ebp)
	movl	-12(%ebp), %eax
	movl	%eax, 20(%esp)
	movl	-16(%ebp), %eax
	movl	%eax, 16(%esp)
	movl	-20(%ebp), %eax
	movl	%eax, 12(%esp)
	movl	-20(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	-16(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	call	foo
	leave
	ret
	.size	boo, .-boo
	.globl	foo
	.type	foo, @function
foo:
	pushl	%ebp
	movl	%esp, %ebp

	movl	12(%ebp), %eax
	movl	8(%ebp), %edx
	addl	%eax, %edx
	movl	16(%ebp), %eax
	addl	%eax, %edx
	movl	20(%ebp), %eax
	addl	%eax, %edx
	movl	24(%ebp), %eax
	addl	%eax, %edx
	movl	28(%ebp), %eax
	addl	%edx, %eax
	popl	%ebp
	ret
	.size	foo, .-foo
	.ident	"GCC: (GNU) 4.8.5 20150623 (Red Hat 4.8.5-44)"
	.section	.note.GNU-stack,"",@progbits
