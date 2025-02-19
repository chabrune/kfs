[bits 32]

global IDT_LOAD
IDT_LOAD:
    ; make new call frame
    push ebp
    mov ebp, esp

    ; load idt
    mov eax, [ebp+8]
    lidt [eax]  ; LOAD IDT *.*

    ; restore old call frame
    mov esp, ebp
    pop ebp
    ret