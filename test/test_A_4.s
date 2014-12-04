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

sum:
	sw $ra, -4($sp)
	sw $fp, -8($sp)
	addi $fp, $sp, -8
	addi $sp, $sp, -20
	move $s0, $a0
	sw $s0, -4($fp)
	move $s0, $a1
	sw $s0, -8($fp)
	lw $s0, -4($fp)
	lw $s1, -8($fp)
	add $s0, $s0, $s1
	sw $s0, -12($fp)
	lw $s0, -12($fp)
	move $v0, $s0
	addi $sp, $fp, 8
	lw $ra, -4($sp)
	lw $fp, -8($sp)
	jr $ra

main:
	sw $ra, -4($sp)
	sw $fp, -8($sp)
	addi $fp, $sp, -8
	addi $sp, $sp, -36
	li $s0, 3
	sw $s0, -4($fp)
	li $s0, 2
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
	jal sum
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
