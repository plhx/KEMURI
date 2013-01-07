.text
.globl main

# H  e  l  l  o  ,     w  o  r  l  d  !
# 48 65 6C 6C 6F 2C 20 77 6F 72 6C 64 21

kmr_push:
	pushl	$0x21646c72
	pushl	$0x6f77202c
	pushl	$0x6f6c6c65
	pushl	$0x48
	addl	$0x03,%esp

kmr_xor:
	popl	%eax
	xorb	%al,%ah
	pushl	%eax
	incl	%esp

kmr_not:
	notb	(%esp)

kmr_dup:
	movl	(%esp),%eax
	decl	%esp
	movb	%al,(%esp)

kmr_rot:
	movzxw	1(%esp),%eax
	movzxb	(%esp),%edx
	movw	%ax,(%esp)
	movb	%dl,2(%esp)

kmr_prn:
	movl	$4,%eax
	movl	$1,%ebx
	movl	%esp,%ecx
	movl	$1,%edx
	int		$0x80

main:
	ret

