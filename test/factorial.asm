.data
var1:.word 7
.text
lw x10 var1        
addi x11 x11 1        
jal x1 fact
jal x0 exit
fact:
addi x2 x2 -8
sw x1 4(x2)
sw x10 0(x2)
addi x5 x10 -1
bge x5 x0 else
add x11 x11 x0
addi x2 x2 8
jalr x0 x1 0
else:
addi x10 x10 -1
jal x1 fact
addi x7 x11 0        
lw x10 0(x2)
lw x1 4(x2)
addi x2 x2 8
mul x11 x7 x10        
jalr x0 x1 0
exit:

