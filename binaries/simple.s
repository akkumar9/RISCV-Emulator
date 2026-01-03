.section .text
.global _start

_start:
# Simple computation: 10 + 20 = 30
addi x10, x0, 10      # a0 = 10
addi x11, x0, 20      # a1 = 20
add  x10, x10, x11    # a0 = a0 + a1 = 30

# Exit syscall
li   a7, 93           # syscall number for exit
ecall                 # make syscall