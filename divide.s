.text
.syntax unified
.thumb
.cpu cortex-m0plus
.global my_div
.type my_div, %function

my_div:
    movs r2, #0

    start:    
    subs r0, r0, r1
    bmi end         
    adds r2, r2, #1
    b start
    end:
    mov r0, r2
    BX LR
.end
