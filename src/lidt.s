global load_IDT

load_IDT:
    lidt [esp + 4]
    ret
