.data
var4: .word 0x5

.text
main:

lw x10 var4
addi x12 x0 1
jal x1 fib
beq x0 x0 ex

fib:
    addi x2 x2 -12
       
    sw x10 4(x2)
    sw x1 0(x2)
    bne x10 x0 l1
    addi x11 x0 0
    addi x2 x2 12
    jalr x0 x1 0
    l1:
        bne x12 x10 l2
           addi x11 x0 1
        addi x2 x2 12
        jalr x0 x1 0
    l2:
        addi x10 x10 -1
        jal x1 fib
        add x13 x11 x0
         sw x13 8(x2)
        addi x10 x10 -1
        jal x1 fib
        
        lw x13 8(x2)
        add x11 x11 x13
        lw x1 0(x2)
        lw x10 4(x2)
        addi x2 x2 12
        jalr x0 x1 0
ex:

