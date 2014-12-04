.data
_prompt: .asciiz "Enter an integer:"
_ret: .asciiz "\n"
.globl main

.text
read:
	li $v0, 4
	la $a0, _prompt
	syscall
	li $v0, 5
	syscall
	jr $ra

write:
	li $v0, 1
	syscall
	li $v0, 4
	la $a0, _ret
	syscall
	move $v0, $0
	jr $ra

gcd:
	sw $ra, -4($sp)
	sw $fp, -8($sp)
	addi $fp, $sp, -8
	addi $sp, $sp, -56
	move $s0, $a1
	sw $s0, -4($fp)
	li $s0, 0
	sw $s0, -8($fp)
	lw $s0, -4($fp)
	lw $s1, -8($fp)
	beq $s0, $s1, label0
	j label1
label0:
	move $s0, $a0
	sw $s0, -12($fp)
	lw $s0, -12($fp)
	move $v0, $s0
	addi $sp, $fp, 8
	lw $ra, -4($sp)
	lw $fp, -8($sp)
	jr $ra
	j label2
label1:
	move $s0, $a1
	sw $s0, -16($fp)
	move $s0, $a0
	sw $s0, -20($fp)
	move $s0, $a1
	sw $s0, -24($fp)
	move $s0, $a0
	sw $s0, -28($fp)
	move $s0, $a1
	sw $s0, -32($fp)
	lw $s0, -28($fp)
	lw $s1, -32($fp)
	div $s0, $s1
	mflo $s0
	sw $s0, -36($fp)
	lw $s0, -24($fp)
	lw $s1, -36($fp)
	mul $s0, $s0, $s1
	sw $s0, -40($fp)
	lw $s0, -20($fp)
	lw $s1, -40($fp)
	sub $s0, $s0, $s1
	sw $s0, -44($fp)
	sw $a0, -4($sp)
	sw $a1, -8($sp)
	sw $a2, -12($sp)
	sw $a3, -16($sp)
	subu $sp, $sp, 16
	subu $sp, $sp, 0
	lw $s0, -44($fp)
	move $a1, $s0
	lw $s0, -16($fp)
	move $a0, $s0
	jal gcd
	addi $sp, $sp, 0
	move $s0, $v0
	addi $sp, $sp, 16
	lw $a0, -4($sp)
	lw $a1, -8($sp)
	lw $a2, -12($sp)
	lw $a3, -16($sp)
	sw $s0, -48($fp)
	lw $s0, -48($fp)
	move $v0, $s0
	addi $sp, $fp, 8
	lw $ra, -4($sp)
	lw $fp, -8($sp)
	jr $ra
label2:

main:
	sw $ra, -4($sp)
	sw $fp, -8($sp)
	addi $fp, $sp, -8
	addi $sp, $sp, -36
	li $s0, 2013
	sw $s0, -4($fp)
	li $s0, 66
	sw $s0, -8($fp)
	lw $s0, -4($fp)
	move $s1, $s0
	sw $s1, -12($fp)
	lw $s0, -8($fp)
	move $s1, $s0
	sw $s1, -16($fp)
	sw $a0, -4($sp)
	sw $a1, -8($sp)
	sw $a2, -12($sp)
	sw $a3, -16($sp)
	subu $sp, $sp, 16
	subu $sp, $sp, 0
	lw $s0, -16($fp)
	move $a1, $s0
	lw $s0, -12($fp)
	move $a0, $s0
	jal gcd
	addi $sp, $sp, 0
	move $s0, $v0
	addi $sp, $sp, 16
	lw $a0, -4($sp)
	lw $a1, -8($sp)
	lw $a2, -12($sp)
	lw $a3, -16($sp)
	sw $s0, -20($fp)
	lw $s0, -20($fp)
	move $s1, $s0
	sw $s1, -24($fp)
	lw $s0, -24($fp)
	subu $sp, $sp, 4
	sw $a0, 0($sp)
	move $a0, $s0
	jal write
	lw $a0, 0($sp)
	addi $sp, $sp, 4
	li $s0, 0
	sw $s0, -28($fp)
	lw $s0, -28($fp)
	move $v0, $s0
	addi $sp, $fp, 8
	lw $ra, -4($sp)
	lw $fp, -8($sp)
	jr $ra
