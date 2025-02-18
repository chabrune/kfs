#include "../include/kernel.h"

typedef struct {
    uint16_t BaseLow;
    uint16_t SegmentSelector;
    uint8_t Reserved;
    uint8_t Flags;
    uint16_t BaseHigh;

} __attribute__((packed)) IDTEntry;

typedef struct {
    uint16_t Limit;
    IDTEntry* Ptr;
} __attribute__((packed)) IDTDescriptor;

typedef enum {
    // tap gate contain address of current instruction (so it can be retried)
    // interrupt contain adress of next instruction
    IDT_FLAG_GATE_TASK              = 0x5,
    IDT_FLAG_GATE_16BIT_INT         = 0x6,
    IDT_FLAG_GATE_16BIT_TRAP        = 0x7,
    IDT_FLAG_GATE_32BIT_INT         = 0xE,
    IDT_FLAG_GATE_32BIT_TRAP        = 0xF,

    // rings are privileges levels
    IDT_FLAG_RING0                  = (0 << 5),
    IDT_FLAG_RING1                  = (1 << 5),
    IDT_FLAG_RING2                  = (2 << 5),
    IDT_FLAG_RING3                  = (3 << 5),

    IDT_FLAG_PRESENT                = 0x80,
} IDT_FLAGS;

IDTEntry g_IDT[256];
IDTDescriptor g_IDTDescriptor = {sizeof(g_IDT) - 1, g_IDT};
void __attribute__((cdecl)) i686_IDT_LOAD(IDTDescriptor* idtDescriptor);