[bits 32]

; void __attribute__((cdecl)) i686_IDT_LOAD(IDTDescriptor* idtDescriptor);

gloabal i686_IDT_LOAD
i686_IDT_LOAD:
    ; make new call frame
    push ebp
    mov ebp, esp

    ; load idt
    mov eax, [ebp+8]
    lidt [eax]

    ; restore old call frame
    mov esp, ebp
    pop ebp
    ret