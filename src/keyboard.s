global keyboard_ISR
extern keyboard_handler

keyboard_ISR:
    pusha
    call keyboard_handler
    mov al, 0x20
    out 0x20, al  
    popa
    iret
