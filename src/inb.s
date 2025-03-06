global inb
; uint_8 inb(uint16_t port);
; dx port
; al value
inb:
    mov dx, [esp+4]
    in al, dx
    ret