global default_ISR
default_ISR:
    pusha
    mov al, 0x20
    out 0x20, al
    popa
    iret