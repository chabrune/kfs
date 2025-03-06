global outb

; void outb(uint16_t port, uint8_t value);
; dx port
; al value
outb:
    mov dx, [esp + 4]
    mov al, [esp + 8]

    out dx, al
    ret