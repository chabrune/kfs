global load_IDT

load_IDT:
    lidt [eax+4]
    ret