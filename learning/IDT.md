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

## **Conclusion**
L’IDT permet de rediriger les interruptions vers des handlers spécifiques grâce aux **Gate Descriptors**.  
Chaque entrée stocke l’adresse du handler, le segment du kernel et le type d’interruption.