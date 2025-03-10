global load_GDT

load_GDT:
    lgdt [esp + 4]
    ret
