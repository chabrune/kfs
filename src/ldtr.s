global load_IDT

load_IDT:
    push ebp
    mov ebp, esp
    mov eax, [ebp+8]
    lidt [eax]  ; LOAD IDT *.*
    mov esp, ebp
    pop ebp
    ret