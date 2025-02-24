L'**Interrupt Descriptor Table (IDT)** est une table qui définit comment les interruptions et exceptions sont gérées en mode protégé. Chaque entrée de l'IDT est appelée un **Gate Descriptor** (descripteur de porte), et il pointe vers un gestionnaire d'interruption (ISR - Interrupt Service Routine).

---

## **Structure d'un Gate Descriptor**
Chaque entrée de l'IDT fait **8 octets (64 bits)** et est structurée comme suit :

| **Bits**      | **Nom**                | **Description** |
|--------------|-----------------------|---------------|
| 0 - 15      | **Offset (Low)**       | Adresse basse du handler |
| 16 - 31     | **Segment Selector**   | Sélecteur du segment de code |
| 32 - 39     | **Réservé**            | Doit être à 0 |
| 40 - 43     | **Gate Type**          | Type de la porte (interrupt, trap, task gate) |
| 44          | **0**                   | Doit être à 0 |
| 45 - 46     | **DPL (Descriptor Privilege Level)** | Privilège requis pour appeler cette interruption |
| 47          | **P (Present)**         | 1 si la porte est valide, 0 sinon |
| 48 - 63     | **Offset (High)**       | Adresse haute du handler |

---

## **Décomposition en Champs**
### **1. Offset (Low) & Offset (High)**
Ces deux champs forment l’adresse **32-bit** du gestionnaire d’interruption.

- **Offset (Low)** (bits 0-15) contient les 16 bits de poids faible de l’adresse.
- **Offset (High)** (bits 48-63) contient les 16 bits de poids fort de l’adresse.

🛠 **Exemple** :  
Si ton gestionnaire d’interruption est à `0x12345678`, alors :
- **Offset Low** = `0x5678`
- **Offset High** = `0x1234`

---

### **2. Segment Selector**
C'est un sélecteur pour le segment de code (en général **0x08**, qui pointe vers le segment de code en mode protégé).

---

### **3. Gate Type**
C'est le type de descripteur. Il y a plusieurs types :

| **Valeur** | **Type** | **Description** |
|------------|---------|---------------|
| 0b0101 (5) | Task Gate | Pour changer de tâche |
| 0b1110 (14) | Interrupt Gate | Appelle un handler et désactive les interruptions (clear IF) |
| 0b1111 (15) | Trap Gate | Appelle un handler sans désactiver les interruptions |

⚠ **On utilise généralement `0x8E` (10001110b)** pour un **Interrupt Gate**.

---

### **4. DPL (Descriptor Privilege Level)**
Définit le niveau de privilège requis pour appeler cette interruption.

| **Valeur** | **Signification** |
|-----------|----------------|
| `00` | Niveau 0 (Kernel) |
| `11` | Niveau 3 (Utilisateur) |

➡ **Les interruptions matérielles ont un DPL de 0.**  
➡ **Les appels système (syscalls) ont un DPL de 3 pour être accessibles par les applications.**

---

### **5. P (Present)**
Indique si l’entrée de l’IDT est valide.

- **1** = Active et utilisable
- **0** = Invalide

---

## **Exemple d’une Entrée IDT**
Si on veut définir une interruption IRQ1 (clavier), voici comment on remplirait une entrée de l'IDT en C :

```c
void idt_set_gate(int num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low = base & 0xFFFF;   // Offset Low (16 bits)
    idt[num].sel = sel;                  // Segment Selector (0x08)
    idt[num].always0 = 0;                 // Réservé (toujours 0)
    idt[num].flags = flags;               // Flags (0x8E = Interrupt Gate, Kernel mode)
    idt[num].base_high = (base >> 16) & 0xFFFF; // Offset High (16 bits)
}
```

**Exemple d’appel pour l’IRQ1 (Clavier) :**
```c
extern void keyboard_handler();
idt_set_gate(33, (uint32_t)keyboard_handler, 0x08, 0x8E);
```

---

## **Résumé en Image**
```
  Bits:      0        15 16       31 32  39 40   43 44  45  46  47 48       63
             +--------+----------+----+----+----+----+----+----------+
             | Offset | Selector |  0 |Type| 0  | DPL| P  | Offset    |
             |  Low   |          |    |    |    |    |    |  High     |
             +--------+----------+----+----+----+----+----+----------+
```

---


Bien sûr ! Voici une **explication détaillée** de tout le processus, étape par étape, pour comprendre comment fonctionne la gestion des interruptions (IRQ) et le clavier dans ton kernel. 🚀  

---

## **1️⃣ Comprendre les interruptions**
Une **interruption** est un signal envoyé au processeur pour lui demander d'exécuter une tâche spéciale, comme :
- **Interruption logicielle** (ex: `int 0x80` pour les syscalls sous Linux).
- **Interruption matérielle (IRQ)** générée par un périphérique (clavier, souris, disque...).

