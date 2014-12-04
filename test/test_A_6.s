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

main:
	sw $ra, -4($sp)
	sw $fp, -8($sp)
	addi $fp, $sp, -8
	addi $sp, $sp, -72
	li $s0, 0
	sw $s0, -4($fp)
	li $s0, 4
	sw $s0, -8($fp)
label0:
	lw $s0, -4($fp)
	move $s1, $s0
	sw $s1, -12($fp)
	lw $s0, -8($fp)
	move $s1, $s0
	sw $s1, -16($fp)
	lw $s0, -12($fp)
	lw $s1, -16($fp)
	blt $s0, $s1, label1
	j label2
label1:
	li $s0, 0
	sw $s0, -20($fp)
	lw $s0, -20($fp)
	move $s1, $s0
	sw $s1, -24($fp)
label3:
	lw $s0, -24($fp)
	move $s1, $s0
	sw $s1, -28($fp)
	lw $s0, -4($fp)
	move $s1, $s0
	sw $s1, -32($fp)
	lw $s0, -28($fp)
	lw $s1, -32($fp)
	blt $s0, $s1, label4
	j label5
label4:
	lw $s0, -24($fp)
	move $s1, $s0
	sw $s1, -36($fp)
	lw $s0, -36($fp)
	subu $sp, $sp, 4
	sw $a0, 0($sp)
	move $a0, $s0
	jal write
	lw $a0, 0($sp)
	addi $sp, $sp, 4
	lw $s0, -24($fp)
	move $s1, $s0
	sw $s1, -40($fp)
	li $s0, 1
	sw $s0, -44($fp)
	lw $s0, -40($fp)
	lw $s1, -44($fp)
	add $s0, $s0, $s1
	sw $s0, -48($fp)
	lw $s0, -48($fp)
	move $s1, $s0
	sw $s1, -24($fp)
	j label3
label5:
	lw $s0, -4($fp)
	move $s1, $s0
	sw $s1, -52($fp)
	li $s0, 1
	sw $s0, -56($fp)
	lw $s0, -52($fp)
	lw $s1, -56($fp)
	add $s0, $s0, $s1
	sw $s0, -60($fp)
	lw $s0, -60($fp)
	move $s1, $s0
	sw $s1, -4($fp)
	j label0
label2:
	li $s0, 0
	sw $s0, -64($fp)
	lw $s0, -64($fp)
	move $v0, $s0
	addi $sp, $fp, 8
	lw $ra, -4($sp)
	lw $fp, -8($sp)
	jr $ra