💡 **Problème** : Le processeur doit savoir **quel code exécuter** quand une interruption survient. C'est là qu'intervient l'**IDT**.

---

## **2️⃣ L'IDT (Interrupt Descriptor Table)**
L'**IDT (Interrupt Descriptor Table)** est une **table** qui stocke des "portes d'interruption" (interrupt gates).  
Chaque **entrée** de cette table contient :
- L'**adresse** d'un gestionnaire d'interruption (**handler**).
- Le **segment** à utiliser (en général `0x08` pour le kernel).
- Les **drapeaux** pour définir le type d'interruption (ex: Ring0, 32-bit...).

### **🔹 Comment est organisée l'IDT ?**
| Entrée | Numéro | Interruption |
|--------|--------|--------------|
| `0x00` | 0 | Division par zéro |
| `0x21` | 33 | IRQ1 - Clavier |
| `0x80` | 128 | Syscalls Linux |

💡 **On doit remplir l'IDT et la charger en mémoire avec `lidt`.**  

---

## **3️⃣ Les IRQ (Interrupt Request)**
Les **IRQ** sont des interruptions matérielles. Les anciens PC utilisent deux contrôleurs **PIC** (Programmable Interrupt Controller) :
- **PIC1** gère les IRQ **0 à 7**.
- **PIC2** gère les IRQ **8 à 15**.

Les IRQ sont **remappées** pour éviter un conflit avec les interruptions processeur (exceptions).  
💡 **Après remapping, l'IRQ1 (clavier) est en fait sur l'interruption 33 (0x21).**

---

## **4️⃣ Étape par étape : Comment le clavier déclenche une interruption**
Voici ce qui se passe quand tu appuies sur une touche :

### **🔹 Étape 1 : Le clavier envoie un signal**
- Quand une touche est pressée, le **contrôleur clavier** envoie un **scancode** au **PIC** via le port `0x60`.
- Le **PIC1** envoie ensuite une **IRQ1** au processeur.

### **🔹 Étape 2 : Le processeur consulte l'IDT**
- Le processeur regarde l'**IDT** et trouve l'entrée **33 (0x21)**.
- Il saute à l'adresse du **handler** défini pour cette interruption.

### **🔹 Étape 3 : Exécution du handler**
- Le processeur exécute le **handler d’interruption** (`keyboard_handler.s`).
- Le handler :
  1. Sauvegarde les registres (`pusha`).
  2. Appelle une fonction en **C** (`keyboard_callback()`).
  3. Envoie un **EOI (End of Interrupt)** au PIC (`out 0x20, al`).
  4. Restaure les registres (`popa`).
  5. Termine avec `iret` (return from interrupt).

### **🔹 Étape 4 : Lire la touche pressée**
- Dans `keyboard_callback()`, on lit le **scancode** avec `inb(0x60)`.
- On le convertit en ASCII et on affiche le caractère.

---

## **5️⃣ Résumé global du processus**
1. **Appui sur une touche** → Envoi d'un signal au **PIC1**.
2. **Le PIC1 envoie une IRQ1 (33)** au CPU.
3. **Le CPU consulte l’IDT** et exécute le **handler** (`keyboard_handler.s`).
4. **Le handler appelle `keyboard_callback()` en C**.
5. **On récupère le scancode via `inb(0x60)`**.
6. **On affiche le caractère sur l'écran**.
7. **On envoie un EOI (`out 0x20, al`)** pour dire que l'interruption est terminée.

---

## **6️⃣ Code complet pour gérer le clavier**
### **🔹 Handler d'interruption en ASM (`keyboard_handler.s`)**
```assembly
global keyboard_handler
extern keyboard_callback

keyboard_handler:
    pusha
    call keyboard_callback
    popa
    mov al, 0x20
    out 0x20, al
    iret
```

---

### **🔹 Handler en C (`keyboard.c`)**
```c
#include "../include/kernel.h"
#include "../include/idt.h"
#include "../include/port.h"

void keyboard_callback() {
    uint8_t scancode = inb(0x60); // Lire le scancode du clavier

    if (scancode & 0x80) {
        // Touche relâchée
    } else {
        putc('A'); // À améliorer avec un mapping scancode -> ASCII
    }
}
```

---

### **🔹 Ajout dans l'IDT (`IDT_initialize()`)**
```c
void IDT_initialize()
{
    IDT_SetGate(33, keyboard_handler, 0x08, IDT_FLAG_PRESENT | IDT_FLAG_RING0 | IDT_FLAG_GATE_32BIT_INT);
    IDT_LOAD(&g_IDTDescriptor);
}
```

---

### **🔹 Activer les interruptions dans `kmain()`**
```c
void kmain(void)
{
    init();
    puts("FuckOs>$");
    IDT_initialize();

    __asm__ volatile ("sti"); // Active les interruptions

    while (1); // Boucle infinie
}
```